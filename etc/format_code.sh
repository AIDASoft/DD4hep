#!/bin/bash
DIR=`dirname $0`;

format_file()
{
    echo "====  Reformatting file: $*";
    emacs --quick --script ${DIR}/format_code.el $*;
}

arg=$1;

if test -f "${arg}";
then
    format_file "${arg}";
elif test -d "${arg}";
then
    for i in `find ${arg} \( -name '*.cpp' -o -name '*.h' -o -name '*.inl' \) -type f -print`;
    do
	format_file $i;
    done;
else
    echo "$0: File/Directory $* does not exist or is not a regular file nor a directory!"
fi;
