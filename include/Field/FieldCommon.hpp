///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  .  All Rights Reserved.
//
// THIS FILE INITIALLY CREATED WITH:
//     TEMPLATE NAME:  lang_cpp_hdr.template
//     COMMAND NAME:   gensrc
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


#ifndef FIELDCOMMON_HPP
#define FIELDCOMMON_HPP


// System Includes
#include <vector>
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"


struct Elf_t
{
  // Variables
  union
  {
    u_int64_t    qwords[9];
    struct
    {
      u_int16_t  value[32];
      u_int32_t  direction_bitfield;
      u_int8_t   position;
      u_int8_t   current;
      struct
      {
        u_int8_t is_icmp:1;
        u_int8_t is_zero:1;
        u_int8_t is_wrapped:1; // Greater than 32 packets in the flow
      };
      u_int8_t   padding;
    };
  };


  // Constructors

  Elf_t(void) throw()
  {
    clear();
    return;
  }


  Elf_t(u_int16_t const p_value, bool const p_is_icmp) throw()
  {
    clear();
    value[0]           = p_value;
    direction_bitfield = 0x1;
    position           = 0;
    is_icmp            = p_is_icmp;
    is_zero            = false;
    is_wrapped         = false;
    return;
  }


  // Operators 

  inline Elf_t & operator=(Elf_t const & p_elf) throw()
  {
    //memcpy(this, &p_elf, sizeof(Elf_t));  // memcpy is as slow or slower

    for (int unsigned idx = 0; idx < 9; ++idx)
    {
      this->qwords[idx] = p_elf.qwords[idx];
    }

    return(*this);
  }


  inline bool operator==(Elf_t const & p_elf) const throw()
  {
    // memcmp is slower

    u_int8_t equal = 1;
    
    for (int unsigned idx = 0; (idx < 9) & equal; ++idx)
    {
      equal &= (this->qwords[idx] == p_elf.qwords[idx]);
    }

    return(1 == equal);
  }


  inline bool operator!=(Elf_t const & p_elf) const throw()
  {
    return(! (*this == p_elf));
  }


  // Constructors

  Elf_t(Elf_t const & p_elf) throw()
  {
    *this = p_elf;
    return;
  }


  // Functions

  inline void clear(void) throw()
  {
    u_int8_t save = this->is_icmp;
    memset(this, 0, sizeof(Elf_t));               // This is faster
    this->is_icmp = save;

    //for (int unsigned idx = 0; idx < 9; ++idx)  // This is slower
    //{
    //  this->qwords[idx] = 0;
    //}

    return;
  }

};


#endif // FIELDCOMMON_HPP
