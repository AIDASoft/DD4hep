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
#ifndef DD4HEP_DDREC_SURFACEINSTALLER_H
#define DD4HEP_DDREC_SURFACEINSTALLER_H 1

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetFactoryHelper.h"

// C/C++ include files
#include <map>
#include <algorithm>



namespace dd4hep{
  namespace rec{
    class VolSurfaceBase;
    class SurfaceType;
  }
}


/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {
  
  /// Namespace for the reconstruction part of the AIDA detector description toolkit
  namespace rec  {
    /// Class describing surface data
    //    class SurfaceData;
    class VolSurfaceBase;
  }
  
  /// Base class to implement surface installers for known detector patterns
  /*
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
    typedef detail::tools::ElementPath   ElementPath;
    typedef detail::tools::PlacementPath PlacementPath;
    typedef rec::VolSurfaceBase        SurfaceData;
    typedef rec::SurfaceType      SurfaceType;
    typedef std::map<TGeoVolume*, SurfaceData* > Surfaces;
    
    /// Reference to the Detector instance
    Detector&         m_detDesc;
    /// Reference to the detector element of the subdetector
    DetElement    m_det;
    /// Map of surface instances keyed by the logical volume
    Surfaces      m_surfaces;
    /// Flag to inhibit useless further scans
    bool          m_stopScanning;
    /// Scan through tree of detector elements
    void scan(DetElement de);

  public:
    /// No default constructor
    SurfaceInstaller() = delete;
    /// No copy constructor
    SurfaceInstaller(const SurfaceInstaller& copy) = delete;
    /// Initializing constructor
    SurfaceInstaller(Detector& description, int argc, char** argv);
    /// Default destructor
    virtual ~SurfaceInstaller() = default;
    /// No assignment
    SurfaceInstaller& operator=(const SurfaceInstaller& copy) = delete;
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
    template <typename T> static long run(Detector& description,int argc,char** argv);
  };

  /// Action routine to execute the test
  template <typename T> inline long SurfaceInstaller::run(Detector& description,int argc,char** argv)  {
    T installer(description, argc, argv);
    installer.scan();
    return 1;
  }

#define DECLARE_SURFACE_INSTALLER(name,class)                           \
  namespace dd4hep {                                                    \
    template long SurfaceInstaller::run< class >(Detector& description,int argc,char** argv); \
  }                                                                     \
  DECLARE_APPLY(name,SurfaceInstaller::run< class >)

}   // End namespace dd4hep


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

  /// Installer class template to create surface installer plugins
  /*
   *  See the base class SurfaceInstaller for further details.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  template <typename UserData> class Installer : public dd4hep::SurfaceInstaller {
  public:
    typedef dd4hep::rec::Vector3D     Vector3D;
    typedef dd4hep::rec::VolSurface   VolSurface;
    typedef dd4hep::rec::VolPlane     VolPlane;
    typedef dd4hep::rec::SurfaceType     Type;
    UserData data;

    /// Default (empty argument handler
    void handle_arguments(int argc, char** argv);
  public:
    /// No default constructor
    Installer() = delete;
    /// No copy constructor
    Installer(const Installer& copy) = delete;
    /// Initializing constructor
    Installer(dd4hep::Detector& description, int argc, char** argv);
    /// Default destructor
    virtual ~Installer() = default;
    /// No assignment
    Installer& operator=(const Installer& copy) = delete;
    /// Install volume information. Default implementation only prints!
    virtual void install(dd4hep::DetElement component, dd4hep::PlacedVolume pv);
    /// Try to handle surface using the surface cache
    bool handleUsingCache(dd4hep::DetElement comp, dd4hep::Volume vol)  const;
    /// Add a new surface to the surface manager and the local cache
    void addSurface(dd4hep::DetElement component, const dd4hep::rec::VolSurface& surf);
    template <typename T> bool checkShape(const T& shape) const   {
      if ( shape.isValid() ) return true;
      invalidInstaller("Shape is not of the required type:"+dd4hep::typeName(typeid(T)));
      return false;
    }
  };

  /// Initializing constructor
  template <typename UserData>
  Installer<UserData>::Installer(dd4hep::Detector& description, int argc, char** argv)
    : dd4hep::SurfaceInstaller(description, argc, argv), data()
  {
    handle_arguments(argc, argv);
  }

  /// Handle surface installation using cached surfaces.
  template <typename UserData>
  bool Installer<UserData>::handleUsingCache(dd4hep::DetElement comp, dd4hep::Volume vol)  const  {
    Surfaces::const_iterator is = m_surfaces.find(vol.ptr());
    if ( is != m_surfaces.end() )  {
      VolSurface surf((*is).second);
      dd4hep::rec::volSurfaceList(comp)->emplace_back(surf);
      return true;
    }
    return false;
  }

  /// Add a new surface to the surface manager and the local cache
  template <typename UserData>
  void Installer<UserData>::addSurface(dd4hep::DetElement component, const dd4hep::rec::VolSurface& surf)   {
    m_surfaces.insert(std::make_pair(surf.volume().ptr(),surf.ptr()));
    dd4hep::rec::volSurfaceList(component)->emplace_back(surf);
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
#endif /* DD4HEP_DDREC_SURFACEINSTALLER_H           */
