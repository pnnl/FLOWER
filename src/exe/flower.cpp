///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
// _____________________________________________________________________________
//
// SYNOPSIS: 
//  [put a single sentence stating purpose of this file] 
//
// OPTIONS: 
//    [list options and their descriptions here] 
// 
// DESCRIPTION: 
//  [describe in detail the purpose and uses of this file] 
//
// EXAMPLES: 
//  [list examples of usage, or whatever is appropriate] 
//
// DESIGN: 
//  [describe in detail the design of this file] 
//
// ENVIRONMENT VARIABLES: 
//  [describe any environment variables used in this file] 
//
// LIMITATIONS: 
//  [list any known limitations/problems with this file] 
//
// SEE ALSO: 
//  [list any other relavent documentation] 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <signal.h>
// External Includes
#include <Diagnostic.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "version_info.hpp"
#include "EventTypes.hpp"
#include "Daemon.hpp"
#include "Exception.hpp"
#include "ExceptionHandler.hpp"
#include "ObjectPool.hpp"
#include "OutputHelper.hpp"
#include "PacketBuilder.hpp"
#include "PacketParser.hpp"
#include "PacketRinger.hpp"
#include "ProgramOptions.hpp"
#include "FlowCache.hpp"
#include "SummaryExporter.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "main"


// Namespaces used
using namespace std;


SignalTranslator<SigAbrt>                g_objSigAbrtTranslator;
SignalTranslator<SigSegv>                g_objSigSegvTranslator;
SignalTranslator<SigTerm>                g_objSigTermTranslator;
SignalTranslator<SigInt>                 g_objSigIntTranslator;
SignalTranslator<FloatingPointException> g_objFloatingPointExceptionTranslator;


int main(int argc, char ** argv)
{

//  DEVELOPER NOTE: Leave this commented code for optimizing Flow layout
/*
  printf("sizeof(FlowKey):                                    % 3u\n",sizeof(FlowKey));
  printf("sizeof(IpAddress_t):                                % 3u\n",sizeof(IpAddress_t));
  printf("sizeof(Elf_t):                                      % 3u\n",sizeof(Elf_t));
  printf("sizeof(PhysicalStruct):                             % 3u\n",sizeof(PhysicalStruct));
  printf("sizeof(Flow):                                       % 3u\n",sizeof(Flow));
  printf("\n");
  printf("offsetof(PhysicalStruct,ip_address):                % 3u\n",offsetof(PhysicalStruct,ip_address));
  printf("offsetof(PhysicalStruct,elf_flags):                 % 3u\n",offsetof(PhysicalStruct,elf_flags));
  printf("\n");
  printf("offsetof(Flow,flow_key):                            % 3u 1st cache line\n",offsetof(Flow,flow_key));
  printf("offsetof(Flow,physical_struct.ip_address):          % 3u 3rd cache line\n",offsetof(Flow,physical_struct.ip_address));
  printf("offsetof(Flow,physical_struct.time_duration):       % 3u 4th cache line\n",offsetof(Flow,physical_struct.time_duration));
  printf("offsetof(Flow,physical_struct.packet_count):        % 3u 5th cache line\n",offsetof(Flow,physical_struct.packet_count));
  printf("offsetof(Flow,physical_struct.tcp_last_seq):        % 3u 6th cache line\n",offsetof(Flow,physical_struct.tcp_last_seq));
  printf("offsetof(Flow,physical_struct.vlan_id)-8):          % 3u 7th cache line\n",offsetof(Flow,physical_struct.vlan_id)-8);
  //printf("offsetof(Flow,final_pad):                           % 3u\n",offsetof(Flow,final_pad));
*/


  // Create an instance of global error and debug handling.
  Diagnostic          diag(argc, argv, Exception::getErrorCodes());
  //DEBUG(TRACE, ENTER);

  // DEVELOPER NOTE: Update only when the data guide version changes
  //                 flr is the name of the application (flower) and
  //                 the ## is the data guide version
  //                 CHANGE THE VALUE IN global.cpp

  // Create the events so that we can pass them to the objects that need them.
  //
  NewInputEvent       new_input_event("NewInputEvent");
  NewPcapDescEvent    new_pcap_desc_event("NewPcapDescEvent");
  IdleSystemEvent     idle_system_event("IdleSystemEvent");
  PacketAddEvent      packet_add_event("PacketAddEvent");
  FlowAddEvent        flow_add_event("FlowAddEvent");
  SummaryAddEvent     summary_add_event("SummaryAddEvent");
  ShutdownSystemEvent shutdown_system_event("ShutdownSystemEvent");
  ExceptionHandler    exception_handler(&shutdown_system_event);

  MetricsEvent        packet_builder_metrics_event("PacketBuilderMetricsEvent");
  MetricsEvent        packet_ringer_metrics_event("PacketRingerMetricsEvent");
  MetricsEvent        packet_parser_metrics_event("PacketParserMetricsEvent");
  MetricsEvent        flow_cache_metrics_event("FlowCacheMetricsEvent");


  //DEBUG(LOTS, "Process the command line options");
  ProgramOptions      po;
  if (! po.checkOptions(argc, argv, getDataGuideVersion()))
  {
    exit(1);
  }

  // DEVELOPER NOTE: 2018/06/28
  //   The following is a backwards compatible hack to print IP addresses
  //   in the RFP standard     format
  //     IPv4 = 127.0.0.1
  //     IPv6 = 2001:0000:4136:e378:8000:63bf:3fff:fdd2
  //   or the CPP non-standard format
  //     IPv4 = 127000000001
  //     IPv6 = 200100004136E378800063BF3FFFFDD2
  //   This is accomplished by providing a function pointer to the format
  //   specified in the flower configuration file. The actual functions
  //   are in uchar2string.[ch]pp.
  asIpv4Ptr = asIpv4;
  asIpv6Ptr = asIpv6;
  if (po.getIpAddressFormat())
  {
    asIpv4Ptr = asIpv4CPP;
    asIpv6Ptr = asIpv6CPP;
  }

  // Create a helper object that knows how to produce files for other objects
  OutputHelper    output_helper(
                                   po.getOutputDataDir(),
                                   po.getOutputFileExt(),
                                   po.getOutputFileGroupId(),
                                   getSiteName(),
                                   getDataGuideVersion(),
                                   "."
                               );
  Daemon daemon(output_helper, "root");

  // Create the first default Flow to add to the Flow Object Pool
  setCacheForceout(po.getCacheForceout());
  setCacheTimeout(po.getCacheTimeout());
  FlowKey flow_key;
  flow_key.buildFlowKeys();
  Flow default_flow(::g_REAL, flow_key);
  ObjectPool<Flow> flow_pool((po.getMaxFlowcacheSize() * 2) + 2, default_flow);

  // Create a PacketRinger to interface with the Linux PF_PACKET mmap api
  // but don't turn it on yet
  PacketRinger    packet_ringer(
                                   output_helper,
                                   &packet_add_event,
                                   po.getCacheTimeout(),
                                   po.getPackets(),
                                   po.bufferPackets(),
                                   po.getMaxPacketbufferSize()
                               );

  // Create the PacketBuilder to interface with the lib_pcap pump
  // but don't turn it on yet
  PacketBuilder   packet_builder(
                                   &new_pcap_desc_event,
                                   &packet_add_event,
                                   po.getPackets(),
                                   po.getSnaplen()
                                );
  // Create the main object that will parse all the libpcap packets
  PacketParser    packet_parser(
                                   &flow_add_event,
                                   flow_pool,
                                   po.getCacheTimeout(),
                                   po.getCacheForceout(),
                                   po.skipIpv4Packets()
                               );
  // Create the cache for holding the network flows
  FlowCache       flow_cache(
                                   &summary_add_event,
                                   flow_pool,
                                   po.getMaxFlowcacheSize(),
                                   po.getMaxFlowcacheSize()
                            );

  vector<MetricsEvent *> metrics_events;
  metrics_events.push_back(&packet_builder_metrics_event);
  metrics_events.push_back(&packet_ringer_metrics_event);
  metrics_events.push_back(&packet_parser_metrics_event);
  metrics_events.push_back(&flow_cache_metrics_event);

  // Create the object for writing the network flows to the output files
  SummaryExporter summary_exporter(
                                   output_helper,
                                   metrics_events,
                                   po.getSummaryForceout(),
                                   po.getVersionRecord(getDataGuideVersion()),
                                   getCsvHeader(po.useCsvHeader()),
                                   po.suppressIpv4Output(),
                                   po.suppressMetricsOutput()
                                  );

  // DEVELOPER NOTE: Wire up all the objects using the delegate event pattern
  //                 Connect the delagates to the pipeline parts before turning
  //                 on the valve from lib_pcap
  //

  packet_builder_metrics_event += new MetricsEvent::New<PacketBuilder>(       "packet_builder_metrics_event",   &packet_builder,   &PacketBuilder::onMetricsEvent);
  packet_ringer_metrics_event  += new MetricsEvent::New<PacketRinger>(        "packet_ringer_metrics_event",    &packet_ringer,    &PacketRinger::onMetricsEvent);
  packet_parser_metrics_event  += new MetricsEvent::New<PacketParser>(        "packet_ringer_metrics_event",    &packet_parser,    &PacketParser::onMetricsEvent);
  flow_cache_metrics_event     += new MetricsEvent::New<FlowCache>(              "flow_cache_metrics_event",       &flow_cache,       &FlowCache::onMetricsEvent);

  packet_add_event      += new PacketAddEvent::New<PacketParser>(        "packet_parser",    &packet_parser,    &PacketParser::onAddEvent);

  flow_add_event        += new FlowAddEvent::New<FlowCache>(             "flow_cache",       &flow_cache,       &FlowCache::onAddEvent);

  new_input_event       += new NewInputEvent::New<FlowCache>(            "flow_cache",       &flow_cache,       &FlowCache::onNewInputEvent);
  new_input_event       += new NewInputEvent::New<SummaryExporter>(      "summary_exporter", &summary_exporter, &SummaryExporter::onNewInputEvent);

  shutdown_system_event += new ShutdownSystemEvent::New<PacketRinger>(   "packet_ringer",    &packet_ringer,    &PacketRinger::onShutdownSystemEvent);
  shutdown_system_event += new ShutdownSystemEvent::New<PacketBuilder>(  "packet_builder",   &packet_builder,   &PacketBuilder::onShutdownSystemEvent);
  shutdown_system_event += new ShutdownSystemEvent::New<PacketParser>(   "packet_parser",    &packet_parser,    &PacketParser::onShutdownSystemEvent);
  shutdown_system_event += new ShutdownSystemEvent::New<FlowCache>(      "flow_cache",       &flow_cache,       &FlowCache::onShutdownSystemEvent);
  shutdown_system_event += new ShutdownSystemEvent::New<SummaryExporter>("summary_exporter", &summary_exporter, &SummaryExporter::onShutdownSystemEvent);
  shutdown_system_event += new ShutdownSystemEvent::New<Daemon>(         "daemon",           &daemon,           &Daemon::onShutdownSystemEvent);

  summary_add_event     += new SummaryAddEvent::New<SummaryExporter>(    "summary_exporter", &summary_exporter, &SummaryExporter::onAddEvent);
/*
*/


  { // DEVELOPER NOTE: Don't remove - helps scope and clean up local variables
    //                 before onShutdownSystemEvent closes out everything
    //
    // Start filling the pipeline
    //
    setFileOrWireFlag(po.useDevice());

    if (po.useDevice())
    {
      if (isPriviledgedUser())
      {
        // We have priviledges so we need to run as a daemon process.
        if (! po.isInteractive())
        {
          daemon.start(po.getDevice());
        }
      }

      // PROCESS packets from a ring (if available and appropriate)
      if (isLinux() && po.useRing())
      {
        if (packet_ringer.initDevice(po.getDevice()))
        {
          packet_ringer.readDevice();
          packet_ringer.finish();
        }
        else
        {
          output("Error reading from device (" + po.getDevice() + ")");
        }
      }
      else
      {
        // PROCESS packets from a network interface
        if (packet_builder.initDevice(po.getDevice()))
        {
          packet_builder.readDevice();
          packet_builder.finish();
        }
        else
        {
          output("Error reading from device (" + po.getDevice() + ")");
        }
      }
    }
    else
    {
      // PROCESS packets from pcap data file(s)
      vector<string> files = po.getInputFiles();
      for (vector<string>::const_iterator itor = files.begin(); itor != files.end(); ++itor)
      {
        if (packet_builder.initFile(*itor))
        {
          packet_builder.readFile();
          packet_builder.finish();
          new_input_event.call(0);
        }
      }
    }
  }

  shutdown_system_event.call(0);

  //DEBUG(TRACE, LEAVE);
  return(0);
}
