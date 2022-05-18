/////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


#ifndef DATA_HPP
#define DATA_HPP


// System Includes
#include <stdexcept>
// External Includes
// Internal Includes
// Application Includes
#include "num2string.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Data"


#undef SINGLE_T
#undef PAIR_T
#undef TRIPLE_T
#undef QUAD_T

#define SINGLE_T (1)
#define PAIR_T   (2)
#define TRIPLE_T (3)
#define QUAD_T   (4)


// Namespaces
using namespace std;


template <typename T, char unsigned SIZE = SINGLE_T>
struct Data
{

public:

  typedef T             value_type;
  typedef T *           iterator;
  typedef T *           const_iterator;
  typedef char unsigned size_type;

  // Getters/Setters

  inline bool isValid(void) const noexcept(true)
  {
    return(true);
  }


  iterator begin(void) noexcept(true)
  {
    return(this->value + 0);
  }


  const_iterator begin(void) const noexcept(true)
  {
    return(this->value + 0);
  }


  iterator end(void) noexcept(true)
  {
    return(this->value + SIZE);
  }


  const_iterator end(void) const noexcept(true)
  {
    return(this->value + SIZE);
  }


  T const & at(char unsigned const idx) const
  {
    assert(idx < SIZE);
    if (idx < SIZE)
    {
      return(this->value[idx]);
    }
    //return(*(T*)0);  // Return a null reference to provoke error
    throw(out_of_range("Index: " + to_string(idx) + ", is greater than " + to_string(SIZE)));
  }


  T & at(char unsigned const idx)
  {
    assert(idx < SIZE);
    if (idx < SIZE)
    {
      return(this->value[idx]);
    }
    //return(*(T*)0);  // Return a null reference to provoke error
    throw(out_of_range("Index: " + to_string(idx) + ", is greater than " + to_string(SIZE)));
  }


  inline size_type size(void) const noexcept(true)
  {
    return(SIZE);
  }


  // Functions

  inline void assign(T const & p_value0) noexcept(true)
  {
    this->value[0] = p_value0;
//printf("Data0:%lX\n", &value[0]);
    return;
  }


  inline void assign(T const & p_value0, T const & p_value1) noexcept(true)
  {
    assign(p_value0);
    this->value[1] = p_value1;
//printf("Data1:%lX\n", &value[1]);
    return;
  }


  inline void assign(T const & p_value0, T const & p_value1, T const & p_value2) noexcept(true)
  {
    assign(p_value0, p_value1);
    this->value[2] = p_value2;
//printf("Data2:%lX\n", &value[2]);
    return;
  }


  inline void assign(T const & p_value0, T const & p_value1, T const & p_value2, T const & p_value3) noexcept(true)
  {
    assign(p_value0, p_value1, p_value2);
    this->value[3] = p_value3;
//printf("Data3:%lX\n", &value[3]);
    return;
  }


  // Constructors

  Data(const iterator p_value) noexcept(true)
  {
    for (int unsigned idx = 0; idx < SIZE; ++idx)
    {
      this->value[idx] = p_value[idx];
    }
    return;
  }


  Data(T const & p_value0)
  {
    if (SINGLE_T != SIZE)
    {
      throw(range_error("SINGLE(1) != " + uitoa10(SIZE)));
    }
    assign(p_value0);
    return;
  }


  Data(T const & p_value0, T const & p_value1)
  {
    if (PAIR_T != SIZE)
    {
      throw(range_error("PAIR(2)   != " + uitoa10(SIZE)));
    }
    assign(p_value0, p_value1);
    return;
  }


  Data(T const & p_value0, T const & p_value1,  T const & p_value2)
  {
    if (TRIPLE_T != SIZE)
    {
      throw(range_error("TRIPLE(3) != " + uitoa10(SIZE)));
    }
    assign(p_value0, p_value1, p_value2);
    return;
  }


  Data(T const & p_value0, T const & p_value1, T const & p_value2, T const & p_value3)
  {
    if (QUAD_T != SIZE)
    {
      throw(range_error("QUAD(4)   != " + uitoa10(SIZE)));
    }
    assign(p_value0, p_value1, p_value2, p_value3);
    return;
  }


  Data(Data const & p_that) noexcept(true)
  {
    *this = p_that;
    return;
  }


  Data(void) noexcept(true)
  {
    memset(value, 0, sizeof(value));
    return;
  }


  // Destructor

  ~Data(void) noexcept(true)
  {
    return;
  }


  // Operators

  Data &    operator=(Data const & p_that) noexcept(true)
  {
    if (! (*this == p_that))
    {
      for (int unsigned idx = 0; idx < SIZE; ++idx)
      {
        this->value[idx] = p_that.value[idx];
      }
    }
    return(*this);
  }


  bool      operator==(Data const & p_that) const noexcept(true)
  {
    for (int unsigned idx = 0; idx < SIZE; ++idx)
    {
      if (this->value[idx] != p_that.value[idx])
      {
        return(false);
      }
    }
    return(true);
  }


  T & operator[](char unsigned idx) noexcept(true)
  {
    return(this->value[idx]);
  }


  T const & operator[](char unsigned idx) const noexcept(true)
  {
    return(this->value[idx]);
  }


friend
  ostream & operator<<(ostream & p_os, const Data & p_single) noexcept(true)
  {
    static int unsigned end = SIZE - 1;
    for (int unsigned idx = 0; idx < end; ++idx)
    {
      p_os << p_single.value[idx] << ",";
    }
    return(p_os << p_single.value[end]);
  }


private:

  // Variables

  T value[SIZE];
}; 

#endif // DATA_HPP
