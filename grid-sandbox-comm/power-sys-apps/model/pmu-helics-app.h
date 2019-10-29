/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef PMU_HELICS_APP_H
#define PMU_HELICS_APP_H

#include "pmu-app.h"
#include "ns3/helics.h"

namespace ns3 {

class PmuHelicsApp : public PmuApp {
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  PmuHelicsApp ();

  PmuHelicsApp (helics::Input *ptr);

  PmuHelicsApp (std::string key);

  virtual ~PmuHelicsApp();

protected:
  virtual void Send();

private:
  helics::Input *m_pInput = nullptr;
  std::string m_helics_key;

  void SetInput(std::string key);
};

}


#endif /* PMU_HELICS_APP_H */