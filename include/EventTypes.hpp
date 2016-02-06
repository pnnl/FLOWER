///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  PNNL.  All Rights Reserved.
//
// THIS FILE INITIALLY CREATED WITH:
//     TEMPLATE NAME:  lang_cpp_hdr.template
//     COMMAND NAME:   gensrc
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


#ifndef EVENTTYPES_HPP
#define EVENTTYPES_HPP

#undef  BOOST_SP_DISABLE_THREADS
#define BOOST_SP_DISABLE_THREADS 1


// System Includes
// External Includes
#include <boost/shared_ptr.hpp>
#include <Diagnostic.hpp>
// Internal Includes
// Application Includes
#include "Exception.hpp"
#include "Event.hpp"
#include "Flow.hpp"
#include "Packet.hpp"


// Namespaces


typedef std::shared_ptr<Flow>       sharedFlow;
typedef std::shared_ptr<Packet>     sharedPacket;

typedef Event<void, sharedPacket>   PacketAddEvent;
typedef Event<void, sharedFlow>     FlowAddEvent;
typedef Event<void, sharedFlow>     SummaryAddEvent;

typedef Event<void, int unsigned>   IdleSystemEvent;
typedef Event<void, int unsigned>   NewInputEvent;
typedef Event<void, int unsigned>   NewOutputEvent;
typedef Event<void, pcap_t *>       NewPcapDescEvent;
typedef Event<void, int unsigned>   ShutdownSystemEvent;

typedef Event<string, int unsigned> MetricsEvent;


#endif // EVENTTYPES_HPP
