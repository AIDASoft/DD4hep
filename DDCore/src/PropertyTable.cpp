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
#include "DD4hep/PropertyTable.h"
#include "DD4hep/NamedObject.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"

#include "DD4hep/detail/Handle.inl"

// C/C++ includes
#include <sstream>
#include <iomanip>

using namespace std;
using namespace dd4hep;

#if ROOT_VERSION_CODE > ROOT_VERSION(6,16,0)

DD4HEP_INSTANTIATE_HANDLE(TGDMLMatrix);

/// Initializing constructor.
PropertyTable::PropertyTable(Detector&     description,
                             const string& table_name,
                             const string& property_name,
                             size_t        num_rows,
                             size_t        num_cols)
{
  unique_ptr<Object> s(new Object(table_name.c_str(), num_rows, num_cols));
  s->SetTitle(property_name.c_str());
  description.manager().AddGDMLMatrix(m_element=s.release());
}
#endif
