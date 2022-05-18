///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
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

  inline variables_map & getOptionMap(void) noexcept(true) // Cannot be const
  {
    return(this->option_map);
  }


  inline int unsigned getOutputFileGroupId(void) const noexcept(true)
  {
    return(this->output_file_group_id);
  }


  inline string getDevice(void) const noexcept(true)
  {
    return(this->device_name);
  }


  inline bool useCsvHeader(void) noexcept(true)
  {
    if (this->option_map.count("header"))
    {
      checkEnum("header", 0, 1);
      return(1 == getOption<int unsigned>("header"));
    }
    return(false);
  }


  // Constructors

  ProgramOptions(void) noexcept(true);


  // Destructor

  ~ProgramOptions(void) noexcept(true)
  {
    return;
  }


  // Public Functions

  bool   checkOptions(int argc, char ** argv, string const & p_data_guide_ver = "0") noexcept(true);
  string getDefaultConfigFile(void) noexcept(true);
  string getVersionRecord(const string & p_record) noexcept(true);


  template<class T>
  const T & getOption(string const & p_option) const
  {
    if (this->option_map.count(p_option))
    {
      return(this->option_map[p_option].as< T >());
    }
    throw(logic_error("Bad option '" + p_option + "'."));
  }


  inline bool useDevice(void) noexcept(true)
  {
    return(this->use_device);
  }


  inline bool useRing(void) noexcept(true)
  {
    return(this->use_ring);
  }


  inline bool isInteractive(void) noexcept(true)
  {
    return(this->interactive);
  }


  inline bool suppressIpv4Output(void) noexcept(true)
  {
    return(this->suppress_ipv4_output);
  }


  inline bool suppressMetricsOutput(void) noexcept(true)
  {
    return(this->suppress_metrics_output);
  }


  inline bool skipIpv4Packets(void) noexcept(true)
  {
    return(this->skip_ipv4_packets);
  }


  inline bool getCppFormat(void) noexcept(true)
  {
    return(this->cpp_format);
  }


  inline int unsigned getCppMaxRecordsPerFile(void) noexcept(true)
  {
    return(this->cpp_max_records_per_file);
  }


  inline bool getIpAddressFormat(void) noexcept(true)
  {
    return(this->ip_address_format);
  }


  inline string getOutputDataDir(void) noexcept(true)
  {
    return(this->output_data_dir);
  }


  inline string getOutputFileExt(void) noexcept(true)
  {
    return(this->output_file_ext);
  }


  inline int unsigned getCacheForceout(void) noexcept(true)
  {
    return(this->cache_forceout);
  }


  inline int unsigned getCacheTimeout(void) noexcept(true)
  {
    return(this->cache_timeout);
  }


  inline int unsigned getSummaryForceout(void) noexcept(true)
  {
    return(this->summary_forceout);
  }


  inline int unsigned getMaxFlowcacheSize(void) noexcept(true)
  {
    return(this->max_flowcache_size);
  }


  inline int unsigned getMaxPacketbufferSize(void) noexcept(true)
  {
    return(this->max_packetbuffer_size);
  }


  inline int unsigned getPackets(void) noexcept(true)
  {
    return(this->packets);
  }


  inline int unsigned getSnaplen(void) noexcept(true)
  {
    return(this->snaplen);
  }


  inline int unsigned getBufferPackets(void) noexcept(true)
  {
    return(this->buffer_packets);
  }


  inline bool bufferPackets(void) noexcept(true)
  {
    return(lexical_cast<bool>(getBufferPackets()));
  }


  inline vector<string> getInputFiles(void) noexcept(true)
  {
    return(getOption< vector<string> >("input-file"));
  }


private:

  // Getters/Setters

  inline string getConfigFile(void) noexcept(true)
  {
    return(this->config_file);
  }


  inline void setDeviceName(string const p_value) noexcept(true)
  {
    this->device_name = p_value;
    return;
  }


  inline void setOutputDataDir(string const p_value) noexcept(true)
  {
    this->output_data_dir = p_value;
    return;
  }


  inline void setOutputFileExt(string const p_value) noexcept(true)
  {
    this->output_file_ext = p_value;
    return;
  }


  inline void setOutputFileGroupId(int unsigned const p_id) noexcept(true)
  {
    this->output_file_group_id = p_id;
    return;
  }


  inline void setUseDevice(bool const p_value) noexcept(true)
  {
    this->use_device = p_value;
    return;
  }


  void setUseRing(void) noexcept(true)
  {
    if (getOptionMap().count("use-ring"))
    {
      checkEnum("use-ring", 0, 1);
      this->use_ring = (1 == getOption<int unsigned>("use-ring"));

      if (this->use_ring && !isLinux())
      {
        FATAL(BadOption, "Exception", "The use-ring option can only be used on a system running Linux");
      }

    }
    return;
  }


  void setSuppressIpv4Output(void) noexcept(true)
  {
    if (getOptionMap().count("suppress-ipv4-output"))
    {
      checkEnum("suppress-ipv4-output", 0, 1);
      this->suppress_ipv4_output = (1 == getOption<int unsigned>("suppress-ipv4-output"));
    }
    return;
  }


  void setSuppressMetricsOutput(void) noexcept(true)
  {
    if (getOptionMap().count("suppress-metrics-output"))
    {
      checkEnum("suppress-metrics-output", 0, 1);
      this->suppress_metrics_output = (1 == getOption<int unsigned>("suppress-metrics-output"));
    }
    return;
  }


  void setIpAddressFormat(void) noexcept(true)
  {
    if (getOptionMap().count("ip-address-format"))
    {
      checkEnum("ip-address-format", 0, 1);
      this->ip_address_format = (1 == getOption<int unsigned>("ip-address-format"));
    }
    return;
  }


  void setCppFormat(void) noexcept(true)
  {
    if (getOptionMap().count("cpp-format"))
    {
      checkEnum("cpp-format", 0, 1);
      this->cpp_format = (1 == getOption<int unsigned>("cpp-format"));
      if (this->cpp_format)
      {
        setDataGuideVersion("04");
      }
    }
    return;
  }


  inline void setCppMaxRecordsPerFile(void) noexcept(true)
  {
    checkRange("pp-max-records-per-file", 0, UINT_MAX);
    this->cpp_max_records_per_file = getOption<int unsigned>("cpp-max-records-per-file");
    return;
  }


  void setSkipIpv4Packets(void) noexcept(true)
  {
    if (getOptionMap().count("skip-ipv4-packets"))
    {
      checkEnum("skip-ipv4-packets", 0, 1);
      this->skip_ipv4_packets = (1 == getOption<int unsigned>("skip-ipv4-packets"));
    }
    return;
  }


  inline void setBufferPackets(void) noexcept(true)
  {
    checkEnum("buffer-packets", 0, 1);
    this->buffer_packets = getOption<int unsigned>("buffer-packets");
    return;
  }


  inline void setPackets(void) noexcept(true)
  {
    checkRange("packets", 0, ULONG_MAX);
    this->packets = getOption<int unsigned>("packets");
    return;
  }


  inline void setCacheForceout(void) noexcept(true)
  {
    checkRange("cache-forceout", 0, 2500);
    this->cache_forceout = getOption<int unsigned>("cache-forceout");
    return;
  }


  inline void setCacheTimeout(void) noexcept(true)
  {
    checkRange("cache-timeout", 0, 2500);
    this->cache_timeout = getOption<int unsigned>("cache-timeout");
    return;
  }


  inline void setSummaryForceout(void) noexcept(true)
  {
    checkRange("summary-forceout", 0, 2500);
    this->summary_forceout = getOption<int unsigned>("summary-forceout");
    return;
  }


  inline void setMaxFlowcacheSize(void) noexcept(true)
  {
    checkSpecial("max-flowcache-size", 0, 30000);
    this->max_flowcache_size = getOption<int unsigned>("max-flowcache-size");
    return;
  }


  inline void setMaxPacketbufferSize(void) noexcept(true)
  {
    checkRange("max-packetbuffer-size", 0, 30000);
    this->max_packetbuffer_size = getOption<int unsigned>("max-packetbuffer-size");
    return;
  }


  inline void setSnaplen(void) noexcept(true)
  {
    checkRange(  "snaplen", 0, 65536);  // Must be less than 65536 - FATAL
    checkSpecial("snaplen", 0, 65535);  // Should be 65535         - WARN
    this->snaplen = getOption<int unsigned>("snaplen");
    return;
  }


  inline void setConfigFile(string const p_value) noexcept(true)
  {
    this->config_file = p_value;
    return;
  }


  inline void setInteractive(bool const p_value) noexcept(true)
  {
    this->interactive = p_value;
    return;
  }


  // Constructors

  ProgramOptions(ProgramOptions const & p_program_options) noexcept(true);


  // Operators

  ProgramOptions & operator=( ProgramOptions const & p_program_options) noexcept(true);
  bool             operator==(ProgramOptions const & p_program_options) const noexcept(true);
  bool             operator!=(ProgramOptions const & p_program_options) const noexcept(true);


  // Variables

  variables_map                  option_map;
  bool                           use_device;
  bool                           suppress_ipv4_output;
  bool                           suppress_metrics_output;
  bool                           cpp_format;
  bool                           skip_ipv4_packets;
  bool                           ip_address_format;
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
  string                         config_file;
  string                         device_name;
  string                         executable_name;
  string                         output_data_dir;
  string                         output_file_ext;
  int unsigned                   cache_forceout;
  int unsigned                   cache_timeout;
  int unsigned                   summary_forceout;
  int unsigned                   max_flowcache_size;
  int unsigned                   max_packetbuffer_size;
  int unsigned                   cpp_max_records_per_file;
  int unsigned                   packets;
  int unsigned                   snaplen;
  int unsigned                   buffer_packets;
  vector<string>                 input_files;


  // Private Functions

  void displayHelp(bool const p_condition) noexcept(true);
  void displayRuntimeVariables(void) noexcept(true);
  void displayVersion(bool const p_condition, string const & p_data_guide_ver) noexcept(true);
  void processConfigFile(void) noexcept(true);
  void processDevice(bool const p_condition) noexcept(true);
  bool processInputFiles(bool const p_condition) noexcept(true);
  void processOutputParams(void) noexcept(true);
  void processSiteName(void) noexcept(true);

  void checkEnum(string const & p_option, int unsigned const p_check0, int unsigned const p_check1);
  void checkRange(string const & p_option, u_int64_t const p_check0, u_int64_t const p_check1);
  void checkGreater(string const & p_option0, string const & p_option1);
  void checkSpecial(string const & p_option, u_int64_t const p_check0, u_int64_t const p_check1);
  void conflictingOptions(string const & p_opt1, string const & p_opt2);
  void optionDependency(string const & p_for_what, string const & p_required_option);
  void requiredOption(string const & p_option);
  void requiredOptions(string const & p_opt1, string const & p_opt2);

};

#endif // PROGRAMOPTIONS_HPP
