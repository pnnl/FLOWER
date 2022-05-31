#!/bin/bash

# NOTE: Run this script from the command line using the following
#       syntax to disown the process and run it in the background:
#  nohup ./process &

# NOTE: To Test, set the following variables
export TESTING=0
export VERBOSE=0


# NOTE: We are going to monitor the 2*.dat files in this DATA_DIR
export CONF_FILE="/opt/flower/conf/flower.conf"
export DATA_DIR=`grep ^output-data-dir $CONF_FILE | awk '{print $NF}'`
#echo "#$DATA_DIR#"
cd $DATA_DIR

# NOTE: 10,000,000 is equal to 10 GB
export FREESPACE=10000000

export MAILTO="darren.curtis@pnnl.gov"
export SLEEPTIME=3600
export REAL=1
export NL=$'\n'

if [ "$TESTING" -eq "1" ]
then
  FREESPACE=600000000
  MAILTO="darren.curtis@pnnl.gov darren.curtis@pnl.gov"
  SLEEPTIME=10
  REAL=0
fi

cd "$DATA_DIR"

while true
do

  # NOTE: Assume everything is fine and reset state
  export SUCCESS=1
  export FILES_REMOVED=""

  for file in `ls -1rt 2*.dat`
  do
    AVAIL=`/bin/df --output=avail "$DATA_DIR" | tail -1`
    if [ "$AVAIL" -ge "$FREESPACE" ]
    then
      break
    fi
    SUCCESS=0
    FILES_REMOVED+=" ${file}$NL"
    if [ "$REAL" -eq "1" ]
    then
      /bin/rm -f "$file"
    fi
    if [ "$VERBOSE" -eq "1" ]
    then
      echo "REMOVE FILE:$file"
    fi
  done

  if [ "$SUCCESS" -eq "0" ]
  then
    # NOTE: We failed so we had to remove some files
    echo "$FILES_REMOVED" | mail -s "FLOWER on petals disk filling up removing file(s)" "$MAILTO" > /dev/null
  fi

  exit 0
  # NOTE: Now let's sleep for 15 minutes and do it all over again
  sleep "$SLEEPTIME"
done

#exit 0


















for i in /opt/nessus_agent/sbin/nessus-service nessusd /usr/sbin/tuned /sbin/rngd
do
  renice -n 19 `ps -ef | grep $i | grep -v grep | awk '{print $2}'` > /dev/null 2>&1
done

for i in abrt-dump-xorg colord plymouthd 
do
  pkill -9 $i                             > /dev/null 2>&1
done

systemctl set-default multi-user.target   > /dev/null 2>&1

for i in abrt-xorg colord gdm plymouth-start
do
  systemctl stop     "$i.service"         > /dev/null 2>&1
  systemctl disable  "$i.service"         > /dev/null 2>&1
done

exit 0
