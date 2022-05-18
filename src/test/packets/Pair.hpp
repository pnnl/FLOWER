///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


#ifndef PAIR_HPP
#define PAIR_HPP


// System Includes
#include <iostream>
#include <locale>
// External Includes
// Internal Includes
// Application Includes
#include "Single.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Pair"


// Namespaces
using namespace std;


template <typename T, typename U = T>
class Pair
{

public:

  // Getter/Setters

  inline T getFirstValue(void) const noexcept(true)
  {
    return(this->first.getValue());
  }


  inline U getSecondValue(void) const noexcept(true)
  {
    return(this->second.getValue());
  }


  inline bool isValid(void) const noexcept(true)
  {
    return(this->first.isValid() | this->second.isValid());
  }


  inline void setValid(bool const p_valid) noexcept(true)
  {
    this->first.setValid(p_valid);
    this->second.setValid(p_valid);
    return;
  }


  inline void assign(T const & p_first_value, U const & p_second_value, bool const p_valid) noexcept(true)
  {
    first.setValue(p_first_value);
    second.setValue(p_second_value);
    setValid(p_valid);
    return;
  }


  // Variables

  Single< T > first;
  Single< U > second;


  // Constructors

  Pair(T const & p_first, U const & p_second, bool const p_valid = false) noexcept(true) :
    first(p_first,   p_valid),
    second(p_second, p_valid)
  {
    return;
  }


  Pair(Pair const & p_that) noexcept(true) :
    first(p_that.first),
    second(p_that.second)
  {
    return;
  }


  // Destructor

  ~Pair(void) noexcept(true)
  {
    return;
  }


  // Operators

  Pair &    operator=(Pair const & p_that) noexcept(true)
  {
    if (! (*this == p_that))
    {
      Pair<T, U> tmp(p_that);
      swap(this->first,  tmp.first);
      swap(this->second, tmp.second);
    }
    return(*this);
  }


  bool      operator==(Pair const & p_that) noexcept(true)
  {
    return((this->first == p_that.first) && (this->second == p_that.second));
  }


friend
  ostream & operator<<(ostream & p_os, const Pair & p_pair) noexcept(true)
  {
    DEBUG(LOTS, ENTER);
    DEBUG(LOTS, LEAVE);
    return(p_os << p_pair.first << "," << p_pair.second);
  }


private:

  // Constructors

  Pair(void) noexcept(true);

}; 

#endif // PAIR_HPP
