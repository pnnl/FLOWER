///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifdef _MSC_VER
#define R_OK 4
#endif


// System Includes
#include <iostream>
#include <iterator>
#include <sys/stat.h>
#include <vector>
// External Includes
#include <boost/lexical_cast.hpp>
#include <boost/date_time/local_time/local_time.hpp>
// Internal Includes
// Application Includes
#include "osUtil.hpp"
#include "PacketBuilder.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "PacketBuilder"


// Namespaces used
using namespace boost::local_time;
using namespace boost::gregorian;
using boost::lexical_cast;


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


// DEVELOPER NOTE: The following have to be static because they are used in the
//                 static function that is called from the static C lib_pcap
//                 function

void PacketBuilder_pcap_handler(u_char * user, struct pcap_pkthdr const * const h, u_char const * const bytes);


// ======================================================================
// Constructors
// ======================================================================


PacketBuilder::PacketBuilder(NewPcapDescEvent * const p_new_pcap_desc_event, PacketAddEvent * const p_packet_add_event, int unsigned const p_max_packets, int unsigned const p_snaplen) throw() :
  pcap_descriptor(NULL),
  new_pcap_desc_event(p_new_pcap_desc_event),
  packet_add_event(p_packet_add_event),
  packet(new Packet(::g_REAL)),
  packet_counter("packet_stats"),
  virtual_time(getEpoch()),
  max_packets(p_max_packets),
  snaplen(p_snaplen),
  pcap_descriptor_initialized(false),
  running(false)
{
  DEBUG(TRACE, ENTER);
  DEBUG(TRACE, LEAVE);
  return;
}


// ======================================================================
// Public Functions
// ======================================================================


bool PacketBuilder::initDevice(string const & p_device) throw()
{
  DEBUG(TRACE, ENTER);

  char         errbuf[PCAP_ERRBUF_SIZE];
  string       error_message;
  int unsigned read_timeout     = 500;
  int unsigned promiscuous_mode = 1;
  bool         result           = false;

  // Zero out the error buffer
  memset(errbuf, 0, sizeof(errbuf));

  getPacketCounter().resetItem();
  setPcapDescriptor(pcap_open_live(p_device.c_str(), getSnaplen(), promiscuous_mode, read_timeout, errbuf));

  error_message = "ERROR: pcap_open_live(" + p_device + "): " + errbuf;
  if (NULL == getPcapDescriptor())
  {
    if (NULL != strstr(errbuf, "not permitted"))
    {
      ERROR(PermissionDenied, p_device, error_message);
    }
    else if (NULL != strstr(errbuf, "No such device"))
    {
      ERROR(NotFound, p_device, error_message);
    }
  }
  else
  {
    // See if pcap returned a warning message
    if (0 != errbuf[0])
    {
      CAUTION(UnknownException, p_device, error_message);
    }
    else
    {
      result = true;
      setRunning(result);
    }
  }

  DEBUG(TRACE, LEAVE);
  return(result);
}


bool PacketBuilder::readDevice(void) throw()
{
  DEBUG(TRACE, ENTER);
  
  int          pcap_ret;
  bool         ok = false;

  if (NULL != getPcapDescriptor())
  {
    getPacketCounter().setStartTime();
    // DEVELOPER NOTE: Using a trampoline technique to pass this class 
    //                 instance into the global PacketBuilder_pcap_handler
    //                 funtion. That way, PacketBuilder_pcap_handler can
    //                 access our PcapProcessPacket member function.
    pcap_ret = pcap_loop(getPcapDescriptor(), getMaxPackets(), PacketBuilder_pcap_handler, (u_char *)this);
    ok = (0 > pcap_ret) ? false : true;
    getPacketCounter().setFinishTime();
  }
  else
  {
    CAUTION(IllegalValue, "Missing Pcap Descriptor", "Pcap Descriptor is NULL");
  }
   
  DEBUG(TRACE, LEAVE);
  return(ok);
}


bool PacketBuilder::initFile(string const & p_filename) throw()
{
  DEBUG(TRACE, ENTER);

  char        errbuf[PCAP_ERRBUF_SIZE];
  struct stat st;
  bool        result = false;

  // Zero out the error buffer
  memset(errbuf, 0, sizeof(errbuf));

  DEBUG(INFO, "Processing PCAP File:%s", p_filename.c_str());

  getPacketCounter().resetItem();
  if (-1 != stat(p_filename.c_str(), &st))
  {
#ifdef _MSC_VER
    if (0 != _access(p_filename.c_str(), R_OK))
#else
    if (0 != access(p_filename.c_str(), R_OK))
#endif
    {
      CAUTION(PermissionDenied, p_filename.c_str(), "Input file cannot be read");
    }
    else
    {
      setPcapDescriptor(pcap_open_offline(p_filename.c_str(), errbuf));

      if (NULL == getPcapDescriptor())
      {
        CAUTION(IllegalValue, p_filename.c_str(), "Input file is not a valid pcap file");
      }
      else
      {
        output("-------------------------------------------------------------------------------");
        output("File: " + p_filename);
        result = true;
        setRunning(result);
      }
    }
  }
  else
  {
    CAUTION(NotFound, p_filename.c_str(), "Input file not found");
  }

  DEBUG(TRACE, LEAVE);
  return(result);
}


bool PacketBuilder::readFile(void) throw()
{
  DEBUG(TRACE, ENTER);

  int         pcap_ret;
  bool        result = false;
  
  if (NULL != getPcapDescriptor())
  {
    getPacketCounter().setStartTime();
    // DEVELOPER NOTE: Using a trampoline technique to pass this class 
    //                 instance into the global PacketBuilder_pcap_handler
    //                 funtion. That way, PacketBuilder_pcap_handler can
    //                 access our PcapProcessPacket member function.
    pcap_ret = pcap_loop(getPcapDescriptor(), getMaxPackets(), PacketBuilder_pcap_handler, (u_char *)this);
    result   = (0 > pcap_ret) ? false : true;
    getPacketCounter().setFinishTime();
  }
  else
  {
    CAUTION(IllegalValue, "Missing Pcap Descriptor", "Pcap Descriptor is NULL");
  }

  DEBUG(TRACE, LEAVE);
  return(result);
}


void PacketBuilder::finish(void) throw()
{
  DEBUG(TRACE, ENTER);

  if (isRunning())
  {
    // close and free resources
    if (isPcapDescriptorInitialized())
    {
      pcap_close(getPcapDescriptor());
    }
  }

  DEBUG(TRACE, LEAVE);
  return;
}


string PacketBuilder::onMetricsEvent(int unsigned const p_level) throw()
{
  DEBUG(TRACE, ENTER);
  string result = "";
  if (isRunning())
  {
    getPacketCounter().setFinishTime(false);
    output("      Packets captured:   " + uitoa10(getPacketCounter().getItemCount()));
    output("      Processing time:    " + getPacketCounter().getItemProcessingTime());
    output("      Packets per second: " + getPacketCounter().getItemsPerSecond());
    result  = "PB";
    result += ":pc#"  + uitoa10(getPacketCounter().getItemCount());
    result += ":ppt#" + getPacketCounter().getItemProcessingTime();
    result += ":pps#" + getPacketCounter().getItemsPerSecond();
    getPacketCounter().resetItem();
    getPacketCounter().setStartTime();
  }
  DEBUG(TRACE, LEAVE);
  return(result);
}


void PacketBuilder::onShutdownSystemEvent(int unsigned const p_level) throw()
{
  DEBUG(TRACE, ENTER);

  if (isRunning())
  {
    if (0 != p_level)
    {
      finish();
    }
    outputSeparator();
    output("Total Packets captured:   " + uitoa10(getPacketCounter().getTotalItemCount()));
    output("Total Processing time:    " + getPacketCounter().getTotalItemProcessingTime());
    output("Total Packets per second: " + getPacketCounter().getTotalItemsPerSecond());
  }

  DEBUG(TRACE, LEAVE);
  return;
}


// ======================================================================
// Private Functions
// ======================================================================


void PacketBuilder::pcapProcessPacket(struct pcap_pkthdr const * const p_header, char unsigned const * const p_data) throw()
{
  DEBUG(TRACE, ENTER);

  setVirtualTime(p_header->ts.tv_sec);
  getPacket()->assign(p_header, p_data);
  getPacketAddEvent()->call(getPacket());
  getPacketCounter().incrItemStats();

  DEBUG(TRACE, LEAVE);
  return;
}


void PacketBuilder_pcap_handler(u_char * user, struct pcap_pkthdr const * const h, u_char const * const bytes)
{
  // DEVELOPER NOTE: Using a trampoline technique to pass this class 
  //                 instance into the global PacketBuilder_pcap_handler
  //                 funtion. That way, PacketBuilder_pcap_handler can
  //                 access our PcapProcessPacket member function.
  class PacketBuilder * mypb = (class PacketBuilder *) user;

  mypb->pcapProcessPacket(h, bytes);
}
