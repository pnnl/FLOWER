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


// System Includes
#include <iostream>
#include <string>
// External Includes
#define BOOST_TEST_MODULE ProgramOptions
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
// Internal Includes
// Application Includes
#include "utility.hpp"
#include "ProgramOptions.hpp"


// Namespaces
using namespace std;
using namespace boost;
using boost::lexical_cast;


int       g_num     =  -1;
int const g_max_num = 128;
char *    g_argv[::g_max_num];


char * getConfigFileArg(void)
{
  string config_file_arg("--config-file=");
  // This has to be char * because sudo will run as root and root
  // may not have FLOWER_HOME set. The HOME variable will be set to
  // the user running the command.
  char const * env_flower_home  = getenv("FLOWER_HOME");
  if (NULL != env_flower_home)
  {
    config_file_arg      += string(env_flower_home);
  }
  else
  {
    char * env_home       = getenv("HOME");
    if (NULL != env_home)
    {
      config_file_arg    += string(env_home);
      config_file_arg    += "/dev/flower";
    }
  }
  config_file_arg        += "/conf/flower.conf";

  //char config_file[PATH_MAX];
  //memset(config_file, '\0', PATH_MAX); // Ensure all data is nullified, in case b is not null terminated
  //strncpy(config_file, config_file_arg.c_str(), sizeof(config_file));
  //config_file[sizeof(config_file) - 1] = '\0';

  char * config_file   = (char *) calloc(config_file_arg.length() + 1, sizeof(char));
  memset(config_file, '\0', config_file_arg.length()); // Ensure all data is nullified, in case b is not null terminated
  strncpy(config_file, config_file_arg.c_str(), config_file_arg.length());
  config_file[config_file_arg.length()] = '\0';

  return(config_file);
}


int getNextNum(void)
{
  ++::g_num;
  BOOST_TEST_MESSAGE("ProgramOptionsSuite getNextNum = " + lexical_cast<string>(::g_num));
  return(::g_num);
}


void setArg(const char * p_arg)
{
  int arg_number = getNextNum();

  ::g_argv[arg_number] = (char *) calloc(strlen(p_arg) + 1, sizeof(char));
  //strncpy(::g_argv[arg_number], p_arg, strlen(p_arg));
  BOOST_TEST_MESSAGE("ProgramOptionsSuite setArg, arg_number = " + lexical_cast<string>(arg_number) + ", argv = " + lexical_cast<string>(::g_argv[arg_number]));

  if (0 < snprintf(::g_argv[arg_number], sizeof(::g_argv[arg_number]), "%s", p_arg))
  {
    BOOST_CHECK_EQUAL(true, false);
    return;
  }

  return;
}


void reset(void)
{
  BOOST_TEST_MESSAGE("ProgramOptionsSuite reset");
  for (int idx = 0; idx < ::g_num; ++idx)
  {
    if (strcmp(::g_argv[idx], "\0") != 0)
    {
      free(::g_argv[idx]);
    }
  }
  for (int idx = 0; idx < ::g_max_num; ++idx)
  {
    ::g_argv[idx] = (char *) calloc(strlen("\n") + 1, sizeof(char));
    strncpy(::g_argv[idx], "\n", strlen("\n"));
  }
  ::g_num = -1;

  return;
}


bool parseOptions(ProgramOptions & p_po)
{
  return(p_po.checkOptions(::g_num + 1, ::g_argv, getDataGuideVersion()), false);
}


struct ProgramOptionsSuiteFixture
{
  ProgramOptions po;

  ProgramOptionsSuiteFixture()
  {
    BOOST_TEST_MESSAGE("ProgramOptionsSuite setup fixture");
    reset();
    setArg(boost::unit_test::framework::master_test_suite().argv[0]);
    return;
  }


  ~ProgramOptionsSuiteFixture()
  {
    BOOST_TEST_MESSAGE("ProgramOptionsSuite teardown fixture");
    return;
  }

};


BOOST_FIXTURE_TEST_SUITE(ProgramOptionsSuite, ProgramOptionsSuiteFixture)


BOOST_AUTO_TEST_CASE(test_help)
{
  BOOST_TEST_MESSAGE("ProgramOptionsSuite test_help");
  setArg("-h");

  cout << "===========================================================" << endl;
  cout << "DEVELOPER NOTE: There should be a HELP message that follows" << endl;
  cout << "===========================================================" << endl;

  BOOST_CHECK_EQUAL(parseOptions(po), false);

  return;
}


BOOST_AUTO_TEST_CASE(test_version)
{
  BOOST_TEST_MESSAGE("ProgramOptionsSuite test_version");
  setArg("--version");

  cout << "==============================================================" << endl;
  cout << "DEVELOPER NOTE: There should be a VERSION message that follows" << endl;
  cout << "==============================================================" << endl;

  BOOST_CHECK_EQUAL(parseOptions(po), false);

  return;
}


BOOST_AUTO_TEST_CASE(test_device)
{
  BOOST_TEST_MESSAGE("ProgramOptionsSuite test_device");
  setArg("--device=eth0");

  cout << "============================================================" << endl;
  cout << "DEVELOPER NOTE: There should be a ERROR message that follows" << endl;
  cout << "============================================================" << endl;

  BOOST_CHECK_EQUAL(parseOptions(po), false);

  return;
}


BOOST_AUTO_TEST_CASE(test_missing_config_file)
{
  BOOST_TEST_MESSAGE("ProgramOptionsSuite test_missing_config_file");
  setArg("--config-file=/better_not_exist.pcap");

  cout << "============================================================" << endl;
  cout << "DEVELOPER NOTE: There should be a ERROR message that follows" << endl;
  cout << "============================================================" << endl;

  BOOST_CHECK_EQUAL(parseOptions(po), false);

  return;
}


BOOST_AUTO_TEST_CASE(test_buffer_packets_bad_value)
{
  BOOST_TEST_MESSAGE("ProgramOptionsSuite test_buffer_packets_bad_value");
  setArg("--buffer-packets=2");
  setArg(getConfigFileArg());
  setArg("t_ProgramOptions");

  cout << "============================================================" << endl;
  cout << "DEVELOPER NOTE: There should be a ERROR message that follows" << endl;
  cout << "============================================================" << endl;

  BOOST_CHECK_EQUAL(parseOptions(po), false);

  return;
}


BOOST_AUTO_TEST_CASE(test_conflict)
{
  BOOST_TEST_MESSAGE("ProgramOptionsSuite test_conflict");
  setArg("--device=eth0");
  setArg("--input-file=./t_ProgramOptions");

  cout << "============================================================" << endl;
  cout << "DEVELOPER NOTE: There should be a ERROR message that follows" << endl;
  cout << "============================================================" << endl;

  try
  {
    BOOST_CHECK_EQUAL(parseOptions(po), false);
  }
  catch (std::exception & e)
  {
    BOOST_TEST_MESSAGE("ProgramOptionsSuite test_conflict, Exception: " + lexical_cast<string>(e.what()));
  }
  catch (string & s)
  {
    BOOST_TEST_MESSAGE("ProgramOptionsSuite test_conflict, Error: " + s);
  }

  return;
}


BOOST_AUTO_TEST_CASE(test_bogus)
{
  BOOST_TEST_MESSAGE("ProgramOptionsSuite test_bogus");
  setArg("--bogus");

  cout << "================================================================" << endl;
  cout << "DEVELOPER NOTE: There should be a BadOption message that follows" << endl;
  cout << "================================================================" << endl;

  try
  {
    BOOST_CHECK_EQUAL(parseOptions(po), false);
  }
  catch (string & s)
  {
    BOOST_TEST_MESSAGE("ProgramOptionsSuite test_bogus, Error: " + s);
  }
  BOOST_CHECK_EQUAL(true, true);

  return;
}


BOOST_AUTO_TEST_SUITE_END()
