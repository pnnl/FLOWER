///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef STATS_HPP
#define STATS_HPP


// System Includes
#include <iostream>
#include <string>
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Stats"


// Namespaces
using namespace std;


//==============================================================================
// Class Stats
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


template <class T>
class Stats
{

public:

  // Getters/Setters

  inline string getName(void) const throw()
  {
    if (this->name.empty())
    {
      return("UNDEFINED");
    }
    return(this->name);
  }


  inline u_int64_t getCount(void) const throw()
  {
    return(this->count);
  }


  inline T getTotal(void) const throw()
  {
    return(this->total);
  }


  // Constructors

  Stats(string const & p_name, u_int64_t const p_count = 0, T const p_total = 0) throw() :
    name(p_name),
    count(p_count),
    total(p_total)
  {
    return;
  }


  // Destructor

  ~Stats(void) throw()
  {
    return;
  }


  // Operators

  Stats &    operator++(void) throw()          // prefix
  {
    this->push(1);
    return(*this);
  }


  Stats &    operator+=(T const & p_count) throw()
  {
    this->push(p_count);
    return(*this);
  }


friend
  ostream &  operator<<(ostream & p_os, const Stats & p_stats) throw()
  {
    return(p_os << p_stats.getName() << "," << p_stats.getCount() << "," << p_stats.getTotal());
  }


  // Public Functions

  inline void reset(void) throw()
  {
    this->count = 0;
    this->total = 0;
  }


  inline void push(T const & p_value = 0) throw()
  {
    // DEVELOPER NOTE: 2009/02/09
    //                 Plug the following into Google to see that we won't overflow
    //                 for 54 Years at 10 Gigabits Ethernet capture at full capacity:
    //                 (((2^64 - 1) bytes)) / (10 GBps) = 
    //u_int64_t check;
    //check = 18446744073709551615 - getTotal();
    //check = ULONG_MAX - getTotal();
    //if (! (check > p_value))
    //{
    //  CAUTION(RangeError, "Cannot add " + uitoa10(p_value) + " to Stats(" + this->getName() + ")", "Current value is " + uitoa10(getTotal()) + ", resetting to 0");
    //  this->reset();
    //}
    this->total += p_value;
    ++this->count;
    return;
  }


  inline T average(void) throw()
  {
    T avg = (T)0;

    if (0 < getCount())
    {
      avg = getTotal() / getCount();
    }
    return(avg);
  }


private:

  // Constructors

  Stats(void) throw();
  Stats(Stats<T> const & p_stats) throw();


  // Operators

  Stats<T> & operator=(Stats<T> const & p_stats) throw();
  bool       operator==(Stats const & p_stats) const throw();
  bool       operator!=(Stats const & p_stats) const throw();
  Stats &    operator++(int p_count) throw()   // postfix
  {
    this->push(p_count);
    return(*this);
  }



  // Variables

  string    name;
  u_int64_t count;
  T         total;

};

#endif // STATS_HPP
