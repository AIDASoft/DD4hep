###################################################
#
# Simple CMakeList.txt file that allows to
# build all DD4hep examples in one go, e.g.
#
# . ./bin/thisDD4hep.sh
# cd examples ;
# mkdir build ; cd build
# cmake ..
#
# F.Gaede, DESY, 2013
#
##################################################
cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)
 	
project(DD4hepExample)
 	
#---Testing-------------------------------------------------------------------------
if(BUILD_TESTING)
include(CTest)
enable_testing()
endif(BUILD_TESTING)
#----------------------------------------------------------------------------------
#
# (un)comment as needed
#
add_subdirectory(ILDExDet)
 	
#add_subdirectory(ILDExSimu)
 	
#add_subdirectory(CLICSiD)
 	
#add_subdirectory(noviceN04)
 	
#---- does not build on mac or ubuntu ...
#add_subdirectory(Segmentation)
