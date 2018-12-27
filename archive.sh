#!/bin/bash

# FROM: https://git-scm.com/docs/git-archive

rm -f latest.zip
git archive -o latest.zip HEAD
