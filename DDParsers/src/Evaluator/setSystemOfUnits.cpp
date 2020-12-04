// -*- C++ -*-
// ----------------------------------------------------------------------

#include "Evaluator/Evaluator.h"
#include "Evaluator/detail/Evaluator.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  namespace tools {

    void Evaluator::Object::setSystemOfUnits(double meter,
					     double kilogram,
					     double second,
					     double ampere,
					     double kelvin,
					     double mole,
					     double candela,
					     double radians)
    {
      const double kilo_  = 1.e+03; // chilioi (Greek) "thousand"
      const double mega_  = 1.e+06; // megas (Greek) "large"
      const double giga_  = 1.e+09; // gigas (Greek) "giant"
      const double tera_  = 1.e+12; // teras (Greek) "monster"
      const double peta_  = 1.e+15; // pente (Greek) "five"

      const double deci_  = 1.e-01; // decimus (Latin) "tenth"
      const double centi_ = 1.e-02; // centum  (Latin) "hundred"
      const double milli_ = 1.e-03; // mille   (Latin) "thousand"
      const double micro_ = 1.e-06; // micro (Latin) or mikros (Greek) "small"
      const double nano_  = 1.e-09; // nanus (Latin) or nanos  (Greek) "dwarf"
      const double pico_  = 1.e-12; // pico (Spanish) "bit"
      const double femto_ = 1.e-15; // femto

      // ======================================================================
      //
      // Base (default) SI units
      // for the basic measurable quantities (dimensions):
      //
      // ======================================================================

      // Length
      // metrum (Latin) and metron (Greek) "measure"
      const double m = meter;
      setVariableNoLock("meter", m);
      setVariableNoLock("metre", m);
      setVariableNoLock("m",     m);

      // Mass
      const double kg = kilogram;
      setVariableNoLock("kilogram", kg);
      setVariableNoLock("kg",       kg);

      // Time
      // minuta secundam (Latin) "second small one"
      const double s = second;
      setVariableNoLock("second", s);
      setVariableNoLock("s",      s);

      // Current
      // ---  honors Andre-Marie Ampere (1775-1836) of France
      const double A = ampere;
      setVariableNoLock("ampere", A);
      setVariableNoLock("amp",    A);
      setVariableNoLock("A",      A);

      // Temperature
      // ---  honors William Thomson, 1st Baron Lord Kelvin (1824-1907) of England
      const double K = kelvin;
      setVariableNoLock("kelvin", K);
      setVariableNoLock("K",      K);

      // Amount of substance
      const double mol = mole;
      setVariableNoLock("mole", mol);
      setVariableNoLock("mol",  mol);

      // Luminous intensity
      const double cd  = candela;
      setVariableNoLock("candela", cd);
      setVariableNoLock("cd",      cd);

      // ======================================================================
      //
      // Supplementary SI units having special symbols:
      //
      // ======================================================================

      const double pi  = 3.14159265358979323846;
      // Plane angle
      // const double rad = 1.;  // Geant4 (rad units)
      //const double rad = pi;    // Degree units

      const double rad = radians ;

      setVariableNoLock("radian", rad);
      setVariableNoLock("rad",    rad);
      setVariableNoLock("milliradian", milli_ * rad);
      setVariableNoLock("mrad",        milli_ * rad);

      const double deg = rad*pi/180.;

      setVariableNoLock("degree", deg);
      setVariableNoLock("deg",    deg);

      // Solid angle
      const double sr  = 1.;
      setVariableNoLock("steradian", sr);
      setVariableNoLock("sr",        sr);

      // ======================================================================
      //
      // Derived SI units having special symbols:
      //
      // ======================================================================

      // Frequency
      // ---  honors Heinrich Rudolf Hertz (1857-1894) of Germany
      const double Hz = 1./s;
      setVariableNoLock("hertz", Hz);
      setVariableNoLock("Hz",    Hz);

      // Force
      // ---  honors Sir Isaac Newton (1642-1727) of England
      const double N = m * kg / (s*s);
      setVariableNoLock("newton", N);
      setVariableNoLock("N",      N);

      // Pressure
      // ---  honors Blaise Pascal (1623-1662) of France
      const double Pa = N / (m*m);
      setVariableNoLock("pascal",    Pa);
      setVariableNoLock("Pa",        Pa);
      setVariableNoLock("hPa", 100.0*Pa);

      const double atm = 101325. * Pa;
      setVariableNoLock("atmosphere", atm);
      setVariableNoLock("atm",        atm);

      const double bar = 100000*Pa;
      setVariableNoLock("bar", bar);

      // Energy
      // ---  honors James Prescott Joule (1818-1889) of England
      const double J = N * m;
      setVariableNoLock("joule", J);
      setVariableNoLock("J",     J);

      // Power
      // ---  honors James Watt (1736-1819) of Scotland
      const double W = J / s;
      setVariableNoLock("watt", W);
      setVariableNoLock("W",    W);

      // Electric charge
      // ---  honors Charles-Augustin de Coulomb (1736-1806) of France
      const double C = A * s;
      setVariableNoLock("coulomb", C);
      setVariableNoLock("C",       C);

      // Electric potential
      // ---  honors Count Alessandro Volta (1745-1827) of Italy
      const double V = J / C;
      setVariableNoLock("volt", V);
      setVariableNoLock("V",    V);

      // Electric resistance
      // ---  honors Georg Simon Ohm (1787-1854) of Germany
      const double ohm = V / A;
      setVariableNoLock("ohm", ohm);

      // Electric conductance
      // ---  honors Ernst Werner von Siemens (1816-1892) or
      //      his brother Sir William (Karl Wilhelm von) Siemens (1823-1883)
      //      of Germany (England)
      const double S = 1./ ohm;
      setVariableNoLock("siemens", S);
      setVariableNoLock("S",       S);

      // Electric capacitance
      // ---  honors Michael Faraday (1791-1867) of England
      const double F = C / V;
      setVariableNoLock("farad", F);
      setVariableNoLock("F",     F);

      // Magnetic flux density
      // ---  honors Nikola Tesla (1856-1943) of Croatia (United States)
      const double T = V * s / (m*m);
      setVariableNoLock("tesla", T);
      setVariableNoLock("T",     T);

      // ---  honors Karl Friedrich Gauss (1777-1855) of Germany
      const double Gs = 1.e-4*T;
      setVariableNoLock("gauss", Gs);
      setVariableNoLock("Gs",    Gs);

      // Magnetic flux
      // ---  honors Wilhelm Eduard Weber (1804-1891) of Germany
      const double Wb = V * s;
      setVariableNoLock("weber", Wb);
      setVariableNoLock("Wb",    Wb);

      // Inductance
      // ---  honors Joseph Henry (1797-1878) of the United States
      const double H = Wb / A;
      setVariableNoLock("henry", H);
      setVariableNoLock("H",     H);

      // Luminous flux
      const double lm = cd * sr;
      setVariableNoLock("lumen", lm);
      setVariableNoLock("lm",    lm);

      // Illuminace
      const double lx = lm / (m*m);
      setVariableNoLock("lux", lx);
      setVariableNoLock("lx",  lx);

      // Radioactivity
      // ---  honors Antoine-Henri Becquerel (1852-1908) of France
      const double Bq = 1./s;
      setVariableNoLock("becquerel", Bq);
      setVariableNoLock("Bq",        Bq);

      // ---  honors Pierre Curie (1859-1906) of France
      //      and Marie Sklodowska Curie (1867-1934) of Poland
      setVariableNoLock("curie", 3.7e+10 * Bq);
      setVariableNoLock("Ci",    3.7e+10 * Bq);

      // Specific energy
      // ---  honors Louis Harold Gray, F.R.S. (1905-1965) of England
      const double Gy = J / kg;
      setVariableNoLock("gray", Gy);
      setVariableNoLock("Gy",   Gy);

      // Dose equivalent
      const double Sv = J / kg;
      setVariableNoLock("sievert", Sv);
      setVariableNoLock("Sv",      Sv);

      // ======================================================================
      //
      // Selected units:
      //
      // ======================================================================

      // Length

      const double mm = milli_ * m;
      setVariableNoLock("millimeter", mm);
      setVariableNoLock("mm",         mm);

      const double cm = centi_ * m;
      setVariableNoLock("centimeter", cm);
      setVariableNoLock("cm",         cm);

      setVariableNoLock("decimeter",  deci_ * m);

      const double km = kilo_ * m;
      setVariableNoLock("kilometer",  km);
      setVariableNoLock("km",         km);

      setVariableNoLock("micrometer", micro_ * m);
      setVariableNoLock("micron",     micro_ * m);
      setVariableNoLock("mum",        micro_ * m);
      setVariableNoLock("um",         micro_ * m);
      setVariableNoLock("nanometer",  nano_  * m);
      setVariableNoLock("nm",         nano_  * m);

      // ---  honors Anders Jonas Angstrom (1814-1874) of Sweden
      setVariableNoLock("angstrom",   1.e-10 * m);

      // ---  honors Enrico Fermi (1901-1954) of Italy
      setVariableNoLock("fermi",      1.e-15 * m);
      setVariableNoLock("femtometer", femto_ * m);
      setVariableNoLock("fm",         femto_ * m);

      // Length^2

      setVariableNoLock("m2",  m*m);
      setVariableNoLock("mm2", mm*mm);
      setVariableNoLock("cm2", cm*cm);
      setVariableNoLock("km2", km*km);

      const double barn = 1.e-28 * m*m;
      setVariableNoLock("barn",      barn);
      setVariableNoLock("millibarn", milli_ * barn);
      setVariableNoLock("mbarn",     milli_ * barn);
      setVariableNoLock("microbarn", micro_ * barn);
      setVariableNoLock("nanobarn",  nano_  * barn);
      setVariableNoLock("picobarn",  pico_  * barn);

      // LengthL^3

      setVariableNoLock("m3",  m*m*m);
      setVariableNoLock("mm3", mm*mm*mm);
      setVariableNoLock("cm3", cm*cm*cm);
      setVariableNoLock("cc",  cm*cm*cm);
      setVariableNoLock("km3", km*km*km);

      const double L = 1.e-3*m*m*m;
      setVariableNoLock("liter", L);
      setVariableNoLock("litre", L);
      setVariableNoLock("L",     L);
      setVariableNoLock("centiliter",  centi_ * L);
      setVariableNoLock("cL",          centi_ * L);
      setVariableNoLock("milliliter",  milli_ * L);
      setVariableNoLock("mL",          milli_ * L);

      // Length^-1

      const double dpt = 1./m;
      setVariableNoLock("diopter", dpt);
      setVariableNoLock("dioptre", dpt);
      setVariableNoLock("dpt",     dpt);

      // Mass

      const double g = 0.001*kg;
      setVariableNoLock("gram", g);
      setVariableNoLock("g",    g);
      setVariableNoLock("milligram",   milli_ * g);
      setVariableNoLock("mg",          milli_ * g);

      // Time

      setVariableNoLock("millisecond", milli_ * s);
      setVariableNoLock("ms",          milli_ * s);
      setVariableNoLock("microsecond", micro_ * s);
      setVariableNoLock("nanosecond",  nano_  * s);
      setVariableNoLock("ns",          nano_  * s);
      setVariableNoLock("picosecond",  pico_  * s);

      // Current

      setVariableNoLock("milliampere", milli_ * A);
      setVariableNoLock("mA",          milli_ * A);
      setVariableNoLock("microampere", micro_ * A);
      setVariableNoLock("nanoampere",  nano_  * A);

      // Frequency

      setVariableNoLock("kilohertz",   kilo_ * Hz);
      setVariableNoLock("kHz",         kilo_ * Hz);
      setVariableNoLock("megahertz",   mega_ * Hz);
      setVariableNoLock("MHz",         mega_ * Hz);

      // Force
      setVariableNoLock("kilonewton",  kilo_ * N);
      setVariableNoLock("kN",          kilo_ * N);

      // Pressure
      setVariableNoLock("kilobar",     kilo_ * bar);
      setVariableNoLock("kbar",        kilo_ * bar);
      setVariableNoLock("millibar",    milli_ * bar);
      setVariableNoLock("mbar",        milli_ * bar);

      // Energy
      setVariableNoLock("kilojoule",   kilo_ * J);
      setVariableNoLock("kJ",          kilo_ * J);
      setVariableNoLock("megajoule",   mega_ * J);
      setVariableNoLock("MJ",          mega_ * J);
      setVariableNoLock("gigajoule",   giga_ * J);
      setVariableNoLock("GJ",          giga_ * J);

      const double e_SI  = 1.60217733e-19;  // positron charge in coulomb
      const double ePlus = e_SI * C;        // positron charge
      const double eV    = ePlus * V;
      setVariableNoLock("electronvolt", eV);
      setVariableNoLock("eV",           eV);
      setVariableNoLock("kiloelectronvolt", kilo_ * eV);
      setVariableNoLock("keV",              kilo_ * eV);
      setVariableNoLock("megaelectronvolt", mega_ * eV);
      setVariableNoLock("MeV",              mega_ * eV);
      setVariableNoLock("gigaelectronvolt", giga_ * eV);
      setVariableNoLock("GeV",              giga_ * eV);
      setVariableNoLock("teraelectronvolt", tera_ * eV);
      setVariableNoLock("TeV",              tera_ * eV);
      setVariableNoLock("petaelectronvolt", peta_ * eV);
      setVariableNoLock("PeV",              peta_ * eV);

      // Power
      setVariableNoLock("kilowatt",    kilo_ * W);
      setVariableNoLock("kW",          kilo_ * W);
      setVariableNoLock("megawatt",    mega_ * W);
      setVariableNoLock("MW",          mega_ * W);
      setVariableNoLock("gigawatt",    giga_ * W);
      setVariableNoLock("GW",          giga_ * W);

      // Electric potential
      setVariableNoLock("kilovolt",    kilo_ * V);
      setVariableNoLock("kV",          kilo_ * V);
      setVariableNoLock("megavolt",    mega_ * V);
      setVariableNoLock("MV",          mega_ * V);

      // Electric capacitance
      setVariableNoLock("millifarad",  milli_ * F);
      setVariableNoLock("mF",          milli_ * F);
      setVariableNoLock("microfarad",  micro_ * F);
      setVariableNoLock("uF",          micro_ * F);
      setVariableNoLock("nanofarad",   nano_  * F);
      setVariableNoLock("nF",          nano_  * F);
      setVariableNoLock("picofarad",   pico_  * F);
      setVariableNoLock("pF",          pico_  * F);

      // Magnetic flux density
      setVariableNoLock("kilogauss",   kilo_ * Gs);
      setVariableNoLock("kGs",         kilo_ * Gs);
    }

  } // namespace tools
}  // namespace dd4hep
