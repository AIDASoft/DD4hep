#!/bin/bash
#==========================================================================
#  AIDA Detector description implementation for LCD
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
#
#
copy_files()   {
    target=${1};
    cd DDCore/include/XML
    echo "+++ Copy header files from `pwd` to: ${target}/XML ....";
    scp config.h DocumentHandler.h  tinyxml.h UriReader.h  XMLChildValue.h \
        XMLDimension.h  XMLElements.h Evaluator.h tinystr.h   UnicodeValues.h \
        XMLDimension.inl  XMLTags.h XML.h ${target}/XML/;
    cd -;
    cd DDCore/src/XML;
    echo "+++ Copy source files from `pwd` to: ${target}/src ....";
    scp DocumentHandler.cpp tinyxmlerror_inl.h  tinyxml_inl.h  tinyxmlparser_inl.h \
        UriReader.cpp XMLChildValue.cpp XMLDimension.cpp  \
        XMLElements.cpp	XMLTags.cpp ${target}/src/;
    cd -;
    cd DDCore/src/Evaluator;
    echo "+++ Copy source files from `pwd` to: ${target}/Evaluator ....";
    scp Evaluator.cpp hash_map.src setStdMath.cpp setSystemOfUnits.cpp stack.src  string.src \
         ${target}/Evaluator/;
    cd -;
    scp doc/externalize/Printout.h    ${target}/XML/;
    scp doc/externalize/Printout.cpp  ${target}/src/;
    scp doc/externalize/ExpressionEvaluator.cpp  ${target}/src/;
}

TARGET=${1};
if test "" = "${TARGET}"; then
    cat <<EOF
 Usage:  bash ${0} <scp-target-directory>
             e.g. ${USER}/cmtuser/Online_vxry/Online/Dataflow
         This script extracts the DD4hep XML interface so that
         it can be used without DD4hep.
EOF
    exit 1;
fi;

echo "+++ Copy property files to target location: ${TARGET}";
copy_files ${TARGET};
