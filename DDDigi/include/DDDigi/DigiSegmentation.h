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
#ifndef DDDIGI_DIGISEGMENTATION_H
#define DDDIGI_DIGISEGMENTATION_H

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

    /// Forward declarations
    class DigiContext;
    class DigiSegmentation;
    template <typename SEGMENTATION> class cell_data;
    template <typename SEGMENTATION> class segmentation_data;    
    
    /// 
    /*
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiCellData   {
    public:
      PlacedVolume  placement  { };
      Volume        volume     { };
      Solid         solid      { };
      CellID        cell_id    { 0 };
      double        signal     { 0.0 };
      mutable bool  kill       { false };
      /// Default constructor
      DigiCellData() = default;
      /// Default move constructor
      DigiCellData(DigiCellData&& copy) = default;
      /// Default copy constructor
      DigiCellData(const DigiCellData& copy) = default;
      /// Default destructor
      virtual ~DigiCellData() = default;
      /// Default move assignment
      DigiCellData& operator=(DigiCellData&& copy) = delete;
      /// Default copy assignment
      DigiCellData& operator=(const DigiCellData& copy) = delete;      
    };

    template <typename SEGMENTATION> 
    void init_segmentation_data(segmentation_data<SEGMENTATION>& data, const Segmentation& seg);

    /// 
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiCellScanner  {
    public:
      typedef std::function<void(DigiContext& context, const DigiCellScanner& env, const DigiCellData&)> cell_handler_t;
    public:
      DigiCellScanner() = default;
      virtual ~DigiCellScanner() = default;
      virtual void operator()(DigiContext& context, PlacedVolume pv, VolumeID vid, const cell_handler_t& cell_handler) = 0;
    };
    std::shared_ptr<DigiCellScanner> create_cell_scanner(Solid solid, Segmentation segment);
    std::shared_ptr<DigiCellScanner> create_cell_scanner(const std::string& typ, Segmentation segment);

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGISEGMENTATION_H
