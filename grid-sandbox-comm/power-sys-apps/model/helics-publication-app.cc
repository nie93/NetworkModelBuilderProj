#include "ns3/address.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

#include "helics-publication-app.h"
#include "pmu-frame-structure.h"
#include "ns3/string.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HelicsPublicationApp");

NS_OBJECT_ENSURE_REGISTERED (HelicsPublicationApp);

TypeId
HelicsPublicationApp::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::HelicsPublicationApp")
          .SetParent<PacketSink> ()
          .SetGroupName ("Applications")
          .AddConstructor<HelicsPublicationApp> ()
          .AddAttribute ("HelicsPublicationKey",
                         "Publication key in HELICS federate configuration.", StringValue (""),
                         MakeStringAccessor (&HelicsPublicationApp::m_helics_pub_key),
                         MakeStringChecker ());
  return tid;
}

HelicsPublicationApp::HelicsPublicationApp ()
{
  NS_LOG_FUNCTION (this);
}

HelicsPublicationApp::~HelicsPublicationApp ()
{
  NS_LOG_FUNCTION (this);
}

void
HelicsPublicationApp::SetPublication (std::string key)
{
  NS_LOG_FUNCTION (this);

  m_pPub = &helics_federate->getPublication (key);
  if (!m_pPub->isValid ())
    {
      NS_LOG_INFO (helics_federate->getName ()
                   << " Number of publications: " << helics_federate->getInputCount ());
      for (int i = 0; i < helics_federate->getPublicationCount (); i++)
        {
          NS_LOG_INFO ("Publication " << i << ": "
                                      << helics_federate->getPublication (i).getKey ());
        }
      NS_FATAL_ERROR ("Invalid key " << key);
    }
}

// Application Methods
void
HelicsPublicationApp::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () == 0)
        { //EOF
          break;
        }
      TestPmuFrame frame;
      packet->RemoveHeader (frame);
      NS_LOG_INFO("Time " << Simulator::Now() << ": Received from PMU " << frame.GetPmuID() << ", t = " << frame.GetTimeStamp() << ", f = " << frame.GetFreq());

      if (m_pPub == nullptr)
        {
          SetPublication (m_helics_pub_key);
        }

      if (frame.GetPmuID () == 0)
        {
          t0 = frame.GetTimeStamp ();
          freq0 = frame.GetFreq ();
        }
      else if (frame.GetPmuID () == 1)
        {
          t1 = frame.GetTimeStamp ();
          freq1 = frame.GetFreq ();
        }
      else
        {
          NS_FATAL_ERROR ("Unknown PMU ID" << frame.GetPmuID ());
        }

      if (Abs (t1 - t0) < Time ("5ms"))
        {
          m_pPub->publish ((double) (freq0 - freq1));
          NS_LOG_INFO("Time " << Simulator::Now() << ": Published " << freq0 - freq1 << ". t0 = " << t0 << ", t1 = " << t1);
        }
    }
}

} // Namespace ns3
