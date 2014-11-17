#define private public
#include "G4AssemblyVolume.hh"
#undef private

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Hack! Wrapper around G4AssemblyVolume to access protected members.
    /** 
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4AssemblyVolume : public G4AssemblyVolume {
    public:
      std::vector<const TGeoNode*> m_entries;
      typedef std::vector<const TGeoNode*> Chain;
      /// Default constructor with initialization
      Geant4AssemblyVolume() {
      }
      /// Default destructor
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
  }
}
