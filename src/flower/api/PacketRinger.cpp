///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////



#ifdef __linux__
// System Includes
#include <algorithm>
#include <arpa/inet.h>  // to get htons()
#include <errno.h>
#include <iostream>
#include <linux/types.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <pcap.h>
#include <poll.h>
#include <string.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h> // getpagesize() and usleep()
#endif
// External Includes
// Internal Includes
// Application Includes
#include "PacketRinger.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "PacketRinger"


// Namespaces used
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

void noOpPacketRingerDeallocator(Packet * p)
{
  // DEVELOPER NOTE: May be a bug in Boost 1.35-0 shared_ptr that causes an
  //                 extra free() call to be made which triggers a Segmentation
  //                 fault. This "NO OP" function fakes out the destructor
  //                 so that the SegFault does not happen.
  //                 May need to try a newer version of Boost
  //cout << "Deallocating PacketRinger's sharedPacket p:\n" << hex << &(*p) << dec << endl;
}


// ======================================================================
// Constructors
// ======================================================================

#ifdef __linux__
PacketRinger::PacketRinger(OutputHelper & p_output_helper, PacketAddEvent * const p_packet_add_event, int unsigned const p_timeout, int unsigned const p_max_packets, bool const p_buffer_packets, int unsigned const p_max_packetbuffer_size) throw() :
  buffer_packets(p_buffer_packets),
  buffer_cnt(p_max_packetbuffer_size),
  output_helper(p_output_helper),
  packet_add_event(p_packet_add_event),
  packet(new Packet(::g_REAL), noOpPacketRingerDeallocator),
  fake_packet(new Packet(::g_FAKE), noOpPacketRingerDeallocator),
  max_packets(p_max_packets),
  timeout(p_timeout),
  heartbeat_count(0),
  tp_frame_size(2048),                         // 2048 byte frames
  tp_block_size(2*tp_frame_size),              // frames per 4K block
  tp_block_nr((64*1024*1024) / tp_block_size), // 64 Mbytes, 30451 Packets MAX
  tp_frame_nr((tp_block_nr * tp_block_size) / tp_frame_size),
  packet_count("packet_stats"),
  running(false)
{
  DEBUG(TRACE, ENTER);
  DEBUG(TRACE, LEAVE);
  return;
}
#else
PacketRinger::PacketRinger(OutputHelper & p_output_helper, PacketAddEvent * const p_packet_add_event, int unsigned const p_timeout, int unsigned const p_max_packets, bool const p_buffer_packets, int unsigned const p_max_packetbuffer_size) throw() :
  buffer_packets(false),
  buffer_cnt(0),
  output_helper(p_output_helper),
  packet_add_event(p_packet_add_event),
  packet(new Packet(::g_REAL), noOpPacketRingerDeallocator),
  fake_packet(new Packet(::g_FAKE), noOpPacketRingerDeallocator),
  max_packets(p_max_packets),
  timeout(p_timeout),
  heartbeat_count(0),
  tp_frame_size(0),
  tp_block_size(0),
  tp_block_nr(0),
  tp_frame_nr(0),
  packet_count("packet_stats"),
  running(false)
{

}
#endif

// ======================================================================
// Public Functions
// ======================================================================


bool PacketRinger::initDevice(string const & p_device) throw()
{
#ifdef __linux__
  DEBUG(TRACE, ENTER);
  int    errno_save;
  string err_message;

  // DEVELOPER NOTE: This code was inspired by sites like:
  // http://www.mjmwired.net/kernel/Documentation/networking/packet_mmap.txt
  // http://wiki.ipxwarzone.com/index.php5?title=Linux_packet_mmap

  setInterfaceName(p_device);

  // Open a socket against that interface
  setSocket(socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)));
  errno_save = errno;

  if (0 > getSocket())   // Did the socket open OK?
  {
    err_message = "The kernel socket did not open correctly: " + static_cast<string>(strerror(errno_save));
    ERROR(TSNH, "Trying to acquire kernel socket", err_message.c_str());
    DEBUG(TRACE, LEAVE);
    return(false);
  }

  // Find the interface index
  struct ifreq req;
  strncpy(req.ifr_name, getInterfaceName().c_str(), IFNAMSIZ);
  int ioctl_result = ioctl(getSocket(), SIOCGIFINDEX, &req);
  errno_save = errno;

  if (0 > ioctl_result)
  {
    err_message = "The ioctl operation Failed: " + static_cast<string>(strerror(errno_save));
    ERROR(TSNH, "Trying to acquire interface index", err_message.c_str());
    DEBUG(TRACE, LEAVE);
    return(false);
  }

  // Bind to just the requested interface
  struct sockaddr_ll sall;
  sall.sll_family   = AF_PACKET;
  sall.sll_protocol = htons(ETH_P_ALL);
  sall.sll_ifindex  = req.ifr_ifindex;
  int bind_result   = bind(getSocket(), (struct sockaddr *)&sall, sizeof(struct sockaddr_ll));
  errno_save = errno;

  if (0 > bind_result)
  {
    err_message = "The bind operation Failed: " + static_cast<string>(strerror(errno_save));
    ERROR(TSNH, "Trying to bind to the network interface", err_message.c_str());
    DEBUG(TRACE, LEAVE);
    return(false);
  }

  if (! promiscuousMode(true))
  {
    return(false);
  }

  // Request a ring
  struct tpacket_req tpr;
  tpr.tp_frame_size  = getTpFrameSize();  // byte frames
  tpr.tp_block_size  = getTpBlockSize();  // frames per megabyte block
  tpr.tp_block_nr    = getTpBlockNr();    // number of blocks
  tpr.tp_frame_nr    = getTpFrameNr();
  size_t map_length  = tpr.tp_block_size * tpr.tp_block_nr;
  int sockopt_result = setsockopt(getSocket(), SOL_PACKET, PACKET_RX_RING, (void *)&tpr, sizeof(tpr));
  errno_save = errno;

  string message;
  message += "Linux PF_PACKET ring: FrameSize(";
  message += uitoa10(getTpFrameSize());
  message += "), BlockSize(";
  message += uitoa10(getTpBlockSize());
  message += "), BlockNum(";
  message += uitoa10(getTpBlockNr());
  message += "), FrameNum(";
  message += uitoa10(getTpFrameNr()) + ")";
  if (0 > sockopt_result)
  {
    err_message = "The setsockopt operation Failed: " + static_cast<string>(strerror(errno_save));
    ERROR(TSNH, message.c_str(), err_message.c_str());
    DEBUG(TRACE, LEAVE);
    return(false);
  }
  else
  {
    output(message);
  }

  setRing((u_int8_t *) mmap(0, map_length, PROT_READ|PROT_WRITE, MAP_SHARED, getSocket(), 0));

  if (! getRing())
  {
    ERROR(TSNH, "Trying to access the kernel packet ring", "Call to getRing() Failed");
    DEBUG(TRACE, LEAVE);
    return(false);
  }

  getPacketCounter().resetItem();
  setRunning(true);
  DEBUG(TRACE, LEAVE);
  return(true);
#else
  return(false);
#endif
}


bool PacketRinger::readDevice(void) throw()
{
#ifdef __linux__
  DEBUG(TRACE, ENTER);

  // DEVELOPER NOTE: The following references to TP or tp mean Turbo Packet

  if (! getRing())
  {
    ERROR(TSNH, "Trying to access the kernel packet ring", "Call to getRing() Failed");
    DEBUG(TRACE, LEAVE);
    return(false);
  }

  struct timeval tv;
  gettimeofday(&tv, NULL);

  time_t     next_heartbeat = tv.tv_sec        + getTimeout();
  u_int64_t  buffer_size    = getBufferCnt()   * getTpFrameSize();
  u_int32_t  ring_length    = getTpBlockSize() * getTpBlockNr();
  u_int8_t * idx            = getRing();

  struct timespec time_req;
  struct timespec time_rem;
  time_req.tv_sec  = 0;
  time_req.tv_nsec = 0;

  setBufferIdx(getRing());
  setRingEnd(getRing() + ring_length);

  getPacketCounter().setStartTime();
  while (0 == getMaxPackets() || getPacketCounter().getTotalItemCount() < getMaxPackets())
  {
    volatile struct tpacket_hdr * hp = (struct tpacket_hdr *) idx;

    if (TP_STATUS_KERNEL == hp->tp_status)
    {
      gettimeofday(&tv, NULL);
      next_heartbeat = tv.tv_sec + getTimeout();
      do
      {
        gettimeofday(&tv, NULL);
        if (tv.tv_sec > next_heartbeat)
        {
#ifndef NDEBUG
          CAUTION(FakeData, "PacketRinger", "Creating FAKE PACKET");
#else
          CAUTION(NotFound, ("No packets read for " + uitoa10(getTimeout()) + " seconds").c_str(), "Please check cables and connections.");
#endif
          getPacketAddEvent()->call(getFakePacket());
          next_heartbeat = tv.tv_sec + getTimeout();
          incrHeartbeatCount();
        }
        nanosleep(&time_req, &time_rem);
      } while (TP_STATUS_KERNEL == hp->tp_status);
    }

#if 0
    cout << " tp_status=" << hp->tp_status << " tp_len="  << hp->tp_len << " tp_snaplen=" << hp->tp_snaplen;
    cout << " tp_mac="    << hp->tp_mac    << " tp_net="  << hp->tp_net << " tp_sec="     << hp->tp_sec;
    cout << " tp_usec="   << hp->tp_usec   << "\n" << hex << "FRAME OFFSET:" << (idx - getRing()) << endl;
    hexdumper(cout, idx + hp->tp_mac, hp->tp_len);
#endif

    // DEVELOPER NOTE: Populate Packet with values and call Event
    setPacketData(idx + hp->tp_mac);
    getPacketHeader().ts.tv_sec  = hp->tp_sec;
    getPacketHeader().ts.tv_usec = hp->tp_usec;
    getPacketHeader().caplen     = hp->tp_snaplen;
    getPacketHeader().len        = hp->tp_len;

    getPacket()->assign(&(getPacketHeader()), getPacketData());
    getPacketAddEvent()->call(getPacket());
    getPacketCounter().incrItemStats();

    idx += getTpFrameSize();
    if (! bufferPackets())
    {
      hp->tp_status = TP_STATUS_KERNEL;
    }

    if (bufferPackets())
    {
      if (idx > getBufferIdx())
      {
        if (((u_int64_t)(idx - getBufferIdx())) > buffer_size)
        {
          struct tpacket_hdr * hp = (struct tpacket_hdr *) getBufferIdx();
          hp->tp_status = TP_STATUS_KERNEL;
          setBufferIdx(getBufferIdx() + getTpFrameSize());
        }
      }
      else
      {
        if (((u_int64_t)((getRingEnd() - getBufferIdx()) + (idx - getRing()))) > buffer_size)
        {
          struct tpacket_hdr * hp = (struct tpacket_hdr *) getBufferIdx();
          hp->tp_status = TP_STATUS_KERNEL;
          setBufferIdx(getBufferIdx() + getTpFrameSize());
        }
      }
      if (getBufferIdx() >= getRingEnd())
      {
        setBufferIdx(getRing());
      }
    }

    if (idx >= getRingEnd())
    {
      idx = getRing();
    }
  }
  getPacketCounter().setFinishTime();
   
  DEBUG(TRACE, LEAVE);
  return(true);
#else
  return(false);
#endif
}


void PacketRinger::writePacketsToFile(void) throw()
{
#ifdef __linux__
  DEBUG(TRACE, ENTER);
  struct pcap_file_header pcap_hdr;

  pcap_hdr.magic         = 0xa1b2c3d4;
  pcap_hdr.version_major = 0x02;
  pcap_hdr.version_minor = 0x04;
  pcap_hdr.thiszone      = 0;
  pcap_hdr.sigfigs       = 0;
  pcap_hdr.snaplen       = 0x60;
  pcap_hdr.linktype      = 0x1;

  string filename = getOutputHelper().getOutputTempFilename(getNow(), "pcap");
  ofstream output_file(filename.c_str(), ofstream::binary);
  output_file.write((const char *)&pcap_hdr, sizeof(pcap_hdr));

  for (u_int64_t i = 0; i < min(getBufferCnt(), getPacketCounter().getTotalItemCount()); ++i)
  {
    struct tpacket_hdr * hp = (struct tpacket_hdr *) getBufferIdx();
    setPacketData(getBufferIdx() + hp->tp_mac);
    getPacketHeader().ts.tv_sec      = hp->tp_sec;
    getPacketHeader().ts.tv_usec     = hp->tp_usec;
    getPacketHeader().caplen         = hp->tp_snaplen;
    getPacketHeader().len            = hp->tp_len;
    getPacket()->assign(&(getPacketHeader()), getPacketData());

    getPacket()->printPacketPcap(output_file);
    setBufferIdx(getBufferIdx() + getTpFrameSize());
    if (getBufferIdx() >= getRingEnd())
    {
      setBufferIdx(getRing());
    }
    hp->tp_status = TP_STATUS_KERNEL;
  }

  output_file.flush();
  output_file.close();
  getOutputHelper().setFileSettings(filename);
  getOutputHelper().renameTempFilename(filename);

  DEBUG(TRACE, LEAVE);
#endif
  return;
}


bool PacketRinger::promiscuousMode(bool p_on_or_off) throw()
{
  DEBUG(TRACE, ENTER);
#ifdef __linux__
  int    errno_save;
  string err_message;
  int    ioctl_result;
  struct ifreq req;

  strncpy(req.ifr_name, getInterfaceName().c_str(), IFNAMSIZ);
  ioctl_result = ioctl(getSocket(), SIOCGIFFLAGS, &req);
  errno_save = errno;

  if (0 > ioctl_result)
  {
    err_message = "The ioctl operation Failed: " + static_cast<string>(strerror(errno_save));
    ERROR(TSNH, "Trying to query interface flags", err_message.c_str());
    DEBUG(TRACE, LEAVE);
    return(false);
  }

  // Turn Promiscuous Mode On or Off
  if (p_on_or_off)
  {
    req.ifr_flags |=  IFF_PROMISC;
  }
  else
  {
    req.ifr_flags &= ~IFF_PROMISC;
  }

  ioctl_result = ioctl(getSocket(), SIOCSIFFLAGS, &req);
  errno_save = errno;

  if (0 > ioctl_result)
  {
    err_message = "The ioctl operation Failed: " + static_cast<string>(strerror(errno_save));
    ERROR(TSNH, "Trying to turn on promiscuous mode on interface", err_message.c_str());
    DEBUG(TRACE, LEAVE);
    return(false);
  }
  
  DEBUG(TRACE, LEAVE);
  return(true);
#else
  return(false);
#endif
}


void PacketRinger::resetCounters(void) throw()
{
  DEBUG(TRACE, ENTER);

  getPacketCounter().resetItem();
  resetHeartbeatCount();

  DEBUG(TRACE, LEAVE);
  return;
}


void PacketRinger::finish(void) throw()
{
  DEBUG(TRACE, ENTER);
#ifdef __linux__
  if (isRunning())
  {
    promiscuousMode(false);
    close(getSocket());
    setRing(NULL);
  }

  DEBUG(TRACE, LEAVE);
#endif
  return;
}


string PacketRinger::onMetricsEvent(int unsigned const p_level) throw()
{
  DEBUG(TRACE, ENTER);
  string result = "";
  if (isRunning())
  {
    getPacketCounter().setFinishTime();
    output("      Heartbeats sent:    " + uitoa10(getHeartbeatCount()));
    output("      Packets captured:   " + uitoa10(getPacketCounter().getItemCount()));
    output("      Processing time:    " + getPacketCounter().getItemProcessingTime());
    output("      Packets per second: " + getPacketCounter().getItemsPerSecond());
    result  = "PR";
    result += ":hbc#" + uitoa10(getHeartbeatCount());
    result += ":pc#"  + uitoa10(getPacketCounter().getItemCount());
    result += ":ppt#" + getPacketCounter().getItemProcessingTime();
    result += ":pps#" + getPacketCounter().getItemsPerSecond();
    resetCounters();
    getPacketCounter().setStartTime();
  }
  DEBUG(TRACE, LEAVE);
  return(result);
}


void PacketRinger::onShutdownSystemEvent(int unsigned const p_level) throw()
{
  DEBUG(TRACE, ENTER);

  if (isRunning())
  {
    if (SIGTERM == p_level)
    {
      finish();
    }
    else if (0 != p_level)
    {
      finish();
      if (bufferPackets())
      {
        writePacketsToFile();
      }
    }
  }

  DEBUG(TRACE, LEAVE);
  return;
}
