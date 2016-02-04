#!/bin/bash

input_file="sc08_data_file_list.txt"
output_file="output"
output_dir="/dev/shm/curtis/flower"

rm -rf $output_dir > /dev/null 2>&1
mkdir -p $output_dir

final_output_file="$output_file.txt"
num_procs=`cat /proc/cpuinfo | grep processor | wc -l`
((num_procs--))
rm -f "$input_file"_* "$output_dir/$output_file".*
rm -f $final_output_file
rm -f "$output_dir/$output_file"_0*

total_file_count=`wc -l $input_file | awk '{print $1}'`
per_file_count=$((($total_file_count / $num_procs) + 1))
split -d --lines=$per_file_count $input_file "$input_file"_
file_list=`/bin/ls -1 "$input_file"_*`

cpu=1
num=0
for file in $file_list
do
  num=$(($cpu - 1))
  out_file="$output_file"_0$num
  echo "time taskset -c $cpu ./processFiles.pl -dp -i $file > $out_file 2>&1"
  time taskset -c $cpu ./processFiles.pl -dp -i $file > "$output_dir/$out_file" 2>&1 &
  ((cpu++))
done

echo "Waiting for jobs to complete"
wait

rm -f "$output_file".txt
cat "$output_dir/$output_file"_0* > $final_output_file
rm -f "$output_dir/$output_file"_0*
rm -f "$input_file"_0*

echo "COMPLETED"
time ./summarize.pl                        > summary.txt 2>&1

exit 0
