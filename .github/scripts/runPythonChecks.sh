#!/bin/bash

source /cvmfs/sft.cern.ch/lcg/views/${VIEW}/setup.sh
cd /Package

echo "RUNNING FLAKE8 CHECK" && \
find . -name "*.py" -and -not -name 'ddsix.py' -exec flake8 {} +
