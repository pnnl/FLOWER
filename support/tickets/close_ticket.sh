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

fail "ERROR: This script no longer works - needs to be updated to support git!"

check_dir ()
{
  if [ ! -d "$1" ]; then
    return 0
  fi

  return 1
}


check_clean ()
{
  count=`git status | egrep -c "^\s*M|^\s*A|^\s*D"`
  echo $count
}


cd $FLOWER_HOME
git update


ticket_num=`echo $1 | sed -e "s/[a-zA-Z_-]//g"`
if [ -z "$ticket_num" ]; then
  fail "ERROR: Must have ticket number (e.g. 45)"
fi
ticket_id="ticket$ticket_num"


count=`check_clean`
if [ "0" != "$count" ]; then
  fail "ERROR: You need to do a commit on the ticket branch first"
fi


# TODO: Change to support git
git copy https://URI/branches/$ticket_id https://URI/tags/POST-$ticket_id -m "Finish Tag for fixing ticket #$ticket_num."
git switch https://URI/master .
git pull > ./git_update.out 2>&1
cat ./git_update.out


count=`cat ./git_update.out | grep -v "^At revision" | wc -l`
rm -f ./git_update.out
if [ "0" != "$count" ]; then
  echo "ERROR: The git pull failed. You need to fix the problem, then run the following two git commands by hand"
  echo "git merge https://URI/tags/PRE-$ticket_id https://URI/tags/POST-$ticket_id"
  echo "git commit -m \"Merged fix for ticket #$ticket_num.\""
  exit $failure
fi


git merge https://URI/tags/PRE-$ticket_id https://URI/tags/POST-$ticket_id
git commit -m "Merged fix for ticket #$ticket_num."
git push

exit 0
