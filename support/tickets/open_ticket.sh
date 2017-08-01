#!/bin/bash

failure=1
success=0


fail ()
{
  echo "$1"
  echo "CREATE NEW TICKET FIX FAILED"
  exit $failure
}


check_dir ()
{
  if [ ! -d "$1" ]; then
    return 0
  fi

  return 1
}


ticket_num=`echo $1 | sed -e "s/[a-zA-Z_-]//g"`
if [ -z "$ticket_num" ]; then
  fail "ERROR: Must have ticket number (e.g. 45)"
fi
ticket_id="ticket$ticket_num"


cd $FLOWER_HOME
URL="https://URI"


git update
git copy $URL/trunk $URL/branches/$ticket_id -m "Creating a private branch to fix ticket #$ticket_num."
git copy $URL/branches/$ticket_id $URL/tags/PRE-$ticket_id -m "Start Tag for fixing ticket #$ticket_num."
git switch $URL/branches/$ticket_id .


mkdir -p $FLOWER_HOME/data/flower/input/$ticket_id  > /dev/null 2>&1
mkdir -p $FLOWER_HOME/data/flower/ref/$ticket_id    > /dev/null 2>&1


git add $FLOWER_HOME/data/flower/input/$ticket_id
git add $FLOWER_HOME/data/flower/ref/$ticket_id


cd $FLOWER_HOME/src/flower/api/test/tickets
rm -f t_$ticket_id 
#ln -s t_test_ticket t_$ticket_id 
touch $ticket_id.conf 
cd $FLOWER_HOME


git add $FLOWER_HOME/src/flower/api/test/tickets/t_$ticket_id 
git add $FLOWER_HOME/src/flower/api/test/tickets/$ticket_id.conf


echo "Put the data files to reproduce the problem in the input directory (e.g. $FLOWER_HOME/data/flower/input/$ticket_id"
echo "Then run the following:"
echo $FLOWER_HOME/src/flower/api/test/tickets/t_$ticket_id 

exit 0
