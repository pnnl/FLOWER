/////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
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

  inline bool isValid(void) const throw()
  {
    return(true);
  }


  iterator begin(void) throw()
  {
    return(this->value + 0);
  }


  const_iterator begin(void) const throw()
  {
    return(this->value + 0);
  }


  iterator end(void) throw()
  {
    return(this->value + SIZE);
  }


  const_iterator end(void) const throw()
  {
    return(this->value + SIZE);
  }


  T const & at(char unsigned const idx) const throw(out_of_range)
  {
    assert(idx < SIZE);
    if (idx < SIZE)
    {
      return(this->value[idx]);
    }
    //return(*(T*)0);  // Return a null reference to provoke error
    throw(out_of_range("Index: " + to_string(idx) + ", is greater than " + to_string(SIZE)));
  }


  T & at(char unsigned const idx) throw(out_of_range)
  {
    assert(idx < SIZE);
    if (idx < SIZE)
    {
      return(this->value[idx]);
    }
    //return(*(T*)0);  // Return a null reference to provoke error
    throw(out_of_range("Index: " + to_string(idx) + ", is greater than " + to_string(SIZE)));
  }


  inline size_type size(void) const throw()
  {
    return(SIZE);
  }


  // Functions

  inline void assign(T const & p_value0) throw()
  {
    this->value[0] = p_value0;
//printf("Data0:%lX\n", &value[0]);
    return;
  }


  inline void assign(T const & p_value0, T const & p_value1) throw()
  {
    assign(p_value0);
    this->value[1] = p_value1;
//printf("Data1:%lX\n", &value[1]);
    return;
  }


  inline void assign(T const & p_value0, T const & p_value1, T const & p_value2) throw()
  {
    assign(p_value0, p_value1);
    this->value[2] = p_value2;
//printf("Data2:%lX\n", &value[2]);
    return;
  }


  inline void assign(T const & p_value0, T const & p_value1, T const & p_value2, T const & p_value3) throw()
  {
    assign(p_value0, p_value1, p_value2);
    this->value[3] = p_value3;
//printf("Data3:%lX\n", &value[3]);
    return;
  }


  // Constructors

  Data(const iterator p_value) throw()
  {
    for (int unsigned idx = 0; idx < SIZE; ++idx)
    {
      this->value[idx] = p_value[idx];
    }
    return;
  }


  Data(T const & p_value0) throw(range_error)
  {
    if (SINGLE_T != SIZE)
    {
      throw(range_error("SINGLE(1) != " + uitoa10(SIZE)));
    }
    assign(p_value0);
    return;
  }


  Data(T const & p_value0, T const & p_value1) throw(range_error)
  {
    if (PAIR_T != SIZE)
    {
      throw(range_error("PAIR(2)   != " + uitoa10(SIZE)));
    }
    assign(p_value0, p_value1);
    return;
  }


  Data(T const & p_value0, T const & p_value1,  T const & p_value2) throw(range_error)
  {
    if (TRIPLE_T != SIZE)
    {
      throw(range_error("TRIPLE(3) != " + uitoa10(SIZE)));
    }
    assign(p_value0, p_value1, p_value2);
    return;
  }


  Data(T const & p_value0, T const & p_value1, T const & p_value2, T const & p_value3) throw(range_error)
  {
    if (QUAD_T != SIZE)
    {
      throw(range_error("QUAD(4)   != " + uitoa10(SIZE)));
    }
    assign(p_value0, p_value1, p_value2, p_value3);
    return;
  }


  Data(Data const & p_that) throw()
  {
    *this = p_that;
    return;
  }


  Data(void) throw()
  {
    memset(value, 0, sizeof(value));
    return;
  }


  // Destructor

  ~Data(void) throw()
  {
    return;
  }


  // Operators

  Data &    operator=(Data const & p_that) throw()
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


  bool      operator==(Data const & p_that) const throw()
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


  T & operator[](char unsigned idx) throw()
  {
    return(this->value[idx]);
  }


  T const & operator[](char unsigned idx) const throw()
  {
    return(this->value[idx]);
  }


friend
  ostream & operator<<(ostream & p_os, const Data & p_single) throw()
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
