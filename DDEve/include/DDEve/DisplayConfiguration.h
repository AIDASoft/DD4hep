//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//==========================================================================
#ifndef DD4HEP_DDEVE_DISPLAYCONFIGURATION_H
#define DD4HEP_DDEVE_DISPLAYCONFIGURATION_H

// Framework include files
#include "TClass.h"

// C/C++ include files
#include <string>
#include <list>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  class Display;

  /// DisplayConfiguration  DisplayConfiguration.h DDEve/DisplayConfiguration.h
  /*
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class  DisplayConfiguration   {
  protected:
    Display* m_display;
  public:
    enum { NO_DATA    = 0,
           CALODATA   = 1<<1, 
           DETELEMENT = 1<<2, 
           VIEW       = 1<<3, 
           PANEL      = 1<<4,
           COLLECTION = 1<<5
    };
    struct Defaults {
      char  load_geo;
      char  show_evt;
      short default_pad;
      int   color;
      float alpha;
    };
    struct Calo3D : public Defaults  {
      float rmin, dz, threshold, towerH, emax;
    };
    struct Calodata : public Defaults {
      float rmin, dz, threshold, towerH, emax;
      float eta_min, eta_max;
      float phi_min, phi_max;
      short n_eta;
      short n_phi;
      int spare;
    };
    struct Panel : public Defaults {
    };
    struct Hits : public Defaults {
      float size;   // Marker size
      float width;
      float threshold;
      float towerH;
      float emax;   // Max energy deposit displayed
      int   type;   // Marker type
    };
    class Config  {
    public:
      union Values  {
        double vals[20];
        Defaults defaults;
        Calo3D calo3d;
        Calodata calodata;
        Panel pane;
        Hits hits;
      } data;
      std::string name;
      std::string hits;
      std::string use;
      int type;
      /// Default constructor
      Config();
      /// Copy constructor
      Config(const Config& c);
      /// Default destructor
      ~Config();
      /// Assignment operator
      Config& operator=(const Config& c);
    };
    typedef std::vector<Config> Configurations;
    class ViewConfig : public Config {
    public:
      std::string type;
      Configurations subdetectors;
      bool show_sensitive;
      bool show_structure;
      /// Default constructor
      ViewConfig();
      /// Copy constructor
      ViewConfig(const ViewConfig& c);
      /// Default destructor
      virtual ~ViewConfig();
      /// Assignment operator
      ViewConfig& operator=(const ViewConfig& c);
    };
    typedef std::list<ViewConfig> ViewConfigurations;
    ViewConfigurations views;
    Configurations     calodata;
    Configurations     collections;
  public:
    /// Initializing constructor
    DisplayConfiguration(Display* eve);
    /// Default destructor
    virtual ~DisplayConfiguration();
    /// Root implementation macro
    ClassDef(DisplayConfiguration,0);
  };

} /* End namespace DD4hep   */

#endif /* DD4HEP_DDEVE_DISPLAYCONFIGURATION_H */

