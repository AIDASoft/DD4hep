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
#include <DD4hep/Printout.h>
#include <DD4hep/detail/SegmentationsInterna.h>
#include <DDDigi/segmentations/CartesianGridXY.h>
#include <DDDigi/segmentations/SegmentationScanner.h>


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    template <>
    void init_segmentation_data<CartesianGridXY>(segmentation_data<CartesianGridXY>& data,
                                                 const Segmentation& seg)
    {
      CartesianGridXYHandle xy_seg = seg;
      const auto& x_f = (*seg.decoder())["x"];
      const auto& y_f = (*seg.decoder())["y"];
      data.segmentation_xy = xy_seg->implementation;
      data.x_grid_size     = data.segmentation_xy->gridSizeX();
      data.y_grid_size     = data.segmentation_xy->gridSizeY();
      data.x_offset        = data.segmentation_xy->offsetX();
      data.y_offset        = data.segmentation_xy->offsetY();
      data.x_f_offset      = x_f.offset();
      data.y_f_offset      = y_f.offset();
      data.x_mask          = x_f.mask();
      data.y_mask          = y_f.mask();
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
      for ( e.x_bin = 0; e.x_bin < nx; ++e.x_bin )   {
        pos[0] = (e.x_bin + 0.5) * segment.x_grid_size;
        e.x_cid = (e.x_bin << segment.x_f_offset) & segment.x_mask;
        for ( e.y_bin = 0; e.y_bin < ny; ++e.y_bin )   {
          pos[1] = (e.x_bin + 0.5) * segment.x_grid_size;
          if ( !sol->Contains(pos) ) continue;
          e.y_cid = (e.y_bin << segment.y_f_offset) & segment.y_mask;
          e.cell_id = vid | e.x_cid | e.y_cid;
          cell_handler(*this, e);
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
    CellScanner<CartesianGridXY,Box>::operator()(PlacedVolume pv, VolumeID vid, const cell_handler_t& cell_handler)
    {
      cell_data_t e;
      e.placement = pv;
      e.volume    = pv.volume();
      e.solid     = e.volume.solid();
      Box       b = e.solid;
      long nx = 2e0 * b->GetDX() / segment.x_grid_size;
      long ny = 2e0 * b->GetDY() / segment.y_grid_size;
      for ( e.x_bin = 0; e.x_bin < nx; ++e.x_bin )   {
        e.x_cid = (e.x_bin << segment.x_f_offset) & segment.x_mask;
        for ( e.y_bin = 0; e.y_bin < ny; ++e.y_bin )   {
          e.y_cid = (e.y_bin << segment.y_f_offset) & segment.y_mask;
          e.cell_id = vid | e.x_cid | e.y_cid;
          cell_handler(*this, e);
        }
      }
    }
  }    // End namespace digi
}      // End namespace dd4hep

DECLARE_DIGICELLSCANNER(DigiCellScanner,CartesianGridXY,Box)

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    template <typename self_t> void
    scan_poly(PlacedVolume pv, VolumeID vid, const typename self_t::cell_handler_t& cell_handler, const self_t& scanner)   {
      typename self_t::cell_data_t e;
      const auto& segment = scanner.segment;
      e.placement     = pv;
      e.volume        = pv.volume();
      e.solid         = e.volume.solid();
      Box           b = e.solid;
      typename self_t::solid_t h = e.solid;
      long nx = 2e0 * b->GetDX() / segment.x_grid_size;
      long ny = 2e0 * b->GetDY() / segment.y_grid_size;
      double pos[3] = {0e0, 0e0, 0e0}, r;
      double rmin = h->GetRmin(0) * h->GetRmin(0);
      double rmax = h->GetRmax(0) * h->GetRmax(0);
      
      for ( e.x_bin = 0; e.x_bin < nx; ++e.x_bin )   {
        pos[0] = (e.x_bin + 0.5) * segment.x_grid_size;
        pos[0] *= pos[0];
        if ( pos[0] > rmax ) continue;
        e.x_cid = (e.x_bin << segment.x_f_offset) & segment.x_mask;
        for ( e.y_bin = 0; e.y_bin < ny; ++e.y_bin )   {
          pos[1] = (e.x_bin + 0.5) * segment.x_grid_size;
          r = pos[0]*pos[0] + pos[1]*pos[1];
          if ( r < rmin || r > rmax ) continue;
          if ( !h->Contains(pos)    ) continue;
          e.y_cid = (e.y_bin << segment.y_f_offset) & segment.y_mask;
          e.cell_id = vid | e.x_cid | e.y_cid;
          cell_handler(scanner, e);
        }
      }
    }

    template <> void
    CellScanner<CartesianGridXY,PolyhedraRegular>::operator()(PlacedVolume pv, VolumeID vid, const cell_handler_t& cell_handler) {
      scan_poly<self_t>(pv, vid, cell_handler, *this);
    }
    template <> void
    CellScanner<CartesianGridXY,Polyhedra>::operator()(PlacedVolume pv, VolumeID vid, const cell_handler_t& cell_handler) {
      scan_poly<self_t>(pv, vid, cell_handler, *this);
    }
    template <> void
    CellScanner<CartesianGridXY,Polycone>::operator()(PlacedVolume pv, VolumeID vid, const cell_handler_t& cell_handler) {
      scan_poly<self_t>(pv, vid, cell_handler, *this);
    }
  }    // End namespace digi
}      // End namespace dd4hep

DECLARE_DIGICELLSCANNER(DigiCellScanner,CartesianGridXY,PolyhedraRegular)
DECLARE_DIGICELLSCANNER(DigiCellScanner,CartesianGridXY,Polyhedra)
DECLARE_DIGICELLSCANNER(DigiCellScanner,CartesianGridXY,Polycone)

DECLARE_DIGICELLSCANNER(DigiCellScanner,CartesianGridXY,Polycone)
namespace dd4hep  {
  typedef IntersectionSolid Intersection;
  typedef SubtractionSolid Subtraction;
  typedef UnionSolid Union;
}
DECLARE_DIGICELLSCANNER(DigiCellScanner,CartesianGridXY,Intersection)
DECLARE_DIGICELLSCANNER(DigiCellScanner,CartesianGridXY,Subtraction)
DECLARE_DIGICELLSCANNER(DigiCellScanner,CartesianGridXY,Union)
