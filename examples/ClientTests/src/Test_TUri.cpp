//==========================================================================
//  AIDA Detector description implementation 
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
// Framework include files
#include <DD4hep/Factories.h>
#include <iostream>

#include <TUri.h>

/// Plugin function: Test ROOT URI object
/**
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    20/06/2024
 */
static int Test_TUri (dd4hep::Detector& , int argc, char** argv)  {
  std::string name;
  for( int i=0; i<argc && argv[i]; ++i )  {
    if ( 0 == ::strncmp("-uri",argv[i],4) )
      name = argv[++i];
  }
  if ( name.empty() )   {
    /// Help printout describing the basic command line interface
    std::cout << "Usage: -plugin <name> -name <uri-string>"
              << std::endl << std::flush;
    ::exit(EINVAL);
  }
  TUri uri(name.c_str());
  std::cout << "--> relative part: " << uri.GetRelativePart() << std::endl;
  uri.Print();
  return 0;
}

DECLARE_APPLY(DD4hep_Test_TUri,Test_TUri)
