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
#ifndef DDCAD_INPUTWRITER_H
#define DDCAD_INPUTWRITER_H

// Framework include files
#include <DD4hep/config.h>
#include <DD4hep/Volumes.h>

/// C/C++ include files
#include <string>
#include <vector>
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Forward declarations
  class Detector;
  
  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cad  {

    /// Interface of the writer class to output geometry shapes from CAD files
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DDCAD
     */
    class OutputWriter   {
    public:
      enum output_flags   {
                           EXPORT_POINT_CLOUDS = 1 << 1,
                           LAST = 0
      };
    public:
      /// Reference to the detector object
      Detector& detector;

    public:
      typedef std::vector<PlacedVolume> VolumePlacements;
      /// Default constructor
      OutputWriter(Detector& detector);
      /// Default destructor
      virtual ~OutputWriter();
      /// Write output file
      virtual int write(const std::string& output_file,
                        const std::string& output_type,
                        const VolumePlacements& places,
                        bool   recursive,
                        double unit_scale = 1.0)  const = 0;      
    };
    
  }        /* End namespace cad                      */
}          /* End namespace dd4hep                   */
#endif // DDCAD_INPUTWRITER_H
