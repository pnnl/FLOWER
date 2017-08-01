///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef PACKETBUILDER_HPP
#define PACKETBUILDER_HPP


// System Includes
#include <pcap.h>
#include <string>
#include <time.h>
// External Includes
#include <boost/date_time/posix_time/posix_time.hpp>
// Internal Includes
// Application Includes
#include "EventTypes.hpp"
#include "ItemCounter.hpp"
#include "Packet.hpp"
#include "Stats.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "PacketBuilder"


// Namespaces
using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;


//==============================================================================
// Class PacketBuilder
//
// API Developer Documentation:   (those who develop/fix this class)
//   [put documentation here for developers that work on this code]
//------------------------------------------------------------------------------
// API User Documentation:  (those who use this class)
//
//   [put a single summary sentence describing this class here]
//
// SYNOPSIS:
//   [put an overview paragraph for this class here]
//
// DESCRIPTION:
//   [put a description section for this class here]
//
// FUTURE FEATURES:
//  None discovered
//
// KNOWN LIMITIATIONS:
//  None discovered
//
// SEE ALSO:
//============================================================================== 


class PacketBuilder
{

public:

  // Getters/Setters

  inline ItemCounter & getPacketCounter(void) throw()
  {
    return(this->packet_counter);
  }


  // Constructors

  PacketBuilder(NewPcapDescEvent * const p_new_pcap_desc_event, PacketAddEvent * const p_packet_add_event, int unsigned const p_max_packets, int unsigned const p_snaplen) throw();


  // Destructor

  ~PacketBuilder(void) throw()
  {
    return;
  }


  // Public Functions

  bool   initDevice(string const & p_device) throw();
  bool   initFile(string const & p_file) throw();
  bool   readDevice(void) throw();
  bool   readFile(void) throw();
  void   onIdleSystemEvent(int unsigned const p_timeout) throw();
  string onMetricsEvent(int unsigned const p_level) throw();
  void   onShutdownSystemEvent(int unsigned const p_level) throw();
  void   finish(void) throw();
  void   pcapProcessPacket(struct pcap_pkthdr const * const p_header, char unsigned const * const p_data) throw();


private:

  // Getters/Setters

  inline int unsigned getMaxPackets(void) const throw()
  {
    return(this->max_packets);
  }


  inline NewPcapDescEvent * getNewPcapDescEvent(void) const throw()
  {
    return(this->new_pcap_desc_event);
  }


  inline sharedPacket & getPacket(void) throw()
  {
    return(this->packet);
  }


  inline PacketAddEvent * getPacketAddEvent(void) const throw()
  {
    return(this->packet_add_event);
  }


  inline pcap_t * getPcapDescriptor(void) const throw()
  {
    return(this->pcap_descriptor);
  }


  inline int unsigned getSnaplen(void) const throw()
  {
    return(this->snaplen);
  }


  inline time_t getVirtualTime(void) throw()
  {
    return (this->virtual_time);
  }


  inline void incrementTime(int unsigned const p_timeout) throw()
  {
    // DEVELOPER NOTE: 2008/07/16: Bill Nickless says this formula is correct
    this->virtual_time += p_timeout - 1;
    return;
  }


  inline bool isPcapDescriptorInitialized(void) throw()
  {
    return(this->pcap_descriptor_initialized);
  }


  inline bool isRunning(void) const throw()
  {
    return(this->running);
  }


  inline void setPcapDescriptor(pcap_t * p_descriptor) throw()
  {
    this->pcap_descriptor = p_descriptor;
    getNewPcapDescEvent()->call(p_descriptor);
    pcap_descriptor_initialized = true;
    return;
  }


  inline void setRunning(bool const & p_value) throw()
  {
    this->running = p_value;
    return;
  }


  inline void setVirtualTime(time_t const p_time) throw()
  {
     this->virtual_time = p_time;
     return;
  }


  // Constructors

  PacketBuilder(void) throw();
  PacketBuilder(PacketBuilder const & p_packet_builder) throw();


  // Operators

  PacketBuilder &  operator=(PacketBuilder const & p_packet_builder) throw();
  bool            operator==(PacketBuilder const & p_packet_builder) const throw();
  bool            operator!=(PacketBuilder const & p_packet_builder) const throw();
  

  // Variables

  pcap_t *           pcap_descriptor;
  NewPcapDescEvent * new_pcap_desc_event;
  PacketAddEvent *   packet_add_event;
  sharedPacket       packet;
  ItemCounter        packet_counter;
  time_t             virtual_time;
  int unsigned       max_packets;
  int unsigned       snaplen;
  bool               pcap_descriptor_initialized;
  bool               running;

};

#endif // PACKETBUILDER_HPP
