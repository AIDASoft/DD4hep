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
#include <DD4hep/PropertyTable.h>
#include <DD4hep/NamedObject.h>
#include <DD4hep/Detector.h>
#include <DD4hep/Printout.h>

#include <DD4hep/detail/Handle.inl>

// C/C++ includes

using namespace dd4hep;

DD4HEP_INSTANTIATE_HANDLE(TGDMLMatrix);

/// Initializing constructor.
PropertyTable::PropertyTable(Detector&     description,
                             const std::string& table_name,
                             const std::string& property_name,
                             size_t        num_rows,
                             size_t        num_cols)
{
  std::unique_ptr<Object> table(new Object(table_name.c_str(), num_rows, num_cols));
  table->SetTitle(property_name.c_str());
  description.manager().AddGDMLMatrix(m_element=table.release());
}
