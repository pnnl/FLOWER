#!/bin/sh

if [ $# -eq 0 ]
then
  echo "Arguments required."
  echo "Usage: build_RPM.sh [gcc|boost|flower|]"
  exit
fi

rpmbuild --define '_topdir %{getenv:HOME}/git/flower/support/build/rpmbuild' -ba $1.spec


