// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
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
namespace DD4hep {

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
      NONE = 1 << 0, STRING = 1 << 1, TYPEINFO = 1 << 2, ALL = STRING | TYPEINFO
    };

    /// Internal class to could object constructions and destructions
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
      counter_t m_count;
      /// Increment counter value
      counter_t m_tot;
      /// Maximum number of simultaneous instances
      counter_t m_max;
    public:
      /// Default constructor
      Counter()
        : m_count(0), m_tot(0), m_max(0) {
      }
      /// Copy constructor
      Counter(const Counter& c)
        : m_count(c.m_count), m_tot(c.m_tot), m_max(c.m_max) {
      }
      /// Destructor
      ~Counter() {
      }
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

} /* End namespace DD4hep             */
#endif    /* DD4HEP_GEOMETRY_INSTANCECOUNT_H     */
