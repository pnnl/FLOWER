///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  PNNL.  All Rights Reserved. 
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_hdr.template 
//     COMMAND NAME:   gensrc 
//
// REPOSITORY INFORMATION: 
//  $Revision: 1.6 $ 
//  $Author: younkin $ 
//  $Date: 2008-06-11 20:34:38 $ 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
//  
///////////////////////////////////////////////////////////////////////////////
#ifndef DIAGNOSTIC_HPP
#define DIAGNOSTIC_HPP

// System Includes
#ifndef _MSC_VER
#include <syslog.h>
#else
#define LOG_DAEMON 0
#define LOG_PID    0
#endif
// External Includes
// Internal Includes
// Application Includes
// System Includes
#include <map>
#include <string>
// External Includes
// Internal Includes
// Application Includes


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Diagnostic"


// Namespaces
using namespace std;


#ifdef _MSC_VER
  #define __MY_FUNC__ __FUNCTION__
#else
  #define __MY_FUNC__ __func__
#endif


///////////////////////////////////////////////////////////////////////////////
// DEVELOPER NOTE:
//   Keep this macro as a one-line function call so that it can be returned or
//   thrown directly.  For example:
//     throw error(...)
///////////////////////////////////////////////////////////////////////////////
/*
#undef  CAUTION
#define CAUTION(err_code, item, context) \
  Diagnostic::caution(err_code,item,context,__CLASS__,__MY_FUNC__,__FILE__,__LINE__)

#undef  ERROR
#define ERROR(err_code, item, context) \
  Diagnostic::error(err_code,item,context,__CLASS__,__MY_FUNC__,__FILE__,__LINE__)

#undef  FATAL
#define FATAL(err_code, item, context) \
  Diagnostic::fatal(err_code,item,context,__CLASS__,__MY_FUNC__,__FILE__,__LINE__)

#undef  SYSLOG
#define SYSLOG(prog) \
  debugSyslog(prog)
*/


//==============================================================================
// Class Diagnostic
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


class Diagnostic
{

public:

  // Get/Set Pairs

  inline int     getArgc(void) const { return(this->my_argc); }
  inline char ** getArgv(void) const { return(this->my_argv); }


  // Constructors

  Diagnostic(void);
  Diagnostic(int & p_argc, char ** p_argv, int const & p_option = LOG_PID, int const & p_facility = LOG_DAEMON);
  Diagnostic(int & p_argc, char ** p_argv, map<string, string> * p_error_codes, int const & p_option = LOG_PID, int const & p_facility = LOG_DAEMON);


  // Destructor

  ~Diagnostic(void);


  // Operators

  // Other Public Functions

  // Static Functions

/*
  inline static void breakpoint(
       string const &     p_message,
       const char * const p_class,
       const char * const p_func,
       char * const       p_file,
       int const          p_line
     )
  {
    ::breakpoint((char *)p_message.c_str(), p_class, p_func, p_file, p_line);
    return;
  }
*/


  static const string caution(
       string const &     p_ecode,
       string const &     p_entity,
       string const &     p_context,
       const char *       p_class,
       const char *       p_func,
       const char *       p_file,
       int const          p_line
     );


  static const string error(
       string const &     p_ecode,
       string const &     p_entity,
       string const &     p_context,
       const char *       p_class,
       const char *       p_func,
       const char *       p_file,
       int const          p_line
     );


  static const string fatal(
       string const &     p_ecode,
       string const &     p_entity,
       string const &     p_context,
       const char *       p_class,
       const char *       p_func,
       const char *       p_file,
       int const          p_line
     );


protected:

  // Member Objects

  // Derived Class Accessors


private:

  // Constructors

  Diagnostic(const Diagnostic&);


  // Operators

  Diagnostic& operator=(const Diagnostic&);
  bool operator==(const Diagnostic&) const;
  bool operator!=(const Diagnostic&) const;


  // Variables

  static map<string, string> * error_codes;
  bool                         internal_alloc;
  char **                      my_argv;
  int                          my_argc;


  // Friends

  // Private Functions

  static string get_mesg_string(const string & p_ecode, const string & p_entity);

};

#endif // class DIAGNOSTIC_HPP
