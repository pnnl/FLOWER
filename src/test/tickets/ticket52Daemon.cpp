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
#include "Daemon.hpp"
#include "EventTypes.hpp"
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

  // Create an instance of global error and debug handling.
  Diagnostic          diag(argc, argv, Exception::getErrorCodes());


  // DEVELOPER NOTE: Update only when the data guide version changes
  //                 flr is the name of the application (flower) and
  //                 the ## is the data guide version
  //                 CHANGE THE VALUE IN global.cpp

  ShutdownSystemEvent shutdown_system_event("ShutdownSystemEvent");
  ExceptionHandler    exception_handler(&shutdown_system_event);

  // Create a helper object that knows how to produce files for other objects
  OutputHelper    output_helper("/data/" APP_NAME, "dat", 0, "test_ticket52", getDataGuideVersion(), ".");

  Daemon daemon(output_helper, "root");

  shutdown_system_event += new ShutdownSystemEvent::New<Daemon>("daemon", &daemon, &Daemon::onShutdownSystemEvent);


  {

    setFileOrWireFlag(true);

    if (isPriviledgedUser())
    {
      setOutputLocation(::g_OUTPUT2LOG, argv[0]);
      // We have priviledges so we need to run as a daemon process.
      daemon.start("ticket52");

      output("START TEST\n");
      sleep(3);
      output("TEST OUTPUT 3...\n");
      sleep(3);
      output("TEST OUTPUT 2...\n");
      sleep(3);
      output("TEST OUTPUT 1...\n");
      sleep(3);
      output("STOP TEST\n");
    }
    else
    {
      ERROR(PermissionDenied, "Trying to start Daemon", "Must be root or priviledged user");
      return(1);
    }

  }

  shutdown_system_event.call(0);
  return(0);
}
