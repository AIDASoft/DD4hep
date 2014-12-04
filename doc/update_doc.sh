#!/bin/bash
DOC_SRC=/home/frankm/MyDocs/LCD/DD4hep;
DOC_DD4HEP=./LaTex;

if test ! -d ./LaTex; 
then
    echo "";
    echo "";
    echo "$0 > The directory ${DOC_DD4HEP} does not exit! -- no action taken";
    echo "";
    echo "";
else
    echo "$0 > Copy images .....";
    cp ${DOC_SRC}/*.png ${DOC_DD4HEP}/;
    echo "$0 > Copy latex files .....";
    cp ${DOC_SRC}/*.tex ${DOC_DD4HEP}/;
    echo "$0 > Copy processed pdf files.....";
    cp ${DOC_SRC}/*.pdf ${DOC_DD4HEP}/../;
    echo "$0 > All done.";
fi;

