
#include "pmu-helper.h"
#include "ns3/pmu-helics-app.h"
#include "ns3/data-rate.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/names.h"

namespace ns3 {

PmuHelper::PmuHelper (std::string protocol, Address address) : m_with_helics (false)
{
  m_factory.SetTypeId ("ns3::PmuApp");
  m_factory.Set ("Protocol", StringValue (protocol));
  m_factory.Set ("Remote", AddressValue (address));

  m_factory_helics.SetTypeId ("ns3::PmuHelicsApp");
  m_factory_helics.Set ("Protocol", StringValue (protocol));
  m_factory_helics.Set ("Remote", AddressValue (address));
}

void
PmuHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  if (m_with_helics)
    {
      m_factory_helics.Set (name, value);
    }
  else
    {
      m_factory.Set (name, value);
    }
}

ApplicationContainer
PmuHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
PmuHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
PmuHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
PmuHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app;
  if (m_with_helics)
    {
      app = m_factory_helics.Create<Application> ();
    }
  else
    {
      app = m_factory.Create<Application> ();
    }

  node->AddApplication (app);

  return app;
}

void
PmuHelper::SetSamplingRate (double samplingRate, uint32_t packetSize)
{
  m_factory.Set ("SamplingRate", DoubleValue (samplingRate));
  m_factory.Set ("PacketSize", UintegerValue (packetSize));
}

} // namespace ns3
