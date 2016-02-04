///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  .  All Rights Reserved. 
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_test.template 
//     COMMAND NAME:   gensrc 
//
// ND file: $Id$ 
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
#define BOOST_TEST_MODULE Data

// System Includes
// External Includes
#include <boost/test/included/unit_test.hpp>
// Internal Includes
// Application Includes
#include "Field/Data.hpp"


#undef SINGLE_T
#undef PAIR_T
#undef TRIPLE_T
#undef QUAD_T

#define SINGLE_T (1)
#define PAIR_T   (2)
#define TRIPLE_T (3)
#define QUAD_T   (4)

const int unsigned ZERO   =  0;
const int unsigned TEN    = 10;
const int unsigned TWENTY = 20;

struct DataSuiteFixture
{
  
  DataSuiteFixture()
  {
    BOOST_TEST_MESSAGE("DataSuite setup fixture");
    return;
  }


  ~DataSuiteFixture()
  {
    BOOST_TEST_MESSAGE("DataSuite teardown fixture");
    return;
  }

};


BOOST_FIXTURE_TEST_SUITE(DataSuite, DataSuiteFixture)


BOOST_AUTO_TEST_CASE(test_single)
{
  // Pre-condition: 
  // Condition: 

  Data<u_int32_t, SINGLE_T> single_A(10);
  single_A[0] = 10;
  Data<u_int32_t, SINGLE_T> single_B;
  single_B[0] = 10;
  Data<u_int32_t, SINGLE_T> single_C(single_A);
  Data<u_int32_t, SINGLE_T> single_D = single_A;
  Data<u_int32_t, SINGLE_T> single_E;
  single_E = single_A;
  Data<u_int32_t, SINGLE_T> single_F;
  single_F[0] = 10;
  Data<u_int32_t, SINGLE_T> single_G;
  single_G.at(0) = 10;

  u_int32_t t_data[SINGLE_T];
  t_data[0] = 10;
  Data<u_int32_t, SINGLE_T> single_H;
  single_H[0] = t_data[0];


  // Post-condition:
  BOOST_CHECK_EQUAL(TEN, single_A.at(0));
  BOOST_CHECK_EQUAL(TEN, single_A.at(0));
  BOOST_CHECK_EQUAL(single_A, single_B);
  BOOST_CHECK_EQUAL(single_A, single_C);
  BOOST_CHECK_EQUAL(single_A, single_D);
  BOOST_CHECK_EQUAL(single_A, single_E);
  BOOST_CHECK_EQUAL(TEN, single_F.at(0));
  BOOST_CHECK_EQUAL(TEN, single_F[0]);
  BOOST_CHECK_EQUAL(TEN, single_G.at(0));
  BOOST_CHECK_EQUAL(TEN, single_G[0]);

  BOOST_CHECK_EQUAL(TEN, single_H.at(0));
  BOOST_CHECK_EQUAL(TEN, single_H[0]);

  return;
}


BOOST_AUTO_TEST_CASE(test_pair)
{
  // Pre-condition:
  // Condition:

  Data<u_int32_t, PAIR_T> pair_A;
  pair_A[0] = 10;
  pair_A[1] = 20;
  Data<u_int32_t, PAIR_T> pair_B;
  pair_B[0] = 10;
  pair_B[1] = 20;
  Data<u_int32_t, PAIR_T> pair_C(pair_A);
  Data<u_int32_t, TRIPLE_T> triple_D[10];

  for (int idx = 0; idx < 10; ++idx)
  {
    triple_D[idx][0] = 30;
    triple_D[idx][1] = 40;
    triple_D[idx][2] = 50;
  }

  u_int32_t t_data[PAIR_T]; 
  t_data[0] = 10;
  t_data[1] = 20;
  Data<u_int32_t, PAIR_T> pair_H;
  pair_H[0] = t_data[0];
  pair_H[1] = t_data[1];
 
  // Post-condition:

  BOOST_CHECK_EQUAL(TWENTY, pair_A.at(1));
  BOOST_CHECK_EQUAL(pair_A.at(0), TEN);
  BOOST_CHECK_EQUAL(pair_A, pair_B);
  BOOST_CHECK_EQUAL(pair_A, pair_C);
  BOOST_CHECK_EQUAL(triple_D[0], triple_D[1]);
  BOOST_CHECK_EQUAL(triple_D[0][0], triple_D[1][0]);
  BOOST_CHECK_EQUAL(3 * sizeof(u_int32_t), sizeof(triple_D[0]));


  BOOST_CHECK_EQUAL(TEN, pair_H.at(0));
  BOOST_CHECK_EQUAL(TEN, pair_H[0]);

  return;
}


BOOST_AUTO_TEST_CASE(test_iterator)
{
  // Pre-condition:
  // Condition:

  typedef Data<u_int32_t, QUAD_T> QuadUint32;
  QuadUint32 quad_A;
  quad_A[0] = 10;
  quad_A[1] = 20;
  quad_A[2] = 30;
  quad_A[3] = 40;

  u_int32_t temp = 10;
  for (QuadUint32::iterator idx = quad_A.begin(); idx != quad_A.end(); ++idx)
  {
    BOOST_CHECK_EQUAL(temp, (*idx));
    temp += 10;
  }

  // Post-condition:

  return;
}


BOOST_AUTO_TEST_CASE(test_boundary)
{
  // Pre-condition:
  // Condition:

  typedef Data<u_int32_t, QUAD_T> QuadUint32;
  QuadUint32 quad_A;
  quad_A[0] = 10;
  quad_A[1] = 20;
  quad_A[2] = 30;
  quad_A[3] = 40;

  // Post-condition:
  BOOST_CHECK_NE(ZERO, quad_A[4]);
  //BOOST_CHECK_THROW(quad_A.at(4), range_error);  // DEVELOPER NOTE: Removed throw calls in Data.hpp to gain performance

  return;
}

#undef SINGLE_T
#undef PAIR_T
#undef TRIPLE_T
#undef QUAD_T

BOOST_AUTO_TEST_SUITE_END()
