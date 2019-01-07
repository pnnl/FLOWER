
# From: https://github.com/OPM/opm-common/blob/master/cmake/Modules/UseSystemInfo.cmake
# probe various system files that may be found
function (distro_name varname)
  file (GLOB has_fake_release   /Users/d3c572/Downloads/fake-release)
  file (GLOB has_os_release     /etc/os-release)
  file (GLOB has_lsb_release    /etc/lsb-release)
  file (GLOB has_system_release /etc/system-release)
  file (GLOB has_centos_release /etc/centos-release)
  file (GLOB has_debian_release /etc/debian-release)
  file (GLOB has_fedora_release /etc/fedora-release)
  file (GLOB has_redhat_release /etc/redhat-release)
  set (_descr)
  # start with /etc/os-release,
  # see <http://0pointer.de/blog/projects/os-release.html>
  if (NOT has_fake_release STREQUAL "")
    read_release (ID FROM /Users/d3c572/Downloads/fake-release INTO _descr)
  # previous "standard", used on older Ubuntu and Debian
  elseif (NOT has_os_release STREQUAL "")
    read_release (ID FROM /etc/os-release INTO _descr)
  # previous "standard", used on older Ubuntu and Debian
  elseif (NOT has_lsb_release STREQUAL "")
    read_release (DISTRIB_ID FROM /etc/lsb-release INTO _descr)
  endif ()
  # RHEL/CentOS etc. has just a text-file
  if (NOT _descr)
    if     (NOT   has_system_release STREQUAL "")
      file (READ /etc/system-release _descr)
    elseif (NOT   has_centos_release STREQUAL "")
      file (READ /etc/centos-release _descr)
    elseif (NOT   has_debian_release STREQUAL "")
      file (READ /etc/debian-release _descr)
    elseif (NOT   has_fedora_release STREQUAL "")
      file (READ /etc/fedora-release _descr)
    elseif (NOT   has_redhat_release STREQUAL "")
      file (READ /etc/redhat-release _descr)
    else ()
      # no yet known release file found
      set (_descr "unknown")
    endif ()
  endif ()
  # return from function (into appropriate variable)
  string (STRIP "${_descr}" _descr)
  set (${varname} "${_descr}" PARENT_SCOPE)
endfunction (distro_name)


# read property from the newer /etc/os-release
function (read_release valuename FROM filename INTO varname)
  file (STRINGS ${filename} _distrib
    REGEX "^${valuename}="
    )
  string (REGEX REPLACE
    "^${valuename}=\"?\(.*\)" "\\1" ${varname} "${_distrib}"
    )
  # remove trailing quote that got globbed by the wildcard (greedy match)
  string (REGEX REPLACE
    "\"$" "" ${varname} "${${varname}}"
    )
  set (${varname} "${${varname}}" PARENT_SCOPE)
  #set (${varname} "${${varname}}" CACHE STRING "Operating system name" FORCE PARENT_SCOPE)
endfunction ()


#
#  Determine the Operating system type and flavor
#  Based on https://gist.github.com/lbaehren/2413369
#
while (1)
  distro_name (CMAKE_OS_NAME)
  break()
  #########################################################################
  ##  Check for new Linux OS Versions
  if (UNIX)
    if (APPLE)
      set (CMAKE_OS_NAME "osx" CACHE STRING "Operating system name" FORCE)
      break()
    else ()
      distro_name (CMAKE_OS_NAME)
      break()
    endif ()
  endif ()
  break()
endwhile ()


MARK_AS_ADVANCED (
  CMAKE_OS_NAME
)
