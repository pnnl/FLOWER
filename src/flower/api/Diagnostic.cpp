///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  PNNL.  All Rights Reserved. 
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_class.template 
//     COMMAND NAME:   gensrc 
//
// REPOSITORY INFORMATION: 
//  $Revision: 1.8 $ 
//  $Author: younkin $ 
//  $Date: 2008-06-11 20:35:07 $ 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
//  
///////////////////////////////////////////////////////////////////////////////

// System Includes
#include <assert.h>
#include <iostream>
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"
#include "Diagnostic.hpp"

// Namespaces

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

map<string, string> * Diagnostic::error_codes = 0;

// ======================================================================
// Get/Set Pairs
// ======================================================================


// ======================================================================
// Constructors
// ======================================================================
Diagnostic::Diagnostic(void)
{
  // HACK
  //debugStart(0, NULL, NULL, NULL, 0, 0, 0);
  this->my_argc           = 0;
  this->my_argv           = 0;
  this->internal_alloc    = true;
  Diagnostic::error_codes = new map<string, string>();
  return;
}


Diagnostic::Diagnostic(int & p_argc, char ** p_argv, int const & p_option, int const & p_facility)
{
  this->internal_alloc    = true;

  Diagnostic::error_codes = new map<string, string>();
  if (NULL != p_argv)
  {
    // HACK
    //debugStartCLI(p_argc, p_argv, &this->my_argc, &this->my_argv, p_option, p_facility);
  }
  else
  {
    // HACK
    //debugStart(0, NULL, NULL, NULL, p_option, p_facility, 0);
  }
  return;
}


Diagnostic::Diagnostic(int & p_argc, char ** p_argv, map<string, string> * p_error_codes, int const & p_option, int const & p_facility)
{
  this->internal_alloc      = false;

  if (NULL != p_argv)
  {
    // HACK
    //debugStartCLI(p_argc, p_argv, &this->my_argc, &this->my_argv, p_option, p_facility);
  }
  else
  {
    // HACK
    //debugStart(0, NULL, NULL, NULL, p_option, p_facility, 0);
  }

  if (NULL != p_error_codes)
  {
    Diagnostic::error_codes = p_error_codes;
  }
  else
  {
    this->internal_alloc    = true;
    Diagnostic::error_codes = new map<string, string>();
  }

  return;
}

// ======================================================================
// Destructor
// ======================================================================


Diagnostic::~Diagnostic(void)
{
  // HACK
  //debugEnd();
  if (true == this->internal_alloc)
  {
    delete Diagnostic::error_codes;
  }
  return;
}


// ======================================================================
// Operators
// ======================================================================


// ======================================================================
// Other Public Functions
// ======================================================================


// ======================================================================
// Static Functions
// ======================================================================
const string Diagnostic::caution(
   string const & p_ecode,
   string const & p_entity,
   string const & p_context,
   const char *   p_class,
   const char *   p_func,
   const char *   p_file,
   int const      p_line
   )
{
  assert(! p_ecode.empty());
  string item = Diagnostic::get_mesg_string(p_ecode, p_entity);
  ::message(p_ecode.c_str(), item.c_str(), p_context.c_str(), p_class, p_func, p_file, p_line, EXIT_SUCCESS);
  return(p_ecode);
}


const string Diagnostic::error(
   string const & p_ecode,
   string const & p_entity,
   string const & p_context,
   const char *   p_class,
   const char *   p_func,
   const char *   p_file,
   int const      p_line
   )
{
  string item = Diagnostic::get_mesg_string(p_ecode, p_entity);
  ::message(p_ecode.c_str(), item.c_str(), p_context.c_str(), p_class, p_func, p_file, p_line, EXIT_SUCCESS);
  return(p_ecode);
}


const string Diagnostic::fatal(
   string const & p_ecode,
   string const & p_entity,
   string const & p_context,
   const char *   p_class,
   const char *   p_func,
   const char *   p_file,
   int const      p_line
   )
{
  string item = Diagnostic::get_mesg_string(p_ecode, p_entity);
  ::message(p_ecode.c_str(), item.c_str(), p_context.c_str(), p_class, p_func, p_file, p_line, EXIT_FAILURE);
  return(p_ecode);
}


// ======================================================================
// Protected Functions
// ======================================================================


// ======================================================================
// Private Functions
// ======================================================================
string Diagnostic::get_mesg_string(string const & p_ecode, string const & p_entity)
{
  string item;
  map<string, string>::iterator iter = Diagnostic::error_codes->find(p_ecode);

  if (p_entity.empty())
  {
    item = "";
  }
  else if (iter != Diagnostic::error_codes->end())
  {
    item = (*iter).second + ":  " + p_entity;
  }
  else
  {
    item = "Unknown Error Code:" + p_ecode + "\n    Problem with:  " + p_entity;
  }

  return(item);
}
