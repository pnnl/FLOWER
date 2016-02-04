#!/bin/sh

totalFiles=`wc -l sc08_data_file_list.txt | awk '{print $1}'`

if [ $# -ne 1 ]; then
  sleep="60"
else
  sleep="$1"
fi

if [ $sleep -lt 60 ]; then
  echo "WARNING: Interval must be great than 60, setting interval to 60"
  sleep="60"
fi

subSleep=1
BeginOfLine=`tput cup 0 0`
cd /dev/shm/$USER/flower
clear

fileCount=`grep -c Reading output* | awk -F':' '{SUM += $2} END {print SUM}'`
intvCount=0
prevCount=0
percent=0
sumCount=0
runtime=1
numLoops=0
while [ $fileCount -lt $totalFiles ]; do
  echo ""
  date=`date`
  prevCount=$fileCount
  if [ ! -f output_00 ]; then
    clear
    echo "COMPLETE"
    exit 0
  fi
  fileCount=`grep -c Reading output* | awk -F':' '{SUM += $2} END {print SUM}'`
  echo "$date"
  if [ $fileCount -ge $totalFiles ]; then
    clear
    echo "COMPLETE"
    exit 0
  fi
  ((filesLeft = $totalFiles - $fileCount))
  ((intvCount = $fileCount  - $prevCount))
  ((intvRate  = $intvCount  / $sleep))
  if [ $intvRate -eq 0 ]; then
    ((intvRate  = 1))
  fi
  ((sumCount += $intvCount))
  if [ $numLoops -gt 0 ]; then
    ((avrgCount = $sumCount  / $numLoops))
  else
    ((avrgCount = 1))
  fi
  ((minLeft   = $filesLeft / $avrgCount / ($sleep / 60)))
  ((percent   = $fileCount * 100 / $totalFiles))
  echo "Total    Files Processed: $fileCount out of $totalFiles"
  echo "Total    Files Left     : $filesLeft"
  echo "Percent Complete        : $percent%"
  echo "Interval Files Processed: $intvCount in last $sleep seconds"
  echo "Average  Files Processed: $avrgCount per $sleep seconds"
  echo "Should be done in       : $minLeft minutes"
  grep -c PROBLEM output*
  ((check = $sleep - 1))
  while [ $check -gt 0 ]; do
    echo -n ${BeginOfLine}Countdown $check...
    sleep $subSleep
    ((check -= $subSleep))
  done
  ((runtime += $sleep))
  ((numLoops++))
  clear
done
