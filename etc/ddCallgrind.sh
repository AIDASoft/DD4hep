#!/bin/bash
##`which valgrind` --tool=callgrind -v --dump-instr=yes --trace-jump=yes $*
`which valgrind` --tool=callgrind -v $*
