/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
 /*
  */
 
 #include "test-pdc-helper.h"
 #include "ns3/string.h"
 #include "ns3/inet-socket-address.h"
 #include "ns3/names.h"
 
 namespace ns3 {
 
 TestPdcHelper::TestPdcHelper (std::string protocol, Address address, Address pdcAddress)
 {
   m_factory.SetTypeId ("ns3::TestPdc");
   m_factory.Set ("Protocol", StringValue (protocol));
   m_factory.Set ("Local", AddressValue (address));
   //m_factory.Set("Remote", AddressValue (remoteAddress));
   //if (pdcAddress){
   m_factory.Set("Remote", AddressValue (pdcAddress));
   //}
 }
 
 void 
 TestPdcHelper::SetAttribute (std::string name, const AttributeValue &value)
 {
   m_factory.Set (name, value);
 }
 
 void 
 TestPdcHelper::SetPdcAttribute (std::string name, const AttributeValue &value)
 {
  s_factory.Set(name,value);
 }

 ApplicationContainer
 TestPdcHelper::Install (Ptr<Node> node) const
 {
   return ApplicationContainer (InstallPriv (node));
 }
 
 ApplicationContainer
 TestPdcHelper::Install (std::string nodeName) const
 {
   Ptr<Node> node = Names::Find<Node> (nodeName);
   return ApplicationContainer (InstallPriv (node));
 }
 
 ApplicationContainer
 TestPdcHelper::Install (NodeContainer c) const
 {
   ApplicationContainer apps;
   for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
     {
       apps.Add (InstallPriv (*i));
     }
 
   return apps;
 }
 
 Ptr<Application>
 TestPdcHelper::InstallPriv (Ptr<Node> node) const
 {
   Ptr<Application> app = m_factory.Create<Application> ();
   node->AddApplication (app);
 
   return app;
 }
 
 } // namespace ns3