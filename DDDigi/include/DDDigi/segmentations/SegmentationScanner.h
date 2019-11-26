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
#ifndef DD4HEP_DDDIGI_SEGMENTATIONSCANNER_H
#define DD4HEP_DDDIGI_SEGMENTATIONSCANNER_H

/// Framework include files
#include <DDDigi/DigiSegmentation.h>
#include <DDDigi/DigiFactories.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// 
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    template <typename SEGMENTATION, typename SOLID> struct CellScanner : public DigiCellScanner  {
    public:
      typedef SOLID                                   solid_t;
      typedef SEGMENTATION                            segmentation_t;
      typedef CellScanner<segmentation_t, solid_t>    self_t;
      typedef cell_data<segmentation_t>               cell_data_t;
      typedef segmentation_data<segmentation_t>       segmentation_data_t;
      typedef DigiCellScanner::cell_handler_t         cell_handler_t;

      segmentation_data_t segment;
      CellScanner(segmentation_t seg)   {
        init_segmentation_data<segmentation_t>(segment, seg);
      }
      virtual void operator()(PlacedVolume pv, VolumeID vid, const cell_handler_t& cell_handler)  override;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_SEGMENTATIONSCANNER_H
