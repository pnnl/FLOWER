///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef FIXEDARRAY_HPP
#define FIXEDARRAY_HPP


// System Includes
#include <assert.h>
#include <memory.h>
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "FixedArray"


//==============================================================================
// Class FixedArray
//
// API Developer Documentation:   (those who develop/fix this class)
//   This is a light-weight wrapper interface for a C array;
//   no memory is allocated, rather it operates on an existing buffer.
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


template <typename T>
class FixedArray
{
public:
  
  // Getters/Setters

  inline int unsigned size(void) const noexcept(true)
  {
    return(getCount());
  }


  inline int unsigned capacity(void) const noexcept(true)
  {
    return(getCount());
  }


  inline T at(int unsigned p_idx) noexcept(true)
  {
    assert(p_idx < getCount());
    return(buffer[p_idx]);
  }


  inline T const at(int unsigned p_idx) const noexcept(true)
  {
    assert(p_idx < getCount());
    return(buffer[p_idx]);
  }


  // DEVELOPER NOTE: The at2 and at4 functions assume network byte order
  //                 and x86 hardware platform.
  inline u_int16_t at2(int unsigned p_idx) noexcept(true)
  {
    assert((p_idx + 1) < getCount());
    return((buffer[p_idx] << 8) + buffer[p_idx + 1]);
  }


  inline u_int16_t const at2(int unsigned p_idx) const noexcept(true)
  {
    assert((p_idx + 1) < getCount());
    return((buffer[p_idx] << 8) + buffer[p_idx + 1]);
  }


  inline u_int32_t at4(int unsigned p_idx) noexcept(true)
  {
    assert((p_idx + 3) < getCount());
    return((buffer[p_idx] << 24) + (buffer[p_idx + 1] << 16) + (buffer[p_idx + 2] << 8) + buffer[p_idx + 3]);
  }


  inline u_int32_t const at4(int unsigned p_idx) const noexcept(true)
  {
    assert((p_idx + 3) < getCount());
    return((buffer[p_idx] << 24) + (buffer[p_idx + 1] << 16) + (buffer[p_idx + 2] << 8) + buffer[p_idx + 3]);
  }


  inline void assign(T * p_data, int unsigned p_size) noexcept(true)
  {
    //assert(p_data);
    this->buffer = p_data;
    this->count  = p_size;
  }


  // Constructor

  FixedArray(T * p_data, int unsigned p_size) noexcept(true) :
    buffer(p_data),
    count(p_size)
  {
    return;
  }


  FixedArray(FixedArray<T> const & p_array) noexcept(true) :
    buffer(p_array.buffer),
    count(p_array.count)
  {
    return;
  }


  // Destructors
 
  ~FixedArray(void) noexcept(true)
  {
    return;
  }


  // Operators

  bool      operator==(FixedArray<T> const & p_that) const noexcept(true)
  {
    if (this->size() == p_that.size())
    {
      return(0 == memcmp(this->getBuffer(),p_that.getBuffer(),this->size()));
    }

    return(false);
  }


  static void hexdump(ostream &o, FixedArray const & p_array)
  {
    // DEVELOPER NOTE:
    //  Adapted from: http://www.cpax.org.uk/prg/windows/structstor.cpp
    int unsigned       size   = p_array.size();
    T *                base   = p_array.getBuffer();
    int unsigned const indent = 4;
    string             literal("");
    string const       hexdigits("0123456789ABCDEF");
    int unsigned       idx;

    for (idx = 0; idx < size; ++idx)
    {
      if (0 == idx % 16)
      {
        if (0 != idx)
        {
          o << "  " << literal;
          literal = "";
          o << "\n";
        }
        for (int unsigned j = 0; j < indent; ++j)
        {
          o << ' ';
        }
      }

      if (isprint(base[idx]))
      {
        literal += base[idx];
      }
      else
      {
        literal += '.';
      }
      if (0 == idx % 16)
      {
        // print offset
        o << hexdigits[(idx & 0xF00) >> 8];
        o << hexdigits[(idx & 0x0F0) >> 4];
        o << hexdigits[(idx & 0x00F)     ];
        o << "  ";
      }

      // print hex data
      o << hexdigits[(base[idx] & 0xF0) >> 4];
      o << hexdigits[(base[idx] & 0x0F)];
      o << ' ';
    }

    idx %= 16;
    while (idx % 16 != 0)
    {
      o << "   ";
      idx++;
    }

    o << "  " << literal;
  }


friend
  ostream & operator<<(ostream & p_os, FixedArray<T> const & p_array) noexcept(true)
  {
    hexdump(p_os, p_array);
    return(p_os);
  }



  // Getters/Setters

  inline int unsigned getCount(void) const noexcept(true)
  {
    return(this->count);
  }


  inline T * getBuffer(void) const noexcept(true)
  {
    return(this->buffer);
  }
  
private:

  // Constructors

  FixedArray(void) noexcept(true);


  // Variables

  T *          buffer;
  int unsigned count;

};

typedef FixedArray<const char unsigned> ByteArray;

#endif // FIXEDARRAY_HPP
