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
#ifndef DDDIGI_SEGMENTATIONS_CARTESIANGRIDXY_H
#define DDDIGI_SEGMENTATIONS_CARTESIANGRIDXY_H

/// Framework include files
#include <DDDigi/DigiSegmentation.h>
#include <DD4hep/CartesianGridXY.h>
#include <DDSegmentation/CartesianGridXY.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    template <> class cell_data<CartesianGridXY> : public DigiCellData {
    public:
      CellID x_cid {0}, y_cid {0};
      CellID x_bin {0}, y_bin {0};
    };

    template <> class segmentation_data<CartesianGridXY> {
    public:
      DDSegmentation::CartesianGridXY* segmentation_xy {0};
      double x_grid_size {0.0};
      double x_offset    {0.0};
      double y_grid_size {0.0};
      double y_offset    {0.0};
      CellID x_mask      {0};
      CellID y_mask      {0};
      int    x_f_offset  {0};
      int    y_f_offset  {0};
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_SEGMENTATIONS_CARTESIANGRIDXY_H
