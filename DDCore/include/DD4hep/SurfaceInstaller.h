//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_DDREC_SURFACEINSTALLER_H
#define DD4HEP_DDREC_SURFACEINSTALLER_H 1

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetFactoryHelper.h"

// C/C++ include files
#include <map>
#include <algorithm>

/// Namespace for the tracking surfaces of the AIDA detector description toolkit
namespace DDSurfaces  {
  /// Class describing the surface types
  class SurfaceType;
}

/// Namespace for the AIDA detector description toolkit
namespace DD4hep  {
  
  /// Namespace for the reconstruction part of the AIDA detector description toolkit
  namespace DDRec  {
    /// Class describing surface data
    //    class SurfaceData;
    class VolSurfaceBase;
  }
  
  /** Base class to implement surface installers for known detector patterns
   *
   *  The class scans the geometry of a subdetector and gives callbacks
   *  to user classes, which then should install in the proper callback
   *  routines the surface instances to the detector elements.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  class SurfaceInstaller  {
  protected:
    typedef Geometry::LCDD         LCDD;
    typedef Geometry::DetElement   DetElement;
    typedef Geometry::Volume       Volume;
    typedef Geometry::PlacedVolume PlacedVolume;
    typedef Geometry::DetectorTools::ElementPath   ElementPath;
    typedef Geometry::DetectorTools::PlacementPath PlacementPath;
    typedef DDRec::VolSurfaceBase      SurfaceData;
    typedef DDSurfaces::SurfaceType SurfaceType;

    typedef std::map<TGeoVolume*, SurfaceData* > Surfaces;
    /// Reference to the LCDD instance
    LCDD&         m_lcdd;
    /// Reference to the detector element of the subdetector
    DetElement    m_det;
    /// Map of surface instances keyed by the logical volume
    Surfaces      m_surfaces;
    /// Flag to inhibit useless further scans
    bool          m_stopScanning;
    /// Scan through tree of detector elements
    void scan(DetElement de);

  public:
    /// Initializing constructor
    SurfaceInstaller(LCDD& lcdd, int argc, char** argv);
    /// Default destructor
    virtual ~SurfaceInstaller() {}
    /// Set flag to stop scanning volumes and detector elements
    void stopScanning()   {  m_stopScanning = true;   }
    /// Indicate error message and throw exception
    void invalidInstaller(const std::string& msg) const;
    /// Shortcut to access the parent detectorelement's volume
    Volume parentVolume(DetElement component)  const;
    /// Shortcut to access the translation vector of a given component
    const double* placementTranslation(DetElement component)  const;
    /// Scan through tree of detector elements
    void scan();
    /// Install volume information. Default implementation only prints!
    virtual void install(DetElement e, PlacedVolume pv);
    /// Executor
    template <typename T> static long run(LCDD& lcdd,int argc,char** argv);
  };

  /// Action routine to execute the test
  template <typename T> inline long SurfaceInstaller::run(Geometry::LCDD& lcdd,int argc,char** argv)  {
    T installer(lcdd, argc, argv);
    installer.scan();
    return 1;
  }

#define DECLARE_SURFACE_INSTALLER(name,class)                           \
  namespace DD4hep {                                                    \
    template long SurfaceInstaller::run< class >(Geometry::LCDD& lcdd,int argc,char** argv); \
  }                                                                     \
  DECLARE_APPLY(name,SurfaceInstaller::run< class >)

}   // End namespace DD4hep


#if defined(DD4HEP_USE_SURFACEINSTALL_HELPER)

#include "DDRec/Surface.h"
#include "DDRec/DetectorData.h"

#ifndef SURFACEINSTALLER_DATA
typedef void* SURFACEINSTALLER_DATA;
#endif

/** If you want to save yourself some typing when creating surface installers,
 *  set the compile macro DD4HEP_USE_SURFACEINSTALL_HELPER LOCALLY !
 *  This will then enable the code below and the only thing you will have to
 *  type is the installer member function to create a measurement surface for
 *  a given volume.
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP
 */
namespace {

  /** Install class template to create surface installer plugins
   *
   *  See the base class SurfaceInstaller for further details.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  template <typename UserData> class Installer : public DD4hep::SurfaceInstaller {
  public:
    typedef DD4hep::DDRec::Vector3D     Vector3D;
    typedef DD4hep::DDRec::VolSurface   VolSurface;
    typedef DD4hep::DDRec::VolPlane     VolPlane;
    typedef DDSurfaces::SurfaceType     Type;
    UserData data;

    /// Default (empty argument handler
    void handle_arguments(int argc, char** argv);
  public:
    /// Initializing constructor
    Installer(LCDD& lcdd, int argc, char** argv);
    /// Default destructor
    virtual ~Installer() {}
    /// Install volume information. Default implementation only prints!
    virtual void install(DetElement component, PlacedVolume pv);
    /// Try to handle surface using the surface cache
    bool handleUsingCache(DetElement comp, Volume vol)  const;
    /// Add a new surface to the surface manager and the local cache
    void addSurface(DetElement component, const DD4hep::DDRec::VolSurface& surf);
    template <typename T> bool checkShape(const T& shape) const   {
      if ( shape.isValid() ) return true;
      invalidInstaller("Shape is not of the required type:"+DD4hep::typeName(typeid(T)));
      return false;
    }
  };

  /// Initializing constructor
  template <typename UserData>
  Installer<UserData>::Installer(LCDD& lcdd, int argc, char** argv)
    : DD4hep::SurfaceInstaller(lcdd, argc, argv) 
  {
    handle_arguments(argc, argv);
  }

  /// Handle surface installation using cached surfaces.
  template <typename UserData>
  bool Installer<UserData>::handleUsingCache(DetElement comp, Volume vol)  const  {
    Surfaces::const_iterator is = m_surfaces.find(vol.ptr());
    if ( is != m_surfaces.end() )  {
      VolSurface surf((*is).second);
      DD4hep::DDRec::volSurfaceList(comp)->push_back(surf);
      return true;
    }
    return false;
  }

  /// Add a new surface to the surface manager and the local cache
  template <typename UserData>
  void Installer<UserData>::addSurface(DetElement component, const DD4hep::DDRec::VolSurface& surf)   {
    m_surfaces.insert(std::make_pair(surf.volume().ptr(),surf.ptr()));
    DD4hep::DDRec::volSurfaceList(component)->push_back(surf);
  }

  /// Default (empty argument handler
  template <typename UserData> void Installer<UserData>::handle_arguments(int, char**)  {}
#ifndef SURFACEINSTALLER_DATA
  template <> void Installer<SURFACEINSTALLER_DATA>::handle_arguments(int, char**)  {}
#endif
}

typedef Installer<SURFACEINSTALLER_DATA> InstallerClass;
DECLARE_SURFACE_INSTALLER(DD4HEP_USE_SURFACEINSTALL_HELPER,InstallerClass)

#endif /* defined(DD4HEP_USE_SURFACEINSTALL_HELPER) */

#endif /* DD4HEP_DDREC_SURFACEINSTALLER_H */
