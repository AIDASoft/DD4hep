// $Id$
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
#include "DD4hep/IDDescriptor.h"
#include "DDSegmentation/CartesianGridXYZ.h"

// C/C++ include files
#include <cmath>
#include <vector>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   XML namespace declaration
   */
  namespace Geometry {

    typedef DDSegmentation::BitField64 BitField64;

    /** @class Segmentation Segmentations.h DD4hep/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Segmentation: public Handle<DDSegmentation::Segmentation> {
    public:
      typedef DDSegmentation::Segmentation BaseSegmentation;

      /** @class Segmentation::Object Segmentations.h DD4hep/Segmentations.h
       *
       * @author  M.Frank
       * @version 1.0
       */
      struct Object {
        /// Magic word to check object integrity
        unsigned long magic;
        /// Flag to use segmentation for hit positioning
        unsigned char useForHitPosition;
        /// Reference to base segmentation
        BaseSegmentation* segmentation;
        /// Standard constructor
        Object();
        /// Default destructor
        virtual ~Object();
      };

    public:
      /// Default constructor
      Segmentation()
          : Handle<Implementation>() {
      }
      /// Initializing constructor creating new object
      template <typename T> Segmentation(T* o, const std::string& nam, const std::string& typ)
          : Handle<Implementation>() {
        o->segmentation = o;
        assign(o, nam, typ);
      }
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Segmentation(const Handle<Q>& e)
          : Handle<Implementation>(e) {
      }
      /// Access flag for hit positioning
      bool useForHitPosition() const;
      /// Accessor: Segmentation type
      std::string type() const;
      /// Accessor: Set segmentation type
      void setType(const std::string& new_type);
      /// Access segmentation object
      BaseSegmentation* segmentation() const;
      /// determine the local position based on the cell ID
      std::vector<double> getLocalPosition(const long64& cellID) const;
      /// determine the cell ID based on the local position
      long64 getCellID(double x, double y, double z) const;
    };

    /** @class SegmentationParams Segmentations.h DD4hep/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct SegmentationParams: public Segmentation {
    public:
      /// Segmentation parameter definition
      typedef std::pair<std::string, double> Parameter;
      /// Segmentation parameter container definition
      typedef std::vector<Parameter> Parameters;
      /// Constructor to be used when reading the already parsed object
      SegmentationParams(const Segmentation& e)
          : Segmentation(e) {
      }
      /// Segmentation type
      std::string type() const;
      /// Access to the parameters
      Parameters parameters() const;
    };

    /** @class ProjectiveCylinder Segmentations.h DD4hep/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct ProjectiveCylinder: public Segmentation {

      struct Data: public Object, public BaseSegmentation {
        int nphi;
        int ntheta;
        int nz;
        /// Default constructor
        Data(BitField64* decoder = 0)
            : Object(), BaseSegmentation(decoder), nphi(0), ntheta(0), nz(0) {
        }
        /// Default destructor
        virtual ~Data();
        /// determine the position based on the cell ID
        virtual std::vector<double> getPosition(const long64& cellID) const;
        /// determine the local position based on the cell ID
        virtual std::vector<double> getLocalPosition(const long64& cellID) const;
        /// determine the cell ID based on the local position
        virtual long64 getCellID(double x, double y, double z) const;
      };
      /// Constructor to be used when reading the already parsed object
      template <typename Q> ProjectiveCylinder(const Handle<Q>& e)
          : Segmentation(e) {
      }
      /// Constructor to create a new segmentation object
      ProjectiveCylinder(LCDD& lcdd);
      /// Accessors: get number of bins in theta
      int thetaBins() const;
      /// Accessors: get number of bins in phi
      int phiBins() const;
      /// Accessors: get number of bins in z
      int zBins() const;
      /// Accessors: set number of bins in theta
      void setThetaBins(int value);
      /// Accessors: set number of bins in phi
      void setPhiBins(int value);
      /// Accessors: set number of bins in Z
      void setZBins(int value);
    };

    /** @class NonProjectiveCylinder Segmentations.h DD4hep/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct NonProjectiveCylinder: public Segmentation {
      struct Data: public Object, public BaseSegmentation {
        double grid_size_phi;
        double grid_size_theta;
        double grid_size_z;
        /// Default constructor
        Data(BitField64* decoder = 0)
            : Object(), BaseSegmentation(decoder) {
          grid_size_phi = grid_size_theta = grid_size_z = 0;
        }
        /// Default destructor
        virtual ~Data();
        /// determine the position based on the cell ID
        virtual std::vector<double> getPosition(const long64& cellID) const;
        /// determine the local position based on the cell ID
        virtual std::vector<double> getLocalPosition(const long64& cellID) const;
        /// determine the cell ID based on the local position
        virtual long64 getCellID(double x, double y, double z) const;
      };
      /// Constructor to be used when reading the already parsed object
      template <typename Q> NonProjectiveCylinder(const Handle<Q>& e)
          : Segmentation(e) {
      }
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

    /** @class ProjectiveZPlane Segmentations.h DD4hep/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct ProjectiveZPlane: public Segmentation {
      struct Data: public Object, public BaseSegmentation {
        int nphi;
        int ntheta;
        int nz;
        /// Default constructor
        Data(BitField64* decoder = 0)
            : Object(), BaseSegmentation(decoder) {
          nphi = ntheta = nz = 0;
        }
        /// Default destructor
        virtual ~Data();
        /// determine the position based on the cell ID
        virtual std::vector<double> getPosition(const long64& cellID) const;
        /// determine the local position based on the cell ID
        virtual std::vector<double> getLocalPosition(const long64& cellID) const;
        /// determine the cell ID based on the local position
        virtual long64 getCellID(double x, double y, double z) const;
      };
      /// Constructor to be used when reading the already parsed object
      template <typename Q> ProjectiveZPlane(const Handle<Q>& e)
          : Segmentation(e) {
      }
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

    /** @class GridXY Segmentations.h DD4hep/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct GridXY: public Segmentation {
      struct Data: public Object, public DDSegmentation::CartesianGridXY {
        /// Default constructor
        Data(BitField64* decoder = 0)
            : Object(), DDSegmentation::CartesianGridXY(decoder) {
        }
        /// Default destructor
        virtual ~Data();
      };
      /// Constructor to be used when reading the already parsed object
      template <typename Q> GridXY(const Handle<Q>& e)
          : Segmentation(e) {
      }
      /// Constructor to create a new segmentation object
      GridXY(LCDD& lcdd, const std::string& typ);
      /// Accessors: set grid size in X
      void setGridSizeX(double value);
      /// Accessors: set grid size in Y
      void setGridSizeY(double value);
      /// Accessors: get grid size in X
      double getGridSizeX() const;
      /// Accessors: get grid size in Y
      double getGridSizeY() const;
    };

    /** @class GridXYZ Segmentations.h DD4hep/Segmentations.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct GridXYZ: public Segmentation {
      struct Data: public Object, public DDSegmentation::CartesianGridXYZ {
        /// Default constructor
        Data(BitField64* decoder = 0)
            : Object(), DDSegmentation::CartesianGridXYZ(decoder) {
        }
        /// Default destructor
        virtual ~Data();
      };
      /// Constructor to be used when reading the already parsed object
      template <typename Q> GridXYZ(const Handle<Q>& e)
          : GridXY(e) {
      }
      /// Constructor to be used when creating a new object.
      GridXYZ(LCDD& lcdd, const std::string& typ);
      /// Accessors: set grid size in X
      void setGridSizeX(double value);
      /// Accessors: set grid size in Y
      void setGridSizeY(double value);
      /// Accessors: set grid size in Z
      void setGridSizeZ(double value);
      /// Accessors: get grid size in X
      double getGridSizeX() const;
      /// Accessors: get grid size in Y
      double getGridSizeY() const;
      /// Accessors: get grid size in Z
      double getGridSizeZ() const;
    };

  } /* End namespace Geometry              */
} /* End namespace DD4hep                */
#endif    /* DD4HEP_GEOMETRY_SEGMENTATIONS_H     */
