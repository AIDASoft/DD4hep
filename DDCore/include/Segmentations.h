#ifndef DETDESC_GEOMETRY_SEGMENTATIONS_H
#define DETDESC_GEOMETRY_SEGMENTATIONS_H
#include "Elements.h"

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   XML namespace declaration
  */
  namespace Geometry  {

    struct Segmentation : public Element   {
      struct Object  {
        unsigned char Attr_useForHitPosition;
        union {
          double values[10];
          struct {
            int Attr_nx;
            int Attr_ny;
            int Attr_nz;
          } cartesian;
          struct {
            double Attr_grid_size_x;
            double Attr_grid_size_y;
            double Attr_grid_size_z;
          } cartesian_grid;
          struct {
            int Attr_nphi;
            int Attr_ntheta;
            int Attr_nz;
          } cylindrical_binning;
          struct {
            double Attr_grid_size_phi;
            double Attr_grid_size_theta;
            double Attr_grid_size_z;
          } cylindrical_grid;        
        } data;
        Object() : Attr_useForHitPosition(0) {
          data.cartesian_grid.Attr_grid_size_x = 0e0;
          data.cartesian_grid.Attr_grid_size_y = 0e0;
          data.cartesian_grid.Attr_grid_size_z = 0e0;
        }
      };
      /// Constructor to be used when reading the already parsed object
      Segmentation(Handle_t e);
      /// Constructor to be used when reading the already parsed object
      Segmentation(const Element& e);
      /// Constructor to create a new segmentation object (to be called by super class only)
      Segmentation(const Document& doc, const std::string& type);
      bool useForHitPosition() const;
      const std::string type() const;
    };

    struct ProjectiveCylinder : public Segmentation  {
      /// Constructor to be used when reading the already parsed object
      ProjectiveCylinder(const Element& e) : Segmentation(e) {}
      /// Constructor to create a new segmentation object
      ProjectiveCylinder(const Document& doc);
      /// Accessors: get number of bins in theta
      int thetaBins() const;
      /// Accessors: get number of bins in phi
      int phiBins() const;
      /// Accessors: set number of bins in theta
      void setThetaBins(int value);
      /// Accessors: set grid size in Y
      void setPhiBins(int value);
    };

    struct NonProjectiveCylinder : public Segmentation  {
      /// Constructor to be used when reading the already parsed object
      NonProjectiveCylinder(const Element& e) : Segmentation(e) {}
      /// Constructor to create a new segmentation object
      NonProjectiveCylinder(const Document& doc);
      /// Accessors: get size of bins in Z
      double gridSizeZ() const;
      /// Accessors: get size of bins in phi
      double gridSizePhi() const;
      /// Accessors: set number of bins in theta
      void setThetaBinSize(double value);
      /// Accessors: set grid size in Y
      void setPhiBinSize(double value);
    };

    struct ProjectiveZPlane : public Segmentation  {
      /// Constructor to be used when reading the already parsed object
      ProjectiveZPlane(const Element& e);
      /// Constructor to create a new segmentation object
      ProjectiveZPlane(const Document& doc);
      /// Accessors: get number of bins in theta
      int thetaBins() const;
      /// Accessors: get number of bins in phi
      int phiBins() const;
      /// Accessors: set number of bins in theta
      void setThetaBins(int value);
      /// Accessors: set grid size in Y
      void setPhiBins(int value);
    };

    struct GridXY : public Segmentation   {
      /// Constructor to be used when reading the already parsed object
      GridXY(const Element& h) : Segmentation(h) {}
      /// Constructor to be used when creating a new object. Data are taken from the input handle
      GridXY(const Document& doc, const std::string& tag);
      /// Constructor to be used when creating a new object.
      GridXY(const Document& doc, const std::string& tag, double size_x, double size_y);
      /// Accessors: set grid size in X
      void setGridSizeX(double value);
      /// Accessors: set grid size in Y
      void setGridSizeY(double value);
    };

    struct GridXYZ : public GridXY  {
      /// Constructor to be used when reading the already parsed object
      GridXYZ(const Element& h) : GridXY(h) {}
      /// Constructor to be used when creating a new object.
      GridXYZ(const Document& doc);
      /// Constructor to be used when creating a new object.
      GridXYZ(const Document& doc, double size_x, double size_y, double size_z);
      /// Accessors: set grid size in Z
      void setGridSizeZ(double value);
    };

    struct CartesianGridXY : public GridXY   {
      /// Constructor to be used when reading the already parsed object
      CartesianGridXY(const Element& element) : GridXY(element) {}
      /// Constructor to be used when creating a new object. Data are taken from the input handle
      CartesianGridXY(const Document& doc) : GridXY(doc,"cartesian_grid_xy") {}
    };

    struct GlobalGridXY : public GridXY   {
      /// Constructor to be used when reading the already parsed object
      GlobalGridXY(const Element& element) : GridXY(element) {}
      /// Constructor to be used when creating a new object. Data are taken from the input handle
      GlobalGridXY(const Document& doc) : GridXY(doc,"global_grid_xy") {}
    };

  }
}         /* End namespace DetDesc                */
#endif    /* DETDESC_GEOMETRY_SEGMENTATIONS_H     */
