/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
 /*
  * Copyright (c) 2007,2008,2009 INRIA, UDCAST
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License version 2 as
  * published by the Free Software Foundation;
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  *
  * Author: Amine Ismail <amine.ismail@sophia.inria.fr>
  *                      <amine.ismail@udcast.com>
  *
  */
 
 #ifndef PDC_APP_H
 #define PDC_APP_H
 
 #include "ns3/application.h"
 #include "ns3/event-id.h"
 #include "ns3/ptr.h"
 #include "ns3/address.h"
 #include "ns3/traced-callback.h"
 #include "ns3/packet-loss-counter.h"
 
 namespace ns3 {
 class PdcApp : public Application
 {
 public:
   static TypeId GetTypeId (void);
   PdcApp ();
   virtual ~PdcApp ();
   uint32_t GetLost (void) const;
 
   uint64_t GetReceived (void) const;
 
   uint16_t GetPacketWindowSize () const;
 
   void SetPacketWindowSize (uint16_t size);
 protected:
   virtual void DoDispose (void);
 
 private:
 
   virtual void StartApplication (void);
   virtual void StopApplication (void);
 
   void HandleRead (Ptr<Socket> socket);
 
   uint16_t m_port; 
   Ptr<Socket> m_socket; 
   Ptr<Socket> m_socket6; 
   uint64_t m_received; 
   PacketLossCounter m_lossCounter; 
 
   TracedCallback<Ptr<const Packet> > m_rxTrace;
 
   TracedCallback<Ptr<const Packet>, const Address &, const Address &> m_rxTraceWithAddresses;
 
 };
 
 } // namespace ns3
 
 #endif /* PDC_APP_H */
