// $Id: Readout.h 951 2013-12-16 23:37:56Z Christian.Grefe@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_CONDITIONINTERNA_H
#define DD4HEP_GEOMETRY_CONDITIONINTERNA_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/BasicGrammar.h"
#include "DD4hep/objects/NamedObject.h"
/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    // Forward declarations
    class DetElement;
    class Condition;

    /*
     *   Conditions internal namespace declaration
     *   Internally defined datastructures are not presented to the
     *   user directly, but are used by dedicated views.
     *
     */
    namespace ConditionsInterna {

      class Object;
      class Entry;
      class IOV;

      /** @class IOV  ConditionsInterna.h DD4hep/ConditionsInterna.h
       * 
       *  Class describing the interval of validty
       *
       *
       * @author  M.Frank
       * @version 1.0
       */
      class IOV   {
	friend class Condition;
      public:
	enum { 
	  UNKNOWN_IOV = 1<<0
	} _IOVTypes;
	/// IOV buffer type: Must be a bitmap!
	int type;
	unsigned char data[16];
	/// Initializing constructor
	IOV(int t=UNKNOWN_IOV);
	/// Standard Destructor
	~IOV();
	/// Move the data content: 'from' will be reset to NULL
	void move(IOV& from);
	/// Create IOV data from string representation
	void fromString(const std::string& rep);
	/// Create string representation of the IOV
	std::string str();
      };

      /** @class Block  ConditionsInterna.h DD4hep/ConditionsInterna.h
       * 
       *  Class describing an opaque conditions data block
       *
       * @author  M.Frank
       * @version 1.0
       */
      class BlockData : public Block   {
	friend class ::DD4hep::Geometry::Condition;
	friend class Object;

      private:
	enum { 
	  PLAIN_DATA = 1<<0, 
	  ALLOC_DATA = 1<<1,
	  BOUND_DATA = 1<<2
	} _DataTypes;
	/// Data buffer: plain data are allocated directly on this buffer
	unsigned int data[16];
	/// Destructor function -- only set if the object is valid
	void (*destruct)(void*);
	/// Constructor function -- only set if the object is valid
	void (*copy)(void*,const void*);
	/// Data buffer type: Must be a bitmap!
	int type;

      public:

	/// Standard initializing constructor
	BlockData();
	/// Standard Destructor
	~BlockData();
	/// Move the data content: 'from' will be reset to NULL
	void move(BlockData& from);
	/// Set data value
	void bind(const BasicGrammar* grammar,
		  void (*ctor)(void*,const void*), 
		  void (*dtor)(void*));
	/// Set data value
	void assign(const void* ptr,const std::type_info& typ);
      };

      /** @class Entry  ConditionsInterna.h DD4hep/ConditionsInterna.h
       * 
       *  The intermediate conditions data
       *
       * @author  M.Frank
       * @version 1.0
       */
      class Entry : public NamedObject  {
      public:
	/// Reference to the detector element
	DetElement detector;
	/// Conditions type (was the tag in the XML)
	std::string type;
	/// The actual conditions data
	std::string value;
	/// The validity string to be interpreted by the updating engine
	std::string validity;
	/// Default constructor
	Entry() {}
	/// Initializing constructor
	Entry(const DetElement& det, const std::string& nam, const std::string& typ, const std::string& valid);
	/// Copy constructor
	Entry(const Entry& c);
	/// Default destructor
	virtual ~Entry();
	/// Assignment operator
	Entry& operator=(const Entry& c);
      };

      /** @class Object  ConditionsInterna.h DD4hep/ConditionsInterna.h
       * 
       *  The data class behind a conditions handle.
       *
       * @author  M.Frank
       * @version 1.0
       */
      class Object : public NamedObject {
      public:
	/// Condition value (in string form)
	std::string value;
	/// Condition validity (in string form)
	std::string validity;
	/// Condition address
	std::string address;
	/// Comment string
	std::string comment;
	/// The detector element
	DetElement detector;
	/// Data block
	BlockData data;
	/// Interval of validity
	IOV iov;
	/// Standard constructor
	Object();
	/// Standard Destructor
	virtual ~Object();
	/// Move data content: 'from' will be reset to NULL
	Object& move(Object& from);
      };

      /** @class Container  ConditionsInterna.h DD4hep/ConditionsInterna.h
       * 
       *  The data class behind a conditions container handle.
       *
       * @author  M.Frank
       * @version 1.0
       */
      class Container : public NamedObject {
      public:
	/// Container definition
	typedef std::map<std::string, Condition> Entries;

	Entries entries;
        /// Standard constructor
        Container();
        /// Default destructor
        virtual ~Container();
	/// Clear all conditions. Auto-delete of all existing entries
	void removeElements();
      };

    } /* End namespace ConditionsInterna    */

    template <typename T> static void copyObject(void* t, const void* s)  {
      *((T*)t) = *((const T*)s);
    }
    template <typename T> static void destructObject(void* p)  {
      T* t = (T*)p;
      t->~T();
    }
    /// Generic getter. Specify the exact type, not a polymorph type
    template <typename T> T& Block::get() {
      if (!grammar || (grammar->type() != typeid(T))) { throw std::bad_cast(); }
      return *(T*)pointer;
    }
    /// Generic getter (const version). Specify the exact type, not a polymorph type
    template <typename T> const T& Block::get() const {
      if (!grammar || (grammar->type() != typeid(T))) { throw std::bad_cast(); }
      return *(T*)pointer;
    }
    /// Bind the data of the conditions object to a given format. 
    template <typename T> Condition& Condition::bind()   {
      Object* o = access();
      ConditionsInterna::BlockData& b = o->data;
      b.bind(&BasicGrammar::instance<T>(),copyObject<T>,destructObject<T>);
      new(b.pointer) T();
      if ( !o->value.empty() ) b.fromString(o->value);
      return *this;
    }
    /// Generic getter. Specify the exact type, not a polymorph type
    template <typename T> T& Condition::get() {
      return access()->data.get<T>();
    }
    /// Generic getter (const version). Specify the exact type, not a polymorph type
    template <typename T> const T& Condition::get() const {
      return access()->data.get<T>();
    }
  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */

#define DD4HEP_DEFINE_CONDITIONS_TYPE(x)	\
  namespace DD4hep { namespace Geometry  {      \
  template Condition& Condition::bind<x>();	\
  template x& Condition::get<x>();		\
  template const x& Condition::get<x>() const;  \
    }}

#define DD4HEP_DEFINE_CONDITIONS_CONT(x)	\
  DD4HEP_DEFINE_CONDITIONS_TYPE(x)		\
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::vector<x>) \
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::list<x>)	\
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::set<x>)

#define DD4HEP_DEFINE_CONDITIONS_U_CONT(x)	\
  DD4HEP_DEFINE_CONDITIONS_CONT(x) 		\
  DD4HEP_DEFINE_CONDITIONS_CONT(unsigned x)


#endif    /* DD4HEP_GEOMETRY_CONDITIONINTERNA_H    */
