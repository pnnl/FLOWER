///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef PACKET_HPP
#define PACKET_HPP


// System Includes
#include "pcap.h"
#include <iostream>
#include <vector>
#include <signal.h>
#include <sys/types.h>
// External Includes
#include <Diagnostic.hpp>
// Internal Includes
// Application Includes
#include "FixedArray.hpp"

#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Packet"


// Namespaces used
using namespace std;
using namespace boost::posix_time;


//==============================================================================
// Class Packet
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


class Packet
{

public:

  // Getters/Setters

  inline const ByteArray & getData(void) const noexcept(true)
  {
    DEBUG(LOTS, ENTER);
    //if (isDebugLevelSet(INFO))
    //{
    //  stringstream ss;  // Only in debug mode
    //  ss << this->ptr_data;
    //  DEBUG(INFO, "Packet::getData():%s", ss.str().c_str());
    //}
    DEBUG(LOTS, LEAVE);
    return(this->ptr_data);
  }


  inline int unsigned getLength(void) const noexcept(true)
  {
    return(this->wirelen);
  }


  inline bool isFake(void) const noexcept(true)
  {
    return(getFake());
  }


  inline int unsigned getCapLength(void) const noexcept(true)
  {
    return(this->caplen);
  }


  inline int unsigned getHeaderSeconds(void) const noexcept(true)
  {
    return(this->tv_sec);
  }


  inline int unsigned getHeaderMicroSeconds(void) const noexcept(true)
  {
    return(this->tv_usec);
  }


  // Constructors

  Packet(bool const p_fake, ByteArray const p_buf = ByteArray(NULL, 0), int unsigned const p_caplen = 0 , int unsigned const p_len = 0, int unsigned const p_tv_sec = 0, int unsigned const p_tv_usec = 0) noexcept(true) ;
  Packet(Packet const & p_packet) noexcept(true);


  // Destructor

  ~Packet(void) noexcept(true)
  {
    return;
  }


  // Public Functions

  void      assign(struct pcap_pkthdr const * const p_header, char unsigned const * const p_data, bool const p_is_fake = ::g_REAL) noexcept(true);
  ostream & printPacket(ostream & p_os) const noexcept(true);
  void printPacketPcap(ostream & p_os) const noexcept(true);


  // Operators

friend
  ostream & operator<<(ostream & p_os, Packet const * p_packet) noexcept(true);
friend
  ostream & operator<<(ostream & p_os, Packet const & p_packet) noexcept(true);


private:

  // Constructors

  Packet(void) noexcept(true);


  // Getters/Setters

  inline bool getFake(void) const noexcept(true)
  {
    return(this->fake);
  }


  // Variables

  ByteArray      ptr_data;  // Points to libpcap data
  int unsigned   wirelen;
  bool           fake;
  int unsigned   tv_sec;
  int unsigned   tv_usec;
  int unsigned   caplen;

};

#endif // PACKET_HPP
