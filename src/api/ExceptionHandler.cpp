///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// System Includes
// External Includes
// Internal Includes
// Application Includes
#include "osUtil.hpp"
#include "EventTypes.hpp"
#include "Exception.hpp"
#include "ExceptionHandler.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "ExceptionHandler"


// Namespaces


//==============================================================================
// Class ExceptionHandler
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


ShutdownSystemEvent * ExceptionHandler::SingletonHandler::shutdown_system_event = 0;


// ======================================================================
// Constructors
// ======================================================================


ExceptionHandler::ExceptionHandler(ShutdownSystemEvent * p_shutdown_system_event) noexcept(true)
{
  static SingletonHandler handler(p_shutdown_system_event);
  return;
}


ExceptionHandler::SingletonHandler::SingletonHandler(ShutdownSystemEvent * p_shutdown_system_event) noexcept(true)
{
  ExceptionHandler::SingletonHandler::shutdown_system_event = p_shutdown_system_event;
  set_terminate(SingletonHandler::Handler);
  return;
}


void ExceptionHandler::SingletonHandler::Handler(void)
{
  //  Exception from construction/destruction of global variables
  try
  {
    output("ERROR_MSG: We received a Signal that the software cannot handle. Aborting");
    throw; //  re-throw
  }
  catch (FloatingPointException & e)
  {
    ExceptionHandler::SingletonHandler::shutdown_system_event->call(SIGTERM);
    FATAL(FloatingPoint, "Float point error", "Shutting down");
  }
  catch (SigAbrt & e)
  {
    ExceptionHandler::SingletonHandler::shutdown_system_event->call(SIGABRT);
    FATAL(Signal, "SIGABRT", "Shutting down");
  }
  catch (SigSegv & e)
  {
    ExceptionHandler::SingletonHandler::shutdown_system_event->call(SIGSEGV);
    FATAL(Signal, "SIGSEGV", "Shutting down");
  }
  catch (SigTerm & e)
  {
    ExceptionHandler::SingletonHandler::shutdown_system_event->call(SIGTERM);
    FATAL(Signal, "SIGTERM", "Shutting down");
  }
  catch (SigInt & e)
  {
    ExceptionHandler::SingletonHandler::shutdown_system_event->call(SIGTERM);
    FATAL(Signal, "Control-C or SIGINT", "Shutting down");
  }
  catch (string & err_code)
  {
    // DEVELOPER NOTE: Currently all of our FLOWER ERROR_MSG macros throw a
    //                 string (e.g. error_code)
    ExceptionHandler::SingletonHandler::shutdown_system_event->call(SIGTERM);
    FATAL(Shutdown, err_code, "");
  }
  catch (...)
  {
    ExceptionHandler::SingletonHandler::shutdown_system_event->call(SIGTERM);
    FATAL(UnknownException, "Unknown Exception", "Shutting down");
  }


  return;

}
