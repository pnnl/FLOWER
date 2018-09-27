#!/bin/bash

pkt_cnt="$FLOWER_HOME/support/other/data_quality/count_pcap_packets.pl"
pkt_cmp="$FLOWER_HOME/support/other/data_quality/compare_pcap_files.pl"
tcpreplay="/usr/local/bin/tcpreplay"
tshark="/usr/sbin/tshark"
tcpspeed="--topspeed"
tcpspeed="--multiplier=250"
intf="eth1"

if [ ! -x "$pkt_cnt" ]; 
then
  pkt_cnt=`which count_pcap_packets.pl 2> /dev/null`
  if [ -z "$pkt_cnt" ]; then
    echo ""
    echo "  WARNING: Cannot find count_pcap_packets.pl, skipping test but passing"
    echo ""
    exit 0
  fi
fi

if [ ! -x "$pkt_cmp" ];
then
  pkt_cmp=`which compare_pcap_files.pl 2> /dev/null`
  if [ -z "$pkt_cmp" ]; then
    echo ""
    echo "  WARNING: Cannot find compare_pcap_files.pl, skipping test but passing"
    echo ""
    exit 0
  fi
fi

if [ ! -x "$tcpreplay" ];
then
  tcpreplay=`which tcpreplay 2> /dev/null`
  if [ -z "$tcpreplay" ]; then
    echo ""
    echo "  WARNING: Cannot find tcpreplay, skipping test but passing"
    echo ""
    exit 0
  fi
fi

if [ ! -x "$tshark" ];
then
  tshark=`which tshark 2> /dev/null`
  if [ -z "$tshark" ]; then
    echo ""
    echo "  WARNING: Cannot find tshark, skipping test but passing"
    echo ""
    exit 0
  fi
fi

current_dir=`dirname $0`

temp_file="$current_dir/$0.out"
cache_timeout=1000
max2buffer=1000  # DEVELOPER NOTE: Don't use a number larger than 30451
min_cnt=0
max_cnt=$max2buffer
files=`find $FLOWER_HOME/data/flower/input -name \*.pcap | grep -v IPX | grep -v 1M.pcap | egrep "cpp_verify|phase_one"`
files_found=`/bin/ls -1 $files | wc -l`


cleanup ()
{
  echo "CLEANUP: removing $temp_file"
  sudo rm -f $temp_file
}


user_noloop ()
{
  sudo rm -f $temp_file
  echo "./t_bg_PacketRinger_user_noloop"
        ./t_bg_PacketRinger_user_noloop > $temp_file 2>&1
  result=`grep -c "No errors detected" $temp_file`
  cat $temp_file
  if [ "$result" != "1" ]; then
    echo "ERROR: File, $t_data, failed with result = $result"
    cleanup
    exit 1
  fi
}


root_noloop ()
{
  sudo rm -f $temp_file
  echo "sudo ./t_bg_PacketRinger_root_noloop"
        sudo ./t_bg_PacketRinger_root_noloop > $temp_file 2>&1
  result=`grep -c "No errors detected" $temp_file`
  cat $temp_file
  if [ "$result" != "1" ]; then
    echo "ERROR: File, $t_data, failed with result = $result"
    cleanup
    exit 1
  fi
}


root_loop_track ()
{
  files_processed=0
  files_skipped=0

  for t_data in `ls -1 $files`
  do
    cnt=`$pkt_cnt $t_data`
    #echo "COUNT:$cnt"

    if [ "$cnt" -gt "$min_cnt" -a "$cnt" -lt "$max_cnt" ]; then
      echo "FILE:$t_data"
      sudo rm -f $temp_file
      rm -f $FLOWER_HOME/data/flower/output/20*.dat > /dev/null 2>&1
  
      echo "sudo tcpreplay --quiet $tcpspeed --intf1=$intf $t_data"
      sudo sh -c "sleep 2; $tcpreplay --quiet $tcpspeed --intf1=$intf $t_data > /dev/null 2>&1" &
  
      sleep 1
      echo "sudo ./t_bg_PacketRinger_root_loop_track $intf $cnt"
      sudo       ./t_bg_PacketRinger_root_loop_track $intf $cnt > $temp_file 2>&1
  
      result=`grep -c "No errors detected" $temp_file`
      cat $temp_file
      if [ "$result" != "1" ]; then
        echo "ERROR: File, $t_data, failed with result = $result"
         cleanup
        exit 1
      fi
  
      ((files_processed++))
    else
      #echo "SKIP:$t_data"
      ((files_skipped++))
    fi
  done
  echo "FILES PROCESSED: $files_processed"
  echo "FILES SKIPPED:   $files_skipped"
}

  
root_loop_buffer ()
{
  files_processed=0
  files_skipped=0

  for t_data in `ls -1 $files`
  do
    cnt=`$pkt_cnt $t_data`
    #echo "COUNT:$cnt"

    if [ "$cnt" -gt "$min_cnt" -a "$cnt" -lt "$max_cnt" ]; then
      echo "FILE:$t_data"
      sudo rm -f $temp_file
      # REMOVE ANY PREVIOUS OUTPUT FILES
      rm -f $FLOWER_HOME/data/flower/output/20*.pcap > /dev/null 2>&1

      echo "sudo tcpreplay --quiet $tcpspeed --intf1=$intf $t_data"
      sudo sh -c "sleep 2; $tcpreplay --quiet $tcpspeed --intf1=$intf $t_data > /dev/null 2>&1" &

      echo "sudo ./t_bg_PacketRinger_root_loop_buffer $intf $cnt"
      sudo       ./t_bg_PacketRinger_root_loop_buffer $intf $cnt > $temp_file 2>&1

      if [ -z "$temp_file" ]; then
        echo "ERROR: Temp File, $temp_file, is empty."
        cleanup
        exit 1
      fi
      if [ ! -f "$temp_file" ]; then
        echo "ERROR: Temp File, $temp_file, not found."
        cleanup
        exit 1
      fi

      result=`grep -c "No errors detected" $temp_file 2> /dev/null`
      cat $temp_file
      if [ "$result" != "1" ]; then
        echo "ERROR: File, $t_data, failed with result = $result."
        cleanup
        exit 1
      fi

      outfiles=`/bin/ls -1 $FLOWER_HOME/data/flower/output/20*.pcap`
      echo "$pkt_cmp $cnt $t_data $outfiles"
      $pkt_cmp       $cnt $t_data $outfiles
      result=$?
      if [ "$result" != "0" ]; then
        echo "ERROR: Out File, $outfiles, failed with result = $result"
        cleanup
        exit 1
      fi
      ((files_processed++))
    else
      ((files_skipped++))
    fi
  done
  echo "FILES PROCESSED: $files_processed"
  echo "FILES SKIPPED:   $files_skipped"
}


# MAIN

main ()
{
  echo ""
  echo "PROCESSING packet_ringer user_noloop tests"
  user_noloop
  echo ""
  echo "PROCESSING packet_ringer root_noloop tests"
  root_noloop
  echo ""
  echo "PROCESSING packet_ringer root_loop_buffer tests"
  root_loop_buffer
  echo ""
  echo "PROCESSING packet_ringer root_loop_track tests"
  root_loop_track

  cleanup
}

main

exit 0
