///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
// _____________________________________________________________________________
//
// PURPOSE OF TEST: 
//  [put a single sentence stating the purpost of this file 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////

// DEVELOPER NOTE: This must be defined before including Boost unit_test.hpp
#define BOOST_TEST_MODULE Elf

// System Includes
// External Includes
#include <boost/test/included/unit_test.hpp>
// Internal Includes
// Application Includes
#include "Field/Data.hpp"
#include "Field/FieldCommon.hpp"
#include "Field/FieldMergePolicy.hpp"
#include "Field/FieldToStringPolicy.hpp"
#include "Field/FieldZeroOutPolicy.hpp"

struct ElfSuiteFixture
{
  
  ElfSuiteFixture()
  {
    BOOST_TEST_MESSAGE("ElfSuite setup fixture");
    return;
  }


  ~ElfSuiteFixture()
  {
    BOOST_TEST_MESSAGE("ElfSuite teardown fixture");
    return;
  }

};


BOOST_FIXTURE_TEST_SUITE(ElfSuite, ElfSuiteFixture)


BOOST_AUTO_TEST_CASE(elf_construct_void)
{
  // Pre-condition: 
  // Condition: 

  Elf_t           e;

  // Post-condition:

  BOOST_CHECK_EQUAL(e.is_zero           , false);

  return;
}


BOOST_AUTO_TEST_CASE(elf_construct_tcp)
{
  // Pre-condition: 
  // Condition: 

  Elf_t           e(0x99,false);

  // Post-condition:

  BOOST_CHECK_EQUAL(e.value[0]          , 0x99);
  BOOST_CHECK_EQUAL(e.direction_bitfield,  0x1);
  BOOST_CHECK_EQUAL(e.position          ,    0);
  BOOST_CHECK_EQUAL(e.is_icmp           ,false);
  BOOST_CHECK_EQUAL(e.is_zero           ,false);
  BOOST_CHECK_EQUAL(e.is_wrapped        ,false);

  return;
}


BOOST_AUTO_TEST_CASE(elf_construct_icmp)
{
  // Pre-condition: 
  // Condition: 

  Elf_t           e(0x8899,1);

  // Post-condition:

  BOOST_CHECK_EQUAL(e.value[0]          ,0x8899);
  BOOST_CHECK_EQUAL(e.direction_bitfield,   0x1);
  BOOST_CHECK_EQUAL(e.position          ,     0);
  BOOST_CHECK_EQUAL(e.is_icmp           ,  true);
  BOOST_CHECK_EQUAL(e.is_zero           , false);
  BOOST_CHECK_EQUAL(e.is_wrapped        , false);

  return;
}


BOOST_AUTO_TEST_CASE(elf_zero)
{
  // Pre-condition: 
  Elf_t          e(0x8899,1);
  Data<Elf_t, 1> d(e);

  BOOST_CHECK_EQUAL(d.at(0).value[0]          ,0x8899);
  BOOST_CHECK_EQUAL(d.at(0).direction_bitfield,   0x1);
  BOOST_CHECK_EQUAL(d.at(0).position          ,     0);
  BOOST_CHECK_EQUAL(d.at(0).is_icmp           ,  true);
  BOOST_CHECK_EQUAL(d.at(0).is_zero           , false);
  BOOST_CHECK_EQUAL(d.at(0).is_wrapped        , false);

  // Condition: 
  ZeroOut< Data<Elf_t, 1> >::zeroOut(d);

  // Post-condition:

  BOOST_CHECK_EQUAL(d.at(0).value[0]          ,   0x0);
  BOOST_CHECK_EQUAL(d.at(0).direction_bitfield,   0x0);
  BOOST_CHECK_EQUAL(d.at(0).position          ,     0);
  BOOST_CHECK_EQUAL(d.at(0).is_zero           ,  true);
  BOOST_CHECK_EQUAL(d.at(0).is_wrapped        , false);

  return;
}

BOOST_AUTO_TEST_CASE(elf_tostring_tcp)
{
  // Pre-condition: 
  Elf_t          e(0x99,false);

  // Condition
  string elf_string;

  ToStringFlags< Data<Elf_t, 1> >::toString(e,elf_string,1);

  // Post-condition:
  BOOST_CHECK_EQUAL(elf_string,",+99");

  return;
}

BOOST_AUTO_TEST_CASE(elf_tostring_icmp)
{
  // Pre-condition: 
  Elf_t          e(0x8899,1);

  // Condition
  string elf_string;

  ToStringFlags< Data<Elf_t, 1> >::toString(e,elf_string,1);

  // Post-condition:

  BOOST_CHECK_EQUAL(elf_string,",+8899");

  return;
}

BOOST_AUTO_TEST_CASE(elf_merge_tcp)
{
  // Pre-condition:
  Elf_t          e00(0x00,false);
  Elf_t          e01(0x01,false);
  Data<Elf_t, 1> d00(e00);
  Data<Elf_t, 1> d01(e01);

  // Condition:
  MergeFlags< Data<Elf_t, 1> >::merge(d00,d01,false,true);

  // Post-condition:
  string elf_string;

  ToStringFlags< Data<Elf_t, 1> >::toString(d00,elf_string,1);

  BOOST_CHECK_EQUAL(elf_string,",+00-01");
}

BOOST_AUTO_TEST_CASE(elf_merge_icmp)
{
  // Pre-condition:
  Elf_t          e00(0x00,true);
  Elf_t          e01(0x01,true);
  Data<Elf_t, 1> d00(e00);
  Data<Elf_t, 1> d01(e01);

  // Condition:
  MergeFlags< Data<Elf_t, 1> >::merge(d00,d01,false,true);

  // Post-condition:
  string elf_string;

  ToStringFlags< Data<Elf_t, 1> >::toString(d00,elf_string,1);

  BOOST_CHECK_EQUAL(elf_string,",+0000-0001");
}

BOOST_AUTO_TEST_CASE(elf_merge_31_tcp)
{
  // Pre-condition
  Elf_t          elf_target(0x00,false);
  Elf_t          elf_newbie(0x01,false);
  Data<Elf_t, 1> target(elf_target);
  Data<Elf_t, 1> newbie(elf_newbie);

  // Condition:
  for (int i=1;i<31;++i)
  {
    newbie.at(0).value[0] = i;
    MergeFlags< Data<Elf_t, 1> >::merge(target,newbie,!(i%2),true);
  }

  // Post-condition:
  string elf_string;

  ToStringFlags< Data<Elf_t, 1> >::toString(target,elf_string,1);

  BOOST_CHECK_EQUAL(elf_string,",+00-01+02-03+04-05+06-07+08-09+0A-0B+0C-0D+0E-0F+10-11+12-13+14-15+16-17+18-19+1A-1B+1C-1D+1E");
}

BOOST_AUTO_TEST_CASE(elf_merge_32_tcp)
{
  // Pre-condition
  Elf_t          elf_target(0x00,false);
  Elf_t          elf_newbie(0x01,false);
  Data<Elf_t, 1> target(elf_target);
  Data<Elf_t, 1> newbie(elf_newbie);

  // Condition:
  for (int i=1;i<32;++i)
  {
    newbie.at(0).value[0] = i;
    MergeFlags< Data<Elf_t, 1> >::merge(target,newbie,!(i%2),true);
  }

  // Post-condition:
  string elf_string;

  ToStringFlags< Data<Elf_t, 1> >::toString(target,elf_string,1);

  BOOST_CHECK_EQUAL(elf_string,",+00-01+02-03+04-05+06-07+08-09+0A-0B+0C-0D+0E-0F+10-11+12-13+14-15+16-17+18-19+1A-1B+1C-1D+1E-1F");
}

BOOST_AUTO_TEST_CASE(elf_merge_33_tcp)
{
  // Pre-condition
  Elf_t          elf_target(0x00,false);
  Elf_t          elf_newbie(0x01,false);
  Data<Elf_t, 1> target(elf_target);
  Data<Elf_t, 1> newbie(elf_newbie);

  // Condition:
  for (int i=1;i<33;++i)
  {
    newbie.at(0).value[0] = i;
    MergeFlags< Data<Elf_t, 1> >::merge(target,newbie,!(i%2),true);
  }

  // Post-condition:
  string elf_string;

  ToStringFlags< Data<Elf_t, 1> >::toString(target,elf_string,1);

  BOOST_CHECK_EQUAL(elf_string,",+00-01+02-03+04-05+06-07+08-09+0A-0B+0C-0D+0E-0F+10-11+12-13+14-15+16-17+18-19+1A<1C>1D<1E>1F<20");
}

BOOST_AUTO_TEST_CASE(elf_merge_34_tcp)
{
  // Pre-condition
  Elf_t          elf_target(0x00,false);
  Elf_t          elf_newbie(0x01,false);
  Data<Elf_t, 1> target(elf_target);
  Data<Elf_t, 1> newbie(elf_newbie);

  // Condition:
  for (int i=1;i<34;++i)
  {
    newbie.at(0).value[0] = i;
    MergeFlags< Data<Elf_t, 1> >::merge(target,newbie,!(i%2),true);
  }

  // Post-condition:
  string elf_string;

  ToStringFlags< Data<Elf_t, 1> >::toString(target,elf_string,1);

  BOOST_CHECK_EQUAL(elf_string,",+00-01+02-03+04-05+06-07+08-09+0A-0B+0C-0D+0E-0F+10-11+12-13+14-15+16-17+18-19+1A>1D<1E>1F<20>21");
}

BOOST_AUTO_TEST_CASE(elf_merge_35_tcp)
{
  // Pre-condition
  Elf_t          elf_target(0x00,false);
  Elf_t          elf_newbie(0x01,false);
  Data<Elf_t, 1> target(elf_target);
  Data<Elf_t, 1> newbie(elf_newbie);

  // Condition:
  for (int i=1;i<35;++i)
  {
    newbie.at(0).value[0] = i;
    MergeFlags< Data<Elf_t, 1> >::merge(target,newbie,!(i%2),true);
  }

  // Post-condition:
  string elf_string;

  ToStringFlags< Data<Elf_t, 1> >::toString(target,elf_string,1);

  BOOST_CHECK_EQUAL(elf_string,",+00-01+02-03+04-05+06-07+08-09+0A-0B+0C-0D+0E-0F+10-11+12-13+14-15+16-17+18-19+1A<1E>1F<20>21<22");
}

BOOST_AUTO_TEST_CASE(elf_merge_36_tcp)
{
  // Pre-condition
  Elf_t          elf_target(0x00,false);
  Elf_t          elf_newbie(0x01,false);
  Data<Elf_t, 1> target(elf_target);
  Data<Elf_t, 1> newbie(elf_newbie);

  // Condition:
  for (int i=1;i<36;++i)
  {
    newbie.at(0).value[0] = i;
    MergeFlags< Data<Elf_t, 1> >::merge(target,newbie,!(i%2),true);
  }

  // Post-condition:
  string elf_string;

  ToStringFlags< Data<Elf_t, 1> >::toString(target,elf_string,1);

  BOOST_CHECK_EQUAL(elf_string,",+00-01+02-03+04-05+06-07+08-09+0A-0B+0C-0D+0E-0F+10-11+12-13+14-15+16-17+18-19+1A>1F<20>21<22>23");
}

BOOST_AUTO_TEST_CASE(elf_merge_37_tcp)
{
  // Pre-condition
  Elf_t          elf_target(0x00,false);
  Elf_t          elf_newbie(0x01,false);
  Data<Elf_t, 1> target(elf_target);
  Data<Elf_t, 1> newbie(elf_newbie);

  // Condition:
  for (int i=1;i<37;++i)
  {
    newbie.at(0).value[0] = i;
    MergeFlags< Data<Elf_t, 1> >::merge(target,newbie,!(i%2),true);
  }

  // Post-condition:
  string elf_string;

  ToStringFlags< Data<Elf_t, 1> >::toString(target,elf_string,1);

  BOOST_CHECK_EQUAL(elf_string,",+00-01+02-03+04-05+06-07+08-09+0A-0B+0C-0D+0E-0F+10-11+12-13+14-15+16-17+18-19+1A<20>21<22>23<24");
}

BOOST_AUTO_TEST_CASE(elf_merge_38_tcp)
{
  // Pre-condition
  Elf_t          elf_target(0x00,false);
  Elf_t          elf_newbie(0x01,false);
  Data<Elf_t, 1> target(elf_target);
  Data<Elf_t, 1> newbie(elf_newbie);

  // Condition:
  for (int i=1;i<38;++i)
  {
    newbie.at(0).value[0] = i;
    MergeFlags< Data<Elf_t, 1> >::merge(target,newbie,!(i%2),true);
  }

  // Post-condition:
  string elf_string;

  ToStringFlags< Data<Elf_t, 1> >::toString(target,elf_string,1);

  BOOST_CHECK_EQUAL(elf_string,",+00-01+02-03+04-05+06-07+08-09+0A-0B+0C-0D+0E-0F+10-11+12-13+14-15+16-17+18-19+1A>21<22>23<24>25");
}

BOOST_AUTO_TEST_SUITE_END()
