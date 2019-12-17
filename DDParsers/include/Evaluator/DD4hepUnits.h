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
// System of Units for dd4hep - similar to Geant3 and TGeo/ROOT
//
// Taken from CLHEP with the following basic translations:
//
// Geant4                                   dd4hep
// ----------------------------------------------------------------
// millimeter         mm=1,cm=10            centimeter  mm=0.1,cm=1
// nanosecond         s=1e9                 second      ns=1e-9,s=1
// Mega electron Volt GeV=1e3               GeV         GeV=1
// radian             rad=1                 rad = 1  //NB: different from TGeo
//
//
#ifndef DD4HEP_TGEOUNITS_H
#define DD4HEP_TGEOUNITS_H

#include "RVersion.h"

#ifndef HAVE_GEANT4_UNITS
// #define HAVE_GEANT4_UNITS
#endif

// We use the ROOT system units if they are avalible (FAILS SOME TESTS FOR NOW)
#if 0
/// ROOT_VERSION_CODE >= ROOT_VERSION(6,12,0)
#include "TGeoSystemOfUnits.h"
/// Main dd4hep namespace. We must import here the ROOT TGeo units
namespace dd4hep {
  using namespace TGeoUnit;
}
#else

/// Main dd4hep namespace. We must import here the ROOT TGeo units
namespace dd4hep {

  //namespace units  {
    //
    // Length [L]
    //
#ifdef HAVE_GEANT4_UNITS
    static constexpr double millimeter = 1.0;
#else
    static constexpr double millimeter = 0.1;
#endif
    static constexpr double millimeter2 = millimeter * millimeter;
    static constexpr double millimeter3 = millimeter * millimeter * millimeter;

    static constexpr double centimeter = 10. * millimeter;
    static constexpr double centimeter2 = centimeter * centimeter;
    static constexpr double centimeter3 = centimeter * centimeter * centimeter;

    static constexpr double meter = 1000. * millimeter;
    static constexpr double meter2 = meter * meter;
    static constexpr double meter3 = meter * meter * meter;

    static constexpr double kilometer = 1000. * meter;
    static constexpr double kilometer2 = kilometer * kilometer;
    static constexpr double kilometer3 = kilometer * kilometer * kilometer;

    static constexpr double parsec = 3.0856775807e+16 * meter;

    static constexpr double micrometer = 1.e-6 * meter;
    static constexpr double nanometer = 1.e-9 * meter;
    static constexpr double angstrom = 1.e-10 * meter;
    static constexpr double fermi = 1.e-15 * meter;

    static constexpr double barn = 1.e-28 * meter2;
    static constexpr double millibarn = 1.e-3 * barn;
    static constexpr double microbarn = 1.e-6 * barn;
    static constexpr double nanobarn = 1.e-9 * barn;
    static constexpr double picobarn = 1.e-12 * barn;

    // symbols
    static constexpr double nm = nanometer;
    static constexpr double um = micrometer;

    static constexpr double mm = millimeter;
    static constexpr double mm2 = millimeter2;
    static constexpr double mm3 = millimeter3;

    static constexpr double cm = centimeter;
    static constexpr double cm2 = centimeter2;
    static constexpr double cm3 = centimeter3;

    static constexpr double m = meter;
    static constexpr double m2 = meter2;
    static constexpr double m3 = meter3;

    static constexpr double km = kilometer;
    static constexpr double km2 = kilometer2;
    static constexpr double km3 = kilometer3;

    static constexpr double pc = parsec;

    //
    // Angle
    //
    // static constexpr double radian = 180. / 3.14159265358979323846;   // => degree=1
    // static constexpr double milliradian = 1.e-3 * radian;
    // static constexpr double degree = 1.;   //= (3.14159265358979323846/180.0)*radian;

    //fg: use radians as default unit as this is needed for all math functions
    //    and everywhere else, except in TGeo shapes -> this is taken care of in shape Handles ....
    static constexpr double radian = 1. ;
    static constexpr double milliradian = 1.e-3 * radian;
    static constexpr double degree = (3.14159265358979323846/180.0)*radian;

    static constexpr double steradian = 1.;

    // symbols
    static constexpr double rad = radian;
    static constexpr double mrad = milliradian;
    static constexpr double sr = steradian;
    static constexpr double deg = degree;

    //
    // Time [T]
    //
#ifdef HAVE_GEANT4_UNITS
    static constexpr double nanosecond = 1.0;
#else
    static constexpr double nanosecond = 1.e-9;
#endif
    static constexpr double second = 1.e+9 * nanosecond;
    static constexpr double millisecond = 1.e-3 * second;
    static constexpr double microsecond = 1.e-6 * second;
    static constexpr double picosecond = 1.e-12 * second;

    static constexpr double hertz = 1. / second;
    static constexpr double kilohertz = 1.e+3 * hertz;
    static constexpr double megahertz = 1.e+6 * hertz;

    // symbols
    static constexpr double ns = nanosecond;
    static constexpr double s = second;
    static constexpr double ms = millisecond;

    //
    // Electric charge [Q]
    //
    static constexpr double eplus = 1.;   // positron charge
    static constexpr double e_SI = 1.602176487e-19;   // positron charge in coulomb
    static constexpr double coulomb = eplus / e_SI;   // coulomb = 6.24150 e+18 * eplus

    //
    // Energy [E]
    //
#ifdef HAVE_GEANT4_UNITS
    static constexpr double megaelectronvolt = 1.0;
#else
    static constexpr double megaelectronvolt = 1.e-3;
#endif
    static constexpr double electronvolt = 1.e-6 * megaelectronvolt;
    static constexpr double kiloelectronvolt = 1.e-3 * megaelectronvolt;
    static constexpr double gigaelectronvolt = 1.e+3 * megaelectronvolt;
    static constexpr double teraelectronvolt = 1.e+6 * megaelectronvolt;
    static constexpr double petaelectronvolt = 1.e+9 * megaelectronvolt;

    static constexpr double joule = electronvolt / e_SI;   // joule = 6.24150 e+12 * MeV

    // symbols
    static constexpr double MeV = megaelectronvolt;
    static constexpr double eV = electronvolt;
    static constexpr double keV = kiloelectronvolt;
    static constexpr double GeV = gigaelectronvolt;
    static constexpr double TeV = teraelectronvolt;
    static constexpr double PeV = petaelectronvolt;

    //
    // Mass [E][T^2][L^-2]
    //
    static constexpr double kilogram = joule * second * second / (meter * meter);
    static constexpr double gram = 1.e-3 * kilogram;
    static constexpr double milligram = 1.e-3 * gram;

    // symbols
    static constexpr double kg = kilogram;
    static constexpr double g = gram;
    static constexpr double mg = milligram;

    //
    // Power [E][T^-1]
    //
    static constexpr double watt = joule / second;   // watt = 6.24150 e+3 * MeV/ns

    //
    // Force [E][L^-1]
    //
    static constexpr double newton = joule / meter;   // newton = 6.24150 e+9 * MeV/mm

    //
    // Pressure [E][L^-3]
    //
#define pascal hep_pascal                               // a trick to avoid warnings
    static constexpr double hep_pascal = newton / m2;   // pascal = 6.24150 e+3 * MeV/mm3
    static constexpr double bar = 100000 * pascal;      // bar    = 6.24150 e+8 * MeV/mm3
    static constexpr double atmosphere = 101325 * pascal;   // atm    = 6.32420 e+8 * MeV/mm3

    //
    // Electric current [Q][T^-1]
    //
    static constexpr double ampere = coulomb / second;   // ampere = 6.24150 e+9 * eplus/ns
    static constexpr double milliampere = 1.e-3 * ampere;
    static constexpr double microampere = 1.e-6 * ampere;
    static constexpr double nanoampere = 1.e-9 * ampere;

    //
    // Electric potential [E][Q^-1]
    //
    static constexpr double megavolt = megaelectronvolt / eplus;
    static constexpr double kilovolt = 1.e-3 * megavolt;
    static constexpr double volt = 1.e-6 * megavolt;

    //
    // Electric resistance [E][T][Q^-2]
    //
    static constexpr double ohm = volt / ampere;   // ohm = 1.60217e-16*(MeV/eplus)/(eplus/ns)

    //
    // Electric capacitance [Q^2][E^-1]
    //
    static constexpr double farad = coulomb / volt;   // farad = 6.24150e+24 * eplus/Megavolt
    static constexpr double millifarad = 1.e-3 * farad;
    static constexpr double microfarad = 1.e-6 * farad;
    static constexpr double nanofarad = 1.e-9 * farad;
    static constexpr double picofarad = 1.e-12 * farad;

    //
    // Magnetic Flux [T][E][Q^-1]
    //
    static constexpr double weber = volt * second;   // weber = 1000*megavolt*ns

    //
    // Magnetic Field [T][E][Q^-1][L^-2]
    //
    static constexpr double tesla = volt * second / meter2;   // tesla =0.001*megavolt*ns/mm2

    static constexpr double gauss = 1.e-4 * tesla;
    static constexpr double kilogauss = 1.e-1 * tesla;

    //
    // Inductance [T^2][E][Q^-2]
    //
    static constexpr double henry = weber / ampere;   // henry = 1.60217e-7*MeV*(ns/eplus)**2

    //
    // Temperature
    //
    static constexpr double kelvin = 1.;

    //
    // Amount of substance
    //
    static constexpr double mole = 1.;

    //
    // Activity [T^-1]
    //
    static constexpr double becquerel = 1. / second;
    static constexpr double curie = 3.7e+10 * becquerel;

    //
    // Absorbed dose [L^2][T^-2]
    //
    static constexpr double gray = joule / kilogram;
    static constexpr double kilogray = 1.e+3 * gray;
    static constexpr double milligray = 1.e-3 * gray;
    static constexpr double microgray = 1.e-6 * gray;

    //
    // Luminous intensity [I]
    //
    static constexpr double candela = 1.;

    //
    // Luminous flux [I]
    //
    static constexpr double lumen = candela * steradian;

    //
    // Illuminance [I][L^-2]
    //
    static constexpr double lux = lumen / meter2;

    //
    // Miscellaneous
    //
    static constexpr double perCent = 0.01;
    static constexpr double perThousand = 0.001;
    static constexpr double perMillion = 0.000001;

    // -*- C++ -*-
    // ----------------------------------------------------------------------
    // HEP coherent Physical Constants
    //
    // This file has been provided by Geant4 (simulation toolkit for HEP).
    //
    // The basic units are :
    //            millimeter
    //            nanosecond
    //            Mega electron Volt
    //            positon charge
    //            degree Kelvin
    //              amount of substance (mole)
    //              luminous intensity (candela)
    //            radian
    //              steradian
    //
    // Below is a non exhaustive list of Physical CONSTANTS,
    // computed in the Internal HEP System Of Units.
    //
    // Most of them are extracted from the Particle Data Book :
    //        Phys. Rev. D  volume 50 3-1 (1994) page 1233
    //
    //        ...with a meaningful (?) name ...
    //
    // You can add your own constants.
    //
    // Author: M.Maire
    //
    // History:
    //
    // 23.02.96 Created
    // 26.03.96 Added constants for standard conditions of temperature
    //          and pressure; also added Gas threshold.
    // 29.04.08   use PDG 2006 values
    // 03.11.08   use PDG 2008 values

    static constexpr double pi = 3.14159265358979323846;
    static constexpr double twopi = 2 * pi;
    static constexpr double halfpi = pi / 2;
    static constexpr double pi2 = pi * pi;

    //
    //
    //
    static constexpr double Avogadro = 6.02214179e+23 / mole;

    //
    // c   = 299.792458 mm/ns
    // c^2 = 898.7404 (mm/ns)^2
    //
    static constexpr double c_light = 2.99792458e+8 * m / s;
    static constexpr double c_squared = c_light * c_light;

    //
    // h     = 4.13566e-12 MeV*ns
    // hbar  = 6.58212e-13 MeV*ns
    // hbarc = 197.32705e-12 MeV*mm
    //
    static constexpr double h_Planck = 6.62606896e-34 * joule * s;
    static constexpr double hbar_Planck = h_Planck / twopi;
    static constexpr double hbarc = hbar_Planck * c_light;
    static constexpr double hbarc_squared = hbarc * hbarc;

    //
    //
    //
    static constexpr double electron_charge = -eplus;   // see SystemOfUnits.h
    static constexpr double e_squared = eplus * eplus;

    //
    // amu_c2 - atomic equivalent mass unit
    //        - AKA, unified atomic mass unit (u)
    // amu    - atomic mass unit
    //
    static constexpr double electron_mass_c2 = 0.510998910 * MeV;
    static constexpr double proton_mass_c2 = 938.272013 * MeV;
    static constexpr double neutron_mass_c2 = 939.56536 * MeV;
    static constexpr double amu_c2 = 931.494028 * MeV;
    static constexpr double amu = amu_c2 / c_squared;

    //
    // permeability of free space mu0    = 2.01334e-16 Mev*(ns*eplus)^2/mm
    // permittivity of free space epsil0 = 5.52636e+10 eplus^2/(MeV*mm)
    //
    static constexpr double mu0 = 4 * pi * 1.e-7 * henry / m;
    static constexpr double epsilon0 = 1. / (c_squared * mu0);

    //
    // electromagnetic coupling = 1.43996e-12 MeV*mm/(eplus^2)
    //
    static constexpr double elm_coupling = e_squared / (4 * pi * epsilon0);
    static constexpr double fine_structure_const = elm_coupling / hbarc;
    static constexpr double classic_electr_radius = elm_coupling / electron_mass_c2;
    static constexpr double electron_Compton_length = hbarc / electron_mass_c2;
    static constexpr double Bohr_radius = electron_Compton_length / fine_structure_const;

    static constexpr double alpha_rcl2 = fine_structure_const * classic_electr_radius * classic_electr_radius;

    static constexpr double twopi_mc2_rcl2 = twopi * electron_mass_c2 * classic_electr_radius * classic_electr_radius;
    //
    //
    //
    static constexpr double k_Boltzmann = 8.617343e-11 * MeV / kelvin;

    //
    // IUPAC standard temperature and pressure (STP)
    // STP uses 273.15 K (0 °C, 32 °F) and (since 1982) 1 bar (100 kPa) and not 1 atm!
    static constexpr double STP_Temperature = 273.15 * kelvin;
    static constexpr double STP_Pressure    = 1. * bar;
    //
    // NTP uses the NIST convention: 20 °C (293.15 K, 68 °F), 1 atm (14.696 psi, 101.325 kPa)
    static constexpr double NTP_Temperature = 293.15 * kelvin;
    static constexpr double NTP_Pressure    = 1. * atmosphere;
    //
    static constexpr double kGasThreshold   = 10. * mg / cm3;
    //
    //
    //
    static constexpr double universe_mean_density = 1.e-25 * g / cm3;
  //}
}
#endif

#endif /* DD4HEP_TGEOUNITS_H */
