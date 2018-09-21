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

  inline ItemCounter & getPacketCounter(void) noexcept(true)
  {
    return(this->packet_counter);
  }


  // Constructors

  PacketBuilder(NewPcapDescEvent * const p_new_pcap_desc_event, PacketAddEvent * const p_packet_add_event, int unsigned const p_max_packets, int unsigned const p_snaplen) noexcept(true);


  // Destructor

  ~PacketBuilder(void) noexcept(true)
  {
    return;
  }


  // Public Functions

  bool   initDevice(string const & p_device) noexcept(true);
  bool   initFile(string const & p_file) noexcept(true);
  bool   readDevice(void) noexcept(true);
  bool   readFile(void) noexcept(true);
  void   onIdleSystemEvent(int unsigned const p_timeout) noexcept(true);
  string onMetricsEvent(int unsigned const p_level) noexcept(true);
  void   onShutdownSystemEvent(int unsigned const p_level) noexcept(true);
  void   finish(void) noexcept(true);
  void   pcapProcessPacket(struct pcap_pkthdr const * const p_header, char unsigned const * const p_data) noexcept(true);


private:

  // Getters/Setters

  inline string getInterfaceName(void) noexcept(true)
  {
    return(this->interface_name);
  }


  inline int unsigned getMaxPackets(void) const noexcept(true)
  {
    return(this->max_packets);
  }


  inline NewPcapDescEvent * getNewPcapDescEvent(void) const noexcept(true)
  {
    return(this->new_pcap_desc_event);
  }


  inline sharedPacket & getPacket(void) noexcept(true)
  {
    return(this->packet);
  }


  inline PacketAddEvent * getPacketAddEvent(void) const noexcept(true)
  {
    return(this->packet_add_event);
  }


  inline pcap_t * getPcapDescriptor(void) const noexcept(true)
  {
    return(this->pcap_descriptor);
  }


  inline int unsigned getSnaplen(void) const noexcept(true)
  {
    return(this->snaplen);
  }


  inline time_t getVirtualTime(void) noexcept(true)
  {
    return (this->virtual_time);
  }


  inline void incrementTime(int unsigned const p_timeout) noexcept(true)
  {
    // DEVELOPER NOTE: 2008/07/16: Bill Nickless says this formula is correct
    this->virtual_time += p_timeout - 1;
    return;
  }


  inline bool isPcapDescriptorInitialized(void) noexcept(true)
  {
    return(this->pcap_descriptor_initialized);
  }


  inline bool isRunning(void) const noexcept(true)
  {
    return(this->running);
  }


  inline void setInterfaceName(string const & p_name) noexcept(true)
  {
    this->interface_name = p_name;
    return;
  }


  inline void setPcapDescriptor(pcap_t * p_descriptor) noexcept(true)
  {
    this->pcap_descriptor = p_descriptor;
    getNewPcapDescEvent()->call(p_descriptor);
    pcap_descriptor_initialized = true;
    return;
  }


  inline void setRunning(bool const & p_value) noexcept(true)
  {
    this->running = p_value;
    return;
  }


  inline void setVirtualTime(time_t const p_time) noexcept(true)
  {
     this->virtual_time = p_time;
     return;
  }


  // Constructors

  PacketBuilder(void) noexcept(true);
  PacketBuilder(PacketBuilder const & p_packet_builder) noexcept(true);


  // Operators

  PacketBuilder &  operator=(PacketBuilder const & p_packet_builder) noexcept(true);
  bool            operator==(PacketBuilder const & p_packet_builder) const noexcept(true);
  bool            operator!=(PacketBuilder const & p_packet_builder) const noexcept(true);
  

  // Variables

  string             interface_name;
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
