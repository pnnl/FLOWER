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
#ifndef _MSC_VER // groups are linux-specific
#include <ctime>
#include <grp.h>
#include <pcap/pcap.h>
#include <pwd.h>
#include <net/if.h>
#include <unistd.h>
#endif
// External Includes
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
// Internal Includes
// Application Includes
#include "CONSTANTS.hpp"
#include "global.hpp"
#include "osUtil.hpp"
#include "EventTypes.hpp"
#include "Exception.hpp"
#include "ExceptionHandler.hpp"
#include "ProgramOptions.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "ProgramOptions"


// Namespaces used
using namespace boost;
using namespace boost::filesystem;
using boost::lexical_cast;


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


// ======================================================================
// Constructors
// ======================================================================


ProgramOptions::ProgramOptions(void) throw() :
  use_device(false),
  use_ring(false),
  command_only("Command Line Only"),
  command_and_config("Command Line And Configuration File"),
  config_only("Configuration File Only"),
  visible("Allowed options on command line and configuration file.\nDefault values are shown as (=DEFAULT)"),
  max_packetbuffer_size(0),
  buffer_packets(0)
{
  DEBUG(TRACE, ENTER);

  //
  // Declare a group of options that will be allowed only on command line
  //
  this->command_only.add_options()
    ("help,h",                                                                  "Print help information")
    ("version,v",                                                               "Print version information")
    ("interactive,I",                                                           "All output will be displayed to the screen")
    ("config-file,c",           value<string>(),                                "Configuration file")
    ("device,i",                value<string>(),                                "Network interface device (e.g. eth1)")
    ("input-file,f",            value< vector<string> >(),                      "PCAP input file(s) to read")
    ("packets,p",               value<int unsigned>()->default_value(0),        "Number of packets to read (NOTE: 0 = all packets)");

  //
  // Declare group of options that will be allowed both on command line and in config file
  //
  this->command_and_config.add_options()
    ("buffer-packets,b",        value<int unsigned>()->default_value(0),           "Capture packets in case of unexpected termination: NOTE: Only works with --use-ring option")
    ("max-packetbuffer-size,m", value<int unsigned>()->default_value(10000),       "Max number of packets to keep in the packet buffer")
    ("cache-timeout,T",         value<int unsigned>()->default_value(120),         "Min time (in seconds) to keep idle flows in the cache")
    ("cache-forceout,C",        value<int unsigned>()->default_value(900),         "Max time (in seconds) to force busy flows from the cache")
    ("summary-forceout,S",      value<int unsigned>()->default_value(900),         "Time (in seconds) to before creating new output files")
    ("output-data-dir,d",       value<string>()->default_value("/data/" APP_NAME), "Output data directory")
    ("output-file-ext,e",       value<string>()->default_value("dat"),             "Output file extension")
    ("site-name,s",             value<string>()->default_value(""),                "Name of site where data is collected")
    ("max-flowcache-size",      value<int unsigned>()->default_value(300000),      "Max number of flows allowed in the flow cache")
    ("use-ring,r",              value<int unsigned>()->default_value(0),           "Use the linux kernel PF_PACKET mmap API rather than libpcap");


  //
  // Declare a group of options that will be allowed only in config file
  //
  this->config_only.add_options()
    ("snaplen",                 value<int unsigned>()->default_value(65535),    "Packet header capture length in bytes")
    ("skip-ipv4-packets",       value<int unsigned>()->default_value(0),        "Skip Processing IPv4 Packets")
    ("suppress-ipv4-output",    value<int unsigned>()->default_value(0),        "Suppress the output of IPv4 Flow records")
    ("output-file-group",       value<string>()->default_value(""),             "The output files will be owned by this group name (from /etc/group)");


  this->cmdline_options.add(this->command_only).add(this->command_and_config);
  this->config_file_options.add(this->command_and_config).add(this->config_only);
  this->visible.add(this->command_only).add(this->command_and_config).add(this->config_only);
  this->position_options.add("input-file", -1);

  DEBUG(TRACE, LEAVE);
  return;
}


bool ProgramOptions::checkOptions(int p_argc, char ** p_argv, string const & p_data_guide_ver) throw()
{
  DEBUG(TRACE, ENTER);
  DEBUG(LOTS, "argc:%d", p_argc);

  try
  {
    try
    {
      store(command_line_parser(p_argc, p_argv).options(this->cmdline_options).positional(this->position_options).run(), getOptionMap());
    }
    catch(std::exception & e)
    {
      FATAL(BadOption, "Processing command line options", lexical_cast<string>(e.what()));
    }

    // Handle command line only options

    displayHelp(getOptionMap().count("help") || p_argc == 1);          // HELP
    displayVersion(getOptionMap().count("version"), p_data_guide_ver); // VERSION

    if (isPriviledgedUser())
    {
      setInteractive(getOptionMap().count("interactive"));
    }
                                                         // INTERACTIVE MODE
    setOutputLocation(::g_OUTPUT2CONSOLE);
    if (isPriviledgedUser() && (! isInteractive()))
    {
      setOutputLocation(::g_OUTPUT2LOG, p_argv[0]);
    }
    processConfigFile();                                 // CONFIG-FILE
    conflictingOptions("input-file", "device");
    requiredOptions("input-file", "device");
    requiredOption("site-name");
    processDevice(getOptionMap().count("device"));       // DEVICE
    if (! processInputFiles(getOptionMap().count("input-file"))) // INPUT-FILE
    {
      return(false);
    }
    setPackets();                                        // PACKETS

    // Handle command line and configuration file options

    setBufferPackets();                                  // BUFFER-PACKETS
    setMaxPacketbufferSize();                            // MAX-PACKETBUFFER-SIZE
    setCacheTimeout();                                   // CACHE-TIMEOUT
    setCacheForceout();                                  // CACHE-FORCEOUT
    setSummaryForceout();                                // SUMMARY-FORCEOUT
    checkGreater("cache-timeout",  "cache-forceout");
    checkGreater("cache-forceout", "summary-forceout");
    processOutputParams();                               // OUTPUT-FILE-GROUP
    processSiteName();                                   // SITE-NAME

    // Handle configuration file options

    setMaxFlowcacheSize();                               // MAX-FLOWCACHE-SIZE
    setSuppressIpv4Output();                             // SUPPRESS-IPV4-OUTPUT
    setSkipIpv4Packets();                                // SKIP-IPV4-PACKETS
  }
  catch(std::exception & e)
  {
    FATAL(BadOption, "Processing options", lexical_cast<string>(e.what()));
  }

#ifndef COMPILE_TIME
#define COMPILE_TIME "ERROR"
#endif

  // Check for expired license
  std::time_t current_time = std::time(nullptr);
//  std::time_t expire_date  = std::stoi(COMPILE_TIME, nullptr) + 15780000; //  Six (6) months in seconds
  //std::time_t expire_date  = std::stoi(COMPILE_TIME, nullptr) + 1; //  Expire in 1 second after compile

  //if (current_time > expire_date)
  //{
  //  displayVersion(getOptionMap().count("version"), p_data_guide_ver); // VERSION
  //  FATAL(PermissionDenied, "Expired License", "License expired on " + string(std::asctime(std::localtime(&expire_date))));
  //}

  displayRuntimeVariables();

  DEBUG(TRACE, LEAVE);
  return(true);
}


void ProgramOptions::displayRuntimeVariables(void) throw()
{
  string bools[2] = {"false", "true"};

  output("===============================================================================");
  output("Runtime Parameters:");
  if (isPriviledgedUser())
  {
    output("  Interactive Mode            = " + bools[getOptionMap().count("interactive")]);
  }
  else
  {
    output("  Interactive Mode            = true");
  }
  output("  Config File                 = " + getConfigFile());
  if (getOptionMap().count("device"))
  {
    output("  Device                      = " + getOption<string>("device"));

  if (isLinux())
  {
    output("  Use PacketRinger            = " + bools[getOption<int unsigned>("use-ring")]);
  }

    output("  Snaplen                     = " + lexical_cast<string>(getSnaplen()));
  }
  if (getOptionMap().count("input-file"))
  {
    vector<string> files = getOption< vector<string> >("input-file");
    for (vector<string>::const_iterator itor = files.begin(); itor != files.end(); ++itor)
    {
      output("  Input File                  = " + (*itor));
    }
  }
  if (0 < getPackets())
  {
    output("  Number of Packets to read   = " + lexical_cast<string>(getPackets()));
  }
  else
  {
    output("  Number of Packets to read   = ALL");
  }
  output("  Buffer Packets              = " + bools[getBufferPackets()]);
  if (bufferPackets())
  {
    output("  Number of Packets to buffer = " + lexical_cast<string>(getMaxPacketbufferSize()));
  }
  output("  Cache Timeout               = " + lexical_cast<string>(getCacheTimeout()));
  output("  Cache Forceout              = " + lexical_cast<string>(getCacheForceout()));
  output("  Summary Forceout            = " + lexical_cast<string>(getSummaryForceout()));
  output("  Output Data Directory       = " + getOutputDataDir());
  output("  Output File Extension       = " + getOutputFileExt());
  output("  Site Name                   = " + getSiteName());
  output("  Max FlowCache Size          = " + lexical_cast<string>(getMaxFlowcacheSize()));
  output("  Suppress IPv4 Flow Records  = " + bools[getOption<int unsigned>("suppress-ipv4-output")]);
  output("  Skip IPv4 Packets           = " + bools[getOption<int unsigned>("skip-ipv4-packets")]);
#ifndef _MSC_VER
  struct group * grp = getgrgid(getOutputFileGroupId());
  output("  Output File Group Name (Id) = " + lexical_cast<string>(grp->gr_name) + " (" + lexical_cast<string>(getOutputFileGroupId()) + ")");
#endif
  output("===============================================================================");

  return;
}


// ======================================================================
// Private Functions
// ======================================================================


void ProgramOptions::displayHelp(bool const p_condition) throw()
{
  if (! p_condition)
  {
    return;
  }

  string message = "\n";
  stringstream ss("");
  ss << this->visible;
  message += ss.str();
  message += "Examples:\n\n";
  message += "  To create network flows from the eth0 network interface:\n";
  message += "    " APP_NAME " --config-file=/etc/" APP_NAME ".conf --device=eth0\n\n";
  message += "  To create network flows from all the pcap files in the /data directory:\n";
  message += "    " APP_NAME " --config-file=/etc/" APP_NAME ".conf /data/*.pcap\n\n";
  message += "  To create network flows and create output files in /data/" APP_NAME ":\n";
  message += "    " APP_NAME " --device=eth0 --output-data-dir=/data/" APP_NAME "\n\n";
  message += "See " APP_NAME "(8) or " APP_NAME ".conf(5) man page for more examples\n";
  output(message);
  exit(EXIT_SUCCESS);
}


string ProgramOptions::getVersionRecord(string const & p_data_guide_ver) throw()
{
#ifndef APP_DESC
#define APP_DESC "ERROR"
#endif
#ifndef APP_NAME
#define APP_NAME "ERROR"
#endif
#ifndef BOOST_VER
#define BOOST_VER "ERROR"
#endif
#ifndef CXX_VER
#define CXX_VER "ERROR"
#endif
#ifndef CXX_OPTIMIZE_LEVEL
#define CXX_OPTIMIZE_LEVEL "ERROR"
#endif
#ifndef CXX_DEBUG_LEVEL
#define CXX_DEBUG_LEVEL "ERROR"
#endif
#ifndef MAJOR_VERSION
#define MAJOR_VERSION "ERROR"
#endif
#ifndef MINOR_VERSION
#define MINOR_VERSION "ERROR"
#endif
#ifndef MICRO_VERSION
#define MICRO_VERSION "ERROR"
#endif

  string pcap_ver = pcap_lib_version();
  string record;

  record  = "Ver:" MAJOR_VERSION "." MINOR_VERSION "." MICRO_VERSION
            ",Compiler:" CXX_VER
            ",OptLevel:" CXX_OPTIMIZE_LEVEL
            ",Debug:" CXX_DEBUG_LEVEL
            ",BoostLibVer:" BOOST_VER
            ",PcapLibVer:" + pcap_ver +
            ",Compiled:" __DATE__ "  " __TIME__ ;
  record += ",DataGuideVer:" + p_data_guide_ver;

  return(record);
}


void ProgramOptions::displayVersion(bool const p_condition, string const & p_data_guide_ver) throw()
{
  if (! p_condition)
  {
    return;
  }

  string cpr;
  cpr += "   Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.\n\n";

  string dis;
  dis += "This software was prepared as an account of work sponsored by\n";
  dis += "an agency of the United States Government. Neither the United\n";
  dis += "States Government nor any agency thereof, nor Battelle Memorial\n";
  dis += "Institute, nor any of their employees, makes any warranty,\n";
  dis += "express or implied, or assumes any legal liability or\n";
  dis += "responsibility for the accuracy, completeness, or usefulness\n";
  dis += "of any information, apparatus, product, or process disclosed,\n";
  dis += "or represents that its use would not infringe privately owned\n";
  dis += "rights. Reference herein to any specific commercial product,\n";
  dis += "process, or service by trade name, trademark, manufacturer, or\n";
  dis += "otherwise does not necessarily constitute or imply its\n";
  dis += "endorsement, recommendation, or favoring by the United States\n";
  dis += "Government or any agency thereof, or Battelle Memorial Institute.\n";
  dis += "The views and opinions of authors expressed herein do not\n";
  dis += "necessarily state or reflect those of the United States Government\n";
  dis += "or any agency thereof.\n\n";
  dis += "   PACIFIC NORTHWEST NATIONAL LABORATORY\n";
  dis += "       operated by BATTELLE for the\n";
  dis += "    UNITED STATES DEPARTMENT OF ENERGY\n";
  dis += "     under Contract DE-AC06-76RL01830\n";

#ifndef APP_DESC
#define APP_DESC "ERROR"
#endif
#ifndef APP_NAME
#define APP_NAME "ERROR"
#endif
#ifndef BOOST_VER
#define BOOST_VER "ERROR"
#endif
#ifndef COMPILE_TIME
#define COMPILE_TIME "ERROR"
#endif
#ifndef CXX_VER
#define CXX_VER "ERROR"
#endif
#ifndef CXX_OPTIMIZE_LEVEL
#define CXX_OPTIMIZE_LEVEL "ERROR"
#endif
#ifndef CXX_DEBUG_LEVEL
#define CXX_DEBUG_LEVEL "ERROR"
#endif
#ifndef MAJOR_VERSION
#define MAJOR_VERSION "ERROR"
#endif
#ifndef MINOR_VERSION
#define MINOR_VERSION "ERROR"
#endif
#ifndef MICRO_VERSION
#define MICRO_VERSION "ERROR"
#endif

  string pcap_ver = pcap_lib_version();
  string message;

  message  = "\n   " APP_DESC "\n"
             "\n   " APP_NAME " version:           " MAJOR_VERSION "." MINOR_VERSION "." MICRO_VERSION
             "\n   Compiled on:              " __DATE__ ", " __TIME__ " (" COMPILE_TIME ")"
             "\n   Compiled with:            " CXX_VER
             "\n     Optimize Level:         " CXX_OPTIMIZE_LEVEL
             "\n     Debug:                  " CXX_DEBUG_LEVEL
             "\n     Boost library version:  " BOOST_VER
             "\n     pcap  library version:  " + pcap_ver;
  message += "\n   Data Guide version:       " + p_data_guide_ver + "\n\n";
  message += cpr;
  message += dis;
  output(message);

  exit(EXIT_SUCCESS);
}


string ProgramOptions::getDefaultConfigFile(void) throw()
{
  // DEVELOPER NOTE: Derived from:
  // http://www.linuxquestions.org/questions/programming-9/get-full-path-of-a-command-in-c-117965/

  // /proc/self is a symbolic link to the process-ID subdir
  // of /proc, e.g. /proc/4323 when the pid of the process
  // of this program is 4323.
  //
  // Inside /proc/<pid> there is a symbolic link to the
  // executable that is running as this <pid>.  This symbolic
  // link is called "exe".
  //
  // So if we read the path where the symlink /proc/self/exe
  // points to we have the full path of the executable.

  string config_file;
  int const MAXPATHLEN = 1024;
  char      fullpath[MAXPATHLEN];

#ifndef _MSC_VER
  int       length = readlink("/proc/self/exe", fullpath, sizeof(fullpath));

  // Catch some errors:
  if (length < 0)
  {
    fprintf(stderr, "Error resolving symlink /proc/self/exe.\n");
    exit(EXIT_FAILURE);
  }
  if (length >= MAXPATHLEN)
  {
    fprintf(stderr, "Path too long. Truncated.\n");
    exit(EXIT_FAILURE);
  }

  // The string this readlink() function returns is appended with a '@'.
  // Strip '@' off the end.
  fullpath[length] = '\0';
#else
  GetModuleFileName(NULL, fullpath, MAX_PATH);
#endif

  path pathname(fullpath);
  path dirname = pathname.parent_path().parent_path();
  dirname /= path("conf/" APP_NAME ".conf");

  return(dirname.string());
}


void ProgramOptions::processConfigFile(void) throw()
{
  DEBUG(TRACE, ENTER);

  string config_file("");
#ifndef _MSC_VER
  if (getOptionMap().count("config-file"))
  {
    config_file = getOption<string>("config-file");
  }
  else
  {
    config_file = getDefaultConfigFile();
  }
#else
  requiredOption("config-file");
  config_file += getOption<string>("config-file");
#endif

  DEBUG(SCALAR, "config_file:%s:", config_file.c_str());

  if ((! config_file.empty()) && exists(config_file))
  {
    // Read in the config file, parse command line, and merge results into option_map
    std::ifstream ifs(config_file.c_str());
    try
    {
      store(parse_config_file(ifs, this->config_file_options), getOptionMap());
    }
    catch(std::exception & e)
    {
      FATAL(BadOption, "Processing configuration file options", lexical_cast<string>(e.what()));
    }
    notify(getOptionMap());
    setConfigFile(config_file);
  }
  else
  {
    FATAL(NotFound, config_file, "Missing configuration file");
  }

  DEBUG(TRACE, LEAVE);
  return;
}


void ProgramOptions::processOutputParams(void) throw()
{
  DEBUG(TRACE, ENTER);

  regex alphanumeric("[a-zA-Z]+[a-zA-Z0-9_]*");

  // OUTPUT-DATA-DIR

  string output_dir = getOption<string>("output-data-dir");
  if (! exists(output_dir))
  {
    FATAL(NotFound, output_dir, "Output directory is missing");
  }
#ifndef _MSC_VER // this is linux-specific
  else
  {
    if (0 != access(output_dir.c_str(), W_OK))
    {
      FATAL(PermissionDenied, output_dir, "Cannot write to output directory");
    }
  }
#endif
  setOutputDataDir(output_dir);


  // OUTPUT-FILE-EXT

  // Make sure file extension is alphanumeric and no spaces
  if (! regex_match(getOption<string>("output-file-ext"), alphanumeric))
  {
    FATAL(AlphaNumeric, getOption<string>("output-file-ext"), "invalid output-file-ext");
  }
  setOutputFileExt(getOption<string>("output-file-ext"));

#ifndef _MSC_VER // this is linux-specific
  gid_t group_id    = getgid();
  bool  group_found = false;
  setOutputFileGroupId(group_id);

  if (getOptionMap().count("output-file-group"))
  {
    if (0 < (getOption<string>("output-file-group")).length())
    {
      if (! regex_match(getOption<string>("output-file-group"), alphanumeric))
      {
        FATAL(AlphaNumeric, getOption<string>("output-file-group"), "invalid output-file-group");
      }
      else
      {
        struct group * grp = getgrnam(getOption<string>("output-file-group").c_str());
        if (! grp)
        {
          FATAL(NotFound, "output-file-group", ("The output-file-group (" + getOption<string>("output-file-group") + ") specified in the configration file does not exist in /etc/group.").c_str());
        }
        group_found = true;
        group_id    = grp->gr_gid;
      }
    }
  }

  gid_t list[NGROUPS_MAX];
  int num_grps = getgroups(NGROUPS_MAX, list);
  bool found   = false;

  for (int i = 0; i < num_grps; ++i)
  {
    if (group_id == list[i])
    {
      setOutputFileGroupId(group_id);
      found = true;
      break;
    }
  }

  if (! found)
  {
    if (group_found)
    {
      string message = "The user is not a member of the group (" + getOption<string>("output-file-group") + ") specified by the output-file-group in the configration file.";
      FATAL(NotFound, "output-file-group", message.c_str());
    }
  }
#endif

  DEBUG(TRACE, LEAVE);
  return;
}


void ProgramOptions::processSiteName(void) throw()
{
  DEBUG(TRACE, ENTER);

  // Make sure site name is alphanumeric, no spaces, and exists
  if (0 == strlen(getOption<string>("site-name").c_str()))
  {
    FATAL(NotFound, "Exception", "The site-name must be specified in the configuration file or on the command line and must start with a letter and be alphanumeric (a-zA-Z0-9_)");
  }

  // Make sure site name is MIN_SITE_NAME_LEN or more characters
  if (MIN_SITE_NAME_LEN > strlen(getOption<string>("site-name").c_str()))
  {
    string message = "The site-name is too short, it must be at least " + uitoa10(MIN_SITE_NAME_LEN) + " characters";
    FATAL(RangeError, "Exception", message.c_str());
  }

  // Make sure site name is not more than MAX_SITE_NAME_LEN characters
  if (MAX_SITE_NAME_LEN < strlen(getOption<string>("site-name").c_str()))
  {
    string message = "The site-name is too long, it cannot be more than " + uitoa10(MAX_SITE_NAME_LEN) + " characters";
    FATAL(RangeError, "Exception", message.c_str());
  }

  // Make sure site name is alphanumeric with no spaces
  regex alphanumeric("[a-zA-Z]+[a-zA-Z0-9_]*");
  if (! regex_match(getOption<string>("site-name"), alphanumeric))
  {
    FATAL(AlphaNumeric, getOption<string>("site-name"), "invalid site name");
  }

  setSiteName(getOption<string>("site-name"));

  DEBUG(TRACE, LEAVE);
  return;
}


void ProgramOptions::processDevice(bool const p_condition) throw()
{
  if (! p_condition)
  {
    return;
  }

  string device = getOption<string>("device");

  if (! checkNetworkInterface(device))
  {
    // DEVELOPER NOTE: checkNetworkInterface prints the error message
    exit(EXIT_FAILURE);
  }

  if (! isPriviledgedUser())
  {
    FATAL(PermissionDenied, "Trying to read from a device", ("Must be root to read from a device(" + device + ")").c_str());
  }

  // SNAPLEN
  setSnaplen();

  setUseDevice(true);

  // USE-RING (Only for Linux OS)
  setUseRing();

  setDeviceName(device);
  return;
}


bool ProgramOptions::processInputFiles(bool const p_condition) throw()
{
  bool result = true;

  if (! p_condition)
  {
    return(result);
  }

  vector<string>           input_files = getOption< vector<string> >("input-file");
  vector<string>::iterator itor;
  for (itor = input_files.begin(); itor != input_files.end(); ++itor)
  {
    if (! exists((*itor)))
    {
      CAUTION(NotFound, "Input File", ("File, " + (*itor) + ", does not exist.").c_str());
      result = false;
    }
  }

  return(result);
}


//  Function used to check that 'option' is used
void ProgramOptions::requiredOption(string const & p_option) throw(logic_error)
{
  DEBUG(TRACE, ENTER);

  if (0 == getOptionMap().count(p_option))
  {
    throw(logic_error("Missing a required option '" + p_option + "'."));
  }

  DEBUG(TRACE, LEAVE);
  return;
}


//  Function used to check that one of 'opt1' and 'opt2' are used
void ProgramOptions::requiredOptions(string const & p_opt1, string const & p_opt2) throw(logic_error)
{
  DEBUG(TRACE, ENTER);

  if (0 == getOptionMap().count(p_opt1) && 0 == getOptionMap().count(p_opt2))
  {
    throw(logic_error("Missing a required options '" + p_opt1 + "' or '" + p_opt2 + "'."));
  }

  DEBUG(TRACE, LEAVE);
  return;
}


void ProgramOptions::checkEnum(string const & p_option, int unsigned const p_check0, int unsigned const p_check1)
{
  if (getOptionMap().count(p_option))
  {
    int unsigned value = getOption<int unsigned>(p_option);
    if ((p_check0 != value) && (p_check1 != value))
    {
      FATAL(RangeError, "Exception", "The value of " + p_option + ", " + lexical_cast<string>(value) + ", must be " + lexical_cast<string>(p_check0) + " or " + lexical_cast<string>(p_check1) + ".");
    }
  }
  return;
}


void ProgramOptions::checkRange(string const & p_option, u_int64_t const p_check0, u_int64_t const p_check1)
{
  if (getOptionMap().count(p_option))
  {
    int unsigned value = getOption<int unsigned>(p_option);
    if (p_check0 > value)
    {
      FATAL(RangeError, "Exception", "The value of " + p_option + ", " + lexical_cast<string>(value) + ", must be greater than " + lexical_cast<string>(p_check0) + ".");
    }
    else if (p_check1 < value)
    {
      CAUTION(RangeError, "Exception", "The value of " + p_option + ", " + lexical_cast<string>(value) + ", should be less than " + lexical_cast<string>(p_check1) + ".");
    }
  }
  return;
}


void ProgramOptions::checkGreater(string const & p_option0, string const & p_option1)
{
  if (getOption<int unsigned>(p_option0) > getOption<int unsigned>(p_option1))
  {
    FATAL(RangeError, "Exception", "The value of " + p_option0 + ", " + lexical_cast<string>(getOption<int unsigned>(p_option0)) + ", must be less than the value of " + p_option1 + ", " + lexical_cast<string>(getOption<int unsigned>(p_option1)) + ".");
  }
  return;
}


void ProgramOptions::checkSpecial(string const & p_option, u_int64_t const p_check0, u_int64_t const p_check1)
{
  if (getOptionMap().count(p_option))
  {
    int unsigned value = getOption<int unsigned>(p_option);
    if (p_check0 > value)
    {
      FATAL(RangeError, "Exception", "The value of " + p_option + ", " + lexical_cast<string>(value) + ", must be greater than " + lexical_cast<string>(p_check0) + ".");
    }
    else if (p_check1 > value)
    {
      CAUTION(RangeError, "Exception", "The value of " + p_option + ", " + lexical_cast<string>(value) + ", should be " + lexical_cast<string>(p_check1) + " unless you REALLY know what you are doing. Use at your own risk.");
    }
  }
  return;
}


//  Function used to check that 'opt1' and 'opt2' are not specified at the same time.
void ProgramOptions::conflictingOptions(string const & p_opt1, string const & p_opt2) throw(logic_error)
{
  DEBUG(TRACE, ENTER);

  if (
      getOptionMap().count(p_opt1) && ! getOptionMap()[p_opt1].defaulted() &&
      getOptionMap().count(p_opt2) && ! getOptionMap()[p_opt2].defaulted()
     )
  {
    throw(logic_error("Conflicting options '" + p_opt1 + "' and '" + p_opt2 + "'."));
  }

  DEBUG(TRACE, LEAVE);
  return;
}


//  Function used to check that if 'for_what' is specified, then
//  'required_option' is specified too.
void ProgramOptions::optionDependency(string const & p_for_what, string const & p_required_option) throw(logic_error)
{
  DEBUG(TRACE, ENTER);

  if (getOptionMap().count(p_for_what) && ! getOptionMap()[p_for_what].defaulted())
  {
    if (0 == getOptionMap().count(p_required_option) || getOptionMap()[p_required_option].defaulted())
    {
      throw(logic_error("Option '" + p_for_what + "' requires option '" + p_required_option + "'."));
    }
  }

  DEBUG(TRACE, LEAVE);
  return;
}
