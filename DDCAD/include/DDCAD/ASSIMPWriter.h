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
#ifndef DDCAD_ASSIMPWRITER_H
#define DDCAD_ASSIMPWRITER_H

/// Framework include files
#include <DDCAD/OutputWriter.h>

/// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cad  {

    /// Writer class to output geometry shapes from CAD files
    /**
     *  As a helper the ASSIMP library is used to interprete the 
     *  CAD formats.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DDCAD
     */
    class ASSIMPWriter : public OutputWriter   {
    public:
      using OutputWriter::OutputWriter;
      /// Default destructor
      virtual ~ASSIMPWriter() = default;
      /// Write output file
      virtual int write(const std::string& output_file,
			const std::string& output_type,
			const VolumePlacements& places,
			bool   recursive,
			double unit_scale = 1.0)  const  override;      
    };
    
  }        /* End namespace cad                      */
}          /* End namespace dd4hep                   */
#endif // DDCAD_ASSIMPWRITER_H
