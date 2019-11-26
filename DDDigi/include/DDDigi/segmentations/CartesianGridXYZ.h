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
#ifndef DD4HEP_DDDIGI_SEGMENTATION_CARTESIANGRIDXYZ_H
#define DD4HEP_DDDIGI_SEGMENTATION_CARTESIANGRIDXYZ_H

/// Framework include files
#include <DDDigi/DigiSegmentation.h>
#include <DD4hep/CartesianGridXYZ.h>
#include <DDSegmentation/CartesianGridXYZ.h>

/// C/C++ include files
#include <functional>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    template <> class cell_data<CartesianGridXYZ> : public CellDataBase {
    public:
      CellID x_cid {0}, y_cid {0}, z_cid {0};
      CellID x_bin {0}, y_bin {0}, z_bin {0};
    };

    template <> class segmentation_data<CartesianGridXYZ> {
    public:
      DDSegmentation::CartesianGridXYZ* segmentation_xyz {0};
      double x_grid_size {0.0}, x_offset {0.0};
      double y_grid_size {0.0}, y_offset {0.0};
      double z_grid_size {0.0}, z_offset {0.0};
      CellID x_mask {0};
      CellID y_mask {0};
      CellID z_mask {0};
      int    x_f_offset {0}, y_f_offset {0}, z_f_offset {0};
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_SEGMENTATION_CARTESIANGRIDXYZ_H
