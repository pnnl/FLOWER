///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef SUMMARYEXPORTER_HPP
#define SUMMARYEXPORTER_HPP


// System Includes
#include <iostream>
#include <string>
// External Includes
// Internal Includes
// Application Includes
#include "EventTypes.hpp"
#include "Flow.hpp"
#include "OutputHelper.hpp"

#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "SummaryExporter"


// Namespaces used
using namespace std;


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


class SummaryExporter
{

public:

  // Getters/Setters

  inline vector<MetricsEvent *> & getMetricsEvents(void) const noexcept(true)
  {
    return(this->metrics_events);
  }


  // Constructors

  SummaryExporter(OutputHelper           & p_output_helper, 
                  vector<MetricsEvent *> & p_metrics_events,
                  int unsigned const       p_summary_forceout,
                  string       const     & p_version_record,
                  string       const     & p_csv_header,
                  bool         const       p_suppress_ipv4,
                  bool         const       p_suppress_metrics
                 ) noexcept(true);


  // Destructor

  ~SummaryExporter(void) noexcept(true)
  {
    if (NULL != summary_file)
    {
      delete summary_file;
    }
    return;
  }


  // Public Functions

  void   onAddEvent(sharedFlow const p_flow) noexcept(true);
  void   onNewInputEvent(int unsigned const p_level) noexcept(true);
  void   onShutdownSystemEvent(int unsigned const p_level) noexcept(true);


private:

  // Getters/Setters

  inline void deleteSummaryFile(void) noexcept(true)
  {
    delete getSummaryFile();
    this->summary_file = NULL;
  }


  inline ofstream * getSummaryFile(void) const noexcept(true)
  {
    return(this->summary_file);
  }


  inline int unsigned getSummaryForceout(void) const noexcept(true)
  {
    return(this->summary_forceout);
  }


  inline string getCurrentFilepath(void) const noexcept(true)
  {
    return(this->current_filepath);
  }


  inline void setCurrentFilepath(string const & p_filepath) noexcept(true)
  {
    this->current_filepath = p_filepath;
    return;
  }


  inline OutputHelper & getOutputHelper(void) const noexcept(true)
  {
    return(this->output_helper);
  }


  inline time_t getFileCloseTime(void) const noexcept(true)
  {
    return(this->file_close_time);
  }


  inline void setFileCloseTime(time_t const p_time) noexcept(true)
  {
    this->file_close_time = p_time;
    return;
  }


  inline bool suppressIpv4(void) const noexcept(true)
  {
    return(this->suppress_ipv4);
  }


  inline bool suppressMetrics(void) const noexcept(true)
  {
    return(this->suppress_metrics);
  }


  // Constructors

  SummaryExporter(void) noexcept(true);
  SummaryExporter(SummaryExporter const & p_summary_exporter) noexcept(true);
  

  // Operators

  SummaryExporter & operator=( SummaryExporter const & p_summary_exporter)       noexcept(true);
  bool              operator==(SummaryExporter const & p_summary_exporter) const noexcept(true);
  bool              operator!=(SummaryExporter const & p_summary_exporter) const noexcept(true);


  // Variables

  int unsigned             summary_forceout;
  OutputHelper &           output_helper;
  vector<MetricsEvent *> & metrics_events;
  time_t                   file_close_time;
  string                   current_filepath;
  basic_ofstream<char> *   summary_file;
  string                   version_record;
  string                   csv_header;
  bool                     suppress_ipv4;
  bool                     suppress_metrics;


  // Private Functions

  void outputMetadata(void) noexcept(true);
  bool openSummaryFile(void) noexcept(true);
  void writeSummary(sharedFlow const & p_flow) noexcept(true);
  void closeSummaryFile(void) noexcept(true);
  void manageFiles(time_t const p_flow_virtual_time) noexcept(true);
  void renameFile(void) noexcept(true);
  void reset(int unsigned const p_level = 0) noexcept(true);

};

#endif // SUMMARYEXPORTER_HPP
