// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
#ifndef DD4HEP_DDEVE_DISPLAYCONFIGURATION_H
#define DD4HEP_DDEVE_DISPLAYCONFIGURATION_H

// Framework include files
#include "TClass.h"

// C/C++ include files
#include <string>
#include <list>
#include <map>


/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  class Display;

  /** @class DisplayConfiguration  DisplayConfiguration.h DDEve/DisplayConfiguration.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class  DisplayConfiguration   {
  protected:
    Display* m_display;
  public:
    enum { CALODATA=1<<1, DETELEMENT=1<<2, VIEW=1<<3, PANEL=1<<4 };
    struct Defaults {
      char load_geo;
      char show_evt;
      short color;
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
    class Config  {
    public:
      union Values  {
	double vals[20];
	Defaults defaults;
	Calo3D calo3d;
	Calodata calodata;
	Panel pane;
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
    Configurations calodata;
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

