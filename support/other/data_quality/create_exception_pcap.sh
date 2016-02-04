#!/bin/sh

infile="exceptions.txt"
outfile="exceptions.pcap"

rm -f $outfile > /dev/null 2>&1

for line in `grep ^EXCEPTION $infile | sort | uniq`
do
  exception=`echo $line | awk -F':' '{print $2}'`
  flowkey=`echo $line | awk -F':' '{print $3}'`
  infile=`echo $line | awk -F':' '{print $4}'`
  if [ "$exception" == "IP_FRAGMENTATION" ]; then
    cmd=`./gen_wireshark_filter.pl -f $flowkey -i $infile`
  else
    cmd=`./gen_wireshark_filter.pl $flowkey -i $infile`
  fi
  echo $flowkey $file
  eval $cmd
done

mergecap -w $outfile *.pcap
