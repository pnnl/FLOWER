///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef FIELDMERGEPOLICY_HPP
#define FIELDMERGEPOLICY_HPP


// System Includes
// External Includes
// Internal Includes
// Application Includes
#include "Exception.hpp"
#include "FieldCommon.hpp"
#include "Data.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "FieldMergePolicy"


// Namespaces
using namespace boost;


#undef  UNUSED
#define UNUSED(a)


// ================================================
//
// Merge Policies
//
// ================================================


template <typename T>
struct MergeFlags;

template <>
struct MergeFlags < Data< Elf_t, 1> >
{

  static bool merge(Data< Elf_t, 1 > & p_field1, Data< Elf_t, 1 > const & p_field2, bool const p_direction, bool const p_valid) noexcept(true)
  {

    if (! p_valid)
    {
      return(true);
    }

    // DEVELOPER NOTE: This is a complicated routine but is optimized for performance.
    //                 Changes to this function will potentially affect ToStringFlags as well.

    // Background:
    // Early Late Flags Algorithm: The Early-Late-Flags (Elf) field is a record of the beginning and end TCP
    // or ICMP flags found in packets that make up a flow.  If there are <= 32 packets in a flow, all flags
    // are kept, if ther are > 32 packets, the first 27 and last 5 are kept. Each Flag entry in the Elf is
    // represented by a direction and a value; in the string representation, the direction in the first part
    // of the conversation  are represented by '+'/'-' characters, the last part of the conversation is represented
    // by '>'/'<' characters (if there are more than 32 packets in the flow.

    // Implementation:
    // Internally, the direction flags are represented by single bits in an unsigned 32-bit integer bitfield.  The values
    // are stored in an array of 32 16-bit integers. TCP flags are 8 bits and ICMP flags are 16 bits, so the Elf
    // can represent either.  As new packets come in and they are merged with the existing flow, the flags are written
    // to the Elf array and bit_field in a straight foreward manor.  Once a 33rd packet arrives, the Elf will be full
    // and the existing data must be over written to one of the last five slots.

    // This look up table is used to determine the next index in the array that will be used after 32 packets
    // have been received in this flow.  It is much faster to use this lut and keep track of the last position
    // than it is to shift the last five values in the array and bit_field like a fifo queue.
    // It is faster to build this table on the stack dynamically than to
    // lose locality and fetch it from static global memory
    u_int8_t const lut[32] = {27,  0,  0,  0,  0,  0,  0,  0,
                               0,  0,  0,  0,  0,  0,  0,  0,
                               0,  0,  0,  0,  0,  0,  0,  0,
                               0,  0,  0, 28, 29, 30, 31, 27};

    // TODO: 2009-04-17: Try optimizing by reversing the if/else below because the more common case may be long running flows
    // TODO: 2009-04-17: Try optimizing by using a switch with sequential monotonically increasing cases on p_field1.position
    if (! p_field1.at(0).is_zero)
    {
      if (p_field1.at(0).position < 31)
      {
        // For less than 32 packets, simply store the flag value and direction bit at the index specified by position
        // "position" is only used for the first 32 entries
        p_field1.at(0).value[++p_field1.at(0).position]  = p_field2.at(0).value[0];
        p_field1.at(0).direction_bitfield               |= (u_int32_t)p_direction << p_field1.at(0).position;
        return(true);
      }
      //else
      //{
        // For more than 32 packets, use the lookup table (lut) to determine the next storage index
        // The variable "current" is only used for entries above 32
        // The variable "current" will be initialized to zero, so the first time it is used, the position will be 27
        p_field1.at(0).current                        = lut[p_field1.at(0).current];
        p_field1.at(0).value[p_field1.at(0).current]  = p_field2.at(0).value[0];
        // The bit at the current position must be overwritten, which requires the existing bit to be zeroed
        // by and'ing with it's complement and then being or'ed with its new value
        p_field1.at(0).direction_bitfield             = (p_field1.at(0).direction_bitfield & (~(1 << p_field1.at(0).current))) | ( static_cast<u_int32_t>(p_direction) << p_field1.at(0).current);
        // If we are at index 27, we have wrapped because we have more than 32 packets in this flow
        // The variable "is_wrapped" is needed by ToStringFlags so it knows how to print the direction flags
        if (27 == p_field1.at(0).current)
        {
          p_field1.at(0).is_wrapped = true;
        }
      //}

      return(true);
    }
    // For less than 32 packets, simply store the flag value and direction bit at the index specified by position
    // "position" is only used for the first 32 entries
    p_field1.at(0).value[p_field1.at(0).position]  = p_field2.at(0).value[0];
    p_field1.at(0).direction_bitfield             |= (u_int32_t)p_direction << p_field1.at(0).position;
    // DEVELOPER NOTE: This is needed when a merge occurs after being zeroed
    p_field1.at(0).is_zero = false;

    return(true);
  }

};


template <typename T>
struct MergeBitsum;

template <>
struct MergeBitsum < Data<u_int16_t, 2> >
{

  static bool merge(Data<u_int16_t, 2> & p_field1, Data<u_int16_t, 2> const & p_field2, bool const p_direction, u_int64_t & p_field1_valid1) noexcept(true)
  {
    if (p_direction)
    {
      p_field1.at(0) |= p_field2.at(0);
      return(true);
    }

    p_field1.at(1) |= p_field2.at(0);
      
    // DEVELOPER NOTE: Force the destination to be valid so it will get printed
    p_field1_valid1 = 1;

    return(true);
  }

};


template <typename T>
struct MergeNone
{

  static bool merge(T & p_field1, T const & p_field2, bool const p_direction) noexcept(true)
  {
    UNUSED(p_field1);
    UNUSED(p_field2);
    UNUSED(p_direction);
    return(true);
  }

};


template <typename T>
struct MergePort;

template <>
struct MergePort < Data<u_int16_t, 2> >
{

  static bool merge(Data<u_int16_t, 2> & p_field1, Data<u_int16_t, 2> const & p_field2, bool const p_direction) noexcept(true)
  {
    // DEVELOPER NOTE: This is needed for IPv4 when field 1 is a fragmented flow.
    if ((0 == p_field1.at(0)) && (0 == p_field1.at(1)))
    {
      p_field1.at(0) = p_field2.at(0);
      p_field1.at(1) = p_field2.at(1);
    }
    UNUSED(p_direction);
    return(true);
  }

};


template <typename T>
struct MergeAdd;

template <>
struct MergeAdd < Data<u_int64_t, 2> >
{

  static bool merge(Data<u_int64_t, 2> & p_field1, Data<u_int64_t, 2> const & p_field2, bool const p_direction) noexcept(true)
  {
    p_field1.at(0) += (p_field2.at(0) * (u_int64_t) p_direction);
    p_field1.at(1) += (p_field2.at(0) * (1 - (u_int64_t) p_direction));

    return(true);

  }

};


template <typename T>
struct MergeAddPayload;

template <>
struct MergeAddPayload < Data<u_int64_t, 4> >
{

  static bool merge(Data<u_int64_t, 4> & p_field1, Data<u_int64_t, 4> const & p_field2, bool const p_direction) noexcept(true)
  {

    // DEVELOPER NOTE: Data index 0 and 1 store the accumulated payload byte counts for source and dest
    //                 Data index 2 and 3 store the previous single packet payload byte counts for soruce and dest
    p_field1.at(0) += (p_field2.at(0) * (u_int64_t) p_direction);
    p_field1.at(1) += (p_field2.at(0) * (1 - (u_int64_t) p_direction));

    if (p_direction)
    {
      p_field1.at(2) = p_field2.at(0);
      return(true);
    }

    p_field1.at(3) = p_field2.at(0);

    return(true);

  }

};


template <>
struct MergeAdd < Data<u_int32_t, 4> >
{

  static bool merge(Data<u_int32_t, 4> & p_field1, Data<u_int32_t, 4> const & p_field2, bool const p_direction) noexcept(true)
  {
    UNUSED(p_direction);
    p_field1.at(2) = p_field2.at(2);
    p_field1.at(3) = p_field2.at(3);
    return(true);
  }

};


template <typename T>
struct MergeAdjustTime;

template <>
struct MergeAdjustTime < Data<u_int32_t, 4> >
{

  static bool merge(Data<u_int32_t, 4> & p_field1, Data<u_int32_t, 4> & p_field2) noexcept(true)
  {
    if (p_field2.at(0) < p_field1.at(0))
    {
      u_int32_t temp1 = p_field1.at(1);
      u_int32_t temp0 = p_field1.at(0);
      p_field1.at(1)  = p_field2.at(1);
      p_field1.at(0)  = p_field2.at(0);
      p_field2.at(1)  = temp1;
      p_field2.at(0)  = temp0;

      p_field2.at(2)  = p_field1.at(2);
      p_field2.at(3)  = p_field1.at(3);
      return(true);
    }

    if ((p_field2.at(0) == p_field1.at(0)) && (p_field2.at(1) < p_field1.at(1)))
    {
      u_int32_t temp1 = p_field1.at(1);
      p_field1.at(1)  = p_field2.at(1);
      p_field2.at(1)  = temp1;
    }

    p_field2.at(2)  = p_field1.at(2);
    p_field2.at(3)  = p_field1.at(3);

    return(true);
  }

};


template <typename T>
struct MergeOr
{

  static bool merge(T & p_field1, T const & p_field2, bool const p_direction) noexcept(true)
  {
    UNUSED(p_direction);
    p_field1.at(0) |= p_field2.at(0);
    return(true);
  }

};


template <typename T>
struct MergeInitseq
{

  static bool merge(T & p_field1, T const & p_field2, bool const p_direction, u_int64_t & p_field1_valid1) noexcept(true)
  {
    if (p_direction)
    {
      return(true);
    }

    if (p_field1_valid1 == 0)
    {
      p_field1.at(2)  = p_field2.at(0);
      p_field1.at(3)  = p_field2.at(1);
      p_field1_valid1 = 1;
    }
    return(true);
  }

};


template <typename T>
struct MergeLastseq
{

  static bool merge(T & p_field1, T const & p_field2, bool const p_direction, u_int64_t & p_field1_valid1) noexcept(true)
  {
    if (p_direction)
    {
      p_field1.at(0)  = p_field2.at(0);  // copy new LSSTSV
      p_field1.at(1)  = p_field2.at(1);  // copy new LSSSN
      p_field1_valid1 = 1;

      return(true);
    }

    p_field1.at(2)  = p_field2.at(0);   // copy new LSDTSV
    p_field1.at(3)  = p_field2.at(1);   // copy new LSDSN
    p_field1_valid1 = 1;

    return(true);
  }

};


template <typename T>
struct MergeHistoricalSeq
{

  static bool merge(T & p_hist_tcp_last_seq, Data<u_int32_t, 4> const & p_last_seq_from_wire, bool const p_direction, bool p_has_payload) noexcept(true)
  {
    // DEVELOPER NOTE: We only update these sequence numbers for packets with payload
    if (! p_has_payload)
    {
      return(true);
    }

    if (p_direction)
    {
      p_hist_tcp_last_seq.at(0) = p_last_seq_from_wire.at(0); // store src timestamp
      p_hist_tcp_last_seq.at(1) = p_last_seq_from_wire.at(1); // store src seq num

      return(true);
    }

    p_hist_tcp_last_seq.at(2) = p_last_seq_from_wire.at(0); // store dst timestamp
    p_hist_tcp_last_seq.at(3) = p_last_seq_from_wire.at(1); // store dst seq num

    return(true);
  }

};


template <typename T>
struct MergeTcpRetrans
{

  static bool merge(T & p_field1,
                    bool const p_direction,
                    Data<u_int32_t, 4> const & p_last_seq_in_cache,
                    Data<u_int32_t, 4> const & p_last_seq_from_wire,
                    bool const p_has_payload,
                    u_int64_t p_previous_src_payload,
                    u_int64_t p_previous_dst_payload,
                    Data<u_int32_t, 4> const & p_hist_last_seq
                   ) noexcept(true)
  {
    // Ignore packets without payloads
    if (! p_has_payload)
    {
      return(true);
    }

    if (p_direction)
    {
      // Source

      // *_seq = timestamp << 32 | seq_num
      u_int64_t cache_seq = (u_int64_t)p_last_seq_in_cache.at(0)  << 32 | p_last_seq_in_cache.at(1);
      u_int64_t wire_seq  = (u_int64_t)p_last_seq_from_wire.at(0) << 32 | p_last_seq_from_wire.at(1);
      u_int64_t hist_seq  = (u_int64_t)p_hist_last_seq.at(0)      << 32 | p_hist_last_seq.at(1);
      
      if (0 == p_previous_src_payload && wire_seq > hist_seq)
      {
        return(true);
      }

      // If the sequence number is increasing, there is nothing to to
      if (wire_seq > cache_seq)
      {
        return(true);
      }
      
      // Increment the TCP retransmit count
      ++p_field1.at(0);
      return(true);
    }

    // Destination

    // *_seq = timestamp << 32 | seq_num
    u_int64_t cache_seq = (u_int64_t)p_last_seq_in_cache.at(2)  << 32 | p_last_seq_in_cache.at(3);
    u_int64_t wire_seq  = (u_int64_t)p_last_seq_from_wire.at(0) << 32 | p_last_seq_from_wire.at(1);
    u_int64_t hist_seq  = (u_int64_t)p_hist_last_seq.at(2)      << 32 | p_hist_last_seq.at(3);

    if (0 == p_previous_dst_payload && wire_seq > hist_seq)
    {
      return(true);
    }

    // If the sequence number is increasing, there is nothing to to
    if (wire_seq > cache_seq)
    {
      return(true);
    }

    // Increment the TCP retransmit count
    ++p_field1.at(1);
    return(true);
  }

};


template <typename T>
struct MergeTbd
{

  static bool merge(T & p_field1, T const & p_field2, bool const p_direction) noexcept(true)
  {
    UNUSED(p_field2);
    UNUSED(p_direction);
    CAUTION(IllegalValue, "MergeTbd", "Trying to merge Field with name, " + p_field1.getName());
    return(false);
  }

};


#undef  UNUSED

#endif // FIELDMERGEPOLICY_HPP
