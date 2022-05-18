///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef ITEMCOUNTER_HPP
#define ITEMCOUNTER_HPP

// System Includes
#include <string>
// External Includes
#include <boost/date_time/posix_time/posix_time.hpp>
// Internal Includes
// Application Includes
#include "Stats.hpp"
#include "num2string.hpp"
#include "timeUtil.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "ItemCounter"


// Namespaces
using namespace std;
using namespace boost::posix_time;


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


class ItemCounter
{

public:

  // Getters/Setters

  inline string calculateItemsPerSecond(time_duration const p_duration, u_int64_t const p_count) const noexcept(true)
  {
    double denom = (p_duration.total_seconds() + (p_duration.fractional_seconds() / 1000000.0));
    if (denom > 0.0)
    {
      return(dtoa((p_count / denom), 2));
    }
    return("0");
  }


  inline ptime getStartTime(void) const noexcept(true)
  {
    return(this->start_time);
  }


  inline ptime getFinishTime(void) const noexcept(true)
  {
    return(this->finish_time);
  }


  u_int64_t getItemCount(void) const noexcept(true)
  {
    return(this->item_stats.getCount());
  }


  u_int64_t getTotalItemCount(void) const noexcept(true)
  {
    return(this->total_item_stats.getCount());
  }


  inline string getItemProcessingTime(void) const noexcept(true)
  {
    return(to_simple_string(getRunTime()));
  }


  inline string getTotalItemProcessingTime(void) const noexcept(true)
  {
    return(to_simple_string(getTotalRunTime()));
  }


  string getItemsPerSecond(void) const noexcept(true)
  {
    return(calculateItemsPerSecond(getRunTime(), getItemCount()));
  }


  string getTotalItemsPerSecond(void) const noexcept(true)
  {
    return(calculateItemsPerSecond(getTotalRunTime(), getTotalItemCount()));
  }


  inline time_duration getRunTime(void) const noexcept(true)
  {
    return(this->run_time);
  }


  inline time_duration getTotalRunTime(void) const noexcept(true)
  {
    return(this->total_run_time);
  }


  inline void incrItemStats(void) noexcept(true)
  {
    ++this->item_stats;
    ++this->total_item_stats;
    return;
  }


  inline void incrItemStats(u_int64_t const p_count) noexcept(true)
  {
    this->item_stats       += p_count;
    this->total_item_stats += p_count;
    return;
  }


  inline void resetItem(void) noexcept(true)
  {
    this->item_stats.reset();
    return;
  }


  inline void resetItemTotal(void) noexcept(true)
  {
    this->total_item_stats.reset();
    return;
  }


  inline void setFinishTime(bool const p_update_duration = true) noexcept(true)
  {
    this->finish_time     = getUTC();
    if (p_update_duration)
    {
      this->run_time        = getFinishTime() - getStartTime();
      this->total_run_time += getRunTime();
    }
    return;
  }


  inline void setStartTime(void) noexcept(true)
  {
    this->start_time = getUTC();
    return;
  }


  // Constructors

  ItemCounter(string const & p_name) noexcept(true);


  // Destructor

  ~ItemCounter(void) noexcept(true)
  {
    return;
  }


private:

  // Constructors

  ItemCounter(void) noexcept(true);
  ItemCounter(ItemCounter const & p_item_counter) noexcept(true);


  // Operators

  ItemCounter &   operator=(ItemCounter const & p_item_counter) noexcept(true);
  bool           operator==(ItemCounter const & p_item_counter) const noexcept(true);
  bool           operator!=(ItemCounter const & p_item_counter) const noexcept(true);
  

  // Variables

  ptime            start_time;
  ptime            finish_time;
  time_duration    run_time;
  time_duration    total_run_time;
  time_t           virtual_time;
  Stats<u_int64_t> item_stats;
  Stats<u_int64_t> total_item_stats;

};

#endif // ITEMCOUNTER_HPP
