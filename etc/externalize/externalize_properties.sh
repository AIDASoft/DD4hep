#!/bin/bash
#==========================================================================
#  AIDA Detector description implementation 
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
do_scp()  {
    echo "Copy files:    scp $*";
    scp $*;
}
#
copy_files()   {
    target=${1};
    cd DDCore/include/DD4hep
    echo "+++ Copy header files from `pwd` to: ${target}/DD4hep ....";
    do_scp  BasicGrammar.h      ComponentProperties.h      \
            Exceptions.h        Plugins.h       Printout.h \
            BasicGrammar_inl.h  ComponentProperties_inl.h  \
            Parsers.h           Primitives.h    ToStream.h \
            ${target}/DD4hep;
    cd -;
    cd DDCore/src;
    echo "+++ Copy source files from `pwd` to: ${target}/src/DD4hep ....";
    do_scp -r parsers               BasicGrammar.cpp   \
           ComponentProperties.cpp  Plugins.cpp        Printout.cpp \
           BasicGrammarTypes.cpp    Exceptions.cpp     \
           Primitives.cpp          ToStream.cpp        \
           ${target}/src/DD4hep/;
    cd -;
    do_scp doc/externalize/PluginManager.cpp ${target}/src/DD4hep/;
    do_scp doc/externalize/ExtraParsers.cpp ${target}/src/DD4hep/parsers/;
    do_scp doc/externalize/ExtraProperties.cpp ${target}/src/DD4hep/;
}

TARGET=${1};
if test "" = "${TARGET}"; then
    cat <<EOF
 Usage:  bash ${0} <scp-target-directory>
             e.g. ${USER}/cmtuser/Online_vxry/Online/Dataflow
         This script extracts the dd4hep property mechanism so that
         it can be used without dd4hep.

 Note:   to use the properties and the parsers, you also MUST
         externalize the Evaluator.

EOF
    exit 1;
fi;

echo "+++ Copy property files to target location: ${TARGET}";
copy_files ${TARGET};
