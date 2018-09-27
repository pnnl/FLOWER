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
#include <errno.h>
#include <fcntl.h>
#ifndef _MSC_VER
#include <pwd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#endif
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"
#include "num2string.hpp"
#include "osUtil.hpp"
#include "Daemon.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Daemon"


// Constructors

Daemon::Daemon(OutputHelper & p_output_helper, string const & p_user, string const p_dir, bool const p_started) noexcept(true) :
  output_helper(p_output_helper),
  device(""),
  user(p_user),
  dir(p_dir),
  started(p_started)
{
  // DEVELOPER NOTE:
  //
  // From http://blog.emptycrate.com/node/219
  //  and
  // From: http://en.wikipedia.org/wiki/Daemon_%28computer_software%29
  // then Extern Link: http://www-theorie.physik.unizh.ch/~dpotter/howto/daemonize//
  //
  //  daemonize the currently running programming
  //  Note: the calls to strerror are not thread safe, but that should not matter
  //  as the application is only just starting up when this function is called
  return;
}


// Public Functions

void Daemon::onShutdownSystemEvent(int unsigned const p_level) noexcept(true)
{
  stop();
  return;
}


bool Daemon::stop(void) noexcept(true)
{
#ifdef _MSC_VER
  return false;
#else

  DEBUG(TRACE, ENTER);
  DEBUG(SCALAR, "DeviceLockFilename:%s:", getDeviceLockFilename().c_str());

  bool result = false;

  if (getStart())
  {
    if (! getDevice().empty())
    {
      // Remove the file lock, close the file, and remove the file.
      if (getOutputHelper().closeLocked(getDeviceLockFilename()))
      {
        // DEVELOPER NOTE: Be paranoid about trying to remove "/"
        if (getDeviceLockFilename().rfind("/") > 2)
        {
          int check = unlink(getDeviceLockFilename().c_str());
          if (0 != check)
          {
            ERROR_MSG(FileIO, "Unable to remove Device Lock File", ("Unable to unlink file, " + getDeviceLockFilename() + " (" + uitoa10(check) + ")").c_str());
          }
          else
          {
            // SUCCESS 
            result = true;
            //ERROR_MSG(FileIO, "Removed Device Lock File", ("Removed device lock file, " + getDeviceLockFilename()).c_str());
          }
        }
        else
        {
          ERROR_MSG(FileIO, "Trying to remove Device Lock File", ("Unable to unlink file, " + getDeviceLockFilename()).c_str());
        }
      }
      else
      {
        ERROR_MSG(FileIO, "Trying to unlock and close Device Lock File", ("Unable to unlock and close file, " + getDeviceLockFilename()).c_str());
      }
    }
    else
    {
      ERROR_MSG(FileIO, "Trying to unlock and close Device Lock File", ("Unable to locate device, " + getDevice()).c_str());
    }
  }
  DEBUG(TRACE, LEAVE);
  return(result);

#endif
}


bool Daemon::lockDevice(string const & p_device) noexcept(true)
{
  setDevice(p_device);

  // Create a lock on the interface so we can only have one instance of
  // APP_NAME reading from a single device
  if (! getOutputHelper().openLocked(getDeviceLockFilename()))
  {
    output("Shutting down: another instance of " APP_NAME " already has a lock on interface (" + getDevice() + ").\nRemove lock file, " + getDeviceLockFilename() + ", if another instance of " APP_NAME " is not running.");
    exit(EXIT_FAILURE);
  }

  return(true);
}


bool Daemon::start(string const & p_device) noexcept(true)
{
#ifdef _MSC_VER
  return(true);
#else

  rlimit rl;

  setDevice(p_device);

  // Already a daemon
  if ((1 == getppid()) || getStart())
  {
    return(true);
  }

  if (0 > getrlimit(RLIMIT_NOFILE, &rl))
  {
    output("Can't get rlimit (" + static_cast<string>(strerror(errno)) + ")");
    exit(EXIT_FAILURE);
  }

  // Drop user if there is one, and we were run as root
  if ((0 == getuid()) || (0 == geteuid()))
  {
    struct passwd * pw = getpwnam(getUser().c_str());
    if (pw)
    {
      syslog(LOG_NOTICE, "Setting user to %s", getUser().c_str());
      setuid(pw->pw_uid);
    }
    else
    {
      output("Unable to run as user (" + getUser() + ")");
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    output("Unable to run as user (" + getUser() + ")");
    exit(EXIT_FAILURE);
  }


  int daemon_status = daemon(0, 0);
  int errno_save    = errno;


  if (0 > daemon_status)
  {
    // Unexpected file descriptors
    string err_message = "Unable to create daemon process: " + static_cast<string>(strerror(errno_save));
    output(err_message);
    exit(EXIT_FAILURE);
  }

  lockDevice(getDevice());
  setStart(true);
  return(true);
#endif
}
