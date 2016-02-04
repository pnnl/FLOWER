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


#ifndef EXCEPTIONHANDLER_HPP
#define EXCEPTIONHANDLER_HPP


// System Includes
#ifndef _MSC_VER
#include <cxxabi.h>
#include <execinfo.h>
#endif
#include <iostream>
#include <signal.h> 
#include <string>
// External Includes
// Internal Includes
// Application Includes


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "ExceptionHandler"


// Namespaces
using namespace std;


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
// SEE ALSO: http://www.ibm.com/developerworks/linux/library/l-cppexcep.html
//============================================================================== 


template <class T>
class SignalTranslator
{

private:

  class SingletonTranslator
  {

  public:

    // Constructors

    SingletonTranslator(void) throw()
    {
      signal(T::GetSignalNumber(), SignalHandler);
      return;
    }


    // Destructor

    ~SingletonTranslator(void) throw()
    {
      return;
    }

    
    // Public Functions

    static void SignalHandler(int p_signal)
    {
      throw(T());
    }

  private:

    // Constructors

    SingletonTranslator(SingletonTranslator const & p_singleton_translator);


    // Operators

    SingletonTranslator & operator=(SingletonTranslator const & p_singleton_translator);

  };


  // Constructors

  SignalTranslator(SignalTranslator const & p_signal_translator);


  // Operators

  SignalTranslator & operator=(SignalTranslator const & p_signal_translator);


public:

  // Constructor

  SignalTranslator(void) throw()
  {
    static SingletonTranslator translator;
    return;
  }


  // Destructor

  ~SignalTranslator(void) throw()
  {
    return;
  }

};


#ifndef _MSC_VER
class ExceptionTracer
{

public:

  ExceptionTracer(void) throw()
  {
    int unsigned const depth = 10;
    void *       addresses[depth];
    char **      strings;
    int          size = backtrace(addresses, depth);
    strings =    backtrace_symbols(addresses, size);

    cout << "Stack frames: " << size << endl;
    for (int i = 0; i < size; i++)
    {
      cout << "#" << i << ": " << addresses[i] << ":" << strings[i] << endl;
    }

    free(strings);
  }

};
#endif


class SigAbrt : public std::exception
{

private:

  string message;

public:

  explicit SigAbrt(const string & p_message = "SigAbrt") throw() :
    message(p_message)
  {
    return;
  }

  virtual ~SigAbrt() throw()
  {
    return;
  }

  virtual const char * what(void) const throw()
  {
    return(message.c_str());
  }

  static string GetSignalName(void) throw()
  {
    return("SIGABRT");
  }

  static int GetSignalNumber(void) throw()
  {
    return(SIGABRT);
  }

};


class SigSegv : public std::exception
{

private:

  string message;

public:

  explicit SigSegv(const string & p_message = "SigSegv") throw() :
    message(p_message)
  {
    return;
  }

  virtual ~SigSegv() throw()
  {
    return;
  }

  virtual const char * what(void) const throw()
  {
    return(message.c_str());
  }

  static string GetSignalName(void) throw()
  {
    return("SIGSEGV");
  }

  static int GetSignalNumber(void) throw()
  {
    return(SIGSEGV);
  }

};


// An example for SIGTERM
class SigTerm : public std::exception
{

private:

  string message;

public:

  explicit SigTerm(const string & p_message = "SIGTERM") throw() :
    message(p_message)
  {
    return;
  }

  virtual ~SigTerm() throw()
  {
    return;
  }

  virtual const char * what(void) const throw()
  {
    return(message.c_str());
  }

  static string GetSignalName(void) throw()
  {
    return("SIGTERM");
  }

  static int GetSignalNumber(void) throw()
  {
    return(SIGTERM);
  }

};


class FloatingPointException : public std::exception
{

private:

  string message;

public:

  explicit FloatingPointException(const string & p_message = "FloatingPointException") throw() :
    message(p_message)
  {
    return;
  }

  virtual ~FloatingPointException() throw()
  {
    return;
  }

  virtual const char * what(void) const throw()
  {
    return(message.c_str());
  }

  static string GetSignalName(void) throw()
  {
    return("SIGFPE");
  }

  static int GetSignalNumber(void) throw()
  {
    return(SIGFPE);
  }

};  


class SigInt : public std::exception
{

private:

  string message;

public:

  explicit SigInt(const string & p_message = "SigInt") throw() :
    message(p_message)
  {
    return;
  }

  virtual ~SigInt() throw()
  {
    return;
  }

  virtual const char * what(void) const throw()
  {
    return(message.c_str());
  }

  static string GetSignalName(void) throw()
  {
    return("SIGINT");
  }

  static int GetSignalNumber(void) throw()
  {
    return(SIGINT);
  }

};  


class ExceptionHandler
{

public:

  // Constructors

  ExceptionHandler(ShutdownSystemEvent * p_shutdown_system_event) throw();


  // Destructor

  ~ExceptionHandler(void) throw()
  {
    return;
  }


private:

  class SingletonHandler
  {

  public:

    // Constructors

    SingletonHandler(ShutdownSystemEvent * p_shutdown_system_event) throw();


    // Destructor

    ~SingletonHandler(void) throw()
    {
      return;
    }


    // Public Functions

    static void Handler(void);


    // Variables

    static ShutdownSystemEvent * shutdown_system_event;

  };

  // Constructors

  ExceptionHandler(void);


  // Operators

  ExceptionHandler & operator=(ExceptionHandler const & p_exception_handler);

};

#endif // EXCEPTIONHANDLER_HPP
