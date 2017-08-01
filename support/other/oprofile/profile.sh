#!/bin/bash

# FROM: http://oprofile.sourceforge.net/docs

id=`id -u`
script=`basename $0`

if [ "0" != "$id" ]; then
  echo ""
  echo "ERROR: The script, $script, must be run by root or with sudo."
  echo ""
  exit 1
fi

if [ -z "$1" ]; then
  echo ""
  echo "ERROR"
  echo "  Usage: sudo $script your_username"
  echo ""
  exit 1
fi

dir=`dirname $script`
outfile="$dir/opreport.txt"
FLOWER_HOME="/home/$1/dev/flower"
exefile="$FLOWER_HOME/bin/flower"
configarg="$FLOWER_HOME/conf/myflower.conf"
outputdirarg="$FLOWER_HOME/data/flower/output"
inputfilearg="$FLOWER_HOME/data/flower/input/flower_ipv*.pcap /var/tmp/*huge.pcap"

if [ -z "$FLOWER_HOME" ]; then
  echo "ERROR: The FLOWER HOME, $FLOWER_HOME, does not exist."
  exit 1
fi

if [ ! -d "$FLOWER_HOME" ]; then
  echo "ERROR: The \$FLOWER_HOME dir, $FLOWER_HOME, does not exist."
  exit 1
fi

if [ -f "$outfile" ]; then
  /bin/rm -f $outfile
fi

if [ ! -f "$exefile" ]; then
  echo "ERROR: The executable, $exefile, does not exist."
  exit 1
fi

if [ ! -x "$exefile" ]; then
  echo "ERROR: The executable, $exefile, is not executable."
  exit 1
fi

if [ ! -f "$configarg" ]; then
  echo "ERROR: The config file, $configarg, does not exist."
  exit 1
fi

if [ ! -d "$outputdirarg" ]; then
  echo "ERROR: The output dir, $outputdirarg, does not exist."
  exit 1
fi

/bin/rm -f $outputdirarg/.200*
/bin/rm -f $outputdirarg/200*

opcontrol --stop
#events="--event=BUS_TRAN_MEM:8000:0xc0:1:1"
#events="--event=CPU_CLK_UNHALTED:800000:0x01:1:1"
events=""
opcontrol --setup --no-vmlinux --separate=library $events
opcontrol --start
$exefile --config-file=$configarg --site-name="pnnl_dev" --output-data-dir=$outputdirarg $inputfilearg
opcontrol --stop
opcontrol --dump
opreport  -t 0.5 --demangle=smart --symbols $exefile | cut -c 1-80

exit 0
