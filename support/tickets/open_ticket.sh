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


cd $ENSIP_HOME
URL="https://cvs.pnl.gov/svn/ensip/ensip"


svn update
svn copy $URL/trunk $URL/branches/$ticket_id -m "Creating a private branch to fix ticket #$ticket_num."
svn copy $URL/branches/$ticket_id $URL/tags/PRE-$ticket_id -m "Start Tag for fixing ticket #$ticket_num."
svn switch $URL/branches/$ticket_id .


mkdir -p $ENSIP_HOME/data/flower/input/$ticket_id  > /dev/null 2>&1
mkdir -p $ENSIP_HOME/data/flower/ref/$ticket_id    > /dev/null 2>&1


svn add $ENSIP_HOME/data/flower/input/$ticket_id
svn add $ENSIP_HOME/data/flower/ref/$ticket_id


cd $ENSIP_HOME/src/flower/api/test/tickets
rm -f t_$ticket_id 
#ln -s t_test_ticket t_$ticket_id 
touch $ticket_id.conf 
cd $ENSIP_HOME


svn add $ENSIP_HOME/src/flower/api/test/tickets/t_$ticket_id 
svn add $ENSIP_HOME/src/flower/api/test/tickets/$ticket_id.conf


echo "Put the data files to reproduce the problem in the input directory (e.g. $ENSIP_HOME/data/flower/input/$ticket_id"
echo "Then run the following:"
echo $ENSIP_HOME/src/flower/api/test/tickets/t_$ticket_id 

exit 0
