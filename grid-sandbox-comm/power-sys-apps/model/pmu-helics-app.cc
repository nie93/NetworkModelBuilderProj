/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "pmu-helics-app.h"
#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/string.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PmuHelicsApp");

NS_OBJECT_ENSURE_REGISTERED (PmuHelicsApp);

TypeId
PmuHelicsApp::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::PmuHelicsApp")
          .SetParent<PmuApp> ()
          .SetGroupName ("Applications")
          .AddConstructor<PmuHelicsApp> ()
          .AddAttribute ("HelicsInputKey", "Input key in HELICS federate configuration.",
                         StringValue (""), MakeStringAccessor (&PmuHelicsApp::m_helics_key),
                         MakeStringChecker ());
  return tid;
}

PmuHelicsApp::PmuHelicsApp ()
{
  NS_LOG_FUNCTION (this);
}

void
PmuHelicsApp::SetInput (std::string key)
{
  NS_LOG_FUNCTION (this);

  m_pInput = &helics_federate->getSubscription (key);
  if (!m_pInput->isValid ())
    {
      NS_LOG_INFO (helics_federate->getName ()
                   << " Number of inputs: " << helics_federate->getInputCount ());
      for (int i = 0; i < helics_federate->getInputCount (); i++)
        {
          NS_LOG_INFO ("Input " << i << ": " << helics_federate->getInput (i).getKey ());
        }
      NS_FATAL_ERROR ("Invalid key " << key);
    }
}

PmuHelicsApp::PmuHelicsApp (helics::Input *ptr) : m_pInput (ptr)
{
  NS_LOG_FUNCTION (this);
}

PmuHelicsApp::PmuHelicsApp (std::string key)
{
  NS_LOG_FUNCTION (this);
  SetInput (key);
}

PmuHelicsApp::~PmuHelicsApp ()
{
  NS_LOG_FUNCTION (this);
}

void
PmuHelicsApp::Send ()
{
  NS_LOG_FUNCTION (this);

  if (!m_pInput)
    {
      SetInput (m_helics_key);
    }

  if (m_pInput->isUpdated ())
    {
      NS_LOG_INFO ("Pub type: " << m_pInput->getPublicationType ());
      if (m_pInput->getPublicationType () == "double")
        {
          m_freq = m_pInput->getValue<double>();
        }
    }
  PmuApp::Send();
}

} // namespace ns3