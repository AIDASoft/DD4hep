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

/// Framework include files
#include "DDCAD/OutputWriter.h"

using namespace dd4hep;
using namespace dd4hep::cad;

/// Default constructor
OutputWriter::OutputWriter(Detector& det)
  : detector(det)
{
}

/// Default destructor
OutputWriter::~OutputWriter()
{
}
