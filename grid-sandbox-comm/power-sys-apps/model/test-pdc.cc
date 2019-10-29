/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
 /*
  */
 #include "ns3/address.h"
 #include "ns3/address-utils.h"
 #include "ns3/log.h"
 #include "ns3/inet-socket-address.h"
 #include "ns3/inet6-socket-address.h"
 #include "ns3/node.h"
 #include "ns3/socket.h"
 #include "ns3/udp-socket.h"
 #include "ns3/simulator.h"
 #include "ns3/socket-factory.h"
 #include "ns3/packet.h"
 #include "ns3/trace-source-accessor.h"
 #include "ns3/udp-socket-factory.h"
 #include "test-pdc.h"
 #include "ns3/uinteger.h"
 #include "ns3/double.h"

#include "pmu-frame-structure.h"

#include "ns3/log.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"
#include "ns3/simulator.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/udp-header.h"
#include "math.h"
#include <vector>
#include <deque>
 
 namespace ns3 {
 
 NS_LOG_COMPONENT_DEFINE ("TestPdc");
 
 NS_OBJECT_ENSURE_REGISTERED (TestPdc);

 uint32_t totalSize = 0;
 
 TypeId 
 TestPdc::GetTypeId (void)
 {
   static TypeId tid = TypeId ("ns3::TestPdc")
     .SetParent<Application> ()
     .SetGroupName("Applications")
     .AddConstructor<TestPdc> ()
     .AddAttribute ("Local",
                    "The Address on which to Bind the rx socket.",
                    AddressValue (),
                    MakeAddressAccessor (&TestPdc::m_local),
                    MakeAddressChecker ())
     .AddAttribute ("Protocol",
                    "The type id of the protocol to use for the rx socket.",
                    TypeIdValue (UdpSocketFactory::GetTypeId ()),
                    MakeTypeIdAccessor (&TestPdc::m_tid),
                    MakeTypeIdChecker ())
     .AddAttribute ("PdcID", "The PDC ID of the device",
                   UintegerValue (0),
                   MakeUintegerAccessor (&TestPdc::m_pdcID),
                   MakeUintegerChecker<uint16_t> ())
     .AddAttribute ("Remote", "The address of the destination",
                   AddressValue (),
                   MakeAddressAccessor (&TestPdc::s_peer),
                   MakeAddressChecker ())
     .AddTraceSource ("Rx",
                      "A packet has been received",
                      MakeTraceSourceAccessor (&TestPdc::m_rxTrace),
                      "ns3::Packet::AddressTracedCallback")
     .AddTraceSource ("RxWithAddresses", "A packet has been received",
                      MakeTraceSourceAccessor (&TestPdc::m_rxTraceWithAddresses),
                      "ns3::Packet::TwoAddressTracedCallback")
   ;
   return tid;
 }
 
 TestPdc::TestPdc ()
  : s_socket (0),
    s_connected (false),
    s_totBytes (0)
 {
   NS_LOG_FUNCTION (this);
   
   m_socket = 0;
   m_totalRx = 0;
   
 }
 
 TestPdc::~TestPdc()
 {
   NS_LOG_FUNCTION (this);
 }
 
 uint64_t TestPdc::GetTotalRx () const
 {
   NS_LOG_FUNCTION (this);
   return m_totalRx;
 }

 Ptr<Socket>
TestPdc::GetSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return s_socket;
}
 
 Ptr<Socket>
 TestPdc::GetListeningSocket (void) const
 {
   NS_LOG_FUNCTION (this);
   return m_socket;
 }
 
 std::list<Ptr<Socket> >
 TestPdc::GetAcceptedSockets (void) const
 {
   NS_LOG_FUNCTION (this);
   return m_socketList;
 }

 void TestPdc::addingDfToTimeStampBuffer (Ptr<Packet> newPacket)
 {
  NS_LOG_FUNCTION (this);

  //If it is first dataframe, create a deque and add the dataframe to it.
  if (timeStampVector.size() < 1)
  {
    std::deque<ns3::Ptr<Packet>> newTimeStampBuffer = CreateNewTimeStampBuffer();
    newTimeStampBuffer.push_back(newPacket);   
    timeStampVector.push_back(newTimeStampBuffer);
    NS_LOG_INFO ("Buffer Created");
    
  }
  //if there is already a buffer in the vector
  else
  {
    //checking the timestamp of all the deques i.e. vector elements
    bool packetAdded = false;
    int loopCount = -1; 
    for (std::deque<std::deque<ns3::Ptr<Packet>>>::iterator it = timeStampVector.begin() ; it != timeStampVector.end(); ++it)
    {
      std::deque<ns3::Ptr<Packet>> innerDeque = *it;
      
      //Keeping track of index
      ++loopCount;

      //comparing new Packet timestamp with already inserted Packets
      TestPmuFrame oldHeader, newHeader;
      Ptr<Packet> oldPacket = innerDeque.front();
      oldPacket->PeekHeader(oldHeader); 
      newPacket->PeekHeader(newHeader);

      //Check if the newPacket belongs to sent buffer i.e. it is an old Timestamp packet
      if (oldHeader.GetTimeStamp() > newHeader.GetTimeStamp())
      {
        NS_LOG_INFO ("OLD PACKET");
        //Discard the packet
        break; 
      }

      //Add the packet if the timestamp matches with any of the deque timestamps  
      if (oldHeader.GetTimeStamp() == newHeader.GetTimeStamp()){
        innerDeque.push_back(newPacket);
        timeStampVector[loopCount] = innerDeque;
        packetAdded = true;
        break;
      }
    }

    //If matching timeStamp doesn't exist, then create new timestamp inner deque and push the new packet in it. 
    if (packetAdded == false)
    { 
        //NS_LOG_INFO ("**********New Timestamp -- New Packet added *********");
        std::deque<ns3::Ptr<Packet>> newTimeStampBuffer = CreateNewTimeStampBuffer();
        newTimeStampBuffer.push_back(newPacket);   
        timeStampVector.push_back(newTimeStampBuffer);
    } 
  }

 }

 std::deque<ns3::Ptr<Packet>> TestPdc::CreateNewTimeStampBuffer()
 {
  std::deque<ns3::Ptr<Packet>> newTimeStampBuffer;
  return newTimeStampBuffer;
 }

 void TestPdc::DoDispose (void)
 {
   NS_LOG_FUNCTION (this);
   m_socket = 0;
   s_socket = 0;
   m_socketList.clear ();
 
   // chain up
   Application::DoDispose ();
 }
 
 
 // Application Methods
 void TestPdc::StartApplication ()    // Called at time specified by Start
 {
   NS_LOG_FUNCTION (this);

   // Create the socket if not already
   if (!m_socket)
     {
       m_socket = Socket::CreateSocket (GetNode (), m_tid);
       if (m_socket->Bind (m_local) == -1)
         {
           NS_FATAL_ERROR ("Failed to bind socket");
         }
       m_socket->Listen ();
       m_socket->ShutdownSend ();
       if (addressUtils::IsMulticast (m_local))
         {
           Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket);
           if (udpSocket)
             {
               // equivalent to setsockopt (MCAST_JOIN_GROUP)
               udpSocket->MulticastJoinGroup (0, m_local);
             }
           else
             {
               NS_FATAL_ERROR ("Error: joining multicast on a non-UDP socket");
             }
         }
     }
 
   m_socket->SetRecvCallback (MakeCallback (&TestPdc::HandleRead, this));
   m_socket->SetAcceptCallback (
     MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
     MakeCallback (&TestPdc::HandleAccept, this));
   m_socket->SetCloseCallbacks (
     MakeCallback (&TestPdc::HandlePeerClose, this),
     MakeCallback (&TestPdc::HandlePeerError, this));
   ScheduleNextTx();
 }
 
 void TestPdc::StopApplication ()     // Called at time specified by Stop
 {
   NS_LOG_FUNCTION (this);
   while(!m_socketList.empty ()) //these are accepted sockets, close them
     {
       Ptr<Socket> acceptedSocket = m_socketList.front ();
       m_socketList.pop_front ();
       acceptedSocket->Close ();
     }
   if (m_socket) 
     {
       m_socket->Close ();
       m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
     }

  Simulator::Cancel (s_sendEvent);
  if(s_socket != 0)
    {
      s_socket->Close ();
    }
  else
    {
      NS_LOG_WARN ("TestPdc found null socket to close in StopApplication");
    } 
 }

// Added functionality for sending data to SuperPDC
void TestPdc::ScheduleNextTx ()
{
  NS_LOG_FUNCTION (this);

  Time nextTime (Seconds (2.0 / 1000)); // Time till next packet
  //Time nextTime (Seconds (1.0));
  //NS_LOG_INFO ("****************nextTime*********************** = " << nextTime);
  s_sendEvent = Simulator::Schedule (nextTime, &TestPdc::ReadyToSend, this);
}

//Sends the data packet
void TestPdc::SendPacket (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT (s_sendEvent.IsExpired ());
  m_txTrace (packet);
  s_socket->Send (packet);
  s_totBytes += packet->GetSize();
  Address localAddress;
  s_socket->GetSockName (localAddress);
  if (InetSocketAddress::IsMatchingType (s_peer))
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetMilliSeconds ()
                   << "ms PDC application at Node " << m_node->GetId() << " sent "
                   <<  packet->GetSize () << " bytes to SUPERPDC at"
                   << InetSocketAddress::ConvertFrom(s_peer).GetIpv4 ()
                   //<< " port " << InetSocketAddress::ConvertFrom (s_peer).GetPort ()
                   << " total Tx " << s_totBytes << " bytes");
      m_txTraceWithAddresses (packet, localAddress, InetSocketAddress::ConvertFrom (s_peer));
    }
  else if (Inet6SocketAddress::IsMatchingType (s_peer))
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds ()
                   << "s PMU application at Node " << m_node->GetId() << " sent "
                   <<  packet->GetSize () << " bytes to "
                   << Inet6SocketAddress::ConvertFrom(s_peer).GetIpv6 ()
                   << " port " << Inet6SocketAddress::ConvertFrom (s_peer).GetPort ()
                   << " total Tx " << s_totBytes << " bytes");
      m_txTraceWithAddresses (packet, localAddress, Inet6SocketAddress::ConvertFrom(s_peer));
    }
  Simulator::Cancel (s_sendEvent);
  ScheduleNextTx ();
}

//Creating the packet to send
void TestPdc::Send (std::deque<ns3::Ptr<Packet>> innerDeque)
{
  NS_LOG_FUNCTION (this);

  Ptr<Packet> newPacket = Create<Packet>();

  //iterate through the innerDeque
  std::deque<ns3::Ptr<Packet>>::iterator it = innerDeque.begin();
  _Float32 floatVal = 0;
  int pdcNumber = 0; 
  TestPmuFrame eachPacketHeader;
  //NS_LOG_INFO( "***********Size of innerDeque while creating Packet sent to the SUPERPDC*** " << innerDeque.size()); 
  
  
  while (it != innerDeque.end()){
    Ptr<Packet> eachPacket = *it++;
    eachPacket->PeekHeader(eachPacketHeader);
    _Float32 eachFreq = eachPacketHeader.GetFreq();
    floatVal = floatVal + eachFreq; 
    pdcNumber++; 
    NS_LOG_INFO("Appending Data for timeStamp: " << eachPacketHeader.GetTimeStamp() 
      << " For PMUid: "<< eachPacketHeader.GetPmuID());
  }

  TestPmuFrame frame(pdcNumber, Simulator::Now(), floatVal);
  newPacket->AddHeader(frame);
  SendPacket(newPacket);
}

void TestPdc::ConnectionSucceeded (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  s_connected = true;
}

void TestPdc::ConnectionFailed (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}

 
void TestPdc::HandleRead (Ptr<Socket> socket)
 {
   NS_LOG_FUNCTION (this << socket);
   Ptr<Packet> packet;
   Address from;
   Address localAddress;
   
   //Address remoteAddress;
   while ((packet = socket->RecvFrom (from)))
     {
       if (packet->GetSize () == 0)
         { //EOF
           break;
         }
       m_totalRx += packet->GetSize ();

       if (InetSocketAddress::IsMatchingType (from))
         {
           TestPmuFrame testFrame; 

           NS_LOG_INFO ("At time " << Simulator::Now ().GetMilliSeconds ()
                        << "ms PDC at node " << m_node->GetId() << " received "
                        <<  packet->GetSize () << " bytes from "
                        << InetSocketAddress::ConvertFrom(from).GetIpv4 ()
                        //<< " port " << InetSocketAddress::ConvertFrom (from).GetPort ()
                        << " total Rx " << m_totalRx << " bytes" 
                        << "Peeking Header "  << packet->PeekHeader(testFrame) 
                        //<< "Reading Data from Header " << testFrame.GetTimeStamp()
                        << " Found Packet Content " << testFrame.GetFreq());

         }
       else if (Inet6SocketAddress::IsMatchingType (from))
         {
           NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds ()
                        << "s packet TestPdc received "
                        <<  packet->GetSize () << " bytes from "
                        << Inet6SocketAddress::ConvertFrom(from).GetIpv6 ()
                        << " port " << Inet6SocketAddress::ConvertFrom (from).GetPort ()
                        << " total Rx " << m_totalRx << " bytes");
         }
       socket->GetSockName (localAddress);
       m_rxTrace (packet, from);
       m_rxTraceWithAddresses (packet, from, localAddress);
       
       //Sending the packet to add to the Buffer
       addingDfToTimeStampBuffer(packet);

       //ScheduleNextTx();
       
     }
 }
 
// Creating for checking socket to superPDC
 void TestPdc::ReadyToSend(){
  NS_LOG_FUNCTION (this);
  // Setting this part for sending data to SuperPDC

        // Create the socket if not already
        if (!s_socket)
          {
            s_socket = Socket::CreateSocket (GetNode (), m_tid);
            if (Inet6SocketAddress::IsMatchingType (s_peer))
              {
                if (s_socket->Bind6 () == -1)
                  {
                    NS_FATAL_ERROR ("Failed to bind socket");
                  }
              }
            else if (InetSocketAddress::IsMatchingType (s_peer) ||
                     PacketSocketAddress::IsMatchingType (s_peer))
              {
                if (s_socket->Bind () == -1)
                  {
                    NS_FATAL_ERROR ("Failed to bind socket");
                  }
              }
            s_socket->Connect (s_peer);
            s_socket->SetAllowBroadcast (true);
            s_socket->ShutdownRecv ();

            s_socket->SetConnectCallback (
              MakeCallback (&TestPdc::ConnectionSucceeded, this),
              MakeCallback (&TestPdc::ConnectionFailed, this));
          }

        if (timeStampVector.size() > 1){
          std::deque<ns3::Ptr<Packet>> sendQueue = timeStampVector.front();
          
          //Delete that element from the buffer
          timeStampVector.erase (timeStampVector.begin());
          Send(sendQueue);
        }
        else
        {
          Simulator::Cancel (s_sendEvent);
          ScheduleNextTx();
        }
 }

 
 void TestPdc::HandlePeerClose (Ptr<Socket> socket)
 {
   NS_LOG_FUNCTION (this << socket);
 }
  
 void TestPdc::HandlePeerError (Ptr<Socket> socket)
 {
   NS_LOG_FUNCTION (this << socket);
 }
  
 
 void TestPdc::HandleAccept (Ptr<Socket> s, const Address& from)
 {
   NS_LOG_FUNCTION (this << s << from);
   s->SetRecvCallback (MakeCallback (&TestPdc::HandleRead, this));
   m_socketList.push_back (s);
 }
 
 } // Namespace ns3