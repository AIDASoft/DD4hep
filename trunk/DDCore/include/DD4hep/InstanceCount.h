// $Id: InstanceCount.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_INSTANCECOUNT_H
#define DD4HEP_GEOMETRY_INSTANCECOUNT_H

// Framework include files
#include <typeinfo>
#include <string>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geoemtry namespace declaration
   */
  namespace Geometry  {

    /** @class InstanceCount InstanceCount.h StorageSvc/InstanceCount.h
     *
     * Small class to enable object construction/destruction tracing
     *
     * @author  Markus Frank
     * @version 1.0
     */
    struct InstanceCount  {
    public:
      typedef long long int counter_t;
      /// Enumeration to steer the output
      enum {  NONE = 1<<0,
	      STRING = 1<<1,
	      TYPEINFO = 1<<2, 
	      ALL=STRING|TYPEINFO
      };
      /** @class Counter
       *
       * Small class to enable object construction/destruction tracing.
       *
       * @author  Markus Frank
       * @version 1.0
       */
      class Counter  {
      private:
	/// Reference counter value
	counter_t m_count;
	/// Increment counter value
	counter_t m_tot;
      public:
	/// Default constructor
      Counter() : m_count(0), m_tot(0)    {                       }
	/// Copy constructor
      Counter(const Counter& c) 
	: m_count(c.m_count), m_tot(c.m_tot){                       }
	/// Destructor
	~Counter()                          {                       }
	/// Increment counter
	void increment()                    {  ++m_count; ++m_tot;  }
	/// Decrement counter
	void decrement()                    {  --m_count;           }
	/// Access counter value
	counter_t value() const             {  return m_count;      }
	/// Access counter value
	counter_t total() const             {  return m_tot;        }
      };
    public:
      /// Standard Constructor - No need to call explicitly
      InstanceCount();
      /// Standard Destructor - No need to call explicitly
      virtual ~InstanceCount();
      /// Access counter object for local caching on optimizations
      static Counter* getCounter(const std::type_info& typ);
      /// Access counter object for local caching on optimizations
      static Counter* getCounter(const std::string& typ);
      /// Increment count according to type information
      template<class T> static void increment(T*) 
      { getCounter(typeid(T))->increment();                 }
      /// Decrement count according to type information
      template<class T> static void decrement(T*) 
      { getCounter(typeid(T))->decrement();                 }
      /// Access current counter
      template<class T> static counter_t get(T*)
      { return getCounter(typeid(T))->value();              }
      /// Increment count according to type information
      static void increment(const std::type_info& typ)
      { getCounter(typ)->increment();                       }
      /// Decrement count according to type information
      static void decrement(const std::type_info& typ)
      { getCounter(typ)->decrement();                       }
      /// Access current counter
      static counter_t get(const std::type_info& typ)
      { return getCounter(typ)->value();                    }
      /// Increment count according to string information
      static void increment(const std::string& typ)
      { getCounter(typ)->increment();                       }
      /// Decrement count according to string information
      static void decrement(const std::string& typ)
      { getCounter(typ)->decrement();                       }
      /// Access current counter
      static counter_t get(const std::string& typ)
      { return getCounter(typ)->value();                    }
      /// Dump list of instance counters
      static void dump(int which=ALL);
      /// Clear list of instance counters
      static void clear(int which=ALL);
      /// Check if tracing is enabled.
      static bool doTrace();
      /// Enable/Disable tracing
      static void doTracing(bool value);
    };

  }       /* End namespace Geometry           */
}         /* End namespace DD4hep             */
#endif    /* DD4HEP_GEOMETRY_INSTANCECOUNT_H     */
