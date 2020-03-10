#!/bin/bash

source /DD4hep/.dd4hep-ci.d/init_x86_64.sh
cd /DD4hep

echo "RUNNING PYLINT PY3K CHECK" && \
find . -name "*.py" -and -not -name 'ddsix.py' -exec pylint --rcfile=.dd4hep-ci.d/DD4hep.pylint.py3k.rc --py3k --msg-template="{path}:{line}: [{msg_id}({symbol}), {obj}] {msg}" {} + && \
echo "" && \
echo "RUNNING FLAKE8 CHECK" && \
find . -name "*.py" -and -not -name 'ddsix.py' -exec flake8 {} +
