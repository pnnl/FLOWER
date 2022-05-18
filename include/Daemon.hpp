///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef DAEMON_HPP
#define DAEMON_HPP


// System Includes
#include <string>
#ifndef _MSC_VER
#include <unistd.h>
#endif
// External Includes
// Internal Includes
// Application Includes
#include "OutputHelper.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Daemon"


// Namespaces
using namespace std;


class Daemon
{

public:

  // Constructors

  Daemon(OutputHelper & p_output_helper, string const & p_user = "daemon", string const p_dir = "/", bool const p_started = false) noexcept(true);


  // Destructors

  ~Daemon(void) noexcept(true)
  {
    DEBUG(TRACE, ENTER);
    DEBUG(TRACE, LEAVE);
    return;
  }


  // Public Functions

  void onShutdownSystemEvent(int unsigned const p_level) noexcept(true);
  bool start(string const & p_device) noexcept(true);
  bool stop(void) noexcept(true);


private:

  // Getters/Setters

  inline string getDeviceLockFilename(void) const noexcept(true)
  {
    return(getOutputHelper().getDeviceFileLock(getDevice()).c_str());
  }


  inline string getDevice(void) const noexcept(true)
  {
    return(this->device);
  }


  inline void setDevice(string const & p_device) noexcept(true)
  {
    this->device = p_device;
    return;
  }


  inline string getDir(void) const noexcept(true)
  {
    return(this->dir);
  }


  inline string getUser(void) const noexcept(true)
  {
    return(this->user);
  }


  inline OutputHelper & getOutputHelper(void) const noexcept(true)
  {
    return(this->output_helper);
  }


  inline bool getStart(void) const noexcept(true)
  {
    return(this->started);
  }


  inline void setStart(bool const p_started) noexcept(true)
  {
    this->started = p_started;
    return;
  }


  // Constructors

  Daemon(void);
  Daemon(Daemon const & p_daemon);


  // Operators

  Daemon & operator=(Daemon const & p_daemon);


  // Private Functions

  bool lockDevice(string const & p_device) noexcept(true);


  // Variables

  OutputHelper & output_helper;
  string         device;
  string         user;
  string         dir;
  bool           started;

};

#endif // DAEMON_HPP
