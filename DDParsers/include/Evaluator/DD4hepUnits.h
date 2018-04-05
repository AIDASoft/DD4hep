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
    static const double millimeter = 0.1;
    static const double millimeter2 = millimeter * millimeter;
    static const double millimeter3 = millimeter * millimeter * millimeter;

    static const double centimeter = 10. * millimeter;
    static const double centimeter2 = centimeter * centimeter;
    static const double centimeter3 = centimeter * centimeter * centimeter;

    static const double meter = 1000. * millimeter;
    static const double meter2 = meter * meter;
    static const double meter3 = meter * meter * meter;

    static const double kilometer = 1000. * meter;
    static const double kilometer2 = kilometer * kilometer;
    static const double kilometer3 = kilometer * kilometer * kilometer;

    static const double parsec = 3.0856775807e+16 * meter;

    static const double micrometer = 1.e-6 * meter;
    static const double nanometer = 1.e-9 * meter;
    static const double angstrom = 1.e-10 * meter;
    static const double fermi = 1.e-15 * meter;

    static const double barn = 1.e-28 * meter2;
    static const double millibarn = 1.e-3 * barn;
    static const double microbarn = 1.e-6 * barn;
    static const double nanobarn = 1.e-9 * barn;
    static const double picobarn = 1.e-12 * barn;

    // symbols
    static const double nm = nanometer;
    static const double um = micrometer;

    static const double mm = millimeter;
    static const double mm2 = millimeter2;
    static const double mm3 = millimeter3;

    static const double cm = centimeter;
    static const double cm2 = centimeter2;
    static const double cm3 = centimeter3;

    static const double m = meter;
    static const double m2 = meter2;
    static const double m3 = meter3;

    static const double km = kilometer;
    static const double km2 = kilometer2;
    static const double km3 = kilometer3;

    static const double pc = parsec;

    //
    // Angle
    //
    // static const double radian = 180. / 3.14159265358979323846;   // => degree=1
    // static const double milliradian = 1.e-3 * radian;
    // static const double degree = 1.;   //= (3.14159265358979323846/180.0)*radian;

    //fg: use radians as default unit as this is needed for all math functions
    //    and everywhere else, except in TGeo shapes -> this is taken care of in shape Handles ....
    static const double radian = 1. ;
    static const double milliradian = 1.e-3 * radian;
    static const double degree = (3.14159265358979323846/180.0)*radian;

    static const double steradian = 1.;

    // symbols
    static const double rad = radian;
    static const double mrad = milliradian;
    static const double sr = steradian;
    static const double deg = degree;

    //
    // Time [T]
    //
    static const double nanosecond = 1.e-9;
    static const double second = 1.e+9 * nanosecond;
    static const double millisecond = 1.e-3 * second;
    static const double microsecond = 1.e-6 * second;
    static const double picosecond = 1.e-12 * second;

    static const double hertz = 1. / second;
    static const double kilohertz = 1.e+3 * hertz;
    static const double megahertz = 1.e+6 * hertz;

    // symbols
    static const double ns = nanosecond;
    static const double s = second;
    static const double ms = millisecond;

    //
    // Electric charge [Q]
    //
    static const double eplus = 1.;   // positron charge
    static const double e_SI = 1.602176487e-19;   // positron charge in coulomb
    static const double coulomb = eplus / e_SI;   // coulomb = 6.24150 e+18 * eplus

    //
    // Energy [E]
    //
    static const double megaelectronvolt = 1.e-3;
    static const double electronvolt = 1.e-6 * megaelectronvolt;
    static const double kiloelectronvolt = 1.e-3 * megaelectronvolt;
    static const double gigaelectronvolt = 1.e+3 * megaelectronvolt;
    static const double teraelectronvolt = 1.e+6 * megaelectronvolt;
    static const double petaelectronvolt = 1.e+9 * megaelectronvolt;

    static const double joule = electronvolt / e_SI;   // joule = 6.24150 e+12 * MeV

    // symbols
    static const double MeV = megaelectronvolt;
    static const double eV = electronvolt;
    static const double keV = kiloelectronvolt;
    static const double GeV = gigaelectronvolt;
    static const double TeV = teraelectronvolt;
    static const double PeV = petaelectronvolt;

    //
    // Mass [E][T^2][L^-2]
    //
    static const double kilogram = joule * second * second / (meter * meter);
    static const double gram = 1.e-3 * kilogram;
    static const double milligram = 1.e-3 * gram;

    // symbols
    static const double kg = kilogram;
    static const double g = gram;
    static const double mg = milligram;

    //
    // Power [E][T^-1]
    //
    static const double watt = joule / second;   // watt = 6.24150 e+3 * MeV/ns

    //
    // Force [E][L^-1]
    //
    static const double newton = joule / meter;   // newton = 6.24150 e+9 * MeV/mm

    //
    // Pressure [E][L^-3]
    //
#define pascal hep_pascal                          // a trick to avoid warnings
    static const double hep_pascal = newton / m2;   // pascal = 6.24150 e+3 * MeV/mm3
    static const double bar = 100000 * pascal;   // bar    = 6.24150 e+8 * MeV/mm3
    static const double atmosphere = 101325 * pascal;   // atm    = 6.32420 e+8 * MeV/mm3

    //
    // Electric current [Q][T^-1]
    //
    static const double ampere = coulomb / second;   // ampere = 6.24150 e+9 * eplus/ns
    static const double milliampere = 1.e-3 * ampere;
    static const double microampere = 1.e-6 * ampere;
    static const double nanoampere = 1.e-9 * ampere;

    //
    // Electric potential [E][Q^-1]
    //
    static const double megavolt = megaelectronvolt / eplus;
    static const double kilovolt = 1.e-3 * megavolt;
    static const double volt = 1.e-6 * megavolt;

    //
    // Electric resistance [E][T][Q^-2]
    //
    static const double ohm = volt / ampere;   // ohm = 1.60217e-16*(MeV/eplus)/(eplus/ns)

    //
    // Electric capacitance [Q^2][E^-1]
    //
    static const double farad = coulomb / volt;   // farad = 6.24150e+24 * eplus/Megavolt
    static const double millifarad = 1.e-3 * farad;
    static const double microfarad = 1.e-6 * farad;
    static const double nanofarad = 1.e-9 * farad;
    static const double picofarad = 1.e-12 * farad;

    //
    // Magnetic Flux [T][E][Q^-1]
    //
    static const double weber = volt * second;   // weber = 1000*megavolt*ns

    //
    // Magnetic Field [T][E][Q^-1][L^-2]
    //
    static const double tesla = volt * second / meter2;   // tesla =0.001*megavolt*ns/mm2

    static const double gauss = 1.e-4 * tesla;
    static const double kilogauss = 1.e-1 * tesla;

    //
    // Inductance [T^2][E][Q^-2]
    //
    static const double henry = weber / ampere;   // henry = 1.60217e-7*MeV*(ns/eplus)**2

    //
    // Temperature
    //
    static const double kelvin = 1.;

    //
    // Amount of substance
    //
    static const double mole = 1.;

    //
    // Activity [T^-1]
    //
    static const double becquerel = 1. / second;
    static const double curie = 3.7e+10 * becquerel;

    //
    // Absorbed dose [L^2][T^-2]
    //
    static const double gray = joule / kilogram;
    static const double kilogray = 1.e+3 * gray;
    static const double milligray = 1.e-3 * gray;
    static const double microgray = 1.e-6 * gray;

    //
    // Luminous intensity [I]
    //
    static const double candela = 1.;

    //
    // Luminous flux [I]
    //
    static const double lumen = candela * steradian;

    //
    // Illuminance [I][L^-2]
    //
    static const double lux = lumen / meter2;

    //
    // Miscellaneous
    //
    static const double perCent = 0.01;
    static const double perThousand = 0.001;
    static const double perMillion = 0.000001;

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

    static const double pi = 3.14159265358979323846;
    static const double twopi = 2 * pi;
    static const double halfpi = pi / 2;
    static const double pi2 = pi * pi;

    //
    //
    //
    static const double Avogadro = 6.02214179e+23 / mole;

    //
    // c   = 299.792458 mm/ns
    // c^2 = 898.7404 (mm/ns)^2
    //
    static const double c_light = 2.99792458e+8 * m / s;
    static const double c_squared = c_light * c_light;

    //
    // h     = 4.13566e-12 MeV*ns
    // hbar  = 6.58212e-13 MeV*ns
    // hbarc = 197.32705e-12 MeV*mm
    //
    static const double h_Planck = 6.62606896e-34 * joule * s;
    static const double hbar_Planck = h_Planck / twopi;
    static const double hbarc = hbar_Planck * c_light;
    static const double hbarc_squared = hbarc * hbarc;

    //
    //
    //
    static const double electron_charge = -eplus;   // see SystemOfUnits.h
    static const double e_squared = eplus * eplus;

    //
    // amu_c2 - atomic equivalent mass unit
    //        - AKA, unified atomic mass unit (u)
    // amu    - atomic mass unit
    //
    static const double electron_mass_c2 = 0.510998910 * MeV;
    static const double proton_mass_c2 = 938.272013 * MeV;
    static const double neutron_mass_c2 = 939.56536 * MeV;
    static const double amu_c2 = 931.494028 * MeV;
    static const double amu = amu_c2 / c_squared;

    //
    // permeability of free space mu0    = 2.01334e-16 Mev*(ns*eplus)^2/mm
    // permittivity of free space epsil0 = 5.52636e+10 eplus^2/(MeV*mm)
    //
    static const double mu0 = 4 * pi * 1.e-7 * henry / m;
    static const double epsilon0 = 1. / (c_squared * mu0);

    //
    // electromagnetic coupling = 1.43996e-12 MeV*mm/(eplus^2)
    //
    static const double elm_coupling = e_squared / (4 * pi * epsilon0);
    static const double fine_structure_const = elm_coupling / hbarc;
    static const double classic_electr_radius = elm_coupling / electron_mass_c2;
    static const double electron_Compton_length = hbarc / electron_mass_c2;
    static const double Bohr_radius = electron_Compton_length / fine_structure_const;

    static const double alpha_rcl2 = fine_structure_const * classic_electr_radius * classic_electr_radius;

    static const double twopi_mc2_rcl2 = twopi * electron_mass_c2 * classic_electr_radius * classic_electr_radius;
    //
    //
    //
    static const double k_Boltzmann = 8.617343e-11 * MeV / kelvin;

    //
    //
    //
    static const double STP_Temperature = 273.15 * kelvin;
    static const double STP_Pressure = 1. * atmosphere;
    static const double kGasThreshold = 10. * mg / cm3;

    //
    //
    //
    static const double universe_mean_density = 1.e-25 * g / cm3;
  //}
}
#endif

#endif /* DD4HEP_TGEOUNITS_H */
