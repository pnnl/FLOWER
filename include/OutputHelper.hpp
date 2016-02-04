///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  PNNL.  All Rights Reserved. 
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


#ifndef OUTPUTHELPER_HPP
#define OUTPUTHELPER_HPP


// System Includes
#include <iostream>
#include <string>
#include <vector>
// External Includes
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
// Internal Includes
// Application Includes
#include "date2string.hpp"
#include "osUtil.hpp"
#include "Exception.hpp"

#ifdef _MSC_VER
#include <windows.h>
#endif


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "OutputHelper"


// Namespaces used
using namespace std;
using namespace boost;
using namespace boost::posix_time;


//==============================================================================
// Class OutputHelper
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


class OutputHelper
{

public:

  // Getters/Setters

  inline string getDeviceFileLock(string const & p_device_name) const throw()
  {
    // DEVELOPER NOTE:
    //  Putting the lock file in /dev/shm allows the system to clean them up on
    //  orderly and unorderly shutdowns/reboots
    return("/dev/shm/"APP_NAME"." + p_device_name + ".lock");
  }


  inline string getOutputLongFilename(time_t const p_time, string const & p_ext = "") const throw()
  {
    return(getAbsFilename(false, p_time, p_ext));
  }


  inline string getOutputTempFilename(time_t const p_time, string const & p_ext = "") const throw()
  {
    return(getAbsFilename(true, p_time, p_ext));
  }


  // Constructors

  OutputHelper(string const & p_output_data_dir, string const & p_output_file_ext, int unsigned const p_group_id, string const & p_site_name, string const & p_data_guide_ver, string const & p_delimeter = ".") throw();


  // Destructor

  ~OutputHelper(void) throw()
  {
    return;
  }


  // Public Functions

  void   renameTempFilename(string const & p_filename) throw();
  void   setFileSettings(string const & p_filename) throw();
  bool   openLocked(string const & p_filename) throw();
  bool   closeLocked(string const & p_filename) throw();
#ifndef _MSC_VER
  int    getFileDescriptor(string const & p_filename) throw();
#else
  HANDLE getFileDescriptor(string const & p_filename) throw();
#endif
  bool   eraseFileDescriptor(string const & p_filename) throw();


private:

  // Getters/Setters

  inline string getAbsFilename(bool const p_hidden, time_t const p_time, string const & p_ext = "") const throw()
  {
    static string lut[2]   = {"", "."};
    static string ymdhms   = "YMDhms";
    string ext             = p_ext;
    string date_str;

    date_str.reserve(36);
    if (0 == ext.size())
    {
      ext = this->output_file_ext;
    }
    return(this->output_data_dir + getPathSeparator() + lut[p_hidden] + getDateString(p_time, ymdhms, date_str) + "-" + this->site_name + "-" + this->data_guide_ver + "." + ext);
  }


  inline string getDelimiter(void) const
  {
    return(this->delimeter);
  }

#ifndef _MSC_VER
  inline map<string, int>    & getFileLookup(void) throw() // Cannot be const
#else
  inline map<string, HANDLE> & getFileLookup(void) throw() // Cannot be const
#endif
  {
    return(this->file_lookup);
  }


  inline string getOutputFileExt(void) const throw()
  {
    return(this->output_file_ext);
  }


  inline int unsigned getOutputFileGroupId(void) const throw()
  {
    return(this->output_file_group_id);
  }


  inline string getSiteName(void) const throw()
  {
    return(this->site_name);
  }


  // Constructors

  OutputHelper(void) throw();

  OutputHelper(OutputHelper const & p_output_helper) throw();


  // Operators

  OutputHelper & operator=(OutputHelper const & p_output_helper) throw();
  bool           operator<(OutputHelper const & p_output_helper)  const throw();
  bool           operator>(OutputHelper const & p_output_helper)  const throw();
  bool           operator==(shared_ptr<OutputHelper> const & p_output_helper) const throw();
  bool           operator!=(shared_ptr<OutputHelper> const & p_output_helper) const throw();


  // Variables

  string           output_data_dir;
  string           output_file_ext;
  int unsigned     output_file_group_id;
  string           site_name;
  string           data_guide_ver;
  string           delimeter;

#ifndef _MSC_VER
  map<string, int>    file_lookup;
#else
  map<string, HANDLE> file_lookup;
#endif

};

#endif // OUTPUTHELPER_HPP
