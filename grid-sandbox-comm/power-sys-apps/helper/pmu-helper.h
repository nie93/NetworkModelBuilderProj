
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef PMU_HELPER_H
#define PMU_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"

namespace ns3 {

class PmuHelper
{
public:
  /**
   * Create an PmuHelper to make it easier to work with PmuApplications
   *
   * \param protocol the name of the protocol to use to send traffic
   *        by the applications. This string identifies the socket
   *        factory type used to create sockets for the applications.
   *        A typical value would be ns3::UdpSocketFactory.
   * \param address the address of the remote node to send traffic
   *        to.
   */
  PmuHelper (std::string protocol, Address address);

  /**
   * Helper function used to set the underlying application attributes.
   *
   * \param name the name of the application attribute to set
   * \param value the value of the application attribute to set
   */
  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * Helper function to set a constant sampling rate.  
   *
   * \param samplingRate number of samples per second
   * \param packetSize size in bytes of the packet payloads generated
   */
  void SetSamplingRate (double samplingRate, uint32_t packetSize = 512);

  /**
   * Install an ns3::PmuApplication on each node of the input container
   * configured with all the attributes set with SetAttribute.
   *
   * \param c NodeContainer of the set of nodes on which an PmuApplication 
   * will be installed.
   * \returns Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (NodeContainer c) const;

  /**
   * Install an ns3::PmuApplication on the node configured with all the 
   * attributes set with SetAttribute.
   *
   * \param node The node on which an PmuApplication will be installed.
   * \returns Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (Ptr<Node> node) const;

  /**
   * Install an ns3::PmuApplication on the node configured with all the 
   * attributes set with SetAttribute.
   *
   * \param nodeName The node on which an PmuApplication will be installed.
   * \returns Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (std::string nodeName) const;

  void
  WithHelics (bool bHelics)
  {
    m_with_helics = bHelics;
  }

private:
  /**
   * Install an ns3::PmuApplication on the node configured with all the 
   * attributes set with SetAttribute.
   *
   * \param node The node on which an PmuApplication will be installed.
   * \returns Ptr to the application installed.
   */
  Ptr<Application> InstallPriv (Ptr<Node> node) const;

  ObjectFactory m_factory; //!< Object factory for PmuApp.
  ObjectFactory m_factory_helics; //!< Object factory for PmuHelicsApp.
  bool m_with_helics;
};

} // namespace ns3

#endif /* PMU_HELPER_H */
