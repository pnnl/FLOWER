#!/bin/bash

export CONF_FILE="/opt/flower/conf/flower.conf"
export DATA_DIR=`grep ^output-data-dir $CONF_FILE | awk '{print $NF}'`
echo "#$DATA_DIR#"
cd $DATA_DIR

files=`ls -alFs 2* | wc -l`

echo    ""
echo -n "Num Files:   $files"

num_days=`echo "scale=2; ${files} / 96" | bc -l`
echo    ""
echo    "Days:        $num_days"
num_days=`echo "scale=0; (${num_days} + 0.5) / 1" | bc -l`

echo -n "Oldest File: "
ls -halFst 2* | tail -1
# | cut -d " " -f 9,10,11,12

echo -n "Newest File: "
ls -halFst 2* | head -1
# | cut -d " " -f 9,10,11,12

echo -n "Disk Space:  "
du -sh .

echo ""
echo "PacketSummary:"
./PacketSummary.pl -d "$DATA_DIR"


startdate=$(date --date="-$num_days day" +"%Y%m%d")
enddate=$(date +"%Y%m%d")

echo ""
echo "Size of files by day:"
num="$startdate"
while [ "$num" -le "$enddate" ]; do 
  echo -n "$num "
  du -ch ${num}* 2> /dev/null| tail -1
  num=$(date --date="$num + 1 day" +"%Y%m%d")
done

echo ""
echo "Packets per second:"
grep "  Packets per second" /var/log/messages* | awk '{print $NF}' | sort -nr | head -5

exit
