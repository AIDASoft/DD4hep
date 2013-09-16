#################################################################################
#
#  environment script for DD4hep examples - initializes DD4hep (and ROOT) as well
# 
#  @author F.Gaede, DESY, 2013
#
#################################################################################


source /afs/cern.ch/user/c/cgrefe/workspace/DD4hep/bin/thisdd4hep.sh

export PATH=$PATH:/afs/cern.ch/user/c/cgrefe/workspace/DD4hep/examples/CLICSiD/bin

if [  ]
then
    export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/afs/cern.ch/user/c/cgrefe/workspace/DD4hep/examples/CLICSiD/lib
else
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/afs/cern.ch/user/c/cgrefe/workspace/DD4hep/examples/CLICSiD/lib
fi
