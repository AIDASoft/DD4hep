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
#include "G4VHitsCollection.hh"
#include "G4VHit.hh"

// C/C++ include files
#include <vector>
#include <string>
#include <typeinfo>
#include <stdexcept>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {
    
    // Forward declarations
    class Geant4HitCollection;
    class Geant4HitWrapper;

    /** @class Geant4HitWrapper Geant4HitCollection.h DDG4/Geant4HitCollection.h
     * 
     *  Default base class for all geant 4 created hits.
     *  The hit is stored in an opaque way and can be accessed by the
     *  collection.
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class Geant4HitWrapper : public G4VHit  {
    private:
    public:
      struct InvalidHit { virtual ~InvalidHit();  };

      struct HitManipulator {
      public:
	typedef std::pair<void*,HitManipulator*> Wrapper;
	typedef void  (*destroy_t)(Wrapper& data);
	typedef void* (*cast_t)(const Wrapper& data);
	const std::type_info& type;
	destroy_t             destroy;
	cast_t                cast;

	/// Initializing Constructor
	HitManipulator(const std::type_info& t, destroy_t d, cast_t c);
	/// Check to collection type against the argument (used during insertion)
	void checkHitType(const std::type_info& type) const;
	/// Check pointer to be of proper type
	template <typename TYPE> Wrapper checkHit(TYPE* obj)   {
	  if ( obj )  {
	    Wrapper wrap(obj,this);
	    HitManipulator* obj_type = HitManipulator::instance<TYPE>();
	    if ( obj_type == this )   {
	      return wrap;
	    }
	    wrap.first = (obj_type->cast)(wrap);
	    if ( wrap.first )  {
	      return wrap;
	    }
	    checkHitType(obj_type->type);
	  }
	  throw std::runtime_error("Invalid hit pointer passed to collection!");
	}
	/// Static function to delete contained hits
	template <typename TYPE> static void deleteHit(Wrapper& obj) {
	  TYPE* p = (TYPE*)obj.first;
	  if ( p ) delete p;
	  obj.first = 0;
	}
	/// Static function to cast to the desired type
	template <typename TYPE> static void* castHit(const Wrapper& obj) {
	  HitManipulator* me = HitManipulator::instance<TYPE>();
	  if ( me == obj.second )   {
	    return (TYPE*)obj.first;
	  }
	  G4VHit* hit = (G4VHit*)obj.first;
	  TYPE* p = dynamic_cast<TYPE*>(hit);
	  if ( p )  {
	    return p;
	  }
	  if ( obj.first )  {
	    obj.second->checkHitType(me->type);
	  }
	  return 0;
	}
	/// Static function to cast to the desired type
	template <typename TYPE> static const void* castHit(const Wrapper& obj) {
	  HitManipulator* me = HitManipulator::instance<TYPE>();
	  if ( me == obj.second )   {
	    return (const TYPE*)obj.first;
	  }
	  G4VHit* hit = (G4VHit*)obj.first;
	  const TYPE* p = dynamic_cast<const TYPE*>(hit);
	  if ( p )  {
	    return p;
	  }
	  if ( obj.first )  {
	    obj.second->checkHitType(me->type);
	  }
	  return 0;
	}
	template <typename TYPE> static HitManipulator* instance()  {
	  static HitManipulator m(typeid(TYPE),deleteHit<TYPE>,castHit<TYPE>);
	  return &m;
	}
      };

    protected:
      typedef HitManipulator::Wrapper Wrapper;
      mutable Wrapper m_data;

    public:
      
      /// Default constructor
      Geant4HitWrapper()  {
        m_data.second = manipulator<InvalidHit>();
	m_data.first = 0;
      }
      /// Copy constructor
      Geant4HitWrapper(const Geant4HitWrapper& v)  {	
	m_data = v.m_data;
	v.m_data.first = 0;
	//v.release();
      }
      /// Copy constructor
      Geant4HitWrapper(const Wrapper& v)  {	
	m_data = v;
      }
      /// Default destructor
      virtual ~Geant4HitWrapper();
      /// Geant4 required object allocator
      void *operator new(size_t);
      /// Geat4 required object destroyer
      void operator delete(void *ptr);
      /// Pointer/Object release
      void* release();
      /// Release data for copy
      Wrapper releaseData();
      /// Generate manipulator object
      template <typename TYPE> static HitManipulator* manipulator()  {
	return HitManipulator::instance<TYPE>();
      }
      /// Assignment transfers the pointer ownership
      Geant4HitWrapper& operator=(const Geant4HitWrapper& v)  {
	m_data = v.m_data;
	v.m_data.first = 0;
	//v.release();
	return *this;
      }
      /// Automatic conversion to the desired type
      template <typename TYPE> operator TYPE*() const   {
	return (TYPE*)(m_data.second->cast(m_data));
      }
    };

    /** @class Geant4HitCollection Geant4HitCollection.h DDG4/Geant4HitCollection.h
     * 
     * Opaque hit collection.
     * This hit collection is for good reasons homomorph,
     * Polymorphism without an explicit type would only
     * confuse most users.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4HitCollection  : public G4VHitsCollection  {
    public:
      typedef std::vector<Geant4HitWrapper>    WrappedHits;
      typedef Geant4HitWrapper::HitManipulator Manip;

      /** @class Compare Geant4HitCollection.h DDG4/Geant4HitCollection.h
       * 
       *  Base class for hit comparisons.
       *
       * @author  M.Frank
       * @version 1.0
       */
      struct Compare {
	/// Comparison function
	virtual void* operator()(const Geant4HitWrapper& w) const = 0;
      };

    protected:
      /// The collection of hit pointers in the wrapped format
      WrappedHits m_hits;
      /// The type of the objects in this collection. Set by the constructor
      Manip*      m_manipulator;

    protected:
      /// Notification to increase the instance counter
      void newInstance();
      /// Find hit in a collection by comparison of attributes
      void* findHit(const Compare& cmp)  const;

    public:
      /// Initializing constructor
      template <typename TYPE>
	Geant4HitCollection(const std::string& det, const std::string& coll, const TYPE*)
	: G4VHitsCollection(det,coll), m_manipulator(Geant4HitWrapper::manipulator<TYPE>())
	{
	  newInstance();
	  m_hits.reserve(200);
	}
      /// Default destructor
      virtual ~Geant4HitCollection();
      /// Access individual hits
      virtual G4VHit* GetHit(size_t which) const      {  return (G4VHit*)&m_hits.at(which); } 
      /// Access the collection size
      virtual size_t  GetSize() const                 {  return m_hits.size();              }
      /// Access the hit wrapper
      Geant4HitWrapper& hit(size_t which)             {  return m_hits.at(which);           }
      /// Access the hit wrapper (CONST)
      const Geant4HitWrapper& hit(size_t which) const {  return m_hits.at(which);           }
      /// Add a new hit with a check, that the hit is of the same type
      template <typename TYPE> void add(TYPE* hit)   {
	Geant4HitWrapper w(m_manipulator->checkHit(hit));
	m_hits.push_back(w);
      }
      /// Find hits in a collection by comparison of attributes
      template <typename TYPE> TYPE* find(const Compare& cmp)  const  {
	return (TYPE*)findHit(cmp);
      }

    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4HITCOLLECTION_H
