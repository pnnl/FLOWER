///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


// System Includes
// External Includes
// Internal Includes
// Application Includes
#include "SummaryExporter.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "SummaryExporter"


// Namespaces used


//==============================================================================
// Class SummaryExporter
//
// API Developer Documentation:   (those who develop/fix this class)
//   [put documentation here for developers that work on this code]
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


// ======================================================================
// Constructors
// ======================================================================


SummaryExporter::SummaryExporter(OutputHelper           & p_output_helper,
                                 vector<MetricsEvent *> & p_metrics_events,
                                 int unsigned const       p_summary_forceout,
                                 int unsigned const       p_max_records_per_file,
                                 string const           & p_version_record,
                                 string const           & p_csv_header,
                                 bool const               p_suppress_ipv4,
                                 bool         const       p_suppress_metrics
                                ) noexcept(true) :
  summary_forceout(p_summary_forceout),
  max_records_per_file(p_max_records_per_file),
  current_records_in_outfile(0),
  output_helper(p_output_helper),
  metrics_events(p_metrics_events),
  file_close_time(getEpoch()),
  summary_file(NULL),
  version_record(p_version_record),
  csv_header(p_csv_header),
  suppress_ipv4(p_suppress_ipv4),
  suppress_metrics(p_suppress_metrics)
{
  DEBUG(TRACE, ENTER);
  DEBUG(TRACE, LEAVE);
  return;
}


// ======================================================================
// Private Functions
// ======================================================================


void SummaryExporter::renameFile(void) noexcept(true)
{
  DEBUG(TRACE, ENTER);
  if (NULL != getSummaryFile())
  {
    if (! suppressMetrics())
    {
      outputMetadata();
    }
    closeSummaryFile();
    deleteSummaryFile();
    getOutputHelper().setFileSettings(getCurrentFilepath());
    getOutputHelper().renameTempFilename(getCurrentFilepath());
  }
  DEBUG(TRACE, LEAVE);
  return;
}


void SummaryExporter::outputMetadata(void) noexcept(true)
{
  string metric_record;
  vector<MetricsEvent *>::iterator itor = getMetricsEvents().begin();
  vector<MetricsEvent *>::iterator etor = getMetricsEvents().end();
  for (itor = itor; itor < etor; ++itor)
  {
    metric_record += "," + (*itor)->call(1);
  }
  *getSummaryFile()   << "3"  << metric_record        << "\n";
  *getSummaryFile()   << "4," << this->version_record << "\n";
  string error_record = getErrorRecord();
  if (! error_record.empty())
  {
    *getSummaryFile() << "5"  << error_record         << "\n";
  }

  return;
}


bool SummaryExporter::openSummaryFile(void) noexcept(true)
{
  bool status = false;

  if (getOutputHelper().openLocked(getCurrentFilepath()))
  {
    this->summary_file = new ofstream(getCurrentFilepath().c_str(), ios::app);
    if (! getSummaryFile() || ! getSummaryFile()->is_open())
    {
      ERROR_MSG(FileIO, "Can't open file", ("Can't open summary file: " + getCurrentFilepath()).c_str());
    }
    *getSummaryFile() << csv_header;
    status = true;
  }
  else
  {
    ERROR_MSG(FileIO, "Can't open or lock file", ("Can't open summary file: " + getCurrentFilepath()).c_str());
  }
  return(status);
}


void SummaryExporter::writeSummary(sharedFlow const & p_flow) noexcept(true)
{
  if (getSummaryFile() && getSummaryFile()->is_open())
  {
    if (p_flow->isIpv4() && suppressIpv4() && (! p_flow->isFake()))
    {
      return;
    }
    *getSummaryFile() << p_flow << "\n";
    current_records_in_outfile++;
  }
  else
  {
    ERROR_MSG(FileIO, "File is not open", ("Trying to write to a unopened file: " + getCurrentFilepath()).c_str());
  }
  return;
}


void SummaryExporter::closeSummaryFile(void) noexcept(true)
{
  // Close the file stream
  getSummaryFile()->close();

  // Remove the lock and close the file
  if (! getOutputHelper().closeLocked(getCurrentFilepath()))
  {
    ERROR_MSG(FileIO, "File failed to unlock or close", ("Trying to unlock and close file: " + getCurrentFilepath()).c_str());
  }
  return;
}


void SummaryExporter::manageFiles(time_t const p_flow_virtual_time) noexcept(true)
{
  DEBUG(TRACE, ENTER);
  if ((p_flow_virtual_time > getFileCloseTime()) || (current_records_in_outfile >= max_records_per_file))
  {
    if (! getCurrentFilepath().empty())
    {
      renameFile();
    }
  
    // Create a new temporary filename
    setCurrentFilepath(getOutputHelper().getOutputTempFilename(p_flow_virtual_time));
    setFileCloseTime(p_flow_virtual_time + getSummaryForceout());
    //setFileTime(p_flow_virtual_time);
    openSummaryFile();
    current_records_in_outfile = 0;
  }
  
  DEBUG(TRACE, LEAVE);
  return;
}


void SummaryExporter::onAddEvent(sharedFlow const p_flow) noexcept(true)
{
  DEBUG(TRACE, ENTER);
  manageFiles(p_flow->getVirtualTime());
  writeSummary(p_flow);
  DEBUG(TRACE, LEAVE);
  return;
}


void SummaryExporter::reset(int unsigned const p_level) noexcept(true)
{
  DEBUG(TRACE, ENTER);
  renameFile();
  setCurrentFilepath("");
  DEBUG(TRACE, LEAVE);
  return;
}


void SummaryExporter::onNewInputEvent(int unsigned const p_level) noexcept(true)
{
  DEBUG(TRACE, ENTER);
  reset(p_level);
  setFileCloseTime(getEpoch());
  DEBUG(TRACE, LEAVE);
  return;
}


void SummaryExporter::onShutdownSystemEvent(int unsigned const p_level) noexcept(true)
{
  DEBUG(TRACE, ENTER);
  reset(p_level);
  DEBUG(TRACE, LEAVE);
  return;
}
