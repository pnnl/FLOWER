///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


#ifndef TIMEUTIL_HPP
#define TIMEUTIL_HPP


// System Includes
#include <climits>
#include <time.h>
// External Includes
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "timeUtil"


// Namespaces

using namespace boost::posix_time;
using namespace boost::gregorian;


// Getters/Setters

inline time_t const getEpoch(void) noexcept(true)
{
  return(0);
}


inline ptime const getEpochUTC(void) noexcept(true)
{
  static ptime epoch(date(1970,1,1));
  return(epoch);
}


inline time_t const getFuture(void) noexcept(true)
{
  return(LONG_MAX);
}


inline ptime const getFutureUTC(void) noexcept(true)
{
  static ptime future(date(2070,1,1));
  return(future);
}


inline time_t const getNow(void) noexcept(true)
{
  time_t now;
  return(time(&now));
}


inline ptime const getUTC(void) noexcept(true)
{
  ptime now(microsec_clock::universal_time());
  return(now);
}


inline u_int64_t getElapsedMilliseconds(void) noexcept(true)
{
  return((u_int64_t)clock());
}

#endif // TIMEUTIL_HPP
