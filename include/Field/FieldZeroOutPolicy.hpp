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


#ifndef FIELDZEROOUTPOLICY_HPP
#define FIELDZEROOUTPOLICY_HPP


// System Includes
#include <string>
// External Includes
#include <Diagnostic.hpp>
// Internal Includes
// Application Includes
#include "Data.hpp"
#include "FieldCommon.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "FieldZeroOutPolicy"


// Namespaces used
using namespace std;


#undef  UNUSED
#define UNUSED(a)


// ================================================
//
// ZeroOut Policies
//
// ================================================


template <typename T>
struct ZeroOutTbd
{

  static void zeroOut(T & p_data) throw()
  {
    DEBUG(INFO, "ZeroOutTbd - Not Implemented Yet");
    UNUSED(p_data);
    return;
  }

};


template <typename T>
struct ZeroOutNone
{

  static void zeroOut(T & p_data) throw()
  {
    UNUSED(p_data);
    return;
  }

};


template <typename T>
struct ZeroOut;

template <>
struct ZeroOut < Data< Elf_t, 1> >
{

  static void zeroOut(Data< Elf_t, 1> & p_data) throw()
  {
    // DEVELOPER NOTE: The bool defines whether the field is ICMP or TCP
    p_data.at(0).clear();
    p_data.at(0).is_zero = true;
    return;
  }

};


template <>
struct ZeroOut < Data<u_int32_t, 4> >
{

  static void zeroOut(Data<u_int32_t, 4> & p_data) throw()
  {
    // DEVELOPER NOTE:
    //  Set the start time to the end time of the last flow so
    //  that the next flow record will accurately reflect the
    //  duration of all the flows in since the last force out
    //  of the data to file. The idea is that you should be able
    //  to take all the Flows across the files and sum up the
    //  durations to get the total duration of the Flows representing
    //  a long running Flow that spans files.
    p_data.at(0) = p_data.at(2);
    p_data.at(1) = p_data.at(3);
    return;
  }

};


template <>
struct ZeroOut < Data<u_int8_t, 1> >
{

  static void zeroOut(Data<u_int8_t, 1> & p_data) throw()
  {
    p_data.at(0) = 0;
    return;
  }

};


template <>
struct ZeroOut < Data<u_int32_t, 1> >
{

  static void zeroOut(Data<u_int32_t, 1> & p_data) throw()
  {
    p_data.at(0) = 0;
    return;
  }

};


template <>
struct ZeroOut < Data<u_int16_t, 2> >
{

  static void zeroOut(Data<u_int16_t, 2> & p_data) throw()
  {
    p_data.at(0) = 0;
    p_data.at(1) = 0;
    return;
  }

};


template <>
struct ZeroOut < Data<u_int64_t, 2> >
{

  static void zeroOut(Data<u_int64_t, 2> & p_data) throw()
  {
    p_data.at(0) = 0;
    p_data.at(1) = 0;
    return;
  }

};


template <>
struct ZeroOut < Data<u_int64_t, 4> >
{

  static void zeroOut(Data<u_int64_t, 4> & p_data) throw()
  {
    p_data.at(0) = 0;
    p_data.at(1) = 0;
    p_data.at(2) = 0;
    p_data.at(3) = 0;
    return;
  }

};


template <typename T>
struct ZeroOutSeq;

template <>
struct ZeroOutSeq < Data<u_int32_t, 2> >
{

  static void zeroOut(Data<u_int32_t, 2> & p_data) throw()
  {
    p_data.at(0) = 0;
    p_data.at(1) = 0;
    return;
  }

};

template <>
struct ZeroOutSeq < Data<u_int32_t, 4> >
{

  static void zeroOut(Data<u_int32_t, 4> & p_data) throw()
  {
    p_data.at(0) = 0;
    p_data.at(1) = 0;
    p_data.at(2) = 0;
    p_data.at(3) = 0;
    return;
  }

};

#undef  UNUSED

#endif // FIELDZEROOUTPOLICY_HPP
