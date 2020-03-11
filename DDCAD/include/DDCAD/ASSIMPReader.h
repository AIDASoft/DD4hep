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
#ifndef DD4HEP_DDCAD_ASSIMPREADER_H
#define DD4HEP_DDCAD_ASSIMPREADER_H

/// Framework include files
#include <DDCAD/InputReader.h>

/// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cad  {

    class ASSIMPReader : public InputReader   {
    public:
      using InputReader::InputReader;
      /// Default destructor
      virtual ~ASSIMPReader() = default;
      /// Read input file
      virtual std::vector<std::unique_ptr<TGeoTessellated> >
      read(const std::string& source, double unit_Length)  const  override;      
    };
    
  }        /* End namespace cad                      */
}          /* End namespace dd4hep                   */
#endif // DD4HEP_DDCAD_ASSIMPREADER_H
