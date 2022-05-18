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
#include "ItemCounter.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "ItemCounter"


// Namespaces used


//==============================================================================
// Class ItemCounter
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


// ======================================================================
// Constructors
// ======================================================================


ItemCounter::ItemCounter(string const & p_name) noexcept(true) :
  start_time(getUTC()),
  finish_time(start_time),
  item_stats(p_name),
  total_item_stats(p_name)
{
  DEBUG(TRACE, ENTER);
  DEBUG(TRACE, LEAVE);
  return;
}


// ======================================================================
// Public Functions
// ======================================================================


// ======================================================================
// Private Functions
// ======================================================================


