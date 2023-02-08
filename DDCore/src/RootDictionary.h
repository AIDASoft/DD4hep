//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  RootDictionary.h
//
//
//  M.Frank
//
//==========================================================================
#ifndef DDCORE_SRC_ROOTDICTIONARY_H
#define DDCORE_SRC_ROOTDICTIONARY_H

// Framework include files
#include "Evaluator/Evaluator.h"
#include "DD4hep/DD4hepRootPersistency.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Grammar.h"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/ConditionsInterna.h"
#include "DD4hep/detail/AlignmentsInterna.h"
#include "DD4hep/detail/VolumeManagerInterna.h"

#include "DD4hep/World.h"
#include "DD4hep/DD4hepUI.h"
#include "DD4hep/Callback.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/FieldTypes.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/DetectorData.h"
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/ComponentProperties.h"
#include "DD4hep/DetectorImp.h"

// C/C++ include files
#include <vector>
#include <map>
 #include <string>

#include "TRint.h"

namespace dd4hep {
  namespace cond {}
  namespace align {}
  namespace detail {}
  void run_interpreter(const std::string& name)  {
    std::pair<int, char**> a(0,0);
    TRint app(name.c_str(), &a.first, a.second);
    app.Run();
  }

  tools::Evaluator& evaluator();
  tools::Evaluator& g4Evaluator();

  std::size_t always (const std::string& src, const std::string& msg) { return printout(ALWAYS,  src, msg); }
  std::size_t verbose(const std::string& src, const std::string& msg) { return printout(VERBOSE, src, msg); }
  std::size_t debug  (const std::string& src, const std::string& msg) { return printout(DEBUG,   src, msg); }
  std::size_t info   (const std::string& src, const std::string& msg) { return printout(INFO,    src, msg); }
  std::size_t warning(const std::string& src, const std::string& msg) { return printout(WARNING, src, msg); }
  std::size_t error  (const std::string& src, const std::string& msg) { return printout(ERROR,   src, msg); }
  std::size_t fatal  (const std::string& src, const std::string& msg) { return printout(FATAL,   src, msg); }
  void        exception(const std::string& src, const std::string& msg) { except(src, "%s", msg.c_str()); }

  namespace detail {
    /// Helper to invoke the ROOT interpreter
    struct interp  {
    public:
      interp() = default;
      virtual ~interp() = default;
      static void run(const std::string& name)  {
	std::pair<int, char**> a(0,0);
	TRint app(name.c_str(), &a.first, a.second);
	app.Run();
      }
    };
    //// Helper to access the evaluator instances
    struct eval  {
    public:
      eval() = default;
      virtual ~eval() = default;
      static dd4hep::tools::Evaluator& instance()     {
	return dd4hep::evaluator();
      }
      static dd4hep::tools::Evaluator& g4instance()   {
	return dd4hep::g4Evaluator();
      }
    };
  }
  class dd4hep_units  {
  public:
    static constexpr double millimeter       = dd4hep::mm;
    static constexpr double millimeter2      = dd4hep::mm2;
    static constexpr double millimeter3      = dd4hep::mm3;
    static constexpr double centimeter       = dd4hep::centimeter;
    static constexpr double centimeter2      = dd4hep::centimeter2;
    static constexpr double centimeter3      = dd4hep::centimeter3;

    static constexpr double meter            = dd4hep::meter;
    static constexpr double meter2           = dd4hep::meter2;
    static constexpr double meter3           = dd4hep::meter3;

    static constexpr double kilometer        = dd4hep::kilometer;
    static constexpr double kilometer2       = dd4hep::kilometer2;
    static constexpr double kilometer3       = dd4hep::kilometer3;


    static constexpr double parsec           = dd4hep::parsec;

    static constexpr double micrometer       = dd4hep::micrometer;
    static constexpr double nanometer        = dd4hep::nanometer;
    static constexpr double angstrom         = dd4hep::angstrom;
    static constexpr double fermi            = dd4hep::fermi;

    static constexpr double barn             = dd4hep::barn;
    static constexpr double millibarn        = dd4hep::millibarn;
    static constexpr double microbarn        = dd4hep::microbarn;
    static constexpr double nanobarn         = dd4hep::nanobarn;
    static constexpr double picobarn         = dd4hep::picobarn;

    // symbols
    static constexpr double nm               = dd4hep::nm;
    static constexpr double um               = dd4hep::um;

    static constexpr double mm               = dd4hep::mm;
    static constexpr double mm2              = dd4hep::mm2;
    static constexpr double mm3              = dd4hep::mm3;

    static constexpr double cm               = dd4hep::cm;
    static constexpr double cm2              = dd4hep::cm2;
    static constexpr double cm3              = dd4hep::cm3;

    static constexpr double m                = dd4hep::m;
    static constexpr double m2               = dd4hep::m2;
    static constexpr double m3               = dd4hep::m3;

    static constexpr double km               = dd4hep::km;
    static constexpr double km2              = dd4hep::km2;
    static constexpr double km3              = dd4hep::km3;

    static constexpr double pc               = dd4hep::pc;
    //
    // Angle
    //
    static constexpr double radian           = dd4hep::radian;
    static constexpr double milliradian      = dd4hep::milliradian;
    static constexpr double degree           = dd4hep::degree;
    static constexpr double steradian        = dd4hep::degree;

    // symbols
    static constexpr double rad              = dd4hep::rad;
    static constexpr double mrad             = dd4hep::mrad;
    static constexpr double sr               = dd4hep::sr;
    static constexpr double deg              = dd4hep::deg;

    //
    // Time [T]
    //
    static constexpr double picosecond       = dd4hep::picosecond;
    static constexpr double nanosecond       = dd4hep::nanosecond;
    static constexpr double microsecond      = dd4hep::microsecond;
    static constexpr double millisecond      = dd4hep::millisecond;
    static constexpr double second           = dd4hep::second;

    static constexpr double hertz            = dd4hep::hertz;
    static constexpr double kilohertz        = dd4hep::kilohertz;
    static constexpr double megahertz        = dd4hep::megahertz;
    // symbols
    static constexpr double ns               = dd4hep::ns;
    static constexpr double s                = dd4hep::s;
    static constexpr double ms               = dd4hep::ms;
    //
    // Electric charge [Q]
    //
    static constexpr double eplus            = dd4hep::eplus;
    static constexpr double e_SI             = dd4hep::e_SI;
    static constexpr double coulomb          = dd4hep::coulomb;
    //
    // Energy [E]
    //
    static constexpr double electronvolt     = dd4hep::electronvolt;
    static constexpr double kiloelectronvolt = dd4hep::kiloelectronvolt;
    static constexpr double megaelectronvolt = dd4hep::megaelectronvolt;
    static constexpr double gigaelectronvolt = dd4hep::gigaelectronvolt;
    static constexpr double teraelectronvolt = dd4hep::teraelectronvolt;
    static constexpr double petaelectronvolt = dd4hep::petaelectronvolt;

    static constexpr double joule            = dd4hep::joule;
    static constexpr double kilojoule        = dd4hep::kilojoule;

    // symbols
    static constexpr double eV               = dd4hep::eV;
    static constexpr double keV              = dd4hep::keV;
    static constexpr double MeV              = dd4hep::MeV;
    static constexpr double GeV              = dd4hep::GeV;
    static constexpr double TeV              = dd4hep::TeV;
    static constexpr double PeV              = dd4hep::PeV;
    //
    // Mass [E][T^2][L^-2]
    //
    static constexpr double kilogram         = dd4hep::kilogram;
    static constexpr double gram             = dd4hep::gram;
    static constexpr double milligram        = dd4hep::milligram;
    // symbols
    static constexpr double kg               = dd4hep::kg;
    static constexpr double g                = dd4hep::g;
    static constexpr double mg               = dd4hep::mg;
    //
    // Power [E][T^-1]
    //
    static constexpr double watt             = dd4hep::watt;
    static constexpr double kilowatt         = dd4hep::kilowatt;
    static constexpr double megawatt         = dd4hep::megawatt;
    //
    // Force [E][L^-1]
    //
    static constexpr double newton           = dd4hep::newton;
    //
    // Pressure [E][L^-3]
    //
    static constexpr double hep_pascal       = dd4hep::hep_pascal;
    static constexpr double bar              = dd4hep::bar;
    static constexpr double atmosphere       = dd4hep::atmosphere;
    //
    // Electric current [Q][T^-1]
    //
    static constexpr double ampere           = dd4hep::ampere;
    static constexpr double milliampere      = dd4hep::milliampere;
    static constexpr double microampere      = dd4hep::microampere;
    static constexpr double nanoampere       = dd4hep::nanoampere;
    //
    // Electric potential [E][Q^-1]
    //
    static constexpr double megavolt         = dd4hep::megavolt;
    static constexpr double kilovolt         = dd4hep::kilovolt;
    static constexpr double volt             = dd4hep::volt;
    //
    // Electric resistance [E][T][Q^-2]
    //
    static constexpr double ohm              = dd4hep::ohm;
    //
    // Electric capacitance [Q^2][E^-1]
    //
    static constexpr double farad            = dd4hep::farad;
    static constexpr double millifarad       = dd4hep::millifarad;
    static constexpr double microfarad       = dd4hep::microfarad;
    static constexpr double nanofarad        = dd4hep::nanofarad;
    static constexpr double picofarad        = dd4hep::picofarad;
    //
    // Magnetic Flux [T][E][Q^-1]
    //
    static constexpr double weber            = dd4hep::weber;
    //
    // Magnetic Field [T][E][Q^-1][L^-2]
    //
    static constexpr double tesla            = dd4hep::tesla;

    static constexpr double gauss            = dd4hep::gauss;
    static constexpr double kilogauss        = dd4hep::kilogauss;
    //
    // Inductance [T^2][E][Q^-2]
    //
    static constexpr double henry            = dd4hep::henry;
    //
    // Temperature
    //
    static constexpr double kelvin           = dd4hep::kelvin;
    //
    // Amount of substance
    //
    static constexpr double mole             = dd4hep::mole;
    //
    // Activity [T^-1]
    //
    static constexpr double becquerel        = dd4hep::becquerel;
    static constexpr double curie            = dd4hep::curie;
    //
    // Absorbed dose [L^2][T^-2]
    //
    static constexpr double gray             = dd4hep::gray;
    static constexpr double kilogray         = dd4hep::kilogray;
    static constexpr double milligray        = dd4hep::milligray;
    static constexpr double microgray        = dd4hep::microgray;
    //
    // Luminous intensity [I]
    //
    static constexpr double candela          = dd4hep::candela;
    //
    // Luminous flux [I]
    //
    static constexpr double lumen            = dd4hep::lumen;
    //
    // Illuminance [I][L^-2]
    //
    static constexpr double lux              = dd4hep::lux;
    //
    // Miscellaneous
    //
    static constexpr double perCent          = dd4hep::perCent;
    static constexpr double perThousand      = dd4hep::perThousand;
    static constexpr double perMillion       = dd4hep::perMillion;

    static constexpr double pi               = dd4hep::pi;
    static constexpr double twopi            = dd4hep::twopi;
    static constexpr double halfpi           = dd4hep::halfpi;
    static constexpr double pi2              = dd4hep::pi2;
    //
    //
    //
    static constexpr double Avogadro         = dd4hep::Avogadro;
    //
    // c   = 299.792458 mm/ns
    // c^2 = 898.7404 (mm/ns)^2
    //
    static constexpr double c_light          = dd4hep::c_light;
      static constexpr double c_squared      = dd4hep::c_squared;
    //
    // h     = 4.13566e-12 MeV*ns
    // hbar  = 6.58212e-13 MeV*ns
    // hbarc = 197.32705e-12 MeV*mm
    //
    static constexpr double h_Planck         = dd4hep::h_Planck;
    static constexpr double hbar_Planck      = dd4hep::hbar_Planck;
    static constexpr double hbarc            = dd4hep::hbarc;
    static constexpr double hbarc_squared    = dd4hep::hbarc_squared;
    //
    //
    //
    static constexpr double electron_charge  = dd4hep::electron_charge;
    static constexpr double e_squared        = dd4hep::e_squared;
    //
    // amu_c2 - atomic equivalent mass unit
    //        - AKA, unified atomic mass unit (u)
    // amu    - atomic mass unit
    //
    static constexpr double electron_mass_c2 = dd4hep::electron_mass_c2;
    static constexpr double proton_mass_c2   = dd4hep::proton_mass_c2;
    static constexpr double neutron_mass_c2  = dd4hep::neutron_mass_c2;
    static constexpr double amu_c2           = dd4hep::amu_c2;
    static constexpr double amu              = dd4hep::amu;
    //
    // permeability of free space mu0    = 2.01334e-16 Mev*(ns*eplus)^2/mm
    // permittivity of free space epsil0 = 5.52636e+10 eplus^2/(MeV*mm)
    //
    static constexpr double mu0              = dd4hep::mu0;
    static constexpr double epsilon0         = dd4hep::epsilon0;
    //
    // electromagnetic coupling = 1.43996e-12 MeV*mm/(eplus^2)
    //
    static constexpr double elm_coupling     = dd4hep::elm_coupling;
    static constexpr double fine_structure_const = dd4hep::fine_structure_const;
    static constexpr double classic_electr_radius = dd4hep::classic_electr_radius;
    static constexpr double electron_Compton_length = dd4hep::electron_Compton_length;
    static constexpr double Bohr_radius      = dd4hep::Bohr_radius;
    static constexpr double alpha_rcl2       = dd4hep::alpha_rcl2;
    static constexpr double twopi_mc2_rcl2   = dd4hep::twopi_mc2_rcl2;
    //
    static constexpr double k_Boltzmann      = dd4hep::k_Boltzmann;
    //
    // IUPAC standard temperature and pressure (STP)
    // STP uses 273.15 K (0 °C, 32 °F) and (since 1982) 1 bar (100 kPa) and not 1 atm!
    static constexpr double Temperature_STP  = dd4hep::Temperature_STP;
    static constexpr double Pressure_STP     = dd4hep::Pressure_STP;
    //
    // NTP uses the NIST convention: 20 °C (293.15 K, 68 °F), 1 atm (14.696 psi, 101.325 kPa)
    static constexpr double Temperature_NTP  = dd4hep::Temperature_NTP;
    static constexpr double Pressure_NTP     = dd4hep::Pressure_NTP;
    //
    static constexpr double kGasThreshold    = dd4hep::kGasThreshold;
    static constexpr double universe_mean_density = dd4hep::universe_mean_density;

    dd4hep_units() = default;
    ~dd4hep_units() = default;
  };
}

namespace dd4hep   {   namespace Parsers   {
    int parse(dd4hep::AlignmentData&, const std::string&);
    int parse(dd4hep::detail::AlignmentObject&, const std::string&);
  }}

// -------------------------------------------------------------------------
// Regular dd4hep dictionaries
// -------------------------------------------------------------------------
#ifdef DD4HEP_DICTIONARY_MODE
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace dd4hep;

#pragma link C++ namespace dd4hep::tools;
#pragma link C++ class dd4hep::tools::Evaluator;
#pragma link C++ function dd4hep::evaluator;
#pragma link C++ function dd4hep::g4Evaluator;

#pragma link C++ namespace dd4hep::detail;
#pragma link C++ namespace dd4hep::cond;
#pragma link C++ namespace dd4hep::align;
#pragma link C++ namespace dd4hep::DDSegmentation;

#pragma link C++ enum dd4hep::PrintLevel;
#pragma link C++ function dd4hep::always;
#pragma link C++ function dd4hep::verbose;
#pragma link C++ function dd4hep::debug;
#pragma link C++ function dd4hep::info;
#pragma link C++ function dd4hep::warning;
#pragma link C++ function dd4hep::error;
#pragma link C++ function dd4hep::fatal;
#pragma link C++ function dd4hep::except;
#pragma link C++ function dd4hep::printout;
#pragma link C++ function dd4hep::exception;

// System of units:
// Length [L]
#pragma link C++ global dd4hep::fermi;
#pragma link C++ global dd4hep::angstrom;
#pragma link C++ global dd4hep::nanometer;
#pragma link C++ global dd4hep::micrometer;
#pragma link C++ global dd4hep::millimeter;
#pragma link C++ global dd4hep::millimeter2;
#pragma link C++ global dd4hep::millimeter3;
#pragma link C++ global dd4hep::centimeter;
#pragma link C++ global dd4hep::centimeter2;
#pragma link C++ global dd4hep::centimeter3;
#pragma link C++ global dd4hep::meter;
#pragma link C++ global dd4hep::meter2;
#pragma link C++ global dd4hep::meter3;
#pragma link C++ global dd4hep::kilometer;
#pragma link C++ global dd4hep::kilometer2;
#pragma link C++ global dd4hep::kilometer3;
#pragma link C++ global dd4hep::parsec;
// Area/cross-section
#pragma link C++ global dd4hep::barn;
#pragma link C++ global dd4hep::millibarn;
#pragma link C++ global dd4hep::microbarn;
#pragma link C++ global dd4hep::nanobarn;
#pragma link C++ global dd4hep::picobarn;
#pragma link C++ global dd4hep::radian;
#pragma link C++ global dd4hep::milliradian;
#pragma link C++ global dd4hep::degree;
#pragma link C++ global dd4hep::steradian;

#pragma link C++ global dd4hep::nm;
#pragma link C++ global dd4hep::um;
#pragma link C++ global dd4hep::mm;
#pragma link C++ global dd4hep::mm2;
#pragma link C++ global dd4hep::mm3;
#pragma link C++ global dd4hep::cm;
#pragma link C++ global dd4hep::cm2;
#pragma link C++ global dd4hep::cm3;
#pragma link C++ global dd4hep::m;
#pragma link C++ global dd4hep::m2;
#pragma link C++ global dd4hep::m3;
#pragma link C++ global dd4hep::km;
#pragma link C++ global dd4hep::km2;
#pragma link C++ global dd4hep::km3;
#pragma link C++ global dd4hep::m;
#pragma link C++ global dd4hep::m2;
#pragma link C++ global dd4hep::pc;
#pragma link C++ global dd4hep::rad;
#pragma link C++ global dd4hep::mrad;
#pragma link C++ global dd4hep::sr;
// Time [T]
#pragma link C++ global dd4hep::nanosecond;
#pragma link C++ global dd4hep::second;
#pragma link C++ global dd4hep::millisecond;
#pragma link C++ global dd4hep::microsecond;
#pragma link C++ global dd4hep::picosecond;
#pragma link C++ global dd4hep::hertz;
#pragma link C++ global dd4hep::kilohertz;
#pragma link C++ global dd4hep::megahertz;

#pragma link C++ global dd4hep::ns;
#pragma link C++ global dd4hep::s;
#pragma link C++ global dd4hep::ms;
// Electric charge [Q]
#pragma link C++ global dd4hep::eplus;
#pragma link C++ global dd4hep::e_SI;
#pragma link C++ global dd4hep::coulomb;
// Energy [E]
#pragma link C++ global dd4hep::joule;
#pragma link C++ global dd4hep::kilojoule;
#pragma link C++ global dd4hep::electronvolt;
#pragma link C++ global dd4hep::kiloelectronvolt;
#pragma link C++ global dd4hep::megaelectronvolt;
#pragma link C++ global dd4hep::gigaelectronvolt;
#pragma link C++ global dd4hep::teraelectronvolt;
#pragma link C++ global dd4hep::petaelectronvolt;

#pragma link C++ global dd4hep::eV;
#pragma link C++ global dd4hep::keV;
#pragma link C++ global dd4hep::MeV;
#pragma link C++ global dd4hep::GeV;
#pragma link C++ global dd4hep::TeV;
#pragma link C++ global dd4hep::PeV;
// Mass [E][T^2][L^-2]
#pragma link C++ global dd4hep::milligram;
#pragma link C++ global dd4hep::gram;
#pragma link C++ global dd4hep::kilogram;
#pragma link C++ global dd4hep::mg;
#pragma link C++ global dd4hep::g;
#pragma link C++ global dd4hep::kg;
// Power [E][T^-1]
#pragma link C++ global dd4hep::watt;
#pragma link C++ global dd4hep::kilowatt;
#pragma link C++ global dd4hep::megawatt;
// Force [E][L^-1]
#pragma link C++ global dd4hep::newton;
// Pressure [E][L^-3]
#pragma link C++ global dd4hep::hep_pascal;
#pragma link C++ global dd4hep::bar;
#pragma link C++ global dd4hep::atmosphere;
// Electric current [Q][T^-1]
#pragma link C++ global dd4hep::nanoampere;
#pragma link C++ global dd4hep::microampere;
#pragma link C++ global dd4hep::milliampere;
#pragma link C++ global dd4hep::ampere;
// Electric potential [E][Q^-1]
#pragma link C++ global dd4hep::volt;
#pragma link C++ global dd4hep::kilovolt;
#pragma link C++ global dd4hep::megavolt;
// Electric resistance [E][T][Q^-2]
#pragma link C++ global dd4hep::ohm;
// Electric capacitance [Q^2][E^-1]
#pragma link C++ global dd4hep::picofarad;
#pragma link C++ global dd4hep::nanofarad;
#pragma link C++ global dd4hep::microfarad;
#pragma link C++ global dd4hep::millifarad;
#pragma link C++ global dd4hep::farad;
// Magnetic Flux [T][E][Q^-1]
#pragma link C++ global dd4hep::weber;
// Magnetic Field [T][E][Q^-1][L^-2]
#pragma link C++ global dd4hep::tesla;
#pragma link C++ global dd4hep::gauss;
#pragma link C++ global dd4hep::kilogauss;
// Inductance [T^2][E][Q^-2]
#pragma link C++ global dd4hep::henry;
// Temperature
#pragma link C++ global dd4hep::kelvin;
// Amount of substance
#pragma link C++ global dd4hep::mole;
// Absorbed dose [L^2][T^-2]
#pragma link C++ global dd4hep::microgray;
#pragma link C++ global dd4hep::milligray;
#pragma link C++ global dd4hep::gray;
#pragma link C++ global dd4hep::kilogray;
// Luminous intensity [I]
#pragma link C++ global dd4hep::candela;
// Illuminance [I][L^-2]
#pragma link C++ global dd4hep::lux;
// Miscellaneous
#pragma link C++ global dd4hep::perCent;
#pragma link C++ global dd4hep::perThousand;
#pragma link C++ global dd4hep::perMillion;
// Physical Constants
#pragma link C++ global dd4hep::pi;
#pragma link C++ global dd4hep::twopi;
#pragma link C++ global dd4hep::halfpi;
#pragma link C++ global dd4hep::pi2;
#pragma link C++ global dd4hep::Avogadro;
#pragma link C++ global dd4hep::c_light;
#pragma link C++ global dd4hep::c_squared;
#pragma link C++ global dd4hep::h_Planck;
#pragma link C++ global dd4hep::hbar_Planck;
#pragma link C++ global dd4hep::hbarc;
#pragma link C++ global dd4hep::hbarc_squared;
#pragma link C++ global dd4hep::electron_charge;
#pragma link C++ global dd4hep::e_squared;
#pragma link C++ global dd4hep::electron_mass_c2;
#pragma link C++ global dd4hep::proton_mass_c2;
#pragma link C++ global dd4hep::neutron_mass_c2;
#pragma link C++ global dd4hep::amu_c2;
#pragma link C++ global dd4hep::amu;
#pragma link C++ global dd4hep::mu0;
#pragma link C++ global dd4hep::epsilon0;
#pragma link C++ global dd4hep::elm_coupling;
#pragma link C++ global dd4hep::fine_structure_const;
#pragma link C++ global dd4hep::classic_electr_radius;
#pragma link C++ global dd4hep::electron_Compton_length;
#pragma link C++ global dd4hep::Bohr_radius;
#pragma link C++ global dd4hep::alpha_rcl2;
#pragma link C++ global dd4hep::twopi_mc2_rcl2;
#pragma link C++ global dd4hep::k_Boltzmann;
#pragma link C++ global dd4hep::Temperature_STP;
#pragma link C++ global dd4hep::Pressure_STP;
#pragma link C++ global dd4hep::Temperature_NTP;
#pragma link C++ global dd4hep::Pressure_NTP;
#pragma link C++ global dd4hep::kGasThreshold;
#pragma link C++ global dd4hep::universe_mean_density;

#ifndef __ROOTCLING__
template std::pair<unsigned int, std::string>;
template class dd4hep::Handle<dd4hep::NamedObject>;
template class std::pair< string, dd4hep::Handle<dd4hep::NamedObject> >;
template class std::map< string, dd4hep::Handle<dd4hep::NamedObject> >;
template class std::pair<dd4hep::Callback,unsigned long>;
#endif

#pragma link C++ class DD4hepRootPersistency+;
#pragma link C++ class DD4hepRootCheck+;
#pragma link C++ class dd4hep::dd4hep_units;

#pragma link C++ class std::pair<unsigned int,std::string>+;
//#pragma link C++ class dd4hep::Callback+;
#pragma link C++ class std::pair<dd4hep::Callback,unsigned long>+;
#pragma link C++ class dd4hep::NamedObject+;
#pragma link C++ class dd4hep::Ref_t+;
#pragma link C++ class dd4hep::Handle<dd4hep::NamedObject>+;
#pragma link C++ class std::pair<std::string, dd4hep::Handle<dd4hep::NamedObject> >+;
#pragma link C++ class std::map<std::string, dd4hep::Handle<dd4hep::NamedObject> >+;
#pragma link C++ class std::map<std::string, dd4hep::Handle<dd4hep::NamedObject> >::iterator;
#pragma link C++ class std::map<std::string, dd4hep::Handle<dd4hep::NamedObject> >::const_iterator;
#pragma link C++ class dd4hep::detail::DD4hepUI;

#ifdef R__MACOSX
// We only need these declarations for the clang compiler
#pragma link C++ function operator==( const std::map<std::string, dd4hep::Handle<dd4hep::NamedObject> >::iterator&,const std::map<std::string, dd4hep::Handle<dd4hep::NamedObject> >::iterator& );
#pragma link C++ function operator!=( const std::map<std::string, dd4hep::Handle<dd4hep::NamedObject> >::iterator&,const std::map<std::string, dd4hep::Handle<dd4hep::NamedObject> >::iterator& );
#endif

#pragma link C++ class dd4hep::BasicGrammar+;
#pragma link C++ class dd4hep::ObjectExtensions+;
template class dd4hep::Handle<TNamed>;
#pragma link C++ class dd4hep::Handle<TNamed>+;

#pragma link C++ class dd4hep::IOV+;
#pragma link C++ class dd4hep::IOVType+;
#pragma link C++ class dd4hep::OpaqueData+;
#pragma link C++ class dd4hep::OpaqueDataBlock+;

#pragma link C++ class dd4hep::Detector+;
#pragma link C++ class dd4hep::DetectorImp+;
#pragma link C++ class dd4hep::DetectorLoad+;
#pragma link C++ class dd4hep::DetectorData+;
#pragma link C++ class dd4hep::DetectorData::ObjectHandleMap+;
#pragma link C++ class dd4hep::Detector::PropertyValues+;
#pragma link C++ class dd4hep::Detector::Properties+;
#pragma link C++ class std::pair<dd4hep::IDDescriptor,dd4hep::DDSegmentation::Segmentation*>+;
#pragma link C++ class std::map<dd4hep::Readout,pair<dd4hep::IDDescriptor,dd4hep::DDSegmentation::Segmentation*> >+;

// These below are the Namedobject instances to be generated ....
//#pragma link C++ class dd4hep::Detector::HandleMap+;
//#pragma link C++ class dd4hep::Detector::HandleMap::iterator;
//#pragma link C++ class dd4hep::Detector::HandleMap::const_iterator;
//#pragma link C++ class dd4hep::Detector::HandleMap::key_type; // == string
//#pragma link C++ class dd4hep::Detector::HandleMap::value_type+;

#pragma link C++ class dd4hep::VolumeManager+;
#pragma link C++ class dd4hep::detail::VolumeManagerObject+;
#pragma link C++ class dd4hep::VolumeManagerContext+;
#pragma link C++ class dd4hep::detail::VolumeManagerContextExtension+;
#pragma link C++ class dd4hep::Handle<dd4hep::detail::VolumeManagerObject>+;
#pragma link C++ class std::pair<Long64_t,dd4hep::VolumeManager>+;
#pragma link C++ class std::map<dd4hep::DetElement,dd4hep::VolumeManager>+;
#pragma link C++ class std::map<dd4hep::VolumeID,dd4hep::VolumeManager>+;
#pragma link C++ class std::map<dd4hep::VolumeID,dd4hep::VolumeManagerContext*>+;

#pragma link C++ class dd4hep::CartesianField+;
#pragma link C++ class dd4hep::CartesianField::Object+;
#pragma link C++ class dd4hep::Handle<dd4hep::CartesianField::Object>+;
#pragma link C++ class dd4hep::OverlayedField+;
#pragma link C++ class dd4hep::OverlayedField::Object+;
#pragma link C++ class dd4hep::Handle<dd4hep::OverlayedField::Object>+;

// FieldTypes.h
#pragma link C++ class dd4hep::ConstantField+;
#pragma link C++ class dd4hep::Handle<dd4hep::ConstantField>+;
#pragma link C++ class dd4hep::SolenoidField+;
#pragma link C++ class dd4hep::Handle<dd4hep::SolenoidField>+;
#pragma link C++ class dd4hep::DipoleField+;
#pragma link C++ class dd4hep::Handle<dd4hep::DipoleField>+;

#pragma link C++ class dd4hep::IDDescriptor+;
#pragma link C++ class dd4hep::IDDescriptorObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::IDDescriptorObject>+;

// Objects.h
#pragma link C++ class dd4hep::Author+;
#pragma link C++ class std::vector<dd4hep::Author>+;

#pragma link C++ class dd4hep::Header+;
#pragma link C++ class dd4hep::HeaderObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::HeaderObject>+;

#pragma link C++ class dd4hep::Constant+;
#pragma link C++ class dd4hep::ConstantObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::ConstantObject>+;
#pragma link C++ class std::vector<dd4hep::Constant>+;

#pragma link C++ class dd4hep::Atom+;
#pragma link C++ class std::vector<dd4hep::Atom>+;
#pragma link C++ class dd4hep::Handle<TGeoElement>+;

#pragma link C++ class dd4hep::Material+;
#pragma link C++ class std::vector<dd4hep::Material>+;
#pragma link C++ class dd4hep::Handle<TGeoMedium>+;

#pragma link C++ class dd4hep::VisAttr+;
#pragma link C++ class std::vector<dd4hep::VisAttr>+;
#pragma link C++ class dd4hep::VisAttrObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::VisAttrObject>+;

#pragma link C++ class dd4hep::Limit+;
#pragma link C++ class std::set<dd4hep::Limit>+;
#pragma link C++ class std::vector<dd4hep::Limit>+;
#pragma link C++ class dd4hep::LimitSet+;
#pragma link C++ class std::vector<dd4hep::LimitSet>+;
#pragma link C++ class dd4hep::LimitSetObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::LimitSetObject>+;
#pragma link C++ class dd4hep::Region+;
#pragma link C++ class dd4hep::RegionObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::RegionObject>+;
#pragma link C++ class std::vector<dd4hep::Region>+;

// Readout.h
#pragma link C++ class std::vector<pair<size_t,string> >+;
#pragma link C++ class dd4hep::Segmentation+;
#pragma link C++ class dd4hep::SegmentationObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::SegmentationObject>+;
#pragma link C++ class dd4hep::HitCollection+;
#pragma link C++ class dd4hep::Readout+;
#pragma link C++ class dd4hep::HitCollection+;
#pragma link C++ class dd4hep::ReadoutObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::ReadoutObject>+;
#pragma link C++ class std::vector<dd4hep::HitCollection>+;
#pragma link C++ class std::vector<dd4hep::Readout>+;
#pragma link C++ class std::vector<dd4hep::HitCollection*>+;
#pragma link C++ class std::vector<const dd4hep::HitCollection*>+;
#pragma link C++ class std::vector<dd4hep::IDDescriptor>+;

// Alignment stuff
#pragma link C++ class dd4hep::Delta+;
#pragma link C++ class dd4hep::Alignment+;
#pragma link C++ class dd4hep::AlignmentData+;
#pragma link C++ class dd4hep::Handle<dd4hep::AlignmentData>+;
//#pragma link C++ class dd4hep::Grammar<dd4hep::AlignmentData>+;

#pragma link C++ class dd4hep::AlignmentCondition+;
#pragma link C++ class dd4hep::detail::AlignmentObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::detail::AlignmentObject>+;
#pragma link C++ class std::pair<dd4hep::DetElement,dd4hep::AlignmentCondition>+;
//#pragma link C++ class dd4hep::Grammar<dd4hep::detail::AlignmentObject>+;

#pragma link C++ class dd4hep::align::GlobalAlignment+;
#pragma link C++ class dd4hep::Handle<TGeoPhysicalNode>+;

// Conditions stuff
#pragma link C++ class dd4hep::Condition+;
#pragma link C++ class std::vector<dd4hep::Condition>+;
#pragma link C++ class dd4hep::ConditionKey+;
#pragma link C++ class std::vector<dd4hep::ConditionKey>+;
#pragma link C++ class dd4hep::detail::ConditionObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::detail::ConditionObject>+;
#pragma link C++ class dd4hep::OpaqueData+;
#pragma link C++ class dd4hep::OpaqueDataBlock+;

// DetElement.h
#pragma link C++ class dd4hep::World+;
#pragma link C++ class dd4hep::WorldObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::WorldObject>+;
#pragma link C++ class dd4hep::DetElement+;
#pragma link C++ class dd4hep::DetElement::Processor+;
#pragma link C++ class dd4hep::DetElementObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::DetElementObject>+;
#pragma link C++ class std::vector<dd4hep::DetElement>+;
#pragma link C++ class std::pair<std::string,dd4hep::DetElement>+;
#pragma link C++ class std::map<std::string,dd4hep::DetElement>+;
#pragma link C++ class std::map<std::string,dd4hep::DetElement>::iterator;
#pragma link C++ class std::map<std::string,dd4hep::DetElement>::const_iterator;

#pragma link C++ class dd4hep::DetectorProcessor+;
#pragma link C++ class dd4hep::DetectorScanner+;

#pragma link C++ class std::pair<dd4hep::DetElement,dd4hep::VolumeManager>+;

#ifdef R__MACOSX
// We only need these declarations for the clang compiler
#pragma link C++ function operator==( const std::map<std::string, dd4hep::DetElement >::iterator&,const std::map<std::string, dd4hep::DetElement >::iterator& );
#pragma link C++ function operator!=( const std::map<std::string, dd4hep::DetElement >::iterator&,const std::map<std::string, dd4hep::DetElement >::iterator& );
#endif

#pragma link C++ class dd4hep::SensitiveDetector+;
#pragma link C++ class dd4hep::SensitiveDetectorObject+;
#pragma link C++ class dd4hep::Handle<dd4hep::SensitiveDetectorObject>+;
#pragma link C++ class vector<dd4hep::SensitiveDetector>+;

#pragma link C++ class std::pair<std::string, std::string>+;
#pragma link C++ class std::map<std::string, std::string>+;
#pragma link C++ class std::map<std::string, std::string>::iterator;
#pragma link C++ class std::map<std::string, std::string>::const_iterator;

#ifdef R__MACOSX
// We only need these declarations for the clang compiler
#pragma link C++ function operator==( const std::map<std::string, std::string>::iterator&, const std::map<std::string, std::string>::iterator& );
#pragma link C++ function operator!=( const std::map<std::string, std::string>::iterator&, const std::map<std::string, std::string>::iterator& );
#endif

#pragma link C++ class std::pair<std::string, std::map<std::string, std::string> >+;
#pragma link C++ class std::map<std::string, std::map<std::string, std::string> >+;
#pragma link C++ class std::map<std::string, std::map<std::string,std::string>>::iterator;
#pragma link C++ class std::map<std::string, std::map<std::string,std::string>>::const_iterator;

#ifdef R__MACOSX
// We only need these declarations for the clang compiler
#pragma link C++ function operator==( const std::map<std::string, std::map<std::string,std::string>>::iterator&, const std::map<std::string, std::map<std::string,std::string>>::iterator& );
#pragma link C++ function operator!=( const std::map<std::string, std::map<std::string,std::string>>::iterator&, const std::map<std::string, std::map<std::string,std::string>>::iterator& );
#endif

#pragma link C++ class dd4hep::Detector+;

#pragma link C++ class dd4hep::detail::interp;
#pragma link C++ class dd4hep::detail::eval;

#pragma link C++ function dd4hep::run_interpreter(const std::string& name);
#pragma link C++ function dd4hep::_toDictionary(const std::string&, const std::string&);
#pragma link C++ function dd4hep::toStringSolid(const TGeoShape*, int);
#pragma link C++ function dd4hep::toStringMesh(const TGeoShape*, int);
#pragma link C++ function dd4hep::toStringMesh(dd4hep::PlacedVolume, int);

#include "DD4hep/ConditionsData.h"
#pragma link C++ class dd4hep::cond::ClientData+;
#pragma link C++ class dd4hep::cond::AbstractMap+;
#pragma link C++ class dd4hep::cond::AbstractMap::Params+;

#endif  // __CINT__
#endif // DDCORE_SRC_ROOTDICTIONARY_H
