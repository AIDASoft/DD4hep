#define private public
#include "G4AssemblyVolume.hh"
#undef private

namespace DD4hep { namespace Simulation {
    struct Geant4AssemblyVolume : public G4AssemblyVolume {
      std::vector<const TGeoNode*> m_entries;
      typedef std::vector<const TGeoNode*> Chain;
      Geant4AssemblyVolume() {
      }
      virtual ~Geant4AssemblyVolume() {
      }
      //std::vector<G4AssemblyTriplet>& triplets()  { return fTriplets; }
      long placeVolume(const TGeoNode* n, G4LogicalVolume* pPlacedVolume, G4Transform3D& transformation) {
	size_t id = fTriplets.size();
	m_entries.push_back(n);
	this->AddPlacedVolume(pPlacedVolume, transformation);
	return (long)id;
      }
      long placeAssembly(const TGeoNode* n, Geant4AssemblyVolume* pPlacedVolume, G4Transform3D& transformation) {
	size_t id = fTriplets.size();
	m_entries.push_back(n);
	this->AddPlacedAssembly(pPlacedVolume, transformation);
	return (long)id;
      }
      void imprint(Geant4GeometryInfo& info,
		   const TGeoNode* n,
		   Chain chain, 
		   Geant4AssemblyVolume* pAssembly,
		   G4LogicalVolume*  pMotherLV,
		   G4Transform3D&    transformation,
		   G4int copyNumBase,
		   G4bool surfCheck );
    };
  }}
