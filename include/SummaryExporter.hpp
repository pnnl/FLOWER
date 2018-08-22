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

  inline vector<MetricsEvent *> & getMetricsEvents(void) const throw()
  {
    return(this->metrics_events);
  }


  // Constructors

  SummaryExporter(OutputHelper & p_output_helper, vector<MetricsEvent *> & p_metrics_events, int unsigned const p_summary_forceout, string const & p_version_record, bool const p_suppress_ipv4) throw();


  // Destructor

  ~SummaryExporter(void) throw()
  {
    if (NULL != summary_file)
    {
      delete summary_file;
    }
    return;
  }


  // Public Functions

  void   onAddEvent(sharedFlow const p_flow) throw();
  void   onNewInputEvent(int unsigned const p_level) throw();
  void   onShutdownSystemEvent(int unsigned const p_level) throw();


private:

  // Getters/Setters

  inline void deleteSummaryFile(void) throw()
  {
    delete getSummaryFile();
    this->summary_file = NULL;
  }


  inline ofstream * getSummaryFile(void) const throw()
  {
    return(this->summary_file);
  }


  inline int unsigned getSummaryForceout(void) const throw()
  {
    return(this->summary_forceout);
  }


  inline string getCurrentFilepath(void) const throw()
  {
    return(this->current_filepath);
  }


  inline void setCurrentFilepath(string const & p_filepath) throw()
  {
    this->current_filepath = p_filepath;
    return;
  }


  inline OutputHelper & getOutputHelper(void) const throw()
  {
    return(this->output_helper);
  }


  inline time_t getFileCloseTime(void) const throw()
  {
    return(this->file_close_time);
  }


  inline void setFileCloseTime(time_t const p_time) throw()
  {
    this->file_close_time = p_time;
    return;
  }


  inline bool suppressIpv4(void) const throw()
  {
    return(this->suppress_ipv4);
  }


  // Constructors

  SummaryExporter(void) throw();
  SummaryExporter(SummaryExporter const & p_summary_exporter) throw();
  

  // Operators

  SummaryExporter & operator=(SummaryExporter const & p_summary_exporter) throw();
  bool              operator==(SummaryExporter const & p_summary_exporter) const throw();
  bool              operator!=(SummaryExporter const & p_summary_exporter) const throw();


  // Variables

  int unsigned             summary_forceout;
  OutputHelper &           output_helper;
  vector<MetricsEvent *> & metrics_events;
  time_t                   file_close_time;
  string                   current_filepath;
  ofstream *               summary_file;
  string                   version_record;
  bool                     suppress_ipv4;


  // Private Functions

  void outputMetadata(void) throw();
  bool openSummaryFile(void) throw();
  void writeSummary(sharedFlow const & p_flow) throw();
  void closeSummaryFile(void) throw();
  void manageFiles(time_t const p_flow_virtual_time) throw();
  void renameFile(void) throw();
  void reset(int unsigned const p_level = 0) throw();

};

#endif // SUMMARYEXPORTER_HPP
