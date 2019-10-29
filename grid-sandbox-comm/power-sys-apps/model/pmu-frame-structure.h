/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef PMU_FRAME_STRUCTURE_H_
#define PMU_FRAME_STRUCTURE_H_

#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/header.h"
#include "ns3/nstime.h"
#include <iostream>

using namespace ns3;

/**
 * \ingroup power-sys-apps
 * A Header implementation for the custom PMU frame structure for initial testing
 */
class TestPmuFrame : public Header
{
public:
  TestPmuFrame ();
  TestPmuFrame (uint16_t pmuID, const Time & t, _Float32 freq);
  virtual ~TestPmuFrame ();

  /**
   * Set the header data.
   * \param pmuID The PMU ID.
   * \param freq The frequency measurement.
   */
  void SetData (uint16_t pmuID, const Time & t, _Float32 freq);
  /**
   * Get the header data.
   * \return The data.
   */
  uint16_t GetPmuID (void) const;
  _Float32 GetFreq (void) const;
  Time GetTimeStamp(void) const;

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual uint32_t GetSerializedSize (void) const;

private:
  uint16_t m_pmuID; //!< PMU ID
  _Float32 m_freq; //!< Frequency measurement
  Time m_timestamp; //!< Time stamp of the measurements
  const static uint32_t TIME_BASE = 0xffffff;
};


#endif /* PMU_FRAME_STRUCTURE_H_ */