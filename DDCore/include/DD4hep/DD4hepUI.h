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
#ifndef DD4HEP_DDCORE_DD4HEPUI_H
#define DD4HEP_DDCORE_DD4HEPUI_H

// Framework includes
#include "DD4hep/Detector.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {

    /// ROOT interactive UI for dd4hep applications
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP
     */
    class DD4hepUI  {
    protected:
      /// Reference to the Detector instance object
      Detector& m_detDesc;
      Handle<NamedObject> m_condMgr;
      Handle<NamedObject> m_alignMgr;
    
    public:
      /// Default constructor
      DD4hepUI(Detector& instance);
      /// Default destructor
      virtual ~DD4hepUI();
      /// Access to the Detector instance
      Detector* instance()  const;
      /// Access to the Detector instance
      Detector* detectorDescription()  const;

      /// Install the dd4hep conditions manager object
      Handle<NamedObject> conditionsMgr()  const;
      /// Load conditions from file
      long loadConditions(const std::string& fname)  const;
    
      /// Install the dd4hep alignment manager object
      Handle<NamedObject> alignmentMgr()  const;

      /// Create ROOT interpreter instance
      long createInterpreter(int argc, char** argv);
      /// Execute ROOT interpreter instance
      long runInterpreter()  const;
      
      /// Detector interface: Manipulate geometry using factory converter
      virtual long apply(const char* factory, int argc, char** argv) const;
      /// Detector interface: Read any geometry description or alignment file
      virtual void fromXML(const std::string& fname, DetectorBuildType type = BUILD_DEFAULT) const;
      /// Detector interface: Re-draw the entire scene
      virtual void redraw() const;
      /// Dump the volume tree
      virtual long dumpVols(int argc=0, char** argv=0)  const;
      /// Dump the DetElement tree
      virtual long dumpDet()  const;
    };
    
  }  
} /* End namespace dd4hep        */
#endif // DD4HEP_DDCORE_DD4HEPUI_H
