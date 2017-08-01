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
#include <iostream>
#include <string>
#ifndef _MSC_VER // groups are linux-specific
#include <errno.h>
#include <grp.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#endif
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"
#include "osUtil.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "osUtil"


// Namespaces
using namespace std;


// Getters/Setters

string const & getPathSeparator(void) throw()
{
#ifndef _MSC_VER
  static string path_separator("/");
#else
  static string path_separator("\\");
#endif
  return(path_separator);
}


bool const isPriviledgedUser(void) throw()
{
#ifndef _MSC_VER
  return((0 == getuid()) || (0 == geteuid()));
#else
  // TODO: Handle 2009/01/06 MS stuff
  return(true);
#endif

}


void outputSeparator(void) throw()
{
  output("-------------------------------------------------------------------------------");
  return;
}


void output(string const & p_message) throw()
{
#ifndef _MSC_VER
  if (getOutputLocation() == g_OUTPUT2LOG)
  {
    syslog(LOG_ERR, p_message.c_str());
  }
  else
  {
    cout << p_message << endl;
  }
#else
  cout << "LOG_ERR: " << p_message << endl;
#endif
  return;
}


bool const isLinux(void) throw()
{
#ifdef __linux__
  return(true);
#else
  return(false);
#endif
}


bool const checkNetworkInterface(string const & p_device) throw()
{
#ifdef __linux__
  // Open a socket against that interface
  int    sock       = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  int    errno_save = errno;
  string err_message;

  if (0 > sock)   // Did the socket open OK?
  {
    err_message = "The kernel socket did not open correctly: " + static_cast<string>(strerror(errno_save));
    ERROR(TSNH, "Trying to acquire kernel socket", err_message.c_str());
    DEBUG(TRACE, LEAVE);
    return(false);
  }

  // Find the interface index
  struct ifreq req;
  strncpy(req.ifr_name, p_device.c_str(), IFNAMSIZ);
  int ioctl_result = ioctl(sock, SIOCGIFINDEX, &req);
  errno_save = errno;

  if (0 > ioctl_result)
  {
    err_message = "The ioctl operation Failed: " + static_cast<string>(strerror(errno_save));
    ERROR(TSNH, "Trying to acquire interface index", err_message.c_str());
    DEBUG(TRACE, LEAVE);
    return(false);
  }

  return(true);
#else
  return(false);
#endif
}

