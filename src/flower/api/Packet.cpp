///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <locale>
#include <pcap.h>
// External Includes
// Internal Includes
// Application Includes
#include "timeUtil.hpp"
#include "Packet.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Packet"


// Namespaces used


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


// ======================================================================
// Constructors
// ======================================================================


Packet::Packet(bool const p_fake, ByteArray const p_buf, int unsigned const p_caplen, int unsigned const p_len, int unsigned const p_tv_sec, int unsigned const p_tv_usec) noexcept(true) :
  ptr_data(p_buf),
  wirelen(p_len),
  fake(p_fake),
  tv_sec(p_tv_sec),
  tv_usec(p_tv_usec),
  caplen(p_caplen)
{
  //debugSetLevel(31);
  DEBUG(LOTS, ENTER);
  DEBUG(LOTS, "p_caplen:%d:", p_caplen);
  DEBUG(LOTS, "p_len:%d:", p_len);
  DEBUG(LOTS, LEAVE);
  //debugSetLevel(0);
  return;
}

 
// ======================================================================
// Public Functions
// ======================================================================


void Packet::assign(struct pcap_pkthdr const * const p_header, char unsigned const * const p_data, bool const p_is_fake) noexcept(true)
{
  DEBUG(LOTS, ENTER);

  // DEVELOPER NOTE: 2009/01/13:
  //   We are going to assume libpcap never gives us bad pointers

  //if (NULL == p_header)
  //{
  //  cout << "p_data is NULL:" << endl;
  //  ERROR_MSG(1);
  //}
  //
  //if (NULL == p_data)
  //{
  //  cout << "p_data is NULL:" << endl;
  //  ERROR_MSG(1);
  //}

  this->ptr_data.assign(p_data, p_header->caplen);
  this->caplen    = p_header->caplen;
  this->wirelen   = p_header->len;
  this->tv_sec    = p_header->ts.tv_sec;
  this->tv_usec   = p_header->ts.tv_usec;
  this->fake      = p_is_fake;

  DEBUG(LOTS, LEAVE);
  return;
}


ostream & Packet::printPacket(ostream & p_os) const noexcept(true)
{
  //DEBUG(LOTS, ENTER);
  stringstream          ss;  // Needed to print out packet

  ss << "Seconds:        " << getHeaderSeconds()      << ',' << endl;
  ss << "uSeconds:       " << getHeaderMicroSeconds() << ',' << endl;
  ss << this->ptr_data                                << ',' << endl;
  ss << "Capture length: " << getCapLength()          << ',' << endl;
  ss << "Wire length:    " << getLength()             << ',' << endl;
  ss << "isFake:         " << getFake() << endl;

  //DEBUG(LOTS, LEAVE);
  return(p_os << ss.str());
}


typedef struct pcaprec_hdr_s {
        u_int32_t ts_sec;         /* timestamp seconds */
        u_int32_t ts_usec;        /* timestamp microseconds */
        u_int32_t incl_len;       /* number of octets of packet saved in file */
        u_int32_t orig_len;       /* actual length of packet */
} pcaprec_hdr_t;


void Packet::printPacketPcap(ostream & p_os) const noexcept(true)
{
  //DEBUG(LOTS, ENTER);
  pcaprec_hdr_t pcaprec_hdr;

  pcaprec_hdr.ts_sec    = getHeaderSeconds();
  pcaprec_hdr.ts_usec   = getHeaderMicroSeconds();
  pcaprec_hdr.incl_len  = getCapLength();
  pcaprec_hdr.orig_len  = getLength();

  p_os.write((const char*)&pcaprec_hdr, sizeof(pcaprec_hdr));
  p_os.write((const char*)this->ptr_data.getBuffer(), this->ptr_data.getCount());

  //DEBUG(LOTS, LEAVE);
  return;
}


// -------------------------------------
// Friends
// -------------------------------------

ostream & operator<<(ostream & p_os, Packet const * const p_packet) noexcept(true)
{
  //DEBUG(LOTS, ENTER);
  //DEBUG(LOTS, LEAVE);
  return(p_packet->printPacket(p_os));
}
