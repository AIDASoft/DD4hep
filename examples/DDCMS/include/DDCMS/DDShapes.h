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
//
// DDCMS is a detector description convention developed by the CMS experiment.
//
//==========================================================================
#ifndef DD4HEP_DDCMS_DDSHAPES_H
#define DD4HEP_DDCMS_DDSHAPES_H

// Framework includes
#include <DD4hep/Shapes.h>

/// C/C++ include files
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace of DDCMS conversion namespace
  namespace cms  {

    /// Class describing a box shape
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \ingroup DD4HEP_EXAMPLES
     */
    class DDBox : public Solid_type<TGeoBBox> {
    public:
      /// Default constructor
      DDBox() = default;
      /// Move constructor
      DDBox(DDBox&& e) = default;
      /// Copy constructor
      DDBox(const DDBox& e) = default;
      /// Constructor to be used with an existing object
      template <typename Q> DDBox(const Q* p) : Solid_type<TGeoBBox>(p) { }
      /// Copy Constructor to be used with an existing object handle
      template <typename Q> DDBox(const Handle<Q>& e) : Solid_type<TGeoBBox>(e) { }
      /// Optional: special constructor with object creation
      DDBox(const std::string& nam, double dx, double dy, double dz);

      /// Move Assignment operator
      DDBox& operator=(DDBox&& copy) = default;
      /// Copy Assignment operator
      DDBox& operator=(const DDBox& copy) = default;
      /// Access half "length" of the box
      double halfX() const    {   return access()->GetDX();  }
      /// Access half "width" of the box
      double halfY() const    {   return access()->GetDY();  }
      /// Access half "depth" of the box
      double halfZ() const    {   return access()->GetDZ();  }
    };

    /// Class describing a polycone shape
    /**
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \ingroup DD4HEP_EXAMPLES
     */
    class DDPolycone : public Solid_type<TGeoPcon> {
    public:
      /// Default constructor
      DDPolycone() = default;
      /// Move Constructor
      DDPolycone(DDPolycone&& e) = default;
      /// Copy Constructor
      DDPolycone(const DDPolycone& e) = default;
      /// Constructor to be used with an existing object
      template <typename Q> DDPolycone(const Q* p) : Solid_type<Object>(p)  {         }
      /// Constructor to be used when passing an already created object
      template <typename Q> DDPolycone(const Handle<Q>& e) : Solid_type<Object>(e) {  }

      /// Optional: special constructor with object creation
      DDPolycone(const std::string& nam,
                 double start_phi, double delta_phi,
                 const std::vector<double>& z,
                 const std::vector<double>& rmin,
                 const std::vector<double>& rmax);

      /// Move Assignment operator
      DDPolycone& operator=(DDPolycone&& copy) = default;
      /// Copy Assignment operator
      DDPolycone& operator=(const DDPolycone& copy) = default;

      double startPhi() const;
      double deltaPhi() const;
      std::vector<double> zVec() const;
      std::vector<double> rMinVec() const;
      std::vector<double> rMaxVec() const;
    };

  }    /* End namespace cms          */
}      /* End namespace dd4hep       */
#endif /* DD4HEP_DDCMS_DDSHAPES_H    */
