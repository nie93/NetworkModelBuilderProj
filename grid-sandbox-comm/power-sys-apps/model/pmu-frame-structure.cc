
#include "pmu-frame-structure.h"
#include "math.h"

TestPmuFrame::TestPmuFrame ()
{
  // we must provide a public default constructor,
  // implicit or explicit, but never private.
}

TestPmuFrame::TestPmuFrame (uint16_t pmuID, const Time &t, _Float32 freq)
    : m_pmuID (pmuID), m_timestamp (t), m_freq (freq)
{
}

TestPmuFrame::~TestPmuFrame ()
{
}

TypeId
TestPmuFrame::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::TestPmuFrame").SetParent<Header> ().AddConstructor<TestPmuFrame> ();
  return tid;
}
TypeId
TestPmuFrame::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
TestPmuFrame::Print (std::ostream &os) const
{
  // This method is invoked by the packet printing
  // routines to print the content of my header.
  //os << "data=" << m_data << std::endl;
  os << "PMU ID: " << m_pmuID << ", freq = " << m_freq;
}
uint32_t
TestPmuFrame::GetSerializedSize (void) const
{
  // we reserve 2 bytes for our header.
  return sizeof (m_pmuID) + 2 * sizeof(uint32_t) + sizeof (m_freq);
}
void
TestPmuFrame::Serialize (Buffer::Iterator start) const
{
  // we can serialize two bytes at the start of the buffer.
  // we write them in network byte order.
  uint32_t SOC, FRACSEC;
  double dSOC;
  FRACSEC = (uint32_t) (modf(m_timestamp.GetSeconds(), &dSOC) * TIME_BASE);
  SOC = (uint32_t) dSOC;
  FRACSEC &= 0xffffff;
  start.WriteU16 (m_pmuID);
  start.WriteU32(SOC);
  start.WriteU32(FRACSEC);
  start.Write ((uint8_t *) &m_freq, sizeof (m_freq));
}
uint32_t
TestPmuFrame::Deserialize (Buffer::Iterator start)
{
  // we can deserialize two bytes from the start of the buffer.
  // we read them in network byte order and store them
  // in host byte order.
  m_pmuID = start.ReadU16 ();
  uint32_t SOC = start.ReadU32();
  uint32_t FRACSEC = start.ReadU32();
  m_timestamp = Time::FromDouble(SOC + (double) FRACSEC / TIME_BASE, Time::S);
  start.Read ((uint8_t *) &m_freq, sizeof (m_freq));

  // we return the number of bytes effectively read.
  return sizeof (m_pmuID) + 2 * sizeof(uint32_t) + sizeof (m_freq);
}

void
TestPmuFrame::SetData (uint16_t pmuID, const Time &t, _Float32 freq)
{
  m_pmuID = pmuID;
  m_timestamp = t;
  m_freq = freq;
}

uint16_t
TestPmuFrame::GetPmuID (void) const
{
  return m_pmuID;
}

_Float32
TestPmuFrame::GetFreq (void) const
{
  return m_freq;
}

Time TestPmuFrame::GetTimeStamp(void) const
{
  return m_timestamp;
}