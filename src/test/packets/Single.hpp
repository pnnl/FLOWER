///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


#ifndef SINGLE_HPP
#define SINGLE_HPP


// System Includes
#include <iostream>
#include <locale>
// External Includes
#include <Diagnostic.hpp>
// Internal Includes
// Application Includes


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Single"


// Namespaces
using namespace std;


template <typename T>
class Single
{

public:

  // Variables

  // DEVELOPER NOTE: Keep T value first in list because it could throw
  //                 during construction. If it fails then no harm. If it
  //                 is last in the list, then the other variables could
  //                 create a memory leak. Reference "C++ Cookbook",
  //                 Recipe 9.5 - "Safely Copying an Object"
  T      value;
  bool   valid;


  // Getters/Setters

  inline T getValue(void) const noexcept(true)
  {
    return(this->value);
  }


  inline void setValue(T const & p_value) noexcept(true)
  {
    this->value = p_value;
    return;
  }


  inline bool isValid(void) const noexcept(true)
  {
    return(this->valid);
  }


  inline void setValid(bool const p_valid) noexcept(true)
  {
    this->valid = p_valid;
    return;
  }


  inline void assign(T const & p_value, bool const p_valid) noexcept(true)
  {
    setValue(p_value);
    setValid(p_valid);
    return;
  }


  // Constructors

  Single(T const & p_value, bool const p_valid = false) noexcept(true) :
    value(p_value),
    valid(p_valid)
  {
//printf("Single:%lX:%lX:%lX:%ld\n", &value, &valid, this, sizeof(*this));
    return;
  }


  Single(Single const & p_that) noexcept(true) :
    value(p_that.value),
    valid(p_that.isValid())
  {
    return;
  }


  Single(void) noexcept(true)
  {
    return;
  }


  // Destructor

  ~Single(void) noexcept(true)
  {
    return;
  }


  // Operators

  Single &  operator=(Single const & p_that) noexcept(true)
  {
    if (! (*this == p_that))
    {
      Single<T> tmp(p_that);
      swap(this->valid, tmp.valid);
      swap(this->value, tmp.value);
    }
    return(*this);
  }


  bool      operator==(Single const & p_that) noexcept(true)
  {
    return(
            (isValid()  == p_that.isValid() ) &&
            (getValue() == p_that.getValue())
          );
  }


friend
  ostream & operator<<(ostream & p_os, const Single & p_single) noexcept(true)
  {
    DEBUG(LOTS, ENTER);
    DEBUG(LOTS, LEAVE);
    return(p_os << p_single.value);
  }


private:

  // Constructors


}; 

#endif // SINGLE_HPP
