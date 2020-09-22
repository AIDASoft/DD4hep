#!/bin/bash

source /cvmfs/sft.cern.ch/lcg/views/${VIEW}/setup.sh
cd /Package

echo "RUNNING PYLINT PY3K CHECK" && \
find . -name "*.py" -and -not -name 'ddsix.py' -exec pylint --rcfile=.github/scripts/DD4hep.pylint.py3k.rc --py3k --msg-template="{path}:{line}: [{msg_id}({symbol}), {obj}] {msg}" {} + && \
echo "" && \
echo "RUNNING FLAKE8 CHECK" && \
find . -name "*.py" -and -not -name 'ddsix.py' -exec flake8 {} +
