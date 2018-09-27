#!/bin/sh

. /etc/rc.d/init.d/functions


failure=1
success=0


fail ()
{
  echo "$1"
  echo "FLOWER TICKET TESTING FAILED"
  exit $failure
}


dir=`dirname $0`
cd $dir


start() {
  timeout=3
  lock_file="/dev/shm/flower.ticket52.lock"
  ./ticket52Daemon
  RETVAL=$?
  if [ "0" == "$RETVAL" ]; then
    sleep $timeout
    if [ -f $lock_file ]; then
      # We need to see if the process becomes a daemon so look for
      # the process and make sure the session flag is set
      detached=`ps afx | grep ticket52Daemon | grep -v grep | awk '{print $3}' | grep -c s`
      pkill ticket52Daemon
      sleep $timeout
      if [ "1" != $detached ]; then
        fail "ERROR: The daemon process did not detach and become a session leader."
      fi
      if [ -f $lock_file ]; then
        fail "ERROR: Lock file, $lock_file, not cleaned up."
      fi
    else
      fail "ERROR: Missing lock file, $lock_file"
    fi
  else
    fail "ERROR: Missing lock file, $lock_file"
  fi
  return $RETVAL
}

start

exit $RETVAL
