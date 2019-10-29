/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
 /*
  */
 
 #ifndef TEST_PDC_H
 #define TEST_PDC_H
 
 #include "ns3/application.h"
 #include "ns3/event-id.h"
 #include "ns3/ptr.h"
 #include "ns3/traced-callback.h"
 #include "ns3/address.h"
 #include "ns3/data-rate.h"
 #include "ns3/socket.h"
 #include <deque>
 
 namespace ns3 {
 
 class Address;
 class Socket;
 class Packet;
 
 class TestPdc : public Application 
 {
 public:
   static TypeId GetTypeId (void);
   TestPdc ();
 
   virtual ~TestPdc ();
 
   uint64_t GetTotalRx () const;
 
   Ptr<Socket> GetListeningSocket (void) const;
 
   std::list<Ptr<Socket> > GetAcceptedSockets (void) const;
  
   Ptr<Socket> GetSocket (void) const;

   /* 
   *  timeStampVector is a deque that contains deques of packets. 
   *  Each element of timeStampVector represents a unique TimeStamp deque of Packets. 
   */
   std::deque<std::deque<ns3::Ptr<Packet>>> timeStampVector;

 protected:
   /*
   *  Creating a new TimeStamp deque
   */
   std::deque<ns3::Ptr<Packet>> CreateNewTimeStampBuffer (void);
   
   /*
   *  Adding new received DataFrame Packet to the 
   *  matched TimeStamp deque
   */
   void addingDfToTimeStampBuffer (Ptr<Packet> newPacket);

   virtual void DoDispose (void);
   
   // Event handlers
   /**
   *  actually sending the packet
   */
   void SendPacket (Ptr<Packet> packet);

   /*
   *  Creates the packets by combining all PMU Data
   *  from the timeStampVector deque and calls SendPacket()
   *  for sending the Data to SuperPDC 
   */
   void Send (std::deque<ns3::Ptr<Packet>> innerDeque);
   

   //Data for now
   uint16_t m_pdcID;
   _Float32 m_freq = 60.0;
   uint16_t pdc_buffer;
   


 private:
   // inherited from Application base class.
   virtual void StartApplication (void);    // Called at time specified by Start
   virtual void StopApplication (void);     // Called at time specified by Stop
 
   void HandleRead (Ptr<Socket> socket);
   void HandleAccept (Ptr<Socket> socket, const Address& from);
   void HandlePeerClose (Ptr<Socket> socket);
   void HandlePeerError (Ptr<Socket> socket);
   void ReadyToSend();
 
   // In the case of TCP, each socket accept returns a new socket, so the 
   // listening socket is stored separately from the accepted sockets
   Ptr<Socket>     m_socket;       
   std::list<Ptr<Socket> > m_socketList; 
 
   Address         m_local;        
   uint64_t        m_totalRx;      
   TypeId          m_tid;
   int             pmu_count; 

   //Added For sending compiled pdc packets
   Ptr<Socket>     s_socket;       //!< Associated socket
   Address         s_peer;         //!< Peer address
   bool            s_connected;    //!< True if connected
   double          s_samplingRate; //!< Sampling rate of the PMU in Hz
   uint32_t        s_pktSize;      //!< Size of packets
   uint64_t        s_maxBytes;     //!< Limit total number of bytes sent
   uint64_t        s_totBytes;     //!< Total bytes sent so far
   EventId         s_sendEvent;    //!< Event id of pending "send packet" event
   TypeId          s_tid;          //!< Type of the socket used
   int64_t         s_delay;        //!< Delay in the PDC processing         
 
   TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;
 
   TracedCallback<Ptr<const Packet>, const Address &, const Address &> m_rxTraceWithAddresses;

   //Added for PDC sending properties
   /// Traced Callback: transmitted packets.
  TracedCallback<Ptr<const Packet> > m_txTrace;

  /// Callbacks for tracing the packet Tx events, includes source and destination addresses
  TracedCallback<Ptr<const Packet>, const Address &, const Address &> m_txTraceWithAddresses;


 private:
   /**
   *  brief Schedule the next packet transmission i.e. the delta_time 
   *  for which PDC will wait to receive data from the PMUs 
   */
   void ScheduleNextTx ();

   /**
   * \brief Handle a Connection Succeed event
   * \param socket the connected socket
   */
   void ConnectionSucceeded (Ptr<Socket> socket);
   /**
   * \brief Handle a Connection Failed event
   * \param socket the not connected socket
   */
   void ConnectionFailed (Ptr<Socket> socket);
 };
 
 } // namespace ns3
 
 #endif /* TEST_PDC_H */