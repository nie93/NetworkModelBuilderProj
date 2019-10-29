/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "helics.h"

namespace ns3 {

std::shared_ptr<helics::CombinationFederate> helics_federate;
helics::Endpoint helics_endpoint;

std::ostream& operator << (std::ostream& stream, const helics::Message &message)
{
  stream << "Message(" << message.time
      << ", " << message.flags
      << ", " << message.data.size() << " bytes"
      << ", " << message.source
      << ", " << message.dest
      << ", " << message.original_source
      << ", " << message.original_dest
      << ")";
  return stream;
}

std::ostream& operator << (std::ostream& stream, std::unique_ptr<helics::Message> message)
{
  stream << "Message(" << message->time
      << ", " << message->flags
      << ", " << message->data.size() << " bytes"
      << ", " << message->source
      << ", " << message->dest
      << ", " << message->original_source
      << ", " << message->original_dest
      << ")";
  return stream;
}

}

