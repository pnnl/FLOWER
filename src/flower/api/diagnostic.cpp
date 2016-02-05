/******************************************************************************
* COPYRIGHT (C) 2008.  .  All Rights Reserved. 
*
* THIS FILE INITIALLY CREATED WITH:  
*     TEMPLATE NAME:  lang_c_lib.template 
*     COMMAND NAME:   gensrc 
*
* REPOSITORY INFORMATION: 
*  $Revision: 1.24 $ 
*  $Author: christy $ 
*  $Date: 2008/06/24 19:40:47 $ 
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

/*
 * DESCRIPTION:
 *   This file contains useful apis for printing messages and errors in a
 *   consistant format any opened file, whether it's a logfile or simply the
 *   stdout. By default all messages are sent to the stderr.
 *
 *   All messages written with these apis will contain the same format, that
 *   is:
 *     :<TYPE>,
 *     :FILE:     <path_to_file> line <line>
 *     :FUNCTION: <function_called_from>
 *     :MESSAGE:  <message>
 *
 *   where
 *     - <TYPE> is a string representing the type of message being displayed.
 *     - <path_to_file> corresponds to the C *__FILE__* macro value
 *     - <line> corresponds to the C *__LINE__* macro value
 *     - <function_called_from> corresponds to the string representation of the
 *                              api/method/function called from
 *     - <message> corresponds to the message to be printed
 *
 *
 *   The <debugTab> function is useful by indenting the aforementioned messages
 *   accordingly.  It is intended to allow one to print messages at different
 *   levels.  Typically, messages printed start on the first column.  With
 *   <debugTab>, one can change this behavior by adding a number of white spaces
 *   in front of this message.  For instance, calling <debugTab> at the
 *   beginning of each function with a value of 1 will print messages for
 *   different functions at a different indent level making it easier to see.
 *   Be sure to call <debugTab> prior to exiting the function with a value of -1
 *   to reset the indent level back.
 *
 *
 * EXAMPLES:
 * :void print_a()
 * :{
 * :  debugTab(1);
 * :  debug(TRACE, ENTER);
 * :  debug(INFO, "in print_a");
 * :  debug(TRACE, LEAVE);
 * :  debugTab(-1);
 * :  return;
 * :}
 *
 * :void print_b()
 * :{
 * :  int v = 0;
 * :  debugTab(1);
 * :  debug(TRACE, ENTER);
 * :  debug(SCALAR, "v = %d", v);
 * :  debug(TRACE, "trace in print_b");
 * :  debug(TRACE, LEAVE);
 * :  debugTab(-1);
 * :  return;
 * :}
 *
 * :int main()
 * :{
 * :  debugTab(1);
 * :  debug(TRACE, ENTER);
 * :  print_a();
 * :  print_b();
 * :  debug(TRACE, LEAVE);
 * :  debugTab(-1);
 * :  return(0);
 * :}
 *
 *   Assuming the <DEBUG> variable is set to 31, the following will
 *   be generated:
 *
 * :TRACE,
 * :File:      D:\HISpace_users\jason\3dsystem\Noc_dll\HISpace.cpp line: 1242
 * :Function:  main
 * :Message:   Entering
 *
 * :  TRACE,
 * :  File:      D:\HISpace_users\jason\3dsystem\Noc_dll\HISpace.cpp line: 1190
 * :  Function:  print_a
 * :  Message:   Entering
 *
 * :  INFO,
 * :  File:      D:\HISpace_users\jason\3dsystem\Noc_dll\HISpace.cpp line: 1191
 * :  Function:  print_a
 * :  Message:   in print_a
 *
 * :  TRACE,
 * :  File:      D:\HISpace_users\jason\3dsystem\Noc_dll\HISpace.cpp line: 1192
 * :  Function:  print_a
 * :  Message:   Leaving
 *
 * :  TRACE,
 * :  File:      D:\HISpace_users\jason\3dsystem\Noc_dll\HISpace.cpp line: 1202
 * :  Function:  print_b
 * :  Message:   Entering
 *
 * :  SCALAR,
 * :  File:      D:\HISpace_users\jason\3dsystem\Noc_dll\HISpace.cpp line: 1203
 * :  Function:  print_b
 * :  Message:   v = 0
 *
 * :  TRACE,
 * :  File:      D:\HISpace_users\jason\3dsystem\Noc_dll\HISpace.cpp line: 1204
 * :  Function:  print_b
 * :  Message:   trace in print_b
 *
 * :  TRACE,
 * :  File:      D:\HISpace_users\jason\3dsystem\Noc_dll\HISpace.cpp line: 1205
 * :  Function:  print_b
 * :  Message:   Leaving
 *
 * :TRACE,
 * :File:      D:\HISpace_users\jason\3dsystem\Noc_dll\HISpace.cpp line: 1245
 * :Function:  main
 * :Message:   Leaving
 *
 *
 *
 *   Assuming the <DEBUG> variable is set to 7, the following will
 *   be generated:
 *
 * :  INFO,
 * :  File:      D:\HISpace_users\jason\3dsystem\Noc_dll\HISpace.cpp line: 1191
 * :  Function:  print_a
 * :  Message:   in print_a
 *
 * :  SCALAR,
 * :  File:      D:\HISpace_users\jason\3dsystem\Noc_dll\HISpace.cpp line: 1203
 * :  Function:  print_b
 * :  Message:   v = 0
 *
 * :  TRACE,
 * :  File:      D:\HISpace_users\jason\3dsystem\Noc_dll\HISpace.cpp line: 1204
 * :  Function:  print_b
 * :  Message:   trace in print_b
 *
 */


/******  System Includes  ******/
#include <math.h>
#ifndef _MSC_VER
#include <syslog.h>
#endif
/******  External Includes  ******/
/******  Internal Includes  ******/
/******  Application Includes  ******/
#include "diagnostic.h"
#include "uthash.h"


/*
 * Macros
 */

#undef __MY_FUNC__
#ifdef _MSC_VER
#  define __MY_FUNC__ (char *)__FUNCTION__
#  define S_IFMT  _S_IFMT
#  define S_IFREG _S_IFREG
#  define DEBUGMAXLINE 1024
#  define basename(a) a
#else
#  define __MY_FUNC__ (char *)__func__
   int unsigned const DEBUGMAXLINE = 1024;
#endif

#undef NEWLINE_
#ifndef _MSC_VER
#define NEWLINE_ "\n"
#else
#define NEWLINE_ "\n\r"
#endif

#undef  NUM_FILE_PTR
#define NUM_FILE_PTR 2

#undef  IO_SIZE
#define IO_SIZE 128

#undef  MAX_DEBUG_LEVEL
#define MAX_DEBUG_LEVEL 31

#undef  NUM_DEBUG_LEVELS
#define NUM_DEBUG_LEVELS 8

#undef  NUM_DEBUG_OPTIONS
#define NUM_DEBUG_OPTIONS 257

#undef  USE_STDERR
#define USE_STDERR 0

#undef  USE_STDOUT
#define USE_STDOUT 1

/*
 * Typedefs
 */

typedef UT_hash_handle HASH;

/*
 *
 * Private Data
 *
 */

typedef struct debugFunctionType
{
  char * function_name;
  int    level;
  HASH   hh;
} debugFunctionType;


typedef struct debug_config
{
  char *              config_filename;
  char *              log_filename[2];
  int unsigned        level;
  int unsigned        printing;
  int unsigned        use_syslog;
  int                 facility;
  int                 option;
  FILE *              jec[NUM_FILE_PTR][NUM_FILE_PTR];
  debugFunctionType * debug_function_list;
} debug_config;

/*
FILE * g_default_file_fp[2];
g_default_file_fp[0]                       = stderr;
g_default_file_fp[1]                       = stdout;
*/

static char const * const g_class_delim    = "::";
static char const * const g_debug_delim    = "=";
static char const * const g_print_err_msg  = "One must call debugStart() first"NEWLINE_;
static char **            g_my_argv        = 0;
static int unsigned       g_print_err_flag = 0;
static int unsigned const g_stdout_console = 3;
static int unsigned const g_stdout_log     = 2;
static int unsigned const g_stderr_console = 1;
static int unsigned const g_stderr_log     = 0;
static int                g_my_argc        = 0;
static int                g_orig_argc      = 0;
static debug_config *     g_debug_cnf      = NULL;


/*
 *
 * Private Prototypes
 *
 */

static int unsigned debugAddFunction (char const * const function_name, int unsigned const p_level);
static void         debugDefineConfigFileName (char const * const p_config_filename);
static void         debugDefineLogFileName (char * const p_filename, int unsigned const p_file_flag);
static void         debugLoadConfigFile (char const * const p_filename);
static int          debugParseCLA (int const p_argc, char * const * p_argv, int * p_new_argc, char *** p_new_argv, int * p_level, char ** p_debug_config, char ** p_debug_outfile, char ** p_debug_errfile, int unsigned * p_printing);
static char *       debugReplaceNewLines (char * p_message);
static int          debugStarted (void);
static char *       debugTrimString (char * p_str);
void                debugUsage (void);
static char *       get_mesg_string (char const * const p_ecode, char const * const p_item, char const * const p_context);
static int unsigned set_bit (int unsigned * number, int unsigned const bit);
static int unsigned unset_bit (int unsigned * number, int unsigned const bit);
static int          write_ (int unsigned const p_level, char const * const p_class, char const * const p_func, char * const p_file, int unsigned const p_line, int unsigned const p_print_flag, char * p_format, ...);

/*
 FIX ME
static  int          debugLogger (char const * const p_level, char const * const p_class, char const * const p_func, char * const p_file, int unsigned const p_line, char * p_format, va_list p_vargs);
 */

#ifdef NDEBUG
static  void        debugDeleteFunction (debugFunctionType * p_function);
static int          debugNameSort (debugFunctionType * p_a, debugFunctionType * p_b);
static  void        debugPrintFunctionList (void);
static  void        debugSortByName (void);
#endif


/*
 *
 * Global
 *
 */


static int unsigned const debugPrintErrMsgErrorBit    = 1;
static int unsigned const debugStartCalledErrorBit    = 2;
static int unsigned const debugCLIBeforeStartErrorBit = 4;

int unsigned const NA                                 = 0;
int unsigned const TRACE                              = 1;
int unsigned const INFO                               = 2;
int unsigned const SCALAR                             = 4;
int unsigned const ARRAY                              = 8;
int unsigned const LOTS                               = 16;
int unsigned const ERROR                              = 32;
int unsigned const CAUTION                            = 64;
int unsigned const FATAL                              = 128;
int unsigned const FORCE                              = 256;

int unsigned const NDEBUG_LEVELS                      = (2<<(NUM_DEBUG_LEVELS-1));
unsigned int       TAB_STOP                           = 2;
int                NUMBER_OF_TABS                     = 0;

char const *       ENTER                              = "Entering";
char const *       LEAVE                              = "Leaving";
char const *       CLASS_LABEL                        = "Class:   ";
char const *       FILE_LABEL                         = "File:    ";
char const *       FUNC_LABEL                         = "Function:";
char               IO_SPACES[IO_SIZE];
char const *       LINE_LABEL                         = "Line: ";
char const *       TYPE_LABEL                         = "Type:    ";
char const *       MESS_LABEL                         = "Message: ";
char const *       g_debug_levels[NUM_DEBUG_OPTIONS]  = {
                                                          "NA",
                                                          "TRACE",
                                                          "INFO",
                                                          "______IT",
                                                          "SCALAR",
                                                          "_____S_T",
                                                          "_____SI_",
                                                          "_____SIT",
                                                          "ARRAY",
                                                          "____A__T",
                                                          "____A_I_",
                                                          "____A_IT",
                                                          "____AS__",
                                                          "____AS_T",
                                                          "____ASI_",
                                                          "____ASIT",
                                                          "LOTS",
                                                          "___L___T",
                                                          "___L__I_",
                                                          "___L__IT",
                                                          "___L_S__",
                                                          "___L_S_T",
                                                          "___L_SI_",
                                                          "___L_SIT",
                                                          "___LA___",
                                                          "___LA__T",
                                                          "___LA_I_",
                                                          "___LA_IT",
                                                          "___LAS__",
                                                          "___LAS_T",
                                                          "___LASI_",
                                                          "___LASIT",
                                                          "ERROR",
                                                          "__E____T",
                                                          "__E___I_",
                                                          "__E___IT",
                                                          "__E__S__",
                                                          "__E__S_T",
                                                          "__E__SI_",
                                                          "__E__SIT",
                                                          "__E_A___",
                                                          "__E_A__T",
                                                          "__E_A_I_",
                                                          "__E_A_IT",
                                                          "__E_AS__",
                                                          "__E_AS_T",
                                                          "__E_ASI_",
                                                          "__E_ASIT",
                                                          "__EL____",
                                                          "__EL___T",
                                                          "__EL__I_",
                                                          "__EL__IT",
                                                          "__EL_S__",
                                                          "__EL_S_T",
                                                          "__EL_SI_",
                                                          "__EL_SIT",
                                                          "__ELA___",
                                                          "__ELA__T",
                                                          "__ELA_I_",
                                                          "__ELA_IT",
                                                          "__ELAS__",
                                                          "__ELAS_T",
                                                          "__ELASI_",
                                                          "__ELASIT",
                                                          "CAUTION",
                                                          "_C_____T",
                                                          "_C____I_",
                                                          "_C____IT",
                                                          "_C___S__",
                                                          "_C___S_T",
                                                          "_C___SI_",
                                                          "_C___SIT",
                                                          "_C__A___",
                                                          "_C__A__T",
                                                          "_C__A_I_",
                                                          "_C__A_IT",
                                                          "_C__AS__",
                                                          "_C__AS_T",
                                                          "_C__ASI_",
                                                          "_C__ASIT",
                                                          "_C_L____",
                                                          "_C_L___T",
                                                          "_C_L__I_",
                                                          "_C_L__IT",
                                                          "_C_L_S__",
                                                          "_C_L_S_T",
                                                          "_C_L_SI_",
                                                          "_C_L_SIT",
                                                          "_C_LA___",
                                                          "_C_LA__T",
                                                          "_C_LA_I_",
                                                          "_C_LA_IT",
                                                          "_C_LAS__",
                                                          "_C_LAS_T",
                                                          "_C_LASI_",
                                                          "_C_LASIT",
                                                          "_CE_____",
                                                          "_CE____T",
                                                          "_CE___I_",
                                                          "_CE___IT",
                                                          "_CE__S__",
                                                          "_CE__S_T",
                                                          "_CE__SI_",
                                                          "_CE__SIT",
                                                          "_CE_A___",
                                                          "_CE_A__T",
                                                          "_CE_A_I_",
                                                          "_CE_A_IT",
                                                          "_CE_AS__",
                                                          "_CE_AS_T",
                                                          "_CE_ASI_",
                                                          "_CE_ASIT",
                                                          "_CEL____",
                                                          "_CEL___T",
                                                          "_CEL__I_",
                                                          "_CEL__IT",
                                                          "_CEL_S__",
                                                          "_CEL_S_T",
                                                          "_CEL_SI_",
                                                          "_CEL_SIT",
                                                          "_CELA___",
                                                          "_CELA__T",
                                                          "_CELA_I_",
                                                          "_CELA_IT",
                                                          "_CELAS__",
                                                          "_CELAS_T",
                                                          "_CELASI_",
                                                          "_CELASIT",
                                                          "FATAL",
                                                          "F______T",
                                                          "F_____I_",
                                                          "F_____IT",
                                                          "F____S__",
                                                          "F____S_T",
                                                          "F____SI_",
                                                          "F____SIT",
                                                          "F___A___",
                                                          "F___A__T",
                                                          "F___A_I_",
                                                          "F___A_IT",
                                                          "F___AS__",
                                                          "F___AS_T",
                                                          "F___ASI_",
                                                          "F___ASIT",
                                                          "F__L____",
                                                          "F__L___T",
                                                          "F__L__I_",
                                                          "F__L__IT",
                                                          "F__L_S__",
                                                          "F__L_S_T",
                                                          "F__L_SI_",
                                                          "F__L_SIT",
                                                          "F__LA___",
                                                          "F__LA__T",
                                                          "F__LA_I_",
                                                          "F__LA_IT",
                                                          "F__LAS__",
                                                          "F__LAS_T",
                                                          "F__LASI_",
                                                          "F__LASIT",
                                                          "F_E_____",
                                                          "F_E____T",
                                                          "F_E___I_",
                                                          "F_E___IT",
                                                          "F_E__S__",
                                                          "F_E__S_T",
                                                          "F_E__SI_",
                                                          "F_E__SIT",
                                                          "F_E_A___",
                                                          "F_E_A__T",
                                                          "F_E_A_I_",
                                                          "F_E_A_IT",
                                                          "F_E_AS__",
                                                          "F_E_AS_T",
                                                          "F_E_ASI_",
                                                          "F_E_ASIT",
                                                          "F_EL____",
                                                          "F_EL___T",
                                                          "F_EL__I_",
                                                          "F_EL__IT",
                                                          "F_EL_S__",
                                                          "F_EL_S_T",
                                                          "F_EL_SI_",
                                                          "F_EL_SIT",
                                                          "F_ELA___",
                                                          "F_ELA__T",
                                                          "F_ELA_I_",
                                                          "F_ELA_IT",
                                                          "F_ELAS__",
                                                          "F_ELAS_T",
                                                          "F_ELASI_",
                                                          "F_ELASIT",
                                                          "FC______",
                                                          "FC_____T",
                                                          "FC____I_",
                                                          "FC____IT",
                                                          "FC___S__",
                                                          "FC___S_T",
                                                          "FC___SI_",
                                                          "FC___SIT",
                                                          "FC__A___",
                                                          "FC__A__T",
                                                          "FC__A_I_",
                                                          "FC__A_IT",
                                                          "FC__AS__",
                                                          "FC__AS_T",
                                                          "FC__ASI_",
                                                          "FC__ASIT",
                                                          "FC_L____",
                                                          "FC_L___T",
                                                          "FC_L__I_",
                                                          "FC_L__IT",
                                                          "FC_L_S__",
                                                          "FC_L_S_T",
                                                          "FC_L_SI_",
                                                          "FC_L_SIT",
                                                          "FC_LA___",
                                                          "FC_LA__T",
                                                          "FC_LA_I_",
                                                          "FC_LA_IT",
                                                          "FC_LAS__",
                                                          "FC_LAS_T",
                                                          "FC_LASI_",
                                                          "FC_LASIT",
                                                          "FCE_____",
                                                          "FCE____T",
                                                          "FCE___I_",
                                                          "FCE___IT",
                                                          "FCE__S__",
                                                          "FCE__S_T",
                                                          "FCE__SI_",
                                                          "FCE__SIT",
                                                          "FCE_A___",
                                                          "FCE_A__T",
                                                          "FCE_A_I_",
                                                          "FCE_A_IT",
                                                          "FCE_AS__",
                                                          "FCE_AS_T",
                                                          "FCE_ASI_",
                                                          "FCE_ASIT",
                                                          "FCEL____",
                                                          "FCEL___T",
                                                          "FCEL__I_",
                                                          "FCEL__IT",
                                                          "FCEL_S__",
                                                          "FCEL_S_T",
                                                          "FCEL_SI_",
                                                          "FCEL_SIT",
                                                          "FCELA___",
                                                          "FCELA__T",
                                                          "FCELA_I_",
                                                          "FCELA_IT",
                                                          "FCELAS__",
                                                          "FCELAS_T",
                                                          "FCELASI_",
                                                          "FCELASIT",
                                                          "FORCE"
                                                        };


/*
 * API FUNCTION: breakpoint
 *
 *
 */
void breakpoint (char * const p_mesg, char const * const p_class, char const * const p_func, char * const p_file, int const p_line)
{
  char reply;

  write_(NA, p_class, p_func, p_file, p_line, USE_STDOUT, p_mesg);
  fprintf(stdout, "%s", "Enter <cr> to continue:  ");
  scanf("%c", &reply);

  return;
}


/*
 * API FUNCTION: debug_
 *
 *
 * DESCRIPTION:
 *   This api is used to print debug messages if the debug level supplied has a
 *   corresponding bit set in the <DEBUG> variable.  This is a
 *   wrapper to debugLogger therefore messages will be written to the
 *   appropriate file.
 *
 *   The function name starts with an underscore to avoid a conflict with a
 *   macro of the same name.  The macro is not capitalized because it is
 *   used more as a function call and the debug_ function will never be called
 *   directly.  The debug macro provides the __FILE__, etc properties to the
 *   debug_ function, so there is no reason to ever call the debug_ function
 *   directly.  Therefore the macro is all that is ever called and therefore
 *   takes on the look of a function call.
 *
 *
 * INPUT ARGUMENTS:
 *   int   level   - The debug level (ie TRACE, INFO, SCALAR, ARRAY, LOTS, ...)
 *   char * func   - The name of function making the actual call
 *   char * file   - The value represented by the macro *__FILE__*
 *   int   line    - The value represented by the macro *__LINE__*
 *   char * format - The format to print the subsequent arguments
 *   ...           - The subsequent arguments to print
 *
 *
 * OUTPUT ARGUMENTS:
 *   n/a
 *
 *
 * RETURN VALUE:
 *   Upon success, the number of characters printed (excluding the trailing
 *   \0 used to end output to strings).  A negative value is returned if an
 *   error is encountered.
 */
int debug_ (int unsigned const p_level, char const * const p_class, char const * const p_func, char * const p_file, int unsigned const p_line, char * p_format, ...)
{
  int     num_bytes;
  va_list vargs;

  va_start(vargs, p_format);
  num_bytes = write_(p_level, p_class, p_func, p_file, p_line, USE_STDOUT, p_format, vargs);
  va_end(vargs);

  return(num_bytes);
}


void debugDefineConfigFileName (char const * const p_config_filename)
{
#ifndef _MSC_VER
  struct passwd * pwd;
  struct stat     file_stat;
  uid_t           euid;
  int             stat_status; 
  int             errno_save; 
#endif
  char *          directory = NULL;
  
 

  /*
   * Determining config file...
   */

  g_debug_cnf->config_filename = NULL;

  if (NULL == p_config_filename)
  {
#ifdef _MSC_VER
    directory = getenv("HOME");
#else
    euid = geteuid();
    pwd = getpwuid(euid);
    if (NULL != pwd)
    {
      directory = pwd->pw_dir;
    }
#endif
    if (NULL != directory)
    {
      g_debug_cnf->config_filename = (char *)malloc((strlen(directory) + 1 + strlen("/.debug")) * sizeof(char));
      if (NULL == g_debug_cnf->config_filename)
      {
        PERROR("malloc");
      }
      sprintf(g_debug_cnf->config_filename, "%s%s", directory, "/.debug");
    }
    else
    {
      fprintf(stderr, NEWLINE_"DEBUG WARNING: Cannot find $HOME in your environment"NEWLINE_ NEWLINE_);
      return;
    }
  }
  else
  {
    g_debug_cnf->config_filename = (char *)malloc((strlen(p_config_filename) + 1) * sizeof(char));
    if (NULL == g_debug_cnf->config_filename)
    {
      PERROR("malloc");
    }
    sprintf(g_debug_cnf->config_filename, "%s", p_config_filename);
  }

#ifndef _MSC_VER
  /*
   * Testing config file...
   */


  if (NULL != g_debug_cnf->config_filename)
  {
    stat_status = stat(g_debug_cnf->config_filename, &file_stat);
    errno_save  = errno;
    if (0 == stat_status)
    {
      /*
       * DEVELOPER NOTE: The following line:
       *                   if (S_ISREG(file_stat.st_mode))
       *                 does not work on windows so we used S_IFMT and S_IFREG instead
       */
      if (1 == (file_stat.st_mode && S_IFREG))
      {
        debugLoadConfigFile(g_debug_cnf->config_filename);
/*
 * Uncomment for testing to list functions in ~/.debug or config_filename
 * JEC - Be sure to recompile with the -D_debug option.
 *      debugPrintFunctionList();
 */
      }
      else
      {
        fprintf(stderr, NEWLINE_"DEBUG WARNING: %s is not a regular file"NEWLINE_ NEWLINE_, g_debug_cnf->config_filename);
      }
    }
    else
    {
      fprintf(stderr, NEWLINE_"DEBUG WARNING: %s - %s: stat(2)"NEWLINE_, g_debug_cnf->config_filename, strerror(errno_save));
    }
  }
#endif

  return;
}


void debugDefineLogFileName (char * const p_filename, int unsigned const p_file_flag)
{
  int errno_save; 

  g_debug_cnf->log_filename[p_file_flag] = p_filename;

  if (NULL != p_filename)
  {
    g_debug_cnf->jec[p_file_flag][1] = fopen(p_filename, "w");
    errno_save                       = errno; 

    if (NULL == g_debug_cnf->jec[p_file_flag][1])
    {
      fprintf(stderr, ":%s: ==> unable to open for writing: %s"NEWLINE_, p_filename, strerror(errno_save));
      exit(1);
    }

    g_debug_cnf->log_filename[p_file_flag] = (char *)malloc((strlen(p_filename) + 1) * sizeof(char));
    if (NULL == g_debug_cnf->log_filename[p_file_flag])
    {
      PERROR("malloc");
    }
    sprintf(g_debug_cnf->log_filename[p_file_flag], "%s", p_filename);
  }

  return;
}


void debugEnd (void)
{
  int                 arg;
  debugFunctionType * f;

  if (NULL == g_debug_cnf)
  {
    if (g_print_err_flag & debugPrintErrMsgErrorBit)
    {
      g_print_err_flag |= debugPrintErrMsgErrorBit;
      fprintf(stderr, "in %s: %s", __MY_FUNC__, g_print_err_msg);
    }
    return;
  }

  if ((NULL != g_debug_cnf->log_filename[USE_STDOUT]) && (NULL != g_debug_cnf->log_filename[USE_STDERR]))
  {
    if (0 == strcmp(g_debug_cnf->log_filename[USE_STDERR], g_debug_cnf->log_filename[USE_STDOUT]))
    {
      if (NULL != g_debug_cnf->jec[USE_STDOUT][1])
      {
        fclose(g_debug_cnf->jec[USE_STDOUT][1]);
      }
    }
    else
    {
      if (NULL != g_debug_cnf->jec[USE_STDERR][1])
      {
        fclose(g_debug_cnf->jec[USE_STDERR][1]);
      }

      if (NULL != g_debug_cnf->jec[USE_STDOUT][1])
      {
        fclose(g_debug_cnf->jec[USE_STDOUT][1]);
      }
    }
  }
  else
  {
    if (NULL != g_debug_cnf->jec[USE_STDOUT][1])
    {
      fclose(g_debug_cnf->jec[USE_STDOUT][1]);
    }
    if (NULL != g_debug_cnf->jec[USE_STDERR][1])
    {
      fclose(g_debug_cnf->jec[USE_STDERR][1]);
    }
  }

  g_debug_cnf->level = 0;
  for (f = g_debug_cnf->debug_function_list; NULL != f; f = (debugFunctionType *)f->hh.next)
  {
    free(f->function_name);
  }

  if (NULL != g_debug_cnf->debug_function_list)
  {
    free(g_debug_cnf->debug_function_list);
  }

  if (NULL != g_debug_cnf->config_filename)
  {
    free(g_debug_cnf->config_filename);
  }

  if (NULL != g_debug_cnf->log_filename[USE_STDOUT])
  {
    free(g_debug_cnf->log_filename[USE_STDOUT]);
  }

  if (NULL != g_debug_cnf->log_filename[USE_STDERR])
  {
    free(g_debug_cnf->log_filename[USE_STDERR]);
  }

#ifndef _MSC_VER
  if (0 != g_debug_cnf->use_syslog)
  {
    closelog();
  }
#endif

  for (arg = 0; arg < g_my_argc; ++arg)
  {
    free(g_my_argv[arg]);
  }

  if (g_my_argv)
  {
    free(g_my_argv);
  }

  free(g_debug_cnf);
  g_debug_cnf       = NULL;
  g_print_err_flag  = 0;
  g_print_err_flag |= debugPrintErrMsgErrorBit;
  return;
}


char * get_mesg_string (char const * p_ecode, char const * p_item, char const * p_context)
{
  const char * blank = "<blank>";
  char       * mesg;
  int          size;

  if (NULL == g_debug_cnf)
  {
    if (g_print_err_flag & debugPrintErrMsgErrorBit)
    {
      g_print_err_flag |= debugPrintErrMsgErrorBit;
      fprintf(stderr, "in %s: %s", __MY_FUNC__, g_print_err_msg);
    }
    return(NULL);
  }

  size  = ( p_ecode )   ? strlen(p_ecode)   : strlen(blank);
  size += ( p_item )    ? strlen(p_item)    : 0;
  size += ( p_context ) ? strlen(p_context) : 0;
  size += 100 + 1;                                // padding and NEWLINE_ to be safe
  mesg  = (char *)malloc(size * sizeof(char));
  if (mesg == (char *)NULL)
  {
    PERROR("malloc");
  }

  if (! g_debug_cnf->use_syslog)
  {
    sprintf(mesg, "Error Code=%s", ( p_ecode ) ? p_ecode : blank);
    if (p_item)
    {
      strcat(mesg, NEWLINE_);
      strcat(mesg, IO_SPACES);
      strcat(mesg, "    ");
      strcat(mesg, p_item);
    }
    if (p_context)
    {
      strcat(mesg, NEWLINE_);
      strcat(mesg, IO_SPACES);
      strcat(mesg, "      ");
      strcat(mesg, p_context);
    }
  }
  else
  {
    sprintf(mesg, "%s: ", ( p_ecode ) ? p_ecode : blank);
    if (p_context)
    {
      strcat(mesg, p_context);
    }
  }

  return(mesg);
}


/*
 * API FUNCTION: error
 *
 *
 * DESCRIPTION:
 *   This api is used to print error messages.  This is a wrapper to debugLogger
 *   therefore messages will be written to the appropriate file.
 *
 *
 * INPUT ARGUMENTS:
 *   int   level   - The debug level (ie TRACE, INFO, SCALAR, ARRAY, LOTS, ...)
 *   char * func   - The name of function making the actual call
 *   char * file   - The value represented by the macro *__FILE__*
 *   int   line    - The value represented by the macro *__LINE__*
 *   char * format - The format to print the subsequent arguments
 *   ...           - The subsequent arguments to print
 *
 *
 * OUTPUT ARGUMENTS:
 *   n/a
 *
 *
 * RETURN VALUE:
 *   Upon success, the number of characters printed (excluding the trailing
 *   \0 used to end output to strings).  A negative value is returned if an
 *   error is encountered.
 */
void caution (char const * const p_ecode, char const * const p_item, char const * const p_context, char const * const p_class, char const * const p_func, char * const p_file, int const p_line)
{
  char * mesg = get_mesg_string(p_ecode, p_item, p_context);
  write_(CAUTION, p_class, p_func, p_file, p_line, USE_STDERR, mesg);
  free(mesg);
  return;
}


void error (char const * const p_ecode, char const * const p_item, char const * const p_context, char const * const p_class, char const * const p_func, char * const p_file, int const p_line)
{
  char * mesg = get_mesg_string(p_ecode, p_item, p_context);
  write_(ERROR, p_class, p_func, p_file, p_line, USE_STDERR, mesg);
  free(mesg);
  return;
}


void fatal (char const * const p_ecode, char const * const p_item, char const * const p_context, char const * const p_class, char const * const p_func, char * const p_file, int const p_line)
{
  char * mesg = get_mesg_string(p_ecode, p_item, p_context);
  write_(FATAL, p_class, p_func, p_file, p_line, USE_STDERR, mesg);
  free(mesg);
  exit(1);
}


void debugSyslog (char const * p_prog)
{
  if (NULL == g_debug_cnf)
  {
    if (g_print_err_flag & debugPrintErrMsgErrorBit)
    {
      g_print_err_flag |= debugPrintErrMsgErrorBit;
      fprintf(stderr, "in %s, file(%s), line(%d): %s", __MY_FUNC__, __FILE__, __LINE__, g_print_err_msg);
    }
    return;
  }

#ifndef _MSC_VER
  openlog(p_prog, g_debug_cnf->option, g_debug_cnf->facility);
  g_debug_cnf->use_syslog = 1;
#endif
  return;
}



/*
 * API FUNCTION: debugGetLevel
 *
 *
 * DESCRIPTION:
 *   This function can be used to get the <DEBUG> level.
 *
 *
 * INPUT ARGUMENTS:
 *   n/a
 *
 *
 * OUTPUT ARGUMENTS:
 *   n/a
 *
 *
 * RETURN VALUE:
 *   int - the current debug level
 */
unsigned int debugGetLevel (void)
{
  if (NULL == g_debug_cnf)
  {
    if (g_print_err_flag & debugPrintErrMsgErrorBit)
    {
      g_print_err_flag |= debugPrintErrMsgErrorBit;
      fprintf(stderr, "in %s: %s", __MY_FUNC__, g_print_err_msg);
    }
    return(0);
  }
  return(g_debug_cnf->level);
}

 
/*
 * API FUNCTION: debugReplaceNewLines
 *
 *
 * DESCRIPTION:
 *   This function is used to replace any newline characters in the message
 *   with the appropriate amount of spaces to align the message.
 *
 *
 * INPUT ARGUMENTS:
 *   char * message - The message to fix newline characters
 *
 *
 * OUTPUT ARGUMENTS:
 *   n/a
 *
 *
 * RETURN VALUE:
 *   char * - The new message with newline characters fixed
 */
static char * debugReplaceNewLines (char * p_message)
{
  char *       new_message;
  unsigned int i;
  unsigned int j       = 0;
  unsigned int len;
  unsigned int num_nl  = 0;
  int          k       = 0;
  int          len_msg = (int)strlen(MESS_LABEL) + NUMBER_OF_TABS;

  if (NULL == p_message)
  {
    return(NULL);
  }

  len = strlen(p_message);
  for (i = 0; i < len; i++)
  {
    if (p_message[i] == '\n')
    {
      num_nl++;
    }
  }

  i           = (num_nl * (len_msg + 1)) + len + 1;
  new_message = (char *)malloc(i * sizeof(char));
  if (NULL == new_message)
  {
    PERROR("malloc");
  }

  for (i = 0; i < len; i++)
  {
    if (p_message[i] == '\n')
    {
      new_message[j++] = '\n';
      for (k = 0; k < len_msg; k++)
      {
        new_message[j++] = ' ';
      }
    }
    else
    {
      new_message[j++] = p_message[i];
    }
  }
  new_message[j] = '\0';
  return(new_message);
}


/*
 * API FUNCTION: debugSetLevel
 *
 *
 * DESCRIPTION:
 *   This function can be used to set the <DEBUG> level.
 *
 *
 * INPUT ARGUMENTS:
 *   int level - the debug level
 *
 *
 * OUTPUT ARGUMENTS:
 *   n/a
 *
 *
 * RETURN VALUE:
 *   n/a
 */
int unsigned debugSetLevel (int unsigned const p_level)
{
  if (NULL == g_debug_cnf)
  {
    if (g_print_err_flag & debugPrintErrMsgErrorBit)
    {
      g_print_err_flag |= debugPrintErrMsgErrorBit;
      fprintf(stderr, "in %s: %s", __MY_FUNC__, g_print_err_msg);
    }
    return(0);
  }

  if (MAX_DEBUG_LEVEL < p_level)
  {
    char long_msg[1024];
    sprintf(long_msg, "The value for level must be 0 - 31, not %d", p_level);
    FATAL("RangeError", "The level for debugSetLevel was too large", long_msg);
  }

  if (NUM_DEBUG_OPTIONS > p_level)
  {
    g_debug_cnf->level = p_level;
  }
  else
  {
    fprintf(stderr, "NOT Changing level to %u"NEWLINE_, p_level);
  }

  return(g_debug_cnf->level == p_level);
}

 
void debugStart (int unsigned const p_level, char * const p_out_filename, char * const p_err_filename, char * const p_config_filename, int const p_option, int const p_facility, int unsigned const p_printing)
{
  if (g_print_err_flag & debugCLIBeforeStartErrorBit)
  {
    fprintf(stderr, NEWLINE_"WARNING: can't call debugStartCLI() or debugStart() more than once"NEWLINE_ NEWLINE_);
    return;
  }

  if (NULL != g_debug_cnf)
  {
    if (g_print_err_flag & debugStartCalledErrorBit)
    {
      g_print_err_flag |= debugStartCalledErrorBit;
      fprintf(stderr, NEWLINE_"WARNING: debugStart() has been called already"NEWLINE_ NEWLINE_);
    }
    return;
  }

/*
 * Uncomment this to test if the config file is a directory
 * config_filename = "/tmp";
 */

  g_debug_cnf = (debug_config *)malloc(sizeof(debug_config));
  if (NULL == g_debug_cnf)
  {
    if (g_print_err_flag & debugPrintErrMsgErrorBit)
    {
      g_print_err_flag |= debugPrintErrMsgErrorBit;
      fprintf(stderr, "in %s: %s", __MY_FUNC__, g_print_err_msg);
    }
    PERROR("malloc");
  }

  g_print_err_flag                 |= debugCLIBeforeStartErrorBit;
  g_debug_cnf->debug_function_list  = NULL;
  g_debug_cnf->option               = p_option;
  g_debug_cnf->facility             = p_facility;
  g_debug_cnf->printing             = p_printing;
  g_debug_cnf->use_syslog           = 0;

  g_debug_cnf->jec[USE_STDERR][0]   = stderr;
  g_debug_cnf->jec[USE_STDERR][1]   = NULL;
  g_debug_cnf->jec[USE_STDOUT][0]   = stdout;
  g_debug_cnf->jec[USE_STDOUT][1]   = NULL;

  debugSetLevel(p_level);

  debugDefineLogFileName(NULL, USE_STDERR);
  debugDefineLogFileName(NULL, USE_STDOUT);

  if ((NULL != p_err_filename) && (NULL != p_out_filename))
  {
    if (0 == strcmp(p_err_filename, p_out_filename))
    {
      debugDefineLogFileName(p_err_filename, USE_STDERR);
      g_debug_cnf->jec[USE_STDOUT][1]       = g_debug_cnf->jec[USE_STDERR][1];
      g_debug_cnf->log_filename[USE_STDOUT] = (char *)malloc((strlen(g_debug_cnf->log_filename[USE_STDERR]) + 1) * sizeof(char));
      if (NULL == g_debug_cnf->log_filename[USE_STDOUT])
      {
        PERROR("malloc");
      }
      sprintf(g_debug_cnf->log_filename[USE_STDOUT], "%s", g_debug_cnf->log_filename[USE_STDERR]);
    }
    else
    {
      debugDefineLogFileName(p_err_filename, USE_STDERR);
      debugDefineLogFileName(p_out_filename, USE_STDOUT);
    }
  }
  else
  {
    if (NULL != p_err_filename)
    {
      debugDefineLogFileName(p_err_filename, USE_STDERR);
    }
    if (NULL != p_out_filename)
    {
      debugDefineLogFileName(p_out_filename, USE_STDOUT);
    }
  }

  if (0 != p_printing)
  {
    if ((p_printing & ((int unsigned)pow(2, g_stdout_log))) && (! (p_printing & ((int unsigned)pow(2, g_stdout_console)))))
    {
      g_debug_cnf->jec[USE_STDOUT][0] = NULL;
    }

    if ((p_printing & ((int unsigned)pow(2, g_stderr_log))) && (! (p_printing & ((int unsigned)pow(2, g_stderr_console)))))
    {
      g_debug_cnf->jec[USE_STDERR][0] = NULL;
    }
  }

  debugDefineConfigFileName(p_config_filename);

  return;
}


int debugParseCLA (int const p_argc, char * const * p_argv, int * p_new_argc, char *** p_new_argv, int * p_level, char ** p_debug_config, char ** p_debug_outfile, char ** p_debug_errfile, int unsigned * p_printing)
{
  char *             token;
  char const * const delim    = "=";
  char **            new_argv = NULL;
  int                new_argc = 0;
  int                opt;

  if (p_new_argc)
  {
    *p_new_argv = 0;
    *p_new_argc = 0;
  }

  new_argv = (char **)calloc(p_argc, sizeof(char *));
  if (! new_argv)
  {
    PERROR("calloc");
  }

  new_argv[new_argc] = (char *)malloc((strlen(p_argv[0]) + 1) * sizeof(char));
  if (! new_argv[new_argc])
  {
    PERROR("malloc");
  }
  strcpy(new_argv[new_argc], p_argv[0]);
  new_argc++;
  opt         = 1;
  *p_printing = 0;
  while (opt < p_argc)
  {
    if      (0 == strcmp(p_argv[opt], "--debugHelp"))
    {
      debugUsage();
    }
    else if (0 != strstr(p_argv[opt], "--debugLevel="))
    {
      token = strtok(p_argv[opt], delim);
      token = strtok(NULL, delim);
      if (token)
      {
        *p_level = atoi(token);
      }
      else
      {
        printf("%s => requires an argument"NEWLINE_, p_argv[opt]);
      }
    }
    else if (0 != strstr(p_argv[opt], "--debugConfig="))
    {
      token = strtok(p_argv[opt], delim);
      token = strtok(NULL, delim);
      if (token)
      {
        *p_debug_config = (char *)malloc((strlen(token) + 1) * sizeof(char));
        if (! *p_debug_config)
        {
          PERROR("malloc");
        }
        strcpy(*p_debug_config, token);
      }
      else
      {
        printf("%s => requires an argument"NEWLINE_, p_argv[opt]);
      }
    }
    else if (0 != strstr(p_argv[opt], "--debugStdout="))
    {
      token = strtok(p_argv[opt], delim);
      token = strtok(NULL, delim);
      if (token)
      {
        *p_debug_outfile = (char *)malloc((strlen(token) + 1) * sizeof(char));
        if (! *p_debug_outfile)
        {
          PERROR("malloc");
        }
        strcpy(*p_debug_outfile, token);
        unset_bit(p_printing, g_stdout_console);
        set_bit(p_printing, g_stdout_log);
      }
      else
      {
        printf("%s => requires an argument"NEWLINE_, p_argv[opt]);
      }
    }
    else if (0 != strstr(p_argv[opt], "--debugStderr="))
    {
      token = strtok(p_argv[opt], delim);
      token = strtok(NULL, delim);
      if (token)
      {
        *p_debug_errfile = (char *)malloc((strlen(token) + 1) * sizeof(char));
        if (! *p_debug_errfile)
        {
          PERROR("malloc");
        }
        strcpy(*p_debug_errfile, token);
        unset_bit(p_printing, g_stderr_console);
        set_bit(p_printing, g_stderr_log);
      }
      else
      {
        printf("%s => requires an argument"NEWLINE_, p_argv[opt]);
      }
    }
    else if (0 != strstr(p_argv[opt], "--debugStdall="))
    {
      token = strtok(p_argv[opt], delim);
      token = strtok(NULL, delim);
      if (token)
      {
        *p_debug_errfile = (char *)malloc((strlen(token) + 1) * sizeof(char));
        *p_debug_outfile = (char *)malloc((strlen(token) + 1) * sizeof(char));
        if ((! *p_debug_errfile) || (! *p_debug_outfile))
        {
          PERROR("malloc");
        }
        strcpy(*p_debug_errfile, token);
        strcpy(*p_debug_outfile, token);
        unset_bit(p_printing, g_stderr_console);
        unset_bit(p_printing, g_stdout_console);
        set_bit(p_printing, g_stderr_log);
        set_bit(p_printing, g_stdout_log);
      }
      else
      {
        printf("%s => requires an argument"NEWLINE_, p_argv[opt]);
      }
    }
    else if (0 != strstr(p_argv[opt], "--debugStdoutLog="))
    {
      token = strtok(p_argv[opt], delim);
      token = strtok(NULL, delim);
      if (token)
      {
        *p_debug_outfile = (char *)malloc((strlen(token) + 1) * sizeof(char));
        if (! *p_debug_outfile)
        {
          PERROR("malloc");
        }
        strcpy(*p_debug_outfile, token);
        set_bit(p_printing, g_stdout_console);
        set_bit(p_printing, g_stdout_log);
      }
      else
      {
        printf("%s => requires an argument"NEWLINE_, p_argv[opt]);
      }
    }
    else if (0 != strstr(p_argv[opt], "--debugStderrLog="))
    {
      token = strtok(p_argv[opt], delim);
      token = strtok(NULL, delim);
      if (token)
      {
        *p_debug_errfile = (char *)malloc((strlen(token) + 1) * sizeof(char));
        if (! *p_debug_errfile)
        {
          PERROR("malloc");
        }
        strcpy(*p_debug_errfile, token);
        set_bit(p_printing, g_stderr_console);
        set_bit(p_printing, g_stderr_log);
      }
      else
      {
        printf("%s => requires an argument"NEWLINE_, p_argv[opt]);
      }
    }
    else if (0 != strstr(p_argv[opt], "--debugStdallLog="))
    {
      token = strtok(p_argv[opt], delim);
      token = strtok(NULL, delim);
      if (token)
      {
        *p_debug_errfile = (char *)malloc((strlen(token) + 1) * sizeof(char));
        *p_debug_outfile = (char *)malloc((strlen(token) + 1) * sizeof(char));
        if ((! *p_debug_errfile) || (! *p_debug_outfile))
        {
          PERROR("malloc");
        }
        strcpy(*p_debug_errfile, token);
        strcpy(*p_debug_outfile, token);
        set_bit(p_printing, g_stdout_log);
        set_bit(p_printing, g_stderr_console);
        set_bit(p_printing, g_stdout_console);
        set_bit(p_printing, g_stderr_log);
      }
      else
      {
        printf("%s => requires an argument"NEWLINE_, p_argv[opt]);
      }
    }
    else
    {
      new_argv[new_argc] = (char *)malloc((strlen(p_argv[opt]) + 1) * sizeof(char));
      if (! new_argv[new_argc])
      {
        PERROR("malloc");
      }
      strcpy(new_argv[new_argc], p_argv[opt]);
      new_argc++;
    }
    opt++;
  }
 
  g_orig_argc = p_argc;
  g_my_argc   = new_argc;
  g_my_argv   = new_argv;
  if (p_new_argc)
  {
    *p_new_argc = new_argc;
    *p_new_argv = new_argv;
  }

  return(1);
}


int debugStarted (void)
{
  int started = 0;

  if (g_print_err_flag & debugCLIBeforeStartErrorBit)
  {
    fprintf(stderr, NEWLINE_"WARNING: can't call debugStartCLI() or debugStart() more than once"NEWLINE_ NEWLINE_);
    started = 1;
  }

  return(started);
}


int debugStartCLI (int const p_argc, char * const * p_argv, int * p_new_argc, char *** p_new_argv, int const p_option, int const p_facility)
{
  char *       debug_config  = NULL;
  char *       debug_errfile = NULL;
  char *       debug_outfile = NULL;
  int unsigned printing      = 0;
  int          level         = 0;
  int          ok            = 1;
  int          idx;

  if (debugStarted())
  {
    return(0);
  }

  if ((p_argc > 1) && (NULL != p_argv))
  {
    ok = debugParseCLA(p_argc, p_argv, p_new_argc, p_new_argv, &level, &debug_config, &debug_outfile, &debug_errfile, &printing);
  }
  else
  {
    *p_new_argc = p_argc;
    *p_new_argv = (char **)malloc((p_argc+1) * sizeof(char *));
    for (idx = 0; idx < p_argc; ++idx)
    {
      *p_new_argv[idx] = p_argv[idx];
    }
  }

  debugStart(level, debug_outfile, debug_errfile, debug_config, p_option, p_facility, printing);
  return(ok);
}


/*
 * API FUNCTION: debugTab
 *
 *
 * DESCRIPTION:
 *   This function is used to increase/decrease the number of spaces printed
 *   before any message.  This function helps keep all output written
 *   consistant looking and easy to read.
 *
 *
 * INPUT ARGUMENTS:
 *   int offset - The number of tabs (2 spaces) to indent
 *
 *
 * OUTPUT ARGUMENTS:
 *   n/a
 *
 *
 * RETURN VALUE:
 *   n/a
 */
void debugTab (int p_offset)
{
  int i;

  NUMBER_OF_TABS += (p_offset * TAB_STOP);
  if (0 > NUMBER_OF_TABS)
  {
    NUMBER_OF_TABS = 0;
  }
  else if (NUMBER_OF_TABS >= IO_SIZE)
  {
    NUMBER_OF_TABS = IO_SIZE;
  }

  for (i = 0; i < NUMBER_OF_TABS; i++)
  {
    IO_SPACES[i] = ' ';
  }
  IO_SPACES[i] = '\0';

  return;
}


static int unsigned debugAddFunction (char const * function_name, int unsigned const p_level)
{
  return(0);
/*
  size_t              len;
  debugFunctionType * f;

  if (NULL == (f = (debugFunctionType *)malloc(sizeof(debugFunctionType))))
  {
    PERROR("malloc");
  }
  f->level         = p_level;
  len              = strlen(function_name);
  f->function_name = (char *)malloc((len + 1) * sizeof(char));
  if (! f->function_name)
  {
    PERROR("malloc");
  }
  strncpy(f->function_name, function_name, len);
  // function_name: key field
  HASH_ADD_STR(g_debug_cnf->debug_function_list, f->function_name, f);
  return(0);
*/
}


#ifdef NDEBUG
static void debugDeleteFunction (debugFunctionType * p_function)
{
  /* function: pointer to delete */
  HASH_DEL(g_debug_cnf->debug_function_list, p_function);
  return;
}
#endif


static void debugLoadConfigFile (char const * const p_filename)
{
  char    buffer[DEBUGMAXLINE];
  char *  key;
  char *  stringValue;
  int     intValue;
  size_t  delimPosition;
  size_t  stringValueSize;
  size_t  valuePosition;
  FILE *  fp = fopen(p_filename, "r");

  clearerr(fp);
  while (NULL != fgets(buffer, DEBUGMAXLINE, fp))
  {
    assert(buffer != NULL);

    delimPosition = strcspn(buffer, g_debug_delim);
    if ('\0' == buffer[delimPosition])
    {
      /* Not a valid line in the file */
      continue;
    }

    valuePosition   = strcspn(buffer + delimPosition + 1, ":");
    stringValueSize = valuePosition + 1;
    key             = (char *)malloc((delimPosition   + 1) * sizeof(char));
    stringValue     = (char *)malloc((stringValueSize + 1) * sizeof(char));
  
    if ((NULL == key) || (NULL == stringValue))
    {
      PERROR("malloc");
    }
    strncpy(key,         buffer,                     delimPosition);
    strncpy(stringValue, buffer + delimPosition + 1, stringValueSize);
  
    key[delimPosition]                        = '\0';
    stringValue[stringValueSize]              = '\0';
    key[strcspn(key,     "\n\r")]             = '\0';
    stringValue[strcspn(stringValue, "\n\r")] = '\0';
    intValue                                  = atoi(stringValue);
    key                                       = debugTrimString(key);
  
    if (0 <= intValue)
    {
      debugAddFunction(key, intValue);
    }
  
    free(key);
    free(stringValue);
  }
  if (0 != ferror(fp))
  {
    PERROR("fgets");
    exit(EXIT_FAILURE);
  }

  fclose(fp);
  return;
}


#ifdef NDEBUG
static int debugNameSort (debugFunctionType * p_a, debugFunctionType * p_b)
{
  return(strcmp(p_a->function_name, p_b->function_name));
}
#endif


#ifdef NDEBUG
static void debugPrintFunctionList (void)
{
  debugFunctionType * f;

  if (NULL == g_debug_cnf)
  {
    if (g_print_err_flag & debugPrintErrMsgErrorBit)
    {
      g_print_err_flag |= debugPrintErrMsgErrorBit;
      fprintf(stderr, "in %s: %s", __MY_FUNC__, g_print_err_msg);
    }
    return;
  }

  for (f = g_debug_cnf->debug_function_list; f != NULL; f = (debugFunctionType *)f->hh.next)
  {
    printf("Function Name = %s, Debug Level = %d"NEWLINE_, f->function_name, f->level);
  }

  return;
}
#endif


#ifdef NDEBUG
static void debugSortByName (void)
{
  HASH_SORT(g_debug_cnf->debug_function_list, debugNameSort);
  return;
}
#endif


static char * debugTrimString (char * p_stringValue)
{
  char * ibuffer;
  char * obuffer;

  if (p_stringValue)
  {
    for (ibuffer = obuffer = p_stringValue; *ibuffer; )
    {
      while (*ibuffer && (isspace(*ibuffer)))
      {
        ibuffer++;
      }
      if (*ibuffer && (obuffer != p_stringValue))
      {
        *(obuffer++) = ' ';
      }
      while (*ibuffer && (!isspace(*ibuffer)))
      {
        *(obuffer++) = *(ibuffer++);
      }
    }
    *obuffer = '\0';
  }
  return(p_stringValue);
}


void debugUsage (void)
{
  printf("\n");
  printf("USAGE: debugHelp\n");
  printf("\n");
  printf("  Debug Command Line Arguments:\n");
  printf("\n");
  printf("  --debugHelp\n");
  printf("  --debugLevel=number\n");
  printf("  --debugConfig=debug_configuration_file\n");
  printf("  --debugStdout=file_to_save_all_stdout\n");
  printf("  --debugStderr=file_to_save_all_stderr\n");
  printf("  --debugStdall=file_to_save_all_stdout_and_stderr\n");
  printf("  --debugStdoutLog=file_to_save_all_stdout\n");
  printf("  --debugStderrLog=file_to_save_all_stderr\n");
  printf("  --debugStdallLog=file_to_save_all_stdout_and_stderr\n");
  printf("\n");
  printf("  Usage Notes:\n");
  printf("\n");
  printf("  --debugLevel takes a number from 0-31 where\n");
  printf("       0 = no messages\n");
  printf("       1 = print out TRACE  messages\n");
  printf("       2 = print out INFO   messages\n");
  printf("       4 = print out SCALAR messages\n");
  printf("       8 = print out ARRAY  messages\n");
  printf("      16 = print out LOTS   messages\n");
  printf("    Numbers can be added together to get combinations of messages.\n");
  printf("      For example,  6 will print out INFO and SCALAR messages.\n");
  printf("      For example, 31 will print out all messages.\n");
  printf("\n");
  printf("  --debugConfig takes a filename that is formatted like\n");
  printf("      Function:Number\n");
  printf("    where function is the name of the Function you want to\n");
  printf("    print out messages that match the Number\n");
  printf("      For example, myFunc:31 will always print out all messages in myFunc.\n");
  printf("      For example, myFunc:0  will override debugLevel number and not\n");
  printf("                             print any messages.\n");
  printf("\n");
  printf("  --debugStdout will create (or overwrite an existing) filename\n");
  printf("    NOTE: debugStdout will send all stdout messages to filename\n");
  printf("  --debugStderr will create (or overwrite an existing) filename\n");
  printf("    NOTE: debugStderr will send all stderr messages to filename\n");
  printf("  --debugStdall will create (or overwrite an existing) filename\n");
  printf("    NOTE: debugStdall will send all stdout and stderr messages to filename\n");
  printf("\n");
  printf("  --debugStdoutLog will create (or overwrite an existing) filename\n");
  printf("    NOTE: debugStdoutLog will send all stdout messages to console and filename\n");
  printf("  --debugStderrLog will create (or overwrite an existing) filename\n");
  printf("    NOTE: debugStderrLog will send all stderr messages to console and filename\n");
  printf("  --debugStdallLog will create (or overwrite an existing) filename\n");
  printf("    NOTE: debugStdallLog will send all stdout and stderr messages to console\n");
  printf("    and filename\n");
  printf("\n");
  exit(0);
}


int unsigned set_bit (int unsigned * number, int unsigned const bit)
{
  *number = *number | (1<<bit);
  return(*number);
}


int unsigned unset_bit (int unsigned * number, int unsigned const bit)
{
  *number = *number & ~(1<<bit);
  return(*number);
}


int write_ (int unsigned const p_level, char const * const p_class, char const * const p_func, char * const p_file, int unsigned const p_line, int unsigned const p_print_flag, char * p_format, ...)
{
  const char *        class_delim = (strlen(p_class)) ? g_class_delim : "";
  char *              class_func;
  char *              new_format;
  char const *        format      = "(%s:%d)";
  char const *        type        = "OTHER";
#ifndef _MSC_VER
  char *              syslog_format;
  int unsigned        priority;
#endif
  int                 count       = 0;
  int unsigned        ifp;
  int unsigned        test_level  = 0;
  debugFunctionType * function    = NULL;
  va_list             vargs;

  if (NULL == g_debug_cnf)
  {
    if (g_print_err_flag & debugPrintErrMsgErrorBit)
    {
      g_print_err_flag |= debugPrintErrMsgErrorBit;
      fprintf(stderr, "in %s, class(%s), func(%s), file(%s), line(%d): %s", __MY_FUNC__, p_class, p_func, p_file, p_line, g_print_err_msg);
    }
    return(-1);
  }

  if (p_print_flag > NUM_FILE_PTR)
  {
    fprintf(stderr, "in %s, class(%s), func(%s), file(%s), line(%d): %d out of range print_flag", __MY_FUNC__, p_class, p_func, p_file, p_line, p_print_flag);
    return(-1);
  }

  test_level = g_debug_cnf->level;
  class_func = (char *)calloc((strlen(p_class) + strlen(p_func) + strlen(class_delim) + 1), sizeof(char));
  if (! class_func)
  {
    PERROR("calloc");
  }
  sprintf(class_func, "%s%s%s", p_class, class_delim, p_func);

  for (function = g_debug_cnf->debug_function_list; function != NULL; function = (debugFunctionType *)function->hh.next)
  {
    if (0 == strcmp(function->function_name, class_func))
    {
      if (0 <= function->level)
      {
        test_level = function->level;
      }
      break;
    }
  }
  free(class_func);

  if (((p_level >= ERROR) && ((p_level % 2) != 0)) && (! (test_level & p_level)) && (p_level != NA))
  {
    return(-1);
  }

  if (p_level <= NDEBUG_LEVELS)
  {
    type = g_debug_levels[p_level];
  }

  va_start(vargs, p_format);
  new_format = debugReplaceNewLines(p_format);
  if (! g_debug_cnf->use_syslog)
  {
    for (ifp = 0; ifp < NUM_FILE_PTR; ++ifp)
    {
      if (NULL == g_debug_cnf->jec[p_print_flag][ifp])
      {
        continue;
      }

      count  = fprintf(g_debug_cnf->jec[p_print_flag][ifp],
                       "%s"NEWLINE_
                       "%s%s"NEWLINE_
                       "%s  %s  %s (line:%d)"NEWLINE_,
                       IO_SPACES,
                       IO_SPACES, (type) ? type : "OTHER",
                       IO_SPACES, FILE_LABEL, p_file, p_line);

      if (p_class && (strlen(p_class) > 0))
      {
        count += fprintf(g_debug_cnf->jec[p_print_flag][ifp],
                         "%s  %s  %s"NEWLINE_,
                         IO_SPACES, CLASS_LABEL, p_class);
      }

      count += fprintf(g_debug_cnf->jec[p_print_flag][ifp],
                       "%s  %s  %s"NEWLINE_
                       "%s  %s  ",
                       IO_SPACES, FUNC_LABEL, (p_func) ? p_func : "<function>",
                       IO_SPACES, MESS_LABEL);
      count += vfprintf(g_debug_cnf->jec[p_print_flag][ifp], new_format, vargs);
      count += fprintf(g_debug_cnf->jec[p_print_flag][ifp], NEWLINE_);
    }
  }
  else
  {
#ifndef _MSC_VER
    priority      = g_debug_cnf->option | g_debug_cnf->facility;
    syslog_format = (char *)malloc((strlen(new_format) + strlen(format) + 10) * sizeof(char));
    if (! syslog_format)
    {
      PERROR("malloc");
    }

    sprintf(syslog_format, "%s - %s", new_format, format);
    syslog(priority, syslog_format, p_file, p_line);
    free(syslog_format);
#endif
  }

  free(new_format);
  va_end(vargs);
  return(count);
}


char * copy_str (char const * const source, char ** destination);
char * copy_str (char const * const source, char ** destination)
{
  if (! destination)
  {
    PERROR("malloc");
  }

  *destination = (char *)malloc((strlen(source) + 1) * sizeof(char));
  if (! *destination)
  {
    PERROR("malloc");
  }

  return(strcpy(*destination, source));
}
