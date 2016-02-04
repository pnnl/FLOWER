#!/bin/bash

failure=1
success=0


fail ()
{
  echo "$1"
  echo "TEST FAILED"
  exit $failure
}


if [ 1 -gt $# ]; then
  fail "Missing ticket number"
fi


dir=`dirname $0`
dir="$ENSIP_HOME/src/flower/api/test/tickets"


ticket_id=`echo $1 | sed -e "s/^ticket//" -e "s/.conf$//"`
if [ -z "$ticket_id" ]; then
  fail "ERROR: Could not find ticket id"
else
  ticket_id="ticket$ticket_id"
fi


input_dir="$ENSIP_HOME/data/flower/input/$ticket_id"
ticket_output_dir="$ENSIP_HOME/data/flower/output/$ticket_id"
ensipoutput_dir="$ENSIP_HOME/data/flower/output"
ref_dir="$ENSIP_HOME/data/flower/ref/$ticket_id"


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


check_dirs $input_dir $ensip_output_dir $ref_dir


#
#  Clean up any old output data
#
check_dir $ticket_output_dir
if [ "1" == "$?" ]; then
  # The directory exists so just in case there are files in that
  # directory, remove it. It will get created later
  /bin/rm -rf $ticket_output_dir
fi


#
#  Make sure the ticket output directory exists
#
mkdir $ticket_output_dir
check_dir $ticket_output_dir
if [ "1" != "$?" ]; then
  fail "ERROR: Directory could not be created: $ticket_output_dir"
fi


#
#  Use the default config file unless one exist for this ticket
#
conf_file="$ENSIP_HOME/conf/flower.conf"
if [ -f "$dir/$ticket_id.conf" ]; then
  conf_file="$dir/$ticket_id.conf"
fi


#
#  Create new output files from the input files
#
input_files="`/bin/ls -1 $input_dir | grep pcap`"
for file in $input_files
do
  $ENSIP_HOME/bin/flower --site-name=pnnl_dev --config-file=$conf_file --output-data-dir=$ticket_output_dir $input_dir/$file
# > /dev/null 2>&1
done


#
#  Check to make sure reference files exist
#
ref_file_cnt="`/bin/ls -1 $ref_dir | wc -l`"
if [ 0 -eq $ref_file_cnt ]; then
  fail "ERROR: Missing reference output files"
fi


#
#  Check to make sure output files exist
#
out_files="`/bin/ls -1 $ticket_output_dir`"
out_file_cnt="`/bin/ls -1 $ticket_output_dir | wc -l`"
if [ 0 -eq $out_file_cnt ]; then
  fail "ERROR: Missing output files"
fi


#
#  Compare reference files to output files
#
ref_files="`/bin/ls -1 $ref_dir`"
for file in $ref_files
do
  #if [ -z "$ref_dir/$file" ]; then
  #  fail "ERROR: Reference file is empty: $file"
  #fi
  cnt=0
  if [ ! -f "$ticket_output_dir/$file" ]; then
    fail "FAIL missing FILE: $file"
  fi
  cnt=`diff -I ^3 -I ^4 -I ^5 $ref_dir/$file $ticket_output_dir/$file | wc -l`
  if [ ${cnt} -gt 0 ]; then
    fail "FAIL on FILE: $file"
  else
    echo "OK  for FILE: $file"
  fi
done


if [ $out_file_cnt -lt $ref_file_cnt ]; then
  for ref_file in $ref_files
  do
    if [ ! -f "$ticket_output_dir/$ref_file" ]; then
      echo "Missing Out File: $ref_file"
    fi
  done
  fail "ERROR: Missing output files"
fi

if [ $out_file_cnt -gt $ref_file_cnt ]; then
  for out_file in $out_files
  do
    if [ ! -f "$ref_dir/$out_file" ]; then
      echo "Missing Ref File: $out_file"
    fi
  done
  fail "ERROR: Missing reference files"
fi


echo "TEST SUCCEEDED"
exit $success
