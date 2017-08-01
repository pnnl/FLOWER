///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
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

  Daemon(OutputHelper & p_output_helper, string const & p_user = "daemon", string const p_dir = "/", bool const p_started = false) throw();


  // Destructors

  ~Daemon(void) throw()
  {
    DEBUG(TRACE, ENTER);
    DEBUG(TRACE, LEAVE);
    return;
  }


  // Public Functions

  void onShutdownSystemEvent(int unsigned const p_level) throw();
  bool start(string const & p_device) throw();
  bool stop(void) throw();


private:

  // Getters/Setters

  inline string getDeviceLockFilename(void) const throw()
  {
    return(getOutputHelper().getDeviceFileLock(getDevice()).c_str());
  }


  inline string getDevice(void) const throw()
  {
    return(this->device);
  }


  inline void setDevice(string const & p_device) throw()
  {
    this->device = p_device;
    return;
  }


  inline string getDir(void) const throw()
  {
    return(this->dir);
  }


  inline string getUser(void) const throw()
  {
    return(this->user);
  }


  inline OutputHelper & getOutputHelper(void) const throw()
  {
    return(this->output_helper);
  }


  inline bool getStart(void) const throw()
  {
    return(this->started);
  }


  inline void setStart(bool const p_started) throw()
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

  bool lockDevice(string const & p_device) throw();


  // Variables

  OutputHelper & output_helper;
  string         device;
  string         user;
  string         dir;
  bool           started;

};

#endif // DAEMON_HPP
