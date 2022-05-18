///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_class.template 
//     COMMAND NAME:   gensrc 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// System Includes
// External Includes
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
// Internal Includes
// Application Includes
#include "date2string.hpp"
#include "num2string.hpp"
#include "Exception.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "date2string"


// Namespaces
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;


// Getters/Setters

string & getDateStringAll(time_t const p_time, string & p_result) noexcept(true)
{
  DEBUG(LOTS, ENTER);
  ptime         ts   = from_time_t(p_time);
  date          date = ts.date();
  time_duration time = ts.time_of_day();

  p_result = uitoa10((((date.year()  * 100) + date.month())   * 100) + date.day())
           + uitoa10((((time.hours() * 100) + time.minutes()) * 100) + time.seconds(), 6)
           +  '.'
           + uitoa10(ts.time_of_day().fractional_seconds(), 6);

  DEBUG(LOTS, LEAVE);
  return(p_result);
}


string & getDateString(time_t const p_time, string & p_format, string & p_result) noexcept(true)
{
  string::iterator itor;
  string::iterator itor_end = p_format.end();
  ptime            ts       = from_time_t(p_time);
  date             date     = ts.date();
  time_duration    time     = ts.time_of_day();

  for (itor = p_format.begin(); itor != itor_end; ++itor)
  {
    if ('Y' == (*itor))
    {
      p_result += uitoa10(static_cast<int unsigned>(date.year()), 4);
      continue;
    }
    if ('M' == (*itor))
    {
      p_result += uitoa10(static_cast<int unsigned>(date.month()), 2);
      continue;
    }
    if ('D' == (*itor))
    {
      p_result += uitoa10(static_cast<int unsigned>(date.day()), 2);
      continue;
    }
    if ('h' == (*itor))
    {
      p_result += uitoa10(static_cast<int unsigned>(time.hours()), 2);
      continue;
    }
    if ('m' == (*itor))
    {
      p_result += uitoa10(static_cast<int unsigned>(time.minutes()), 2);
      continue;
    }
    if ('s' == (*itor))
    {
      p_result += uitoa10(static_cast<int unsigned>(time.seconds()), 2);
      continue;
    }
    if ('u' == (*itor))
    {
      p_result += uitoa10(static_cast<int unsigned>(time.fractional_seconds()), 6);
      continue;
    }
    p_result += (*itor);
  }

  return(p_result);
}
