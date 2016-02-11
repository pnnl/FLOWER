#!/bin/bash

ECHO=/bin/echo
#ECHO=

BRANCH=`git status | head -1 | grep -i "on branch" | awk '{print $NF}'`

if [[    "$BRANCH" == "develop" ||
         "$BRANCH" == "master"  ||
         "$BRANCH" == "origin"  ||
      -z "$BRANCH"                  ]]; then
  echo "ERROR: Cannot close branch ($BRANCH)"
  exit 1
fi

$ECHO git checkout develop

$ECHO git merge --no-ff $BRANCH

$ECHO git push origin develop
$ECHO git push origin $BRANCH

exit 0
