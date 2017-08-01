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


SummaryExporter::SummaryExporter(OutputHelper & p_output_helper, vector<MetricsEvent *> & p_metrics_events, int unsigned const p_summary_forceout, string const & p_version_record, bool const p_suppress_ipv4) throw() :
  summary_forceout(p_summary_forceout),
  output_helper(p_output_helper),
  metrics_events(p_metrics_events),
  file_close_time(getEpoch()),
  summary_file(NULL),
  version_record(p_version_record),
  suppress_ipv4(p_suppress_ipv4)
{
  DEBUG(TRACE, ENTER);
  DEBUG(TRACE, LEAVE);
  return;
}


// ======================================================================
// Private Functions
// ======================================================================


void SummaryExporter::renameFile(void) throw()
{
  DEBUG(TRACE, ENTER);
  if (NULL != getSummaryFile())
  {
    outputMetadata();
    closeSummaryFile();
    deleteSummaryFile();
    getOutputHelper().setFileSettings(getCurrentFilepath());
    getOutputHelper().renameTempFilename(getCurrentFilepath());
  }
  DEBUG(TRACE, LEAVE);
  return;
}


void SummaryExporter::outputMetadata(void) throw()
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


bool SummaryExporter::openSummaryFile(void) throw()
{
  bool status = false;

  if (getOutputHelper().openLocked(getCurrentFilepath()))
  {
    this->summary_file = new ofstream(getCurrentFilepath().c_str(), ios::app);
    if (!getSummaryFile() || !getSummaryFile()->is_open())
    {
      ERROR(FileIO, "Can't open file", ("Can't open summary file: " + getCurrentFilepath()).c_str());
    }
    status = true;
  }
  else
  {
    ERROR(FileIO, "Can't open or lock file", ("Can't open summary file: " + getCurrentFilepath()).c_str());
  }
  return(status);
}


void SummaryExporter::writeSummary(sharedFlow const & p_flow) throw()
{
  if (getSummaryFile() && getSummaryFile()->is_open())
  {
    if (p_flow->isIpv4() && suppressIpv4() && (! p_flow->isFake()))
    {
      return;
    }
    *getSummaryFile() << p_flow << "\n";
  }
  else
  {
    ERROR(FileIO, "File is not open", ("Trying to write to a unopened file: " + getCurrentFilepath()).c_str());
  }
  return;
}


void SummaryExporter::closeSummaryFile(void) throw()
{
  // Close the file stream
  getSummaryFile()->close();

  // Remove the lock and close the file
  if (! getOutputHelper().closeLocked(getCurrentFilepath()))
  {
    ERROR(FileIO, "File failed to unlock or close", ("Trying to unlock and close file: " + getCurrentFilepath()).c_str());
  }
  return;
}


void SummaryExporter::manageFiles(time_t const p_flow_virtual_time) throw()
{
  DEBUG(TRACE, ENTER);
  if (p_flow_virtual_time > getFileCloseTime())
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
  }
  
  DEBUG(TRACE, LEAVE);
  return;
}


void SummaryExporter::onAddEvent(sharedFlow const p_flow) throw()
{
  DEBUG(TRACE, ENTER);
  manageFiles(p_flow->getVirtualTime());
  writeSummary(p_flow);
  DEBUG(TRACE, LEAVE);
  return;
}


void SummaryExporter::reset(int unsigned const p_level) throw()
{
  DEBUG(TRACE, ENTER);
  renameFile();
  setCurrentFilepath("");
  DEBUG(TRACE, LEAVE);
  return;
}


void SummaryExporter::onNewInputEvent(int unsigned const p_level) throw()
{
  DEBUG(TRACE, ENTER);
  reset(p_level);
  setFileCloseTime(getEpoch());
  DEBUG(TRACE, LEAVE);
  return;
}


void SummaryExporter::onShutdownSystemEvent(int unsigned const p_level) throw()
{
  DEBUG(TRACE, ENTER);
  reset(p_level);
  DEBUG(TRACE, LEAVE);
  return;
}
