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
#ifndef DD4HEP_DDCAD_INPUTREADER_H
#define DD4HEP_DDCAD_INPUTREADER_H

// Framework include files
#include <DD4hep/config.h>

#include <TGeoTessellated.h>

/// C/C++ include files
#include <string>
#include <vector>
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cad  {

    class InputReader   {
    public:
      /// Default constructor
      InputReader();
      /// Default destructor
      virtual ~InputReader();
      /// Read input file
      virtual std::vector<std::unique_ptr<TGeoTessellated> >
      read(const std::string& source, double unit_length)  const  = 0;
    };
    
  }        /* End namespace cad                      */
}          /* End namespace dd4hep                   */
#endif // DD4HEP_DDCAD_INPUTREADER_H
