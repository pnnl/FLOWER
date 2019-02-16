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
// External Includes
#define BOOST_TEST_MODULE SummaryExporter
#include <boost/filesystem/operations.hpp>
// Internal Includes
// Application Includes
#include "date2string.hpp"
#include "utility.hpp"
#include "OutputHelper.hpp"
#include "SummaryExporter.hpp"


struct SummaryExporterSuiteFixture 
{
  SummaryExporter *      summary_exporter;
  string                 file_ext;
  string                 out_dir;
  string                 site_name;
  string                 version_record;
  string                 csv_header;
  int unsigned           force_out;
  bool                   suppress_ipv4_output;
  OutputHelper *         output_helper;
  vector<MetricsEvent *> metrics_events;

  SummaryExporterSuiteFixture()
  {
    BOOST_TEST_MESSAGE("SummaryExporterSuite setup fixture");
    file_ext             = "dat";
    out_dir              = ".";
    site_name            = "pnnl_dev";
    force_out            = 15;
    suppress_ipv4_output = false;
    version_record       = "Test Version Record";
    csv_header           = "";

    output_helper = new OutputHelper(out_dir, file_ext, 500, site_name, getDataGuideVersion(), ".");
    summary_exporter = new SummaryExporter(*output_helper, metrics_events, (int unsigned const)force_out, version_record, csv_header, suppress_ipv4_output);

    return;
  }


  ~SummaryExporterSuiteFixture()
  {
    BOOST_TEST_MESSAGE("SummaryExporterSuite teardown fixture");
    delete summary_exporter;

    return;
  }

};


void addFakeSummary(SummaryExporter * const p_summary_exporter, int unsigned const p_forceout)
{
  sharedFlow                   flow(new Flow(::g_FAKE));
  p_summary_exporter->onAddEvent(flow);

  return;
}


void addSummary(SummaryExporter * const p_summary_exporter, time_t const p_time, int unsigned const p_key, bool const p_check = true)
{
  sharedFlow flow = generateFlow(true, p_time, p_key);
  p_summary_exporter->onAddEvent(flow);

  // Post-conditions: True for any test that calls this function
  //   The FlowAddEvent should have been called
  //   The summary_exporter should have the flow we added

  return;
}


BOOST_FIXTURE_TEST_SUITE(SummaryExporterSuite, SummaryExporterSuiteFixture)


BOOST_AUTO_TEST_CASE(close_open_output_file_on_shutdown)
{
  // Pre-condition:
  //   NONE
  // Conditions:
  //   Add a single summary
  //   The system is shutdown
  // Post-conditions:
  //   The output .file should NOT exist
  //   The output file should exist

  time_t now       = generateTimestamp();
  string filename1 = output_helper->getOutputTempFilename(now);
  string filename2 = output_helper->getOutputLongFilename(now);

  // Condition: Add a single summary
  addSummary(summary_exporter, now, 1);

  // Condition: The system is shutdown
  summary_exporter->onShutdownSystemEvent(0);

  // Post-condition: The output .file should NOT exist
  ifstream dot_ifs(filename1.c_str(), ifstream::in);
  BOOST_CHECK_EQUAL(boost::filesystem::exists(filename1), false);

  // Post-condition: The output file should exist
  BOOST_CHECK_EQUAL(boost::filesystem::exists(filename2), true);

  // DEVELOPER NOTE: In order for a file to exist without the . the corresponding . file had to be closed.
  BOOST_CHECK(boost::filesystem::remove(filename2));

  return;
}


BOOST_AUTO_TEST_CASE(write_flow_to_correct_timestamp_based_output_file)
{
  // Pre-condition:
  //   NONE
  // Conditions:
  //   Add a single summary
  //   Add another summary with a timestamp greater than forceout
  //   The system is shutdown
  // Post-conditions:
  //   There should be 2 output files with appropriate dates

  time_t now1      = generateTimestamp();
  time_t now2      = generateTimestamp(getCacheForceout() + 1);
  string filename1 = output_helper->getOutputLongFilename(now1);
  string filename2 = output_helper->getOutputLongFilename(now2);

  // Condition: Add a single summary
  addSummary(summary_exporter, now1, 1);

  // Condition: Add another summary with a timestamp greater than forceout
  addSummary(summary_exporter, now2, 2);

  // Post-condition: The output file should exist
  BOOST_CHECK_EQUAL(boost::filesystem::exists(filename1), true);

  // Condition: The system is shutdown
  summary_exporter->onShutdownSystemEvent(0);

  // Post-condition: The output file should exist
  BOOST_CHECK_EQUAL(boost::filesystem::exists(filename2), true);

  BOOST_CHECK(boost::filesystem::remove(filename1));
  BOOST_CHECK(boost::filesystem::remove(filename2));

  return;
}


BOOST_AUTO_TEST_SUITE_END()
