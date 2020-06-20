#!/bin/bash

FLOWER_STAGE_DIR='/path_to/FLOWER_STAGE'
FLOWER_OUTPUT_DIR='/path_to/FLOWER_OUTPUT'
flo_STAGE_DIR='/path_to/flo_STAGE'
flo_OUTPUT_DIR='/path_to/flo_OUTPUT'

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

for flower_input_file in `ls -1 $FLOWER_STAGE_DIR/2*flr04.dat 2> /dev/null`
do
  flo_output_file=`echo $flower_input_file | awk -F'/' '{print $NF}' | sed -e "s/flr04/flo30/"`
  $DIR/convert_flower2flo.pl -i $flower_input_file -o "$flo_STAGE_DIR/$flo_output_file"
  mv $flower_input_file $FLOWER_OUTPUT_DIR
  mv $flo_STAGE_DIR/$flo_output_file $flo_OUTPUT_DIR
done