#!/bin/bash

failure=1
success=0


fail ()
{
  echo ""
  echo "$1"
  echo ""
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


check_clean ()
{
  count=`svn status | egrep -c "^\s*M|^\s*A|^\s*D"`
  echo $count
}


cd $ENSIP_HOME
svn update


ticket_num=`echo $1 | sed -e "s/[a-zA-Z_-]//g"`
if [ -z "$ticket_num" ]; then
  fail "ERROR: Must have ticket number (e.g. 45)"
fi
ticket_id="ticket$ticket_num"


count=`check_clean`
if [ "0" != "$count" ]; then
  fail "ERROR: You need to do a commit on the ticket branch first"
fi


svn copy https://cvs.pnl.gov/svn/ensip/ensip/branches/$ticket_id https://cvs.pnl.gov/svn/ensip/ensip/tags/POST-$ticket_id -m "Finish Tag for fixing ticket #$ticket_num."
svn switch https://cvs.pnl.gov/svn/ensip/ensip/trunk .
svn update > ./svn_update.out 2>&1
cat ./svn_update.out


count=`cat ./svn_update.out | grep -v "^At revision" | wc -l`
rm -f ./svn_update.out
if [ "0" != "$count" ]; then
  echo "ERROR: The svn update failed. You need to fix the problem, then run the following two svn commands by hand"
  echo "svn merge https://cvs.pnl.gov/svn/ensip/ensip/tags/PRE-$ticket_id https://cvs.pnl.gov/svn/ensip/ensip/tags/POST-$ticket_id"
  echo "svn commit -m \"Merged fix for ticket #$ticket_num.\""
  exit $failure
fi


svn merge https://cvs.pnl.gov/svn/ensip/ensip/tags/PRE-$ticket_id https://cvs.pnl.gov/svn/ensip/ensip/tags/POST-$ticket_id
svn commit -m "Merged fix for ticket #$ticket_num."

exit 0
