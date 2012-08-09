// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_GEOMETRY_SEGMENTATIONS_H
#define DD4HEP_GEOMETRY_SEGMENTATIONS_H

// Framework include files
#include "DD4hep/Handle.h"

// C/C++ include files
#include <cmath>

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
      public:
      enum { REGULAR=0, EXTENDED=1 };

      struct Object  {
	/// Magic word to check object integrity
	unsigned long magic;
	/// Segmentation type (REGULAR or EXTENDED)
	unsigned char type;
	/// Flag to use segmentation for hit positioning
        unsigned char useForHitPosition;
	/// Spares to start 16 byte Byte aligned
	unsigned char _spare[6];

        union Data {
	  /// Maximal size and data buffer for specialized user segentations
          double values[32];
	  /// Extension buffer for specialized user segentations, where above values are insufficient
	  struct Extension {
	    const std::type_info* info;
	    void (*destructor)(void*);
	    void* ptr;
	  } extension;
	  /// No the regular structures for default segmentations
          struct Cartesian {
            int nx;
            int ny;
            int nz;
          } cartesian;
          struct CartesianGrid {
            double grid_size_x;
            double grid_size_y;
            double grid_size_z;
          } cartesian_grid;
          struct CylindricalBinning  {
            int nphi;
            int ntheta;
            int nz;
          } cylindrical_binning;
          struct CylindricalGrid   {
            double grid_size_phi;
            double grid_size_theta;
            double grid_size_z;
          } cylindrical_grid;
	    
        } data;
	Object();
	~Object();
      };

      protected:
      /// Templated destructor function
      template <typename T> static void  _delete(void* ptr) { delete (T*)(ptr); }
      /// Add an extension object to the detector element
      void* i_setExtension(void* ptr, const std::type_info& info, void (*destruct)(void*));
      /// Access an existing extension object from the detector element
      void* i_extension(const std::type_info& info)  const;

      public:
      /// Default constructor
      Segmentation() : Handle<Implementation>() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Segmentation(const Handle<Q>& e) 
	: Handle<Implementation>(e){}
      /// Constructor to create a new segmentation object (to be called by super class only)
      Segmentation(const std::string& type);
      /// Accessor to ata structure
      Object& _data() const                            {  return *data<Object>();               }
      /// Access flag for hit positioning
      bool useForHitPosition() const;
      /// Segmentation type
      const std::string type() const;
      /// Extend the segmentation object with an arbitrary structure accessible by the type
      template<typename IFACE, typename CONCRETE> IFACE* setExtension(CONCRETE* c)    
      {  return (IFACE*)i_addExtension(dynamic_cast<IFACE*>(c),typeid(IFACE),_delete<IFACE>);   }
      /// Access extension element by the type
      template <class T> T* extension()  const         {  return (T*)i_extension(typeid(T));    }
      /// Access extension element by the type
      template <class T> T* extensionUnchecked() const {  return (T*)_data().data.extension.ptr;}

      /// Compute the coordinate in one dimension given a eauidistant bin value.
      static double binCenter(int bin, double width)   {  return (double(bin) + .5) * width;    }
      /// Compute the equidistant bin given a coordinate in one dimension.
      static int bin(double value, double width)       {  return int(floor(value/width));       }
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
      ProjectiveCylinder();
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
      NonProjectiveCylinder();
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
      ProjectiveZPlane();
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
      /// Constructor to create a new segmentation object
      GridXY();
      /// Constructor to be used when creating a new object. Data are taken from the input handle
      GridXY(const std::string& tag);
      /// Constructor to be used when creating a new object.
      GridXY(const std::string& tag, double size_x, double size_y);
      /// Accessors: set grid size in X
      void setGridSizeX(double value);
      /// Accessors: set grid size in Y
      void setGridSizeY(double value);
      /// Accessors: get grid size in X
      double getGridSizeX()const;
      /// Accessors: get grid size in Y
      double getGridSizeY()const;
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
      GridXYZ();
      /// Constructor to be used when creating a new object.
      GridXYZ(double size_x, double size_y, double size_z);
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
      CartesianGridXY() : GridXY("cartesian_grid_xy") {}
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
      GlobalGridXY() : GridXY("global_grid_xy") {}
    };


  }       /* End namespace Geometry              */
}         /* End namespace DD4hep                */
#endif    /* DD4HEP_GEOMETRY_SEGMENTATIONS_H     */
