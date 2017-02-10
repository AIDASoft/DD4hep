//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
/* 
 Plugin invocation:
 ==================
 This plugin behaves like a main program.
 Invoke the plugin with something like this:

 geoPluginRun -volmgr -destroy -plugin DD4hep_Alignment2Condition

*/
// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DD4hep/objects/AlignmentsInterna.h"

#include "DD4hep/Factories.h"

#include <cerrno>

using namespace std;
using namespace DD4hep;
using Alignments::Alignment;
using Alignments::AlignmentData;
using Alignments::AlignmentCondition;
using Conditions::Condition;
using Conditions::ConditionKey;

/// Plugin function: Alignment program example
/**
 *  Factory: DD4hep_Alignment2Condition
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int Alignment_to_Condition (Geometry::LCDD& , int argc, char** argv)  {
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-help",argv[i],2) || 0 == ::strncmp("-?",argv[i],2) )  {
      /// Help printout describing the basic command line interface
      cout <<
        "Usage: -plugin <name>                                                         \n"
        "     name:   factory name     DD4hep_AlignmentExample_read_xml                \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
  }

  AlignmentCondition ac("alignment");
  ac->hash = ConditionKey::hashCode(ac.name());
  
  printout(INFO,"Example","Alignment condition:  \"%s\"  Key:%016llX",ac.name(),ac->hash);
  printout(INFO,"Example","Alignment condition ptr:  %p",ac.ptr());

  Condition con(ac);
  AlignmentData& data = ac.data();
  printout(INFO,"Example","Alignment condition data: %p",(void*)&data);
  printout(INFO,"Example","Condition opaque pointer: %p",(void*)con.data().ptr());
  printout(INFO,"Example","Offset to opaque pointer: %uld",con->offset());
  printout(INFO,"Example","Computed Pointer:         %p",(void*)((char*)con.ptr()+con->offset()));
  printout(INFO,"Example","Computed payload pointer: %p",(void*)con->payload());

  Alignment align(&data);
  printout(INFO,"Example","Alignment object pointer: %p",(void*)align.ptr());
  printout(INFO,"Example","Alignment key:            %016llX",align.key());
  printout(INFO,"Example","Condition key:            %016llX",con.key());

  Condition c(data.condition);
  printout(INFO,"Example","Condition ptr from alignment:  %p",(void*)c.ptr());
  
  return 1;
}

DECLARE_APPLY(DD4hep_Alignment2Condition,Alignment_to_Condition)
