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
#ifndef DDCAD_ASSIMPREADER_H
#define DDCAD_ASSIMPREADER_H

/// Framework include files
#include <DDCAD/InputReader.h>

/// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cad  {

    /// Reader class to input geometry shapes from CAD files
    /**
     *  As a helper the ASSIMP library is used to interprete the 
     *  CAD formats.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DDCAD
     */
    class ASSIMPReader : public InputReader   {
    public:
      long flags = 0;
    public:
      using InputReader::InputReader;

      /// Default destructor
      virtual ~ASSIMPReader() = default;

      /// Read input file
      virtual std::vector<std::unique_ptr<TGeoTessellated> >
      readShapes(const std::string& source, double unit_Length)  const  override;
      
      /// Read input file and create a volume-set
      virtual std::vector<std::unique_ptr<TGeoVolume> >
      readVolumes(const std::string& source, double unit_length)  const override;
    };
  }        /* End namespace cad                      */
}          /* End namespace dd4hep                   */
#endif // DDCAD_ASSIMPREADER_H
