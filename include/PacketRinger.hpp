///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef PACKETRINGER_HPP
#define PACKETRINGER_HPP

// System Includes
#include <string>
// External Includes
// Internal Includes
// Application Includes
#include "EventTypes.hpp"
#include "ItemCounter.hpp"
#include "OutputHelper.hpp"
#include "Packet.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "PacketRinger"


// Namespaces
using namespace std;


//==============================================================================
// Class PacketRinger
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


class PacketRinger
{

public:

  // Getters/Setters

  inline int unsigned getHeartbeatCount(void) const throw()
  {
    return(this->heartbeat_count);
  }


  inline ItemCounter & getPacketCounter(void) throw()
  {
    return(this->packet_count);
  }


  sharedPacket & getFakePacket(void) throw()
  {
    static struct timeval tv;
#ifndef _MSC_VER
    gettimeofday(&tv, NULL);
#endif
    getPacketHeader().ts.tv_sec  = tv.tv_sec;
    getPacketHeader().ts.tv_usec = tv.tv_usec;
    getPacketHeader().caplen     = 0;
    getPacketHeader().len        = 0;
    this->fake_packet->assign(&(getPacketHeader()), getPacketData(), ::g_FAKE);

    return(this->fake_packet);
  }


  // Constructors

  PacketRinger(OutputHelper & p_output_helper, PacketAddEvent * const p_packet_add_event, int unsigned const p_timeout, int unsigned const p_max_packets, bool const p_buffer_packets, int unsigned const p_max_packetbuffer_size) throw();


  // Destructor

  ~PacketRinger(void) throw()
  {
    return;
  }


  // Public Functions

  static bool checkDevice(string const & p_device, struct ifreq * p_req = NULL) throw();

  void   finish() throw();
  bool   initDevice(string const & p_device) throw();
  string onMetricsEvent(int unsigned const p_level) throw();
  void   onShutdownSystemEvent(int unsigned const p_level) throw();
  bool   readDevice(void) throw();
  void   writePacketsToFile(void) throw();


private:

  // Getters/Setters

  inline bool bufferPackets(void) const throw()
  {
    return(this->buffer_packets);
  }


  inline u_int64_t getBufferCnt(void) const throw()
  {
    return(this->buffer_cnt);
  }


  inline u_int8_t * getBufferIdx(void) const throw()
  {
    return(this->buffer_idx);
  }


  inline int getSocket(void) const throw()
  {
    return(this->network_socket);
  }


  inline string getInterfaceName(void) const throw()
  {
    return(this->interface_name);
  }


  inline int unsigned getMaxPackets(void) const throw()
  {
    return(this->max_packets);
  }


  inline OutputHelper & getOutputHelper(void) const throw()
  {
    return(this->output_helper);
  }


  inline sharedPacket & getPacket(void) throw()
  {
    return(this->packet);
  }


  inline PacketAddEvent * getPacketAddEvent(void) const throw()
  {
    return(this->packet_add_event);
  }


  inline char unsigned * getPacketData(void) throw()
  {
    return(this->packet_data);
  }


  inline struct pcap_pkthdr & getPacketHeader(void) throw()
  {
    return(this->packet_header);
  }


  inline u_int8_t * getRing(void) const throw()
  {
    return(this->ring);
  }


  inline u_int8_t * getRingEnd(void) const throw()
  {
    return(this->ring_end);
  }


  inline int unsigned getTimeout(void) const throw()
  {
    return(this->timeout);
  }


  inline int unsigned getTpFrameSize(void) const throw()
  {
    return(this->tp_frame_size);
  }


  inline int unsigned getTpFrameNr(void) const throw()
  {
    return(this->tp_frame_nr);
  }


  inline int unsigned getTpBlockSize(void) const throw()
  {
    return(this->tp_block_size);
  }


  inline int unsigned getTpBlockNr(void) const throw()
  {
    return(this->tp_block_nr);
  }


  inline void incrHeartbeatCount(void) throw()
  {
    ++this->heartbeat_count;
    return;
  }


  inline bool isRunning(void) const throw()
  {
    return(this->running);
  }


  inline void setBufferIdx(u_int8_t * p_buffer_idx) throw()
  {
    this->buffer_idx = p_buffer_idx;
    return;
  }


  inline void resetHeartbeatCount(void) throw()
  {
    this->heartbeat_count = 0;
    return;
  }


  inline void setInterfaceName(string const & p_name) throw()
  {
    this->interface_name = p_name;
    return;
  }


  inline void setPacketData(char unsigned * p_value) throw()
  {
    this->packet_data = p_value;
    return;
  }


  inline void setRing(u_int8_t * p_value) throw()
  {
    this->ring = p_value;
    return;
  }


  inline void setRingEnd(u_int8_t * p_value) throw()
  {
    this->ring_end = p_value;
    return;
  }


  inline void setRunning(bool const & p_value) throw()
  {
    this->running = p_value;
    return;
  }


  inline void setSocket(int const p_value) throw()
  {
    this->network_socket = p_value;
    return;
  }


  // Constructors

  PacketRinger(void) throw();
  PacketRinger(PacketRinger const & p_packet_ringer) throw();


  // Operators

  PacketRinger &   operator=(PacketRinger const & p_packet_ringer) throw();
  bool            operator==(PacketRinger const & p_packet_ringer) const throw();
  bool            operator!=(PacketRinger const & p_packet_ringer) const throw();
  

  // Functions

  void resetCounters(void) throw();
  bool promiscuousMode(bool p_on_or_off) throw();


  // Variables

  string             interface_name;
  bool               buffer_packets;
  u_int64_t          buffer_cnt;
  u_int8_t *         buffer_idx;
  u_int8_t *         ring;
  u_int8_t *         ring_end;
  char unsigned *    packet_data;
  struct pcap_pkthdr packet_header;
  OutputHelper &     output_helper;
  PacketAddEvent *   packet_add_event;
  sharedPacket       packet;
  sharedPacket       fake_packet;
  int unsigned       max_packets;
  int unsigned       timeout;
  int unsigned       heartbeat_count;
  int unsigned const tp_frame_size;
  int unsigned const tp_block_size;
  int unsigned const tp_block_nr;
  int unsigned const tp_frame_nr;
  int                network_socket;
  ItemCounter        packet_count;
  bool               running;

};

#endif // PACKETRINGER_HPP
