/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "pmu-app.h"
#include "pmu-frame-structure.h"

#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/string.h"
#include "ns3/pointer.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PmuApp");

NS_OBJECT_ENSURE_REGISTERED (PmuApp);

TypeId PmuApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PmuApp")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<PmuApp> ()
    .AddAttribute ("PmuID", "The PMU ID of the device",
                   UintegerValue (0),
                   MakeUintegerAccessor (&PmuApp::m_pmuID),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("SamplingRate", "The sampling rate of the PMU.",
                   DoubleValue (50.0),
                   MakeDoubleAccessor (&PmuApp::m_samplingRate),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("PacketSize", "The size of packets sent in on state",
                   UintegerValue (512),
                   MakeUintegerAccessor (&PmuApp::m_pktSize),
                   MakeUintegerChecker<uint32_t> (1))
    .AddAttribute ("Remote", "The address of the destination",
                   AddressValue (),
                   MakeAddressAccessor (&PmuApp::m_peer),
                   MakeAddressChecker ())
    .AddAttribute ("MaxBytes", 
                   "The total number of bytes to send. Once these bytes are sent, "
                   "no packet is sent again, even in on state. The value zero means "
                   "that there is no limit.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&PmuApp::m_maxBytes),
                   MakeUintegerChecker<uint64_t> ())
    .AddAttribute ("Protocol", "The type of protocol to use. This should be "
                   "a subclass of ns3::SocketFactory",
                   TypeIdValue (UdpSocketFactory::GetTypeId ()),
                   MakeTypeIdAccessor (&PmuApp::m_tid),
                   // This should check for SocketFactory as a parent
                   MakeTypeIdChecker ())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&PmuApp::m_txTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("TxWithAddresses", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&PmuApp::m_txTraceWithAddresses),
                     "ns3::Packet::TwoAddressTracedCallback")
  ;
  return tid;
}

PmuApp::PmuApp ()
  : m_socket (0),
    m_connected (false),
    m_totBytes (0)
{
  NS_LOG_FUNCTION (this);
}

PmuApp::~PmuApp()
{
  NS_LOG_FUNCTION (this);
}

void 
PmuApp::SetMaxBytes (uint64_t maxBytes)
{
  NS_LOG_FUNCTION (this << maxBytes);
  m_maxBytes = maxBytes;
}

Ptr<Socket>
PmuApp::GetSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socket;
}

void
PmuApp::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_socket = 0;
  // chain up
  Application::DoDispose ();
}

// Application Methods
void PmuApp::StartApplication () // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);

  // Create the socket if not already
  if (!m_socket)
    {
      m_socket = Socket::CreateSocket (GetNode (), m_tid);
      if (Inet6SocketAddress::IsMatchingType (m_peer))
        {
          if (m_socket->Bind6 () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
        }
      else if (InetSocketAddress::IsMatchingType (m_peer) ||
               PacketSocketAddress::IsMatchingType (m_peer))
        {
          if (m_socket->Bind () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
        }
      m_socket->Connect (m_peer);
      m_socket->SetAllowBroadcast (true);
      m_socket->ShutdownRecv ();

      m_socket->SetConnectCallback (
        MakeCallback (&PmuApp::ConnectionSucceeded, this),
        MakeCallback (&PmuApp::ConnectionFailed, this));
    }

  // Insure no pending event
  Simulator::Cancel (m_sendEvent);

  // If we are not yet connected, there is nothing to do here
  // The ConnectionComplete upcall will start timers at that time
  //if (!m_connected) return;
  ScheduleNextTx ();  // Schedule the send packet event
}

void PmuApp::StopApplication () // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);

  Simulator::Cancel (m_sendEvent);
  if(m_socket != 0)
    {
      m_socket->Close ();
    }
  else
    {
      NS_LOG_WARN ("PmuApp found null socket to close in StopApplication");
    }
}

// Private helpers
void PmuApp::ScheduleNextTx ()
{
  NS_LOG_FUNCTION (this);

  Time nextTime (Seconds ( 1.0 / m_samplingRate )); // Time till next packet
  NS_LOG_LOGIC ("nextTime = " << nextTime << ", fs = " << m_samplingRate);
  
  m_sendEvent = Simulator::Schedule (nextTime, &PmuApp::Send, this);
}

void PmuApp::SendPacket (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT (m_sendEvent.IsExpired ());
  m_txTrace (packet);
  m_socket->Send (packet);
  m_totBytes += packet->GetSize();
  Address localAddress;
  m_socket->GetSockName (localAddress);
  if (InetSocketAddress::IsMatchingType (m_peer))
    {

      NS_LOG_INFO ("At time " << Simulator::Now ().GetMilliSeconds ()
                   << "ms PMU application at Node " << m_node->GetId() << " sent "
                   <<  packet->GetSize () << " bytes to "
                   << InetSocketAddress::ConvertFrom(m_peer).GetIpv4 ()
                   << " port " << InetSocketAddress::ConvertFrom (m_peer).GetPort ()
                   << " total Tx " << m_totBytes << " bytes"
                   << " Sent Frequency value " << m_freq
                   << " With Processing Delay of "<< (Simulator::Now().GetNanoSeconds() - m_delay) << " ns");
      m_txTraceWithAddresses (packet, localAddress, InetSocketAddress::ConvertFrom (m_peer));
    }
  else if (Inet6SocketAddress::IsMatchingType (m_peer))
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds ()
                   << "s PMU application at Node " << m_node->GetId() << " sent "
                   <<  packet->GetSize () << " bytes to "
                   << Inet6SocketAddress::ConvertFrom(m_peer).GetIpv6 ()
                   << " port " << Inet6SocketAddress::ConvertFrom (m_peer).GetPort ()
                   << " total Tx " << m_totBytes << " bytes");
      m_txTraceWithAddresses (packet, localAddress, Inet6SocketAddress::ConvertFrom(m_peer));
    }
  ScheduleNextTx ();
}

void PmuApp::Send ()
{
  NS_LOG_FUNCTION (this);
  m_delay = Simulator::Now().GetNanoSeconds();
  NS_LOG_INFO(" *********Initiating Send At Node*********** " << m_node->GetId() <<" sent at Time " 
    << m_delay << " ns");

  Ptr<Packet> packet = Create<Packet>();
  //NS_LOG_INFO("Empty Data Packet at Node " << m_node->GetId() <<" created at Time " << m_delay);
  TestPmuFrame frame(m_pmuID, Simulator::Now(), m_freq);
  packet->AddHeader(frame);
  SendPacket(packet);
}

void PmuApp::ConnectionSucceeded (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  m_connected = true;
}

void PmuApp::ConnectionFailed (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}


} // Namespace ns3
