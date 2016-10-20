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

// Framework include files
#include "DD4hep/World.h"
#include "DD4hep/Printout.h"
#include "DD4hep/objects/DetectorInterna.h"

using namespace DD4hep::Geometry;

/// Access the conditions loading
World::ConditionsLoader& World::conditionsLoader() const   {
  ConditionsLoader* loader = access()->conditionsLoader;
  if ( !loader )   {
    except("Conditions","+++ No ConditionsLoader registered to this World instance!");
  }
  return *loader;
}

/// Access to the alignment loading
World::AlignmentsLoader& World::alignmentsLoader() const   {
  AlignmentsLoader* loader = access()->alignmentsLoader;
  if ( !loader )  {
    except("Alignments","+++ No AlignmentsLoader registered to this World instance!");
  }
  return *loader;
}

/// Access the detector descrion tree
LCDD& World::lcdd() const   {
  return *(access()->lcdd);
}

