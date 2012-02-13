// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_GEOMETRY_SEGMENTATIONS_H
#define DD4hep_GEOMETRY_SEGMENTATIONS_H

// Framework include files
#include "DD4hep/Handle.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {
  
  /*
   *   XML namespace declaration
   */
  namespace Geometry  {

    /** @class Segmentation Segmentations.h DD4hep/lcdd/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Segmentation : public Ref_t   {
      struct Object  {
	unsigned long magic;
        unsigned char useForHitPosition;
        union {
          double values[10];
          struct {
            int nx;
            int ny;
            int nz;
          } cartesian;
          struct {
            double grid_size_x;
            double grid_size_y;
            double grid_size_z;
          } cartesian_grid;
          struct {
            int nphi;
            int ntheta;
            int nz;
          } cylindrical_binning;
          struct {
            double grid_size_phi;
            double grid_size_theta;
            double grid_size_z;
          } cylindrical_grid;        
        } data;
        Object() : useForHitPosition(0) {
          data.cartesian_grid.grid_size_x = 0;
          data.cartesian_grid.grid_size_y = 0;
          data.cartesian_grid.grid_size_z = 0;
        }
      };
      /// Default constructor
      Segmentation() : Handle<Implementation>() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Segmentation(const Handle<Q>& e) 
	: Handle<Implementation>(e){}
      /// Constructor to create a new segmentation object (to be called by super class only)
      Segmentation(LCDD& lcdd, const std::string& type);
      Object& _data() const {  return *data<Object>(); }
      bool useForHitPosition() const;
      const std::string type() const;
    };

    /** @class ProjectiveCylinder Segmentations.h DD4hep/lcdd/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct ProjectiveCylinder : public Segmentation  {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> ProjectiveCylinder(const Handle<Q>& e) : Segmentation(e) {}
      /// Constructor to create a new segmentation object
      ProjectiveCylinder(LCDD& lcdd);
      /// Accessors: get number of bins in theta
      int thetaBins() const;
      /// Accessors: get number of bins in phi
      int phiBins() const;
      /// Accessors: set number of bins in theta
      void setThetaBins(int value);
      /// Accessors: set grid size in Y
      void setPhiBins(int value);
    };

    /** @class NonProjectiveCylinder Segmentations.h DD4hep/lcdd/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct NonProjectiveCylinder : public Segmentation  {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> NonProjectiveCylinder(const Handle<Q>& e) : Segmentation(e) {}
      /// Constructor to create a new segmentation object
      NonProjectiveCylinder(LCDD& lcdd);
      /// Accessors: get size of bins in Z
      double gridSizeZ() const;
      /// Accessors: get size of bins in phi
      double gridSizePhi() const;
      /// Accessors: set number of bins in theta
      void setThetaBinSize(double value);
      /// Accessors: set grid size in Y
      void setPhiBinSize(double value);
    };

    /** @class ProjectiveZPlane Segmentations.h DD4hep/lcdd/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct ProjectiveZPlane : public Segmentation  {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> ProjectiveZPlane(const Handle<Q>& e) : Segmentation(e) {}
      /// Constructor to create a new segmentation object
      ProjectiveZPlane(LCDD& lcdd);
      /// Accessors: get number of bins in theta
      int thetaBins() const;
      /// Accessors: get number of bins in phi
      int phiBins() const;
      /// Accessors: set number of bins in theta
      void setThetaBins(int value);
      /// Accessors: set grid size in Y
      void setPhiBins(int value);
    };

    /** @class GridXY Segmentations.h DD4hep/lcdd/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct GridXY : public Segmentation   {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> GridXY(const Handle<Q>& e) : Segmentation(e) {}
      /// Constructor to be used when creating a new object. Data are taken from the input handle
      GridXY(LCDD& lcdd, const std::string& tag);
      /// Constructor to be used when creating a new object.
      GridXY(LCDD& lcdd, const std::string& tag, double size_x, double size_y);
      /// Accessors: set grid size in X
      void setGridSizeX(double value);
      /// Accessors: set grid size in Y
      void setGridSizeY(double value);
    };

    /** @class GridXYZ Segmentations.h DD4hep/lcdd/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct GridXYZ : public GridXY  {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> GridXYZ(const Handle<Q>& e) : GridXY(e) {}
      /// Constructor to be used when creating a new object.
      GridXYZ(LCDD& lcdd);
      /// Constructor to be used when creating a new object.
      GridXYZ(LCDD& lcdd, double size_x, double size_y, double size_z);
      /// Accessors: set grid size in Z
      void setGridSizeZ(double value);
    };

    /** @class CartesianGridXY Segmentations.h DD4hep/lcdd/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct CartesianGridXY : public GridXY   {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> CartesianGridXY(const Handle<Q>& e) : GridXY(e) {}
      /// Constructor to be used when creating a new object. Data are taken from the input handle
      CartesianGridXY(LCDD& lcdd) : GridXY(lcdd,"cartesian_grid_xy") {}
    };

    /** @class GlobalGridXY Segmentations.h DD4hep/lcdd/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct GlobalGridXY : public GridXY   {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> GlobalGridXY(const Handle<Q>& e) : GridXY(e) {}
      /// Constructor to be used when creating a new object. Data are taken from the input handle
      GlobalGridXY(LCDD& lcdd) : GridXY(lcdd,"global_grid_xy") {}
    };

  }       /* End namespace Geometry               */
}         /* End namespace DD4hep                */
#endif    /* DD4hep_GEOMETRY_SEGMENTATIONS_H     */
