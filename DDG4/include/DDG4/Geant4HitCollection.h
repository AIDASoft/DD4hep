// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4HITCOLLECTION_H
#define DD4HEP_DDG4_GEANT4HITCOLLECTION_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DDG4/ComponentUtils.h"
#include "G4VHitsCollection.hh"
#include "G4VHit.hh"

// C/C++ include files
#include <vector>
#include <string>
#include <typeinfo>
#include <stdexcept>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4Sensitive;
    class Geant4HitCollection;
    class Geant4HitWrapper;

    /// Generic wrapper class for hit structures created in Geant4 sensitive detectors
    /**
     *  Default base class for all geant 4 created hits.
     *  The hit is stored in an opaque way and can be accessed by the
     *  collection.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4HitWrapper: public G4VHit {
    private:
    public:
      /// Helper class to indicate invalid hit wrappers or containers.
      class InvalidHit {
      public:
        virtual ~InvalidHit();
      };

      /// Generic type manipulation class for generic hit structures created in Geant4 sensitive detectors
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_SIMULATION
       */
      class HitManipulator {
      public:
        typedef std::pair<void*, HitManipulator*> Wrapper;
#ifdef __CINT__
        const ComponentCast* cast;
        const ComponentCast* vec_type;
#else
        const ComponentCast& cast;
        const ComponentCast& vec_type;
#endif
        /// Initializing Constructor
        HitManipulator(const ComponentCast& c, const ComponentCast& v);
        /// Default destructor
        ~HitManipulator();
        /// Check pointer to be of proper type
        template <typename TYPE> Wrapper castHit(TYPE* obj) {
          if (obj) {
            ComponentCast& me = ComponentCast::instance<TYPE>();
            Wrapper wrap(obj, this);
            if (&cast == &me) {
              return wrap;
            }
            // Need to ensure that the container types are the same!
            wrap.first = me.apply_dynCast(cast, obj);
            return wrap;
          }
          throw std::runtime_error("Invalid hit pointer passed to collection!");
        }
        /// Static function to delete contained hits
        template <typename TYPE> static void deleteHit(Wrapper& obj) {
          TYPE* p = (TYPE*) obj.first;
          if (p)
            delete p;
          obj.first = 0;
        }
        template <typename TYPE> static HitManipulator* instance() {
          static HitManipulator hm(ComponentCast::instance<TYPE>(), ComponentCast::instance<std::vector<TYPE*> >());
          return &hm;
        }
      };

      typedef HitManipulator::Wrapper Wrapper;
    protected:
      mutable Wrapper m_data;

    public:

      /// Default constructor
    Geant4HitWrapper() : G4VHit() {
        m_data.second = manipulator<InvalidHit>();
        m_data.first = 0;
      }
      /// Copy constructor
    Geant4HitWrapper(const Geant4HitWrapper& v) : G4VHit() {
        m_data = v.m_data;
        v.m_data.first = 0;
        //v.release();
      }
      /// Copy constructor
    Geant4HitWrapper(const Wrapper& v) : G4VHit()  {
        m_data = v;
      }
      /// Default destructor
      virtual ~Geant4HitWrapper();
      /// Geant4 required object allocator
      void *operator new(size_t);
      /// Geant4 required object destroyer
      void operator delete(void *ptr);
      /// Pointer/Object release
      void* release();
      /// Release data for copy
      Wrapper releaseData();
      /// Access to cast grammar
      HitManipulator* manip() const {
        return m_data.second;
      }
      /// Pointer/Object access
      void* data() {
        return m_data.first;
      }
      /// Pointer/Object access (CONST)
      void* data() const {
        return m_data.first;
      }
      /// Generate manipulator object
      template <typename TYPE> static HitManipulator* manipulator() {
        return HitManipulator::instance<TYPE>();
      }
      /// Assignment transfers the pointer ownership
      Geant4HitWrapper& operator=(const Geant4HitWrapper& v) {
        if ( this != & v )  {
          m_data = v.m_data;
          v.m_data.first = 0;
          //v.release();
        }
        return *this;
      }
      /// Automatic conversion to the desired type
      template <typename TYPE> operator TYPE*() const {
        return (TYPE*) m_data.second->
          cast.apply_dynCast(ComponentCast::instance<TYPE>(),m_data.first);
        //cast.apply_downCast(ComponentCast::instance<TYPE>(),m_data.first);
      }
    };

    /// Generic hit container class using Geant4HitWrapper objects
    /**
     * Opaque hit collection.
     * This hit collection is for good reasons homomorph,
     * Polymorphism without an explicit type would only
     * confuse most users.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4HitCollection: public G4VHitsCollection {
    public:
      /** Local type declarations  */
      /// Hit wrapper
      typedef std::vector<Geant4HitWrapper>    WrappedHits;
      /// Hit manipulator
      typedef Geant4HitWrapper::HitManipulator Manip;

      /// Generic class template to compare/select hits in Geant4HitCollection objects
      /**
       *
       *  Base class for hit comparisons.
       *
       * \author  M.Frank
       * \version 1.0
       *  \ingroup DD4HEP_SIMULATION
       */
      class Compare {
      public:
        /// Default destructor
        virtual ~Compare();
        /// Comparison function
        virtual void* operator()(const Geant4HitWrapper& w) const = 0;
      };

    protected:
      /// The collection of hit pointers in the wrapped format
      WrappedHits m_hits;
      /// Handle to the sensitive detector
      Geant4Sensitive* m_detector;
      /// The type of the objects in this collection. Set by the constructor
      Manip* m_manipulator;

    protected:
      /// Notification to increase the instance counter
      void newInstance();
      /// Find hit in a collection by comparison of attributes
      void* findHit(const Compare& cmp) const;
      /// Release all hits from the Geant4 container and pass ownership to the caller
      void releaseData(const ComponentCast& cast, std::vector<void*>* result);
      /// Release all hits from the Geant4 container. Ownership stays with the container
      void getData(const ComponentCast& cast, std::vector<void*>* result);

    public:
      /// Initializing constructor (C++ version)
      template <typename TYPE>
        Geant4HitCollection(const std::string& det, const std::string& coll, Geant4Sensitive* sd)
        : G4VHitsCollection(det, coll), m_detector(sd),
        m_manipulator(Geant4HitWrapper::manipulator<TYPE>())   {
        newInstance();
        m_hits.reserve(200);
      }
      /// Initializing constructor
      template <typename TYPE>
        Geant4HitCollection(const std::string& det, const std::string& coll, Geant4Sensitive* sd, const TYPE*)
        : G4VHitsCollection(det, coll), m_detector(sd),
        m_manipulator(Geant4HitWrapper::manipulator<TYPE>())   {
        newInstance();
        m_hits.reserve(200);
      }
      /// Default destructor
      virtual ~Geant4HitCollection();
      /// Type information of the object stored
      const ComponentCast& type() const;
      /// Type information of the vector type for extracting data
      const ComponentCast& vector_type() const;
      /// Clear the collection (Deletes all valid references to real hits)
      virtual void clear();
      /// Set the sensitive detector
      void setSensitive(Geant4Sensitive* detector)   {
        m_detector = detector;
      }
      /// Access the sensitive detector
      Geant4Sensitive* sensitive() const   {
        return m_detector;
      }
      /// Access individual hits
      virtual G4VHit* GetHit(size_t which) const {
        return (G4VHit*) &m_hits.at(which);
      }
      /// Access the collection size
      virtual size_t GetSize() const {
        return m_hits.size();
      }
      /// Access the hit wrapper
      Geant4HitWrapper& hit(size_t which) {
        return m_hits.at(which);
      }
      /// Access the hit wrapper (CONST)
      const Geant4HitWrapper& hit(size_t which) const {
        return m_hits.at(which);
      }
      /// Add a new hit with a check, that the hit is of the same type
      template <typename TYPE> void add(TYPE* hit_pointer) {
        Geant4HitWrapper w(m_manipulator->castHit(hit_pointer));
        m_hits.push_back(w);
      }
      /// Find hits in a collection by comparison of attributes
      template <typename TYPE> TYPE* find(const Compare& cmp) const {
        return (TYPE*) findHit(cmp);
      }
      /// Release all hits from the Geant4 container and pass ownership to the caller
      template <typename TYPE> std::vector<TYPE*> releaseHits() {
        std::vector<TYPE*> vec;
        if (m_hits.size() != 0) {
          releaseData(ComponentCast::instance<TYPE>(), (std::vector<void*>*) &vec);
        }
        return vec;
      }
      /// Release all hits from the Geant4 container and pass ownership to the caller
      void releaseHitsUnchecked(std::vector<void*>& result);

      /// Release all hits from the Geant4 container. Ownership stays with the container
      template <typename TYPE> std::vector<TYPE*> getHits() {
        std::vector<TYPE*> vec;
        if (m_hits.size() != 0) {
          getData(ComponentCast::instance<TYPE>(), (std::vector<void*>*) &vec);
        }
        return vec;
      }
      /// Release all hits from the Geant4 container. Ownership stays with the container
      void getHitsUnchecked(std::vector<void*>& result);
    };


    /// Specialized hit selector based on the hit's position.
    /**
     *  Class for hit matching using the hit position.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    template<typename TYPE, typename POS> class PositionCompare : public Geant4HitCollection::Compare {
    public:
      const POS& pos;
      /// Constructor
    PositionCompare(const POS& p) : pos(p)  {      }
      /// Comparison function
      virtual void* operator()(const Geant4HitWrapper& w) const;
    };

    template <typename TYPE, typename POS>
      void* PositionCompare<TYPE,POS>::operator()(const Geant4HitWrapper& w) const {
      TYPE* h = w;
      return pos == h->position ? h : 0;
    }

    /// Specialized hit selector based on the hit's cell identifier.
    /**
     *  Class for hit matching using the hit's cell identifier.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    template<typename TYPE> class CellIDCompare : public Geant4HitCollection::Compare {
    public:
      long long int id;
      /// Constructor
    CellIDCompare(long long int i) : id(i) {      }
      /// Comparison function.
      virtual void* operator()(const Geant4HitWrapper& w) const;
    };

    template <typename TYPE>
      void* CellIDCompare<TYPE>::operator()(const Geant4HitWrapper& w) const {
      TYPE* h = w;
      if ( id == h->cellID )
        return h;
      return 0;
    }

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4HITCOLLECTION_H
