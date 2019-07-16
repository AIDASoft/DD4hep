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
#ifndef DD4HEP_DDDIGI_DIGIACTION_H
#define DD4HEP_DDDIGI_DIGIACTION_H

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/Callback.h"
#include "DD4hep/ComponentProperties.h"
#include "DDDigi/DigiContext.h"

// C/C++ include files
#include <string>
#include <cstdarg>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

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
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class TypeName : public std::pair<std::string, std::string> {
    public:
      /// Default constructor
      TypeName() = default;
      /// Copy constructor
      TypeName(const TypeName& copy) = default;
      /// Copy constructor from pair
      TypeName(const std::pair<std::string, std::string>& c)
        : std::pair<std::string, std::string>(c) {      }
      /// Initializing constructor
      TypeName(const std::string& typ, const std::string& nam)
        : std::pair<std::string, std::string>(typ, nam) {      }
      /// Assignment operator
      TypeName& operator=(const TypeName& copy) = default;
      /// Split string pair according to default delimiter ('/')
      static TypeName split(const std::string& type_name);
      /// Split string pair according to custom delimiter
      static TypeName split(const std::string& type_name, const std::string& delim);
    };

    /// Default base class for all Digitizer actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class DigiAction {
      friend class DigiKernel;

    protected:
      /// Reference to the Digi context
      const DigiKernel&  m_kernel;
      /// Action name
      std::string        m_name;
      /// Property pool
      PropertyManager    m_properties;
      /// Reference count. Initial value: 1
      long               m_refCount    = 1;
      /// Default property: Output level
      int                m_outputLevel = 3;
      ///
      
      /// Property: Support parallel execution
      bool               m_parallel    = false;

    protected:

      /// Functor to access elements by name
      struct FindByName  {
        std::string _n;
        FindByName(const std::string& n) : _n(n) {}
        bool operator()(const DigiAction* a) { return a->name() == _n; }
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
        Actors() = default;
        ~Actors()  = default;
        size_t size()    const        { return m_v.size();              }
        void clear()                  { m_v.clear();                    }
        void add(T* obj)              { m_v.emplace_back(obj);          }
        void add_front(T* obj)        { m_v.insert(m_v.begin(), obj);   }
        const typename _V::value_type& operator[](size_t i)  const
        {  return m_v[i];}
        typename _V::value_type& operator[](size_t i)  {  return m_v[i];}
        operator const _V&() const    { return m_v;                     }
        operator _V&()                { return m_v;                     }
        const _V* operator->() const  { return &m_v;                    }
        _V* operator->()              { return &m_v;                    }
        typename _V::iterator begin() { return m_v.begin();             }
        typename _V::iterator end()   { return m_v.end();               }
        typename _V::const_iterator begin() const { return m_v.begin(); }
        typename _V::const_iterator end()   const { return m_v.end();   }
        
        /// Context updates
        void updateContext(DigiContext* ctxt)  {
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
        template <typename R, typename Q, typename A0> void operator()(R (Q::*pmf)(A0&), A0& a0) {
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
        template <typename R, typename Q, typename A0> void operator()(R (Q::*pmf)(A0&) const, A0& a0) const {
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

    protected:
      /// Inhibit default constructor
      DigiAction() = delete;
      /// Inhibit copy constructor
      DigiAction(const DigiAction& copy) = delete;
      /// Inhibit move constructor
      DigiAction(DigiAction&& copy) = delete;
      /// Inhibit assignment operator
      DigiAction& operator=(const DigiAction& copy) = delete;
      /// Inhibit assignment operator
      DigiAction& operator=(DigiAction&& copy) = delete;

      /// Default destructor
      virtual ~DigiAction();

    public:
      /// Standard constructor
      DigiAction(const DigiKernel& kernel, const std::string& nam);
      /// Increase reference count
      long addRef();
      /// Decrease reference count. Implicit destruction
      long release();
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
      /// Access parallization flag
      bool executeParallel()  const  {
        return m_parallel;
      }      
      /// Set the parallization flag; returns previous value
      bool setExecuteParallel(bool new_value);
      
      /// Declare property
      template <typename T> DigiAction& declareProperty(const std::string& nam, T& val);
      /// Declare property
      template <typename T> DigiAction& declareProperty(const char* nam, T& val);
      /// Check property for existence
      bool hasProperty(const std::string& name) const;
      /// Access single property
      Property& property(const std::string& name);
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

      /// Main functional callback
      virtual void execute(DigiContext& context)   const = 0;
    };

    /// Declare property
    template <typename T> DigiAction& DigiAction::declareProperty(const std::string& nam, T& val) {
      m_properties.add(nam, val);
      return *this;
    }

    /// Declare property
    template <typename T> DigiAction& DigiAction::declareProperty(const char* nam, T& val) {
      m_properties.add(nam, val);
      return *this;
    }
  }    // End namespace digi
}      // End namespace dd4hep

#endif // DD4HEP_DDDIGI_DIGIACTION_H
