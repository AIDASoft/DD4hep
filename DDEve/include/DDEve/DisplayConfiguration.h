//==========================================================================
//  AIDA Detector description implementation 
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
namespace dd4hep {

  // Forward declarations
  class Display;

  /// Generic display configuration structure for DDEve
  /*
   * The data content is filled from the XML configuration file
   * and then used when building the application interface
   *
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

    /// Default base class for all configurations
    struct Defaults {
      char  load_geo;
      char  show_evt;
      short default_pad;
      int   color;
      float alpha;
    };

    /// Configuration class for 3D calorimeter display
    struct Calo3D : public Defaults  {
      float rmin, dz, threshold, towerH, emax;
    };

    /// Configuration class for 3D calorimeter data display
    struct Calodata : public Defaults {
      float rmin, dz, threshold, towerH, emax;
      float eta_min, eta_max;
      float phi_min, phi_max;
      short n_eta;
      short n_phi;
      int spare;
    };

    /// Generic panel configuration 
    struct Panel : public Defaults {
    };

    /// Configuration class for hit data display
    struct Hits : public Defaults {
      float size;   // Marker size
      float width;
      float threshold;
      float towerH;
      float emax;   // Max energy deposit displayed
      int   type;   // Marker type
    };

    /// Container with full display configuration
    class Config  {
    public:
      /// Union to store specific data. Discriminator is Config::type
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

    /// View configuration
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

    /// Container with view configurations
    ViewConfigurations views;
    /// Container with calorimeter data configurations
    Configurations     calodata;
    /// Container for data collection configurations
    Configurations     collections;
  public:
    /// Initializing constructor
    DisplayConfiguration(Display* eve);
    /// Default destructor
    virtual ~DisplayConfiguration();
    /// Root implementation macro
    ClassDef(DisplayConfiguration,0);
  };
}        /* End namespace dd4hep                */
#endif   /* DD4HEP_DDEVE_DISPLAYCONFIGURATION_H */

