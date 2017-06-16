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
#ifndef DD4HEP_GEOMETRY_INSTANCECOUNT_H
#define DD4HEP_GEOMETRY_INSTANCECOUNT_H

// Framework include files
#include <typeinfo>
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Helper to support object counting when debugging memory leaks
  /**
   * Small class to enable object construction/destruction tracing
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  struct InstanceCount {
  public:
    typedef long long int counter_t;
    /// Enumeration to steer the output
    enum {
      NONE     = 1 << 0,
      STRING   = 1 << 1,
      TYPEINFO = 1 << 2,
      ALL      = STRING | TYPEINFO
    };

    /// detaill class to could object constructions and destructions
    /**
     * Small class to enable object construction/destruction tracing.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP
     */
    class Counter {
    private:
      /// Reference counter value
      counter_t m_count = 0;
      /// Increment counter value
      counter_t m_tot = 0;
      /// Maximum number of simultaneous instances
      counter_t m_max = 0;
    public:
      /// Default constructor
      Counter() = default;
      /// Copy constructor
      Counter(const Counter& c) = default;
      /// Destructor
      ~Counter() = default;
      /// Increment counter
      void increment() {
        ++m_count;
        ++m_tot;
        m_max = std::max(m_max,m_count);
      }
      /// Decrement counter
      void decrement() {
        --m_count;
      }
      /// Access counter value
      counter_t value() const {
        return m_count;
      }
      /// Access counter value
      counter_t total() const {
        return m_tot;
      }
      /// Access maximum counter value
      counter_t maximum() const {
        return m_max;
      }
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
    template <class T> static void increment(T*) {
      increment(typeid(T));
    }
    /// Decrement count according to type information
    template <class T> static void decrement(T*) {
      decrement(typeid(T));
    }
    /// Access current counter
    template <class T> static counter_t get(T*) {
      return getCounter(typeid(T))->value();
    }
    /// Increment count according to type information
    static void increment(const std::type_info& typ);
    /// Decrement count according to type information
    static void decrement(const std::type_info& typ);
    /// Access current counter
    static counter_t get(const std::type_info& typ) {
      return getCounter(typ)->value();
    }
    /// Increment count according to string information
    static void increment(const std::string& typ);
    /// Decrement count according to string information
    static void decrement(const std::string& typ);
    /// Access current counter
    static counter_t get(const std::string& typ) {
      return getCounter(typ)->value();
    }
    /// Dump list of instance counters
    static void dump(int which = ALL);
    /// Clear list of instance counters
    static void clear(int which = ALL);
    /// Check if tracing is enabled.
    static bool doTrace();
    /// Enable/Disable tracing
    static void doTracing(bool value);
  };

  /// Helper class to count call stack depths of certain functions
  /**
   * Small class to count re-entrancy calls
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  template <typename T> struct Increment {
    static int& counter() { static int cnt=0; return cnt; }
    Increment()   { ++counter();   }
    ~Increment()  { --counter();   }
  };

} /* End namespace dd4hep             */
#endif    /* DD4HEP_GEOMETRY_INSTANCECOUNT_H     */
