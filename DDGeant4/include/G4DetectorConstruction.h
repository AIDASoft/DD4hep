#ifndef DD4hep_G4DetectorConstruction_h
#define DD4Hep_G4DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;

namespace DD4hep {
  namespace Geometry {
    class LCDD;
  }
  class G4DetectorConstruction : public G4VUserDetectorConstruction
  {
  public:
    
    G4DetectorConstruction(const std::string&);
    ~G4DetectorConstruction();
    G4VPhysicalVolume* Construct();
    Geometry::LCDD& GetLCDD() { return m_lcdd;}
  private:
    Geometry::LCDD& m_lcdd;
    std::string m_compactfile;
  };
  
}
#endif

