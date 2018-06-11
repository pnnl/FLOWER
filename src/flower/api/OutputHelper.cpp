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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <exception>
// External Includes
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
// Internal Includes
// Application Includes
#include "num2string.hpp"
#include "osUtil.hpp"
#include "OutputHelper.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "OutputHelper"


// Namespaces used
namespace fs = boost::filesystem;


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


// ======================================================================
// Constructors
// ======================================================================


OutputHelper::OutputHelper(string const & p_output_data_dir, string const & p_output_file_ext, int unsigned const p_output_file_group_id, string const & p_site_name, string const & p_data_guide_ver, string const & p_delimeter) throw() :
  output_data_dir(p_output_data_dir),
  output_file_ext(p_output_file_ext),
  output_file_group_id(p_output_file_group_id),
  site_name(p_site_name),
  data_guide_ver(p_data_guide_ver),
  delimeter(p_delimeter)
{
  DEBUG(TRACE, ENTER);
  DEBUG(TRACE, LEAVE);
  return;
}

 
// ======================================================================
// Public Functions
// ======================================================================


void OutputHelper::renameTempFilename(string const & p_temp_filename) throw()
{
  DEBUG(TRACE, ENTER);
  string filename = p_temp_filename;

  if (! filename.empty())
  {
    DEBUG(INFO, "Renaming file");
    filename.erase(filename.rfind(getPathSeparator() + getDelimiter()) + 1, 1);

    fs::path src(p_temp_filename);
    fs::path dst(filename);
    string error_message("Trying to rename " + p_temp_filename + " to " + filename);
    if (! exists(src))
    {
      ERROR_MSG(NotFound, "Missing temporary file", error_message.c_str());
    }
    if (exists(dst))
    {
      ERROR_MSG(FileIO, "File already exists", error_message.c_str());
    }
    try
    {
      rename(src, dst);
    }
    catch (std::exception & e)
    {
      ERROR_MSG(FileIO, "Rename temporary file", error_message.c_str());
    }
  }
  DEBUG(TRACE, LEAVE);
  return;
}


void OutputHelper::setFileSettings(string const & p_filename) throw()
{
  DEBUG(TRACE, ENTER);
  fs::path filename(p_filename);
  if (exists(filename))
  {

#ifndef _MSC_VER
    chown(p_filename.c_str(), (uid_t)-1, getOutputFileGroupId());
    chmod(p_filename.c_str(), S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
#endif

  }
  else
  {
    string error_message("Trying to chown and chmod file: " + p_filename);
    ERROR_MSG(NotFound, "Missing file", error_message.c_str());
  }
  DEBUG(TRACE, LEAVE);
  return;
}

#ifndef _MSC_VER

int OutputHelper::getFileDescriptor(string const & p_filename) throw()
{
  DEBUG(TRACE, ENTER);
  int fd = -1;

  map<string, int>::iterator end  = getFileLookup().end();
  map<string, int>::iterator itor = getFileLookup().find(p_filename);

  if (end != itor)
  {
    fd = (*itor).second;
  }

  DEBUG(TRACE, LEAVE);
  return(fd);
}


bool OutputHelper::eraseFileDescriptor(string const & p_filename) throw()
{
  DEBUG(TRACE, ENTER);

  bool                       result = false;
  map<string, int>::iterator end    = getFileLookup().end();
  map<string, int>::iterator itor   = getFileLookup().find(p_filename);

  if (end != itor)
  {
    getFileLookup().erase(itor);
    result = true;
  }

  DEBUG(TRACE, LEAVE);
  return(result);
}

#else

HANDLE OutputHelper::getFileDescriptor(string const & p_filename) throw()
{
  DEBUG(TRACE, ENTER);
  HANDLE fd = INVALID_HANDLE_VALUE;

  fd = (getFileLookup())[p_filename];

  DEBUG(TRACE, LEAVE);
  return(fd);
}

bool OutputHelper::eraseFileDescriptor(string const & p_filename) throw()
{
  DEBUG(TRACE, ENTER);

  bool result = true;

  getFileLookup().erase(p_filename);

  DEBUG(TRACE, LEAVE);
  return(result);
}

#endif


bool OutputHelper::openLocked(string const & p_filename) throw()
{
  DEBUG(TRACE, ENTER);

#ifndef _MSC_VER

  string message = "Trying to open and lock file, " + p_filename;
  int    fd      = open(p_filename.c_str(), O_WRONLY|O_CREAT|O_EXCL, 0666);

  if (0 <= fd)
  {
    // the OPEN worked so now LOCK the file
    struct flock f_lock;
    f_lock.l_type   = F_WRLCK;
    f_lock.l_whence = SEEK_SET;
    f_lock.l_start  = 0;
    f_lock.l_len    = 0;
    int result      = fcntl(fd, F_SETLK, &f_lock);
    if (0 <= result)
    {
      getFileLookup().insert(make_pair(p_filename, fd));
      DEBUG(TRACE, LEAVE);
      return(true);
    }
    else
    {
      ERROR_MSG(FileIO, "Can't open or lock file", (message + " (" + strerror(errno) + ")").c_str());
    }
  }
  else if ((0 > fd) && (errno == EEXIST))
  {
    // The file already exist; another process is holding the lock
    ERROR_MSG(FileIO, "Can't lock existing file", (message + " (" + strerror(errno) + ")").c_str());
  }
  else
  {
    ERROR_MSG(FileIO, "Undefined Behavior", (message + " (" + strerror(errno) + ")").c_str());
  }

#else

  // Handle MS stuff

  HANDLE fileHandle = CreateFile(p_filename.c_str(), FILE_GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  DWORD  Error      = GetLastError();

  if (INVALID_HANDLE_VALUE != fileHandle)
  {
    this->file_lookup.insert(make_pair(p_filename, fileHandle));
    DEBUG(TRACE, LEAVE);
    return(true);
  }
  else
  {
    string message = "Trying to open and lock file, " + p_filename;

    if (ERROR_FILE_EXISTS == Error)
    {
      // The file already exist; another process is holding the lock
      ERROR_MSG(FileIO, "Can't lock existing file", message.c_str()); 
    }
    else
    {
      ERROR_MSG(FileIO, "Can't open file", message.c_str()); 
    }
  }

#endif

  DEBUG(TRACE, LEAVE);
  return(false);
}


bool OutputHelper::closeLocked(string const & p_filename) throw()
{
  DEBUG(TRACE, ENTER);
  bool result    = false;
  string message = "Trying to unlock and close file, " + p_filename; 

#ifndef _MSC_VER

  if (exists(fs::path(p_filename)))
  {
    if (0 == close(getFileDescriptor(p_filename)))
    {
      result = true;
    }
    else
    {
      CAUTION(FileIO, "Can't close file", (message + " (" + strerror(errno) + ")").c_str());
      result = true;
    }
  }
  else
  {
    ERROR_MSG(NotFound, "Can't find file", message.c_str()); 
  }

#else

  // TODO: 2009/01/06: Create fileExists function for MS Windows

  HANDLE fileHandle = getFileDescriptor(p_filename);

  // Check to see if the is a valid file handle in the map
  if ((INVALID_HANDLE_VALUE != fileHandle) && (NULL != fileHandle))
  {
    // DEVELOPER NOTE: Overlapped just contains the file offset and is not
    //                 being used for asynchronous processing.
    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));

    if (CloseHandle(fileHandle))
    {
      result = true;
    }
    else
    {
      DWORD Error = GetLastError();
      ERROR_MSG(FileIO, "Can't close file", message.c_str());
    }
  }
  
#endif

  (getFileLookup())[p_filename];
  DEBUG(TRACE, LEAVE);
  return(result);
}
