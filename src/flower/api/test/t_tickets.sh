#!/bin/bash

failure=1
success=0


fail ()
{
  echo "$1"
  echo "FLOWER TICKET TESTING FAILED"
  exit $failure
}


check_dir ()
{
  if [ ! -d "$1" ]; then
    return 0
  fi

  return 1
}


check_dirs ()
{
  local dir 

  for dir in $*
  do
    check_dir $dir
    if [ "0" == "$?" ]; then
      fail "ERROR: Directory not found: $dir"
    fi
  done

  return 1
}


dir=`dirname $0`
dir=`/bin/pwd`
dir="$ENSIP_HOME/src/flower/api/test"
tickets_test_dir="$dir/tickets"
check_dirs $tickets_test_dir

# Change dir to where the ticket conf files reside
cd $tickets_test_dir
make
hash -r

test_files=`/bin/ls -1 *.conf`
for conf_file in $test_files
do
  echo "FILE: $conf_file"
  if [ `echo $conf_file | grep -c sudo` == 1 ]; then
    sudo t_test_ticket $conf_file
  else
    t_test_ticket $conf_file
  fi
  if [ "$success" != "$?" ]; then
    fail "ERROR: Ticket test failed for file, $tickets_test_dir/$conf_file"
  fi
done


echo "FLOWER TICKET TESTING SUCCEEDED"
exit $success
