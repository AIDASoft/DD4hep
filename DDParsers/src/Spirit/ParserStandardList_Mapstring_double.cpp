// $Id: ParserStandardList_Mapstring_double.cpp 2166 2016-03-17 16:37:06Z /DC=ch/DC=cern/OU=Organic Units/OU=Users/CN=sailer/CN=683529/CN=Andre Sailer $
//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

#include "Parsers/spirit/ParsersStandardListCommon.h"
namespace dd4hep{ namespace Parsers{
IMPLEMENT_STL_MAP_PARSER(std::map,std::string,double)
  }}
