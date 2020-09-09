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
#ifndef DD4HEP_DD4HEPUI_H
#define DD4HEP_DD4HEPUI_H

// Framework includes
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"

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
      /// Default visualiztion level
      int visLevel = 4;

    public:
      /// Default constructor
      DD4hepUI(Detector& instance);
      /// Default destructor
      virtual ~DD4hepUI();
      /// Access to the Detector instance
      Detector* instance()  const;
      /// Access to the Detector instance
      Detector* detectorDescription()  const;
      /// Set the printout level from the interactive prompt
      PrintLevel setPrintLevel(PrintLevel level)   const;
      /// Set the visualization level when invoking the display
      int setVisLevel(int level);

      /// Install the dd4hep conditions manager object
      Handle<NamedObject> conditionsMgr()  const;
      /// Load conditions from file
      long loadConditions(const std::string& fname)  const;

      /// Dump the entire detector description object to a root file
      long saveROOT(const char* file_name)    const;

      /// Import the entire detector description object from a root file
      long importROOT(const char* file_name)    const;

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

      /// Detector interface: Draw the scene on a OpenGL pane
      virtual void draw() const;
      /// Detector interface: Re-draw the entire scene
      virtual void redraw() const;

      /// Detector interface: Draw detector sub-tree the scene on a OpenGL pane
      virtual void drawSubtree(const char* path) const;
      /// Detector interface: Re-draw the entire sub-tree scene
      virtual void redrawSubtree(const char* path) const;

      /// Dump the volume tree
      virtual long dumpVols(int argc = 0, char** argv = 0)  const;
      /// Dump the DetElement tree with placement volumes
      virtual long dumpDet(const char* path = 0)  const;
      /// Dump the DetElement tree with volume materials
      virtual long dumpDetMaterials(const char* path = 0)  const;
      /// Dump the raw DetElement tree 
      virtual long dumpStructure(const char* path = 0)  const;
    };
    
  }  
} /* End namespace dd4hep        */
#endif // DD4HEP_DD4HEPUI_H
