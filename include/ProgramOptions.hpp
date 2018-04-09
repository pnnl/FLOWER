///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef PROGRAMOPTIONS_HPP
#define PROGRAMOPTIONS_HPP


// System Includes
#include <string>
#include <cstdlib>
#include <exception>      // std::exception
#include <iostream>
#include <fstream>
#include <iterator>
#include <sys/stat.h>
// External Includes
#include <boost/program_options.hpp>
#include <Diagnostic.hpp>
// Internal Includes
// Application Includes
#include "osUtil.hpp"
#include "Exception.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "ProgramOptions"


// Namespaces used
using namespace std;
using namespace boost::program_options;


//==============================================================================
// Class ProgramOptions
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


class ProgramOptions
{

public:

  // Getters/Setters

  inline variables_map & getOptionMap(void) throw() // Cannot be const
  {
    return(this->option_map);
  }


  inline int unsigned getOutputFileGroupId(void) const throw()
  {
    return(this->output_file_group_id);
  }


  inline string getDevice(void) const throw()
  {
    return(this->device_name);
  }


  // Constructors

  ProgramOptions(void) throw();


  // Destructor

  ~ProgramOptions(void) throw()
  {
    return;
  }


  // Public Functions

  bool   checkOptions(int argc, char ** argv, string const & p_data_guide_ver = "0") throw();
  string getDefaultConfigFile(void) throw();
  string getVersionRecord(const string & p_record) throw ();


  template<class T>
  const T & getOption(string const & p_option) const throw(logic_error)
  {
    if (this->option_map.count(p_option))
    {
      return(this->option_map[p_option].as< T >());
    }
    throw(logic_error("Bad option '" + p_option + "'."));
  }


  inline bool useDevice(void) throw()
  {
    return(this->use_device);
  }


  inline bool useRing(void) throw()
  {
    return(this->use_ring);
  }


  inline bool isInteractive(void) throw()
  {
    return(this->interactive);
  }


  inline bool suppressIpv4Output(void) throw()
  {
    return(this->suppress_ipv4_output);
  }


  inline bool skipIpv4Packets(void) throw()
  {
    return(this->skip_ipv4_packets);
  }


  inline string getOutputDataDir(void) throw()
  {
    return(this->output_data_dir);
  }


  inline string getOutputFileExt(void) throw()
  {
    return(this->output_file_ext);
  }


  inline int unsigned getCacheForceout(void) throw()
  {
    return(this->cache_forceout);
  }


  inline int unsigned getCacheTimeout(void) throw()
  {
    return(this->cache_timeout);
  }


  inline int unsigned getSummaryForceout(void) throw()
  {
    return(this->summary_forceout);
  }


  inline int unsigned getMaxFlowcacheSize(void) throw()
  {
    return(this->max_flowcache_size);
  }


  inline int unsigned getMaxPacketbufferSize(void) throw()
  {
    return(this->max_packetbuffer_size);
  }


  inline int unsigned getPackets(void) throw()
  {
    return(this->packets);
  }


  inline int unsigned getSnaplen(void) throw()
  {
    return(this->snaplen);
  }


  inline int unsigned getBufferPackets(void) throw()
  {
    return(this->buffer_packets);
  }


  inline bool bufferPackets(void) throw()
  {
    return(lexical_cast<bool>(getBufferPackets()));
  }


  inline vector<string> getInputFiles(void) throw()
  {
    return(getOption< vector<string> >("input-file"));
  }


private:

  // Getters/Setters

  inline string getConfigFile(void) throw()
  {
    return(this->config_file);
  }


  inline void setDeviceName(string const p_value) throw()
  {
    this->device_name = p_value;
    return;
  }


  inline void setOutputDataDir(string const p_value) throw()
  {
    this->output_data_dir = p_value;
    return;
  }


  inline void setOutputFileExt(string const p_value) throw()
  {
    this->output_file_ext = p_value;
    return;
  }


  inline void setOutputFileGroupId(int unsigned const p_id) throw()
  {
    this->output_file_group_id = p_id;
    return;
  }


  inline void setUseDevice(bool const p_value) throw()
  {
    this->use_device = p_value;
    return;
  }


  void setUseRing(void) throw()
  {
    if (getOptionMap().count("use-ring"))
    {
      if (! isLinux())
      {
        FATAL(BadOption, "Exception", "The use-ring option can only be used on a system running Linux");
      }
      checkEnum("use-ring", 0, 1);
      this->use_ring = (1 == getOption<int unsigned>("use-ring"));
    }
    return;
  }


  void setSuppressIpv4Output(void) throw()
  {
    if (getOptionMap().count("suppress-ipv4-output"))
    {
      checkEnum("suppress-ipv4-output", 0, 1);
      this->suppress_ipv4_output = (1 == getOption<int unsigned>("suppress-ipv4-output"));
    }
  }


  void setSkipIpv4Packets(void) throw()
  {
    if (getOptionMap().count("skip-ipv4-packets"))
    {
      checkEnum("skip-ipv4-packets", 0, 1);
      this->skip_ipv4_packets = (1 == getOption<int unsigned>("skip-ipv4-packets"));
    }
  }


  inline void setBufferPackets(void) throw()
  {
    checkEnum("buffer-packets", 0, 1);
    this->buffer_packets = getOption<int unsigned>("buffer-packets");
    return;
  }


  inline void setPackets(void) throw()
  {
    checkRange("packets", 0, ULONG_MAX);
    this->packets = getOption<int unsigned>("packets");
    return;
  }


  inline void setCacheForceout(void) throw()
  {
    checkRange("cache-forceout", 0, 2500);
    this->cache_forceout = getOption<int unsigned>("cache-forceout");
    return;
  }


  inline void setCacheTimeout(void) throw()
  {
    checkRange("cache-timeout", 0, 2500);
    this->cache_timeout = getOption<int unsigned>("cache-timeout");
    return;
  }


  inline void setSummaryForceout(void) throw()
  {
    checkRange("summary-forceout", 0, 2500);
    this->summary_forceout = getOption<int unsigned>("summary-forceout");
    return;
  }


  inline void setMaxFlowcacheSize(void) throw()
  {
    checkSpecial("max-flowcache-size", 0, 30000);
    this->max_flowcache_size = getOption<int unsigned>("max-flowcache-size");
    return;
  }


  inline void setMaxPacketbufferSize(void) throw()
  {
    checkRange("max-packetbuffer-size", 0, 30000);
    this->max_packetbuffer_size = getOption<int unsigned>("max-packetbuffer-size");
    return;
  }


  inline void setSnaplen(void) throw()
  {
    checkRange(  "snaplen", 0, 65536);  // Must be less than 65536 - FATAL
    checkSpecial("snaplen", 0, 65535);  // Should be 65535         - WARN
    this->snaplen = getOption<int unsigned>("snaplen");
    return;
  }


  inline void setConfigFile(string const p_value) throw()
  {
    this->config_file = p_value;
    return;
  }


  inline void setInteractive(bool const p_value) throw()
  {
    this->interactive = p_value;
    return;
  }


  // Constructors

  ProgramOptions(ProgramOptions const & p_program_options) throw();


  // Operators

  ProgramOptions & operator=(ProgramOptions const & p_program_options) throw();
  bool             operator==(ProgramOptions const & p_program_options) const throw();
  bool             operator!=(ProgramOptions const & p_program_options) const throw();


  // Variables

  variables_map                  option_map;
  bool                           use_device;
  bool                           suppress_ipv4_output;
  bool                           skip_ipv4_packets;
  bool                           use_ring;
  bool                           interactive;
  int unsigned                   output_file_group_id;
  options_description            command_only;
  options_description            command_and_config;
  options_description            config_only;
  options_description            cmdline_options;
  options_description            config_file_options;
  options_description            visible;
  positional_options_description position_options;
  string                         device_name;
  string                         config_file;
  string                         executable_name;
  string                         output_data_dir;
  string                         output_file_ext;
  int unsigned                   cache_forceout;
  int unsigned                   cache_timeout;
  int unsigned                   summary_forceout;
  int unsigned                   max_flowcache_size;
  int unsigned                   max_packetbuffer_size;
  int unsigned                   packets;
  int unsigned                   snaplen;
  int unsigned                   buffer_packets;
  vector<string>                 input_files;


  // Private Functions

  void displayHelp(bool const p_condition) throw();
  void displayRuntimeVariables(void) throw();
  void displayVersion(bool const p_condition, string const & p_data_guide_ver) throw();
  void processConfigFile(void) throw();
  void processDevice(bool const p_condition) throw();
  bool processInputFiles(bool const p_condition) throw();
  void processOutputParams(void) throw();
  void processSiteName(void) throw();

  void checkEnum(string const & p_option, int unsigned const p_check0, int unsigned const p_check1);
  void checkRange(string const & p_option, u_int64_t const p_check0, u_int64_t const p_check1);
  void checkGreater(string const & p_option0, string const & p_option1);
  void checkSpecial(string const & p_option, u_int64_t const p_check0, u_int64_t const p_check1);
  void conflictingOptions(string const & p_opt1, string const & p_opt2) throw(logic_error);
  void optionDependency(string const & p_for_what, string const & p_required_option) throw(logic_error);
  void requiredOption(string const & p_option) throw(logic_error);
  void requiredOptions(string const & p_opt1, string const & p_opt2) throw(logic_error);

};

#endif // PROGRAMOPTIONS_HPP
