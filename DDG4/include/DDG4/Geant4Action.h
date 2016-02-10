#ifndef DD4HEP_DDG4_GEANT4ACTION_H
#define DD4HEP_DDG4_GEANT4ACTION_H

// Framework include files
// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/Printout.h"
#include "DD4hep/ComponentProperties.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Callback.h"

// Geant4 forward declarations
class G4Run;
class G4Event;
class G4Step;
class G4Track;
class G4TrackStack;
class G4EventGenerator;
class G4VTrajectory;
class G4TrackingManager;
class G4UIdirectory;

// C/C++ include files
#include <string>
#include <cstdarg>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4UIMessenger;

    /// Cast operator
    template <typename TO, typename FROM> TO fast_cast(FROM from) {
#ifdef USE_FASTCAST
      return static_cast<TO>(from);
#else
      return dynamic_cast<TO>(from);
#endif
    }

    /// Helper class to handle strings of the format "type/name"
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class TypeName : public std::pair<std::string, std::string> {
    public:
      /// Default constructor
      TypeName()
        : std::pair<std::string, std::string>() {
      }
      TypeName(const std::pair<std::string, std::string>& c)
        : std::pair<std::string, std::string>(c) {
      }
      TypeName(const std::string& typ, const std::string& nam)
        : std::pair<std::string, std::string>(typ, nam) {
      }
      /// Split string pair according to default delimiter ('/')
      static TypeName split(const std::string& type_name);
      /// Split string pair according to custom delimiter
      static TypeName split(const std::string& type_name, const std::string& delim);
    };

    /// Default base class for all Geant 4 actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Action {
    protected:
      /// Reference to the Geant4 context
      Geant4Context*     m_context;
      /// Control directory of this action
      Geant4UIMessenger* m_control;

      /// Default property: Output level
      int                m_outputLevel;
      /// Default property: Flag to create control instance
      bool               m_needsControl;
      /// Action name
      std::string        m_name;
      /// Property pool
      PropertyManager    m_properties;
      /// Reference count. Initial value: 1
      long               m_refCount;

    public:
      /// Functor to update the context of a Geant4Action object
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_SIMULATION
       */
      class ContextSwap   {
        /// reference to the context;
        Geant4Context* context;
        Geant4Action*  action;
      public:
        /// Constructor
        ContextSwap(Geant4Action* a,Geant4Context* c) : action(a)  {
          context = action->context();
          action->updateContext(c);
        }
        /// Destructor
        ~ContextSwap()  { 
          action->updateContext(context);
        }
      };

    protected:

      /// Functor to access elements by name
      struct FindByName  {
        std::string _n;
        FindByName(const std::string& n) : _n(n) {}
        bool operator()(const Geant4Action* a) { return a->name() == _n; }
      };
      /// Actor class to manipulate action groups
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_SIMULATION
       */
      template <typename T> class Actors {
      public:
        typedef typename std::vector<T*> _V;
        _V m_v;
        Actors() {
        }
        ~Actors() {
        }
        void clear() {
          m_v.clear();
        }
        void add(T* obj) {
          m_v.push_back(obj);
        }
        operator const _V&() const {
          return m_v;
        }
        operator _V&() {
          return m_v;
        }
        const _V* operator->() const {
          return &m_v;
        }
        _V* operator->() {
          return &m_v;
        }
        typename _V::iterator begin() { return m_v.begin(); }
        typename _V::iterator end()   { return m_v.end();   }
        typename _V::const_iterator begin() const  { return m_v.begin(); }
        typename _V::const_iterator end()   const  { return m_v.end();   }
        
        /// Context updates
        void updateContext(Geant4Context* ctxt)  {
          (*this)(&T::updateContext,ctxt);
        }
        /// Element access by name
        template <typename F> typename _V::value_type get(const F& f)  const   {
          if (!m_v.empty())  {
            typename _V::const_iterator i=std::find_if(m_v.begin(),m_v.end(),f);
            return i==m_v.end() ? 0 : (*i);
          }
          return 0;
        }
        /// NON-CONST actions
        template <typename R, typename Q> void operator()(R (Q::*pmf)()) {
          if (m_v.empty())
            return;
          for (typename _V::iterator i = m_v.begin(); i != m_v.end(); ++i)
            ((*i)->*pmf)();
        }
        template <typename R, typename Q, typename A0> void operator()(R (Q::*pmf)(A0), A0 a0) {
          if (m_v.empty())
            return;
          for (typename _V::iterator i = m_v.begin(); i != m_v.end(); ++i)
            ((*i)->*pmf)(a0);
        }
        template <typename R, typename Q, typename A0, typename A1> void operator()(R (Q::*pmf)(A0, A1), A0 a0, A1 a1) {
          if (m_v.empty())
            return;
          for (typename _V::iterator i = m_v.begin(); i != m_v.end(); ++i)
            ((*i)->*pmf)(a0, a1);
        }
        /// CONST actions
        template <typename R, typename Q> void operator()(R (Q::*pmf)() const) const {
          if (m_v.empty())
            return;
          for (typename _V::const_iterator i = m_v.begin(); i != m_v.end(); ++i)
            ((*i)->*pmf)();
        }
        template <typename R, typename Q, typename A0> void operator()(R (Q::*pmf)(A0) const, A0 a0) const {
          if (m_v.empty())
            return;
          for (typename _V::const_iterator i = m_v.begin(); i != m_v.end(); ++i)
            ((*i)->*pmf)(a0);
        }
        template <typename R, typename Q, typename A0, typename A1> void operator()(R (Q::*pmf)(A0, A1) const, A0 a0,
                                                                                    A1 a1) const {
          if (m_v.empty())
            return;
          for (typename _V::const_iterator i = m_v.begin(); i != m_v.end(); ++i)
            ((*i)->*pmf)(a0, a1);
        }
        /// CONST filters
        template <typename Q> bool filter(bool (Q::*pmf)() const) const {
          if (!m_v.empty())
            return true;
          for (typename _V::const_iterator i = m_v.begin(); i != m_v.end(); ++i)
            if (!((*i)->*pmf)())
              return false;
          return true;
        }
        template <typename Q, typename A0> bool filter(bool (Q::*pmf)(A0) const, A0 a0) const {
          if (m_v.empty())
            return true;
          for (typename _V::const_iterator i = m_v.begin(); i != m_v.end(); ++i)
            if (!((*i)->*pmf)(a0))
              return false;
          return true;
        }
        template <typename Q, typename A0, typename A1> bool filter(bool (Q::*pmf)(A0, A1) const, A0 a0, A1 a1) const {
          if (m_v.empty())
            return true;
          for (typename _V::const_iterator i = m_v.begin(); i != m_v.end(); ++i)
            if (!((*i)->*pmf)(a0, a1))
              return false;
          return true;
        }
      };

      /// Default destructor
      virtual ~Geant4Action();

    public:
      /// Standard constructor
      Geant4Action(Geant4Context* context, const std::string& nam);
      /// Increase reference count
      long addRef();
      /// Decrease reference count. Implicit destruction
      long release();
      /// Access the context
      Geant4Context* context()  const {
        return m_context;
      }
      /// Set or update client context
      virtual void updateContext(Geant4Context* ctxt)  {
        m_context = ctxt; 
      }
      /// Set or update client for the use in a new thread fiber
      virtual void configureFiber(Geant4Context* thread_context);
      /// Access name of the action
      const std::string& name() const {
        return m_name;
      }
      /// Access name of the action
      const char* c_name() const {
        return m_name.c_str();
      }
      /// Set the object name.
      void setName(const std::string& new_name) {
        m_name = new_name;
      }
      /// Access to the properties of the object
      PropertyManager& properties() {
        return m_properties;
      }
      /// Access the output level
      PrintLevel outputLevel() const  {
        return (PrintLevel)m_outputLevel;
      }
      /// Set the output level; returns previous value
      PrintLevel setOutputLevel(PrintLevel new_level);
      /// Access to the UI messenger
      Geant4UIMessenger* control() const;
      /// Enable and install UI messenger
      virtual void enableUI();
      /// Declare property
      template <typename T> Geant4Action& declareProperty(const std::string& nam, T& val);
      /// Declare property
      template <typename T> Geant4Action& declareProperty(const char* nam, T& val);
      /// Check property for existence
      bool hasProperty(const std::string& name) const;
      /// Access single property
      Property& property(const std::string& name);
      /// Set object properties
      Geant4Action& setProperties(PropertyConfigurator& setup);

      /// Install property control messenger if wanted
      virtual void installMessengers();
      /// Install command control messenger if wanted
      virtual void installCommandMessenger();
      /// Install property control messenger if wanted
      virtual void installPropertyMessenger();

      /// Support for messages with variable output level using output level
      void print(const char* fmt, ...) const;
      /// Support for messages with variable output level using output level-1
      void printM1(const char* fmt, ...) const;
      /// Support for messages with variable output level using output level-2
      void printM2(const char* fmt, ...) const;
      /// Support for messages with variable output level using output level+1
      void printP1(const char* fmt, ...) const;
      /// Support for messages with variable output level using output level+2
      void printP2(const char* fmt, ...) const;

      /// Support of debug messages.
      void debug(const char* fmt, ...) const;
      /// Support of info messages.
      void info(const char* fmt, ...) const;
      /// Support of warning messages.
      void warning(const char* fmt, ...) const;
      /// Support of error messages.
      void error(const char* fmt, ...) const;
      /// Action to support error messages.
      bool return_error(bool return_value, const char* fmt, ...) const;
      /// Support of fatal messages. Throws exception
      void fatal(const char* fmt, ...) const;
      /// Support of exceptions: Print fatal message and throw runtime_error.
      void except(const char* fmt, ...) const;

      /// Abort Geant4 Run by throwing a G4Exception with type RunMustBeAborted
      void abortRun(const std::string& exception, const char* fmt, ...) const;

      /// Access to the main run action sequence from the kernel object
      Geant4RunActionSequence& runAction() const;
      /// Access to the main event action sequence from the kernel object
      Geant4EventActionSequence& eventAction() const;
      /// Access to the main stepping action sequence from the kernel object
      Geant4SteppingActionSequence& steppingAction() const;
      /// Access to the main tracking action sequence from the kernel object
      Geant4TrackingActionSequence& trackingAction() const;
      /// Access to the main stacking action sequence from the kernel object
      Geant4StackingActionSequence& stackingAction() const;
      /// Access to the main generator action sequence from the kernel object
      Geant4GeneratorActionSequence& generatorAction() const;
    };

    /// Declare property
    template <typename T> Geant4Action& Geant4Action::declareProperty(const std::string& nam, T& val) {
      m_properties.add(nam, val);
      return *this;
    }

    /// Declare property
    template <typename T> Geant4Action& Geant4Action::declareProperty(const char* nam, T& val) {
      m_properties.add(nam, val);
      return *this;
    }
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4ACTION_H
