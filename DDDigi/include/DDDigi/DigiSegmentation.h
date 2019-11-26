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
#ifndef DD4HEP_DDDIGI_DIGISEGMENTATION_H
#define DD4HEP_DDDIGI_DIGISEGMENTATION_H

/// Framework include files
#include <DD4hep/Segmentations.h>
#include <DD4hep/Volumes.h>
#include <DD4hep/Shapes.h>

/// C/C++ include files
#include <functional>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations

    struct CellDataBase  {
      CellID cell_id {0};
      PlacedVolume  placement;
      Volume        volume;
      Solid         solid;
    };
    template <typename SEGMENTATION> struct cell_data;
    template <typename SEGMENTATION> struct segmentation_data;
    
    template <typename SEGMENTATION> 
    void init_segmentation_data(segmentation_data<SEGMENTATION>& data, const Segmentation& seg);

    /// 
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    struct DigiCellScanner  {
    public:
      typedef std::function<void(const DigiCellScanner& env, const CellDataBase&)> cell_handler_t;
    public:
      DigiCellScanner() = default;
      virtual ~DigiCellScanner() = default;
      virtual void operator()(PlacedVolume pv, VolumeID vid, const cell_handler_t& cell_handler) = 0;
    };
    std::shared_ptr<DigiCellScanner> create_cell_scanner(Solid solid, Segmentation segment);
    std::shared_ptr<DigiCellScanner> create_cell_scanner(const std::string& typ, Segmentation segment);

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGISEGMENTATION_H
