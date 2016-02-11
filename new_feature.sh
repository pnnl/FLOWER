#!/bin/bash

ECHO=/bin/echo
ECHO=

if [[ -z "$1" || $# -eq 0 ]]; then
  echo "No argument supplied"
  exit 1
fi

$ECHO git checkout -b $1 develop
