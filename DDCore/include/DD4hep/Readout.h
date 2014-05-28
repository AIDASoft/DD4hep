// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4hep_GEOMETRY_READOUT_H
#define DD4hep_GEOMETRY_READOUT_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/Segmentations.h"

// C/C++ include files
#include <map>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    class DetElement;

    /** @class Readout  Readout.h DD4hep/Readout.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Readout: public Ref_t {
    public:

      /** @class Readout::Object  Readout.h DD4hep/Readout.h
       *
       * @author  M.Frank
       * @version 1.0
       */
      struct Object: public TNamed {
        /// Handle to the readout segmentation
        Segmentation segmentation;
        /// Handle to the volume
        Volume readoutWorld;
        /// Handle to the field descriptor
        IDDescriptor id;
        /// Standard constructor
        Object();
        /// Default destructor
        virtual ~Object();
      };
    public:
      /// Default constructor
      Readout()
          : Ref_t() {
      }
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Readout(const Handle<Q>& e)
          : Ref_t(e) {
      }
      /// Initializing constructor
      Readout(const std::string& name);
      /// Assign IDDescription to readout structure
      void setIDDescriptor(const Ref_t& spec) const;
      /// Access IDDescription structure
      IDDescriptor idSpec() const;
      /// Assign segmentation structure to readout
      void setSegmentation(const Segmentation& segment) const;
      /// Access segmentation structure
      Segmentation segmentation() const;
    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */
#endif    /* DD4hep_GEOMETRY_READOUT_H      */
