/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
 /*
  */
 #ifndef TEST_PDC_HELPER_H
 #define TEST_PDC_HELPER_H
 
 #include "ns3/object-factory.h"
 #include "ns3/ipv4-address.h"
 #include "ns3/node-container.h"
 #include "ns3/application-container.h"
 
 namespace ns3 {
 
 class TestPdcHelper
 {
 public:
   TestPdcHelper (std::string protocol, Address address, Address pdcAddress);
 
   void SetAttribute (std::string name, const AttributeValue &value);
   

   void SetPdcAttribute (std::string name, const AttributeValue &value);

   //void SetSamplingRate (double samplingRate, uint32_t packetSize = 512);

   ApplicationContainer Install (NodeContainer c) const;
 
   ApplicationContainer Install (Ptr<Node> node) const;
 
   ApplicationContainer Install (std::string nodeName) const;
   

   // void
   // WithHelics (bool bHelics)
   // {
   //  m_with_helics = bHelics;
   // }

 private:
   Ptr<Application> InstallPriv (Ptr<Node> node) const;
   ObjectFactory m_factory; 
   // ObjectFactory m_factory_helics; //!< Object factory for PmuHelicsApp.
   // bool m_with_helics;
   ObjectFactory s_factory;
 };
 
 } // namespace ns3
 
 #endif /* TEST_PDC_HELPER_H */