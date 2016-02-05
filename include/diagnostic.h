/*******************************************************************************
* COPYRIGHT (C) 2008.  .  All Rights Reserved. 
*
* THIS FILE INITIALLY CREATED WITH:  
*     TEMPLATE NAME:  lang_c_hdr.template 
*     COMMAND NAME:   gensrc 
*
* REPOSITORY INFORMATION: 
*  $Revision: 1.18 $ 
*  $Author: christy $ 
*  $Date: 2008/06/06 22:35:01 $ 
*
* _____________________________________________________________________________
* 
* SYNOPSIS: 
*  [put a single sentence stating purpose of this file] 
* 
* DESCRIPTION: 
*  [describe in detail the purpose and uses of this file] 
*
* EXAMPLES: 
*  [list examples of usage, or whatever is appropriate] 
*
* DESIGN: 
*  [describe in detail the design of this file] 
*
* ENVIRONMENT VARIABLES: 
*  [describe any environment variables used in this file] 
*
* LIMITATIONS: 
*  [list any known limitations/problems with this file] 
*
* SEE ALSO: 
*  [list any other relavent documentation] 
*******************************************************************************/

#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#ifdef __cplusplus
extern "C" {
#else
#  undef  __CLASS__
#  define __CLASS__ ""
#endif

/*******************************************************************************
* The defintion of CREATOR is checked for the sake of using global variables
* in a library.  One source file must declare the variables initially while the
* remaining source files must declare them using extern.  To do this, the
* source file doing the initial declaration will define CREATOR and then undef
* it after the #include statement.  The other source files do nothing with
* regard to CREATOR, thus SCOPE will be defined as 'extern' for them.
*******************************************************************************/
#ifdef CREATOR
#  define SCOPE
#else
#  define SCOPE  extern
#endif /* CREATOR */

/******  System Includes  ******/
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MSC_VER
#  include <libgen.h>
#  include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#endif

/******  External Includes  ******/
/******  Internal Includes  ******/
/******  Application Includes  ******/

/*
 *
 * Macros(s)
 *
 */

#undef  PERROR
#define PERROR(p_a) perror(p_a ", file(" __FILE__ ")")

#undef  DEBUG
#ifdef NDEBUG
#  define DEBUG(...);
#  undef  isDebugLevelSet
#  define isDebugLevelSet(p_a) 0
#  undef  debugEnd
#  define debugEnd();
#  undef  debugForce
#  define debugForce(p_level, p_class, p_func, p_file, p_line, p_format, ...);
#  undef  debugGetLevel
#  define debugGetLevel() 0
#  undef  debugSetLevel
#  define debugSetLevel(p_level);
#  undef  debugStart
#  define debugStart(p_level, p_outputFileName, p_configFileName, p_option, p_facility, p_printing) ;
#  undef  debugStartCLI
#  define debugStartCLI(p_argc, p_argv, p_new_argc, p_new_argv, p_option, p_facility);
#  undef  debugTab
#  define debugTab(p_offset);
#else
#  undef  isDebugLevelSet
#  define isDebugLevelSet(p_a) (debugGetLevel() & p_a)
#  ifdef _MSC_VER
#    define DEBUG(p_a, ...) if (isDebugLevelSet(p_a)) { debug_(p_a, __CLASS__, (char *)__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__); }
#  else
#    define DEBUG(p_a, ...) if (isDebugLevelSet(p_a)) { debug_(p_a, __CLASS__, __func__, __FILE__, __LINE__, __VA_ARGS__); }
#  endif


/*
 *
 * Api(s)
 * */

int          debug_ (int unsigned const p_level, char const * p_class, char const * p_func, char const * p_file, int unsigned const line, char const * format, ...);
void         debugEnd ();
unsigned int debugGetLevel ();
int unsigned debugSetLevel (int unsigned const p_level);
void         debugStart (int unsigned const p_level, char * const p_out_filename, char * const p_err_filename, char * const p_config_filename, int const p_option, int const p_facility, int unsigned const p_printing);
int          debugStartCLI (int const p_argc, char * const * p_argv, int * p_new_argc, char *** p_new_argv, int const p_option, int const p_facility);
void         debugTab (int p_offset);

#endif


#undef BREAKPOINT
#undef CAUTION
#undef ERROR
#undef FATAL
#define BREAKPOINT(p_mesg)
#define CAUTION(p_ecode, p_item, p_context)
#define ERROR(p_ecode, p_item, p_context)
#define FATAL(p_ecode, p_item, p_context)
/*
#ifdef _MSC_VER
#  define BREAKPOINT(p_mesg) breakpoint(p_mesg, __CLASS__, (char *)__FUNCTION__, __FILE__, __LINE__)
#  define CAUTION(p_ecode, p_item, p_context) caution(p_ecode, p_item, p_context, __CLASS__, (char *)__FUNCTION__, __FILE__, __LINE__)
#  define ERROR(p_ecode, p_item, p_context) error(p_ecode, p_item, p_context, __CLASS__, (char *)__FUNCTION__, __FILE__, __LINE__)
#  define FATAL(p_ecode, p_item, p_context) fatal(p_ecode, p_item, p_context, __CLASS__, (char *)__FUNCTION__, __FILE__, __LINE__)
#else
#  define BREAKPOINT(p_mesg) breakpoint(p_mesg, __CLASS__, (char *)__func__, __FILE__, __LINE__)
#  define CAUTION(p_ecode, p_item, p_context) caution(p_ecode, p_item, p_context, __CLASS__, (char *)__func__, __FILE__, __LINE__)
#  define ERROR(p_ecode, p_item, p_context) error(p_ecode, p_item, p_context, __CLASS__, (char *)__func__, __FILE__, __LINE__)
#  define FATAL(p_ecode, p_item, p_context) fatal(p_ecode, p_item, p_context, __CLASS__, (char *)__func__, __FILE__, __LINE__)
#endif
*/


void breakpoint(char * const p_mesg, char const * const p_class, char const * const p_func, char * const p_file, int const p_line);
void caution(char const * p_ecode, char const * p_item, char const * p_context, char const * p_class, char const * p_func, char const * p_file, int const p_line);
void error  (char const * p_ecode, char const * p_item, char const * p_context, char const * p_class, char const * p_func, char const * p_file, int const p_line);
void fatal  (char const * p_ecode, char const * p_item, char const * p_context, char const * p_class, char const * p_func, char const * p_file, int const p_line);
void debugSyslog(char const * p_prog);


/*
 *
 * Global variables
 *
 */

extern const char *       ENTER;
extern const char *       LEAVE;
extern const unsigned int NA;
extern const unsigned int TRACE;
extern const unsigned int INFO;
extern const unsigned int SCALAR;
extern const unsigned int ARRAY;
extern const unsigned int LOTS;
extern const unsigned int FORCE;

#ifdef __cplusplus
}
#endif

#endif /* DIAGNOSTIC_H */
