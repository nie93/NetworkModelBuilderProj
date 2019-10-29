
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef HELICS_PUBLICATION_APP_H
#define HELICS_PUBLICATION_APP_H

#include "pmu-app.h"
#include "ns3/helics.h"
#include "ns3/packet-sink.h"

namespace ns3 {

class HelicsPublicationApp : public PacketSink {
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  HelicsPublicationApp ();

  HelicsPublicationApp (helics::Publication *ptr);

  HelicsPublicationApp (std::string key);

  virtual ~HelicsPublicationApp();

  void SetPublication(std::string key);

protected:
  // Inherited from PacketSink
  virtual void HandleRead (Ptr<Socket> socket);   

private:
  helics::Publication *m_pPub = nullptr;
  std::string m_helics_pub_key;

  Time t0, t1;
  double freq0, freq1;

};

}


#endif /* HELICS_PUBLICATION_APP_H */