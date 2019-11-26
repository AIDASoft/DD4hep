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

/// Framework include files
#include <DD4hep/detail/SegmentationsInterna.h>
#include <DDDigi/segmentations/CartesianGridXYZ.h>
#include <DDDigi/segmentations/SegmentationScanner.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    template <>
    void init_segmentation_data<CartesianGridXYZ>(segmentation_data<CartesianGridXYZ>& data,
                                                 const Segmentation& seg)
    {
      CartesianGridXYZHandle xyz_seg = seg;
      const auto& x_f  = (*seg.decoder())["x"];
      const auto& y_f  = (*seg.decoder())["y"];
      const auto& z_f  = (*seg.decoder())["z"];
      data.segmentation_xyz = xyz_seg->implementation;
      data.x_grid_size      = data.segmentation_xyz->gridSizeX();
      data.y_grid_size      = data.segmentation_xyz->gridSizeY();
      data.z_grid_size      = data.segmentation_xyz->gridSizeZ();
      data.x_offset         = data.segmentation_xyz->offsetX();
      data.y_offset         = data.segmentation_xyz->offsetY();
      data.z_offset         = data.segmentation_xyz->offsetZ();
      data.x_f_offset       = x_f.offset();
      data.y_f_offset       = y_f.offset();
      data.z_f_offset       = z_f.offset();
      data.x_mask           = x_f.mask();
      data.y_mask           = y_f.mask();
      data.z_mask           = z_f.mask();
    }

    template <typename SEGMENTATION, typename SOLID> void
    CellScanner<SEGMENTATION,SOLID>::operator()(PlacedVolume pv, VolumeID vid, const cell_handler_t& cell_handler) {
      typename self_t::cell_data_t e;
      e.placement     = pv;
      e.volume        = pv.volume();
      e.solid         = e.volume.solid();
      Box           b = e.solid;
      double   pos[3] = {0e0, 0e0, 0e0};
      typename self_t::solid_t sol = e.solid;
      long nx = 2e0 * b->GetDX() / segment.x_grid_size;
      long ny = 2e0 * b->GetDY() / segment.y_grid_size;
      long nz = 2e0 * b->GetDZ() / segment.z_grid_size;
      for ( e.x_bin = 0; e.x_bin < nx; ++e.x_bin )   {
        pos[0] = (e.x_bin + 0.5) * segment.x_grid_size;
        e.x_cid = (e.x_bin << segment.x_f_offset) & segment.x_mask;
        for ( e.y_bin = 0; e.y_bin < ny; ++e.y_bin )   {
          pos[1] = (e.x_bin + 0.5) * segment.x_grid_size;
          e.y_cid = (e.y_bin << segment.y_f_offset) & segment.y_mask;
          for ( e.z_bin=0; e.z_bin < nz; ++e.z_bin )   {
            e.z_cid = (e.z_bin << segment.z_f_offset) & segment.z_mask;
            pos[2] = (e.z_bin + 0.5) * segment.z_grid_size;
            if ( !sol->Contains(pos) ) continue;
            e.cell_id = vid | e.x_cid | e.y_cid | e.y_cid;
            e.cell_id = vid | e.x_cid | e.y_cid;
            cell_handler(*this, e);
          }
        }
      }
    }
  }    // End namespace digi
}      // End namespace dd4hep

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    template <> void
    CellScanner<CartesianGridXYZ,Box>::operator()(PlacedVolume pv, VolumeID vid, const cell_handler_t& cell_handler)
    {
      cell_data_t e;
      e.placement = pv;
      e.volume    = pv.volume();
      e.solid     = e.volume.solid();
      Box       b = e.solid;
      long nx = 2e0 * b->GetDX() / segment.x_grid_size;
      long ny = 2e0 * b->GetDY() / segment.y_grid_size;
      long nz = 2e0 * b->GetDZ() / segment.z_grid_size;
      for ( e.x_bin=0; e.x_bin < nx; ++e.x_bin )   {
        e.x_cid = (e.x_bin << segment.x_f_offset) & segment.x_mask;
        for ( e.y_bin=0; e.y_bin < ny; ++e.y_bin )   {
          e.y_cid = (e.y_bin << segment.y_f_offset) & segment.y_mask;
          for ( e.z_bin=0; e.z_bin < nz; ++e.z_bin )   {
            e.z_cid = (e.z_bin << segment.z_f_offset) & segment.z_mask;
            e.cell_id = vid | e.x_cid | e.y_cid | e.y_cid;
            cell_handler(*this, e);
          }
        }
      }
    }
  }    // End namespace digi
}      // End namespace dd4hep

DECLARE_DIGICELLSCANNER(DigiCellScanner,CartesianGridXYZ,Box)

namespace dd4hep  {
  typedef IntersectionSolid Intersection;
  typedef SubtractionSolid Subtraction;
  typedef UnionSolid Union;
}
DECLARE_DIGICELLSCANNER(DigiCellScanner,CartesianGridXYZ,Intersection)
DECLARE_DIGICELLSCANNER(DigiCellScanner,CartesianGridXYZ,Subtraction)
DECLARE_DIGICELLSCANNER(DigiCellScanner,CartesianGridXYZ,Union)

