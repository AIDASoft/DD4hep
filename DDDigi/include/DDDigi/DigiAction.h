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
#ifndef DDDIGI_DIGIACTION_H
#define DDDIGI_DIGIACTION_H

// Framework include files
#include <DD4hep/Printout.h>
#include <DD4hep/ComponentProperties.h>
#include <DDDigi/DigiContext.h>

// C/C++ include files
#include <string>
#include <cstdarg>

#if defined(G__ROOT) || defined(__CLING__) || defined(__ROOTCLING__)
#define DDDIGI_DEFINE_ACTION_DEFAULT_CTOR(action)  public: action() = default;
#else
#define DDDIGI_DEFINE_ACTION_DEFAULT_CTOR(action)  protected: action() = delete;
#endif

/// 1) Allow default constructor (necessary for ROOT)
/// 2) Inhibit move constructor
/// 3) Inhibit copy constructor
/// 4) Inhibit move operator
/// 5) Inhibit assignment operator
#define DDDIGI_DEFINE_ACTION_CONSTRUCTORS(action)  \
  DDDIGI_DEFINE_ACTION_DEFAULT_CTOR(action)        \
  protected:                                       \
  action(action&& copy) = delete;                  \
  action(const action& copy) = delete;             \
  action& operator=(action&& copy) = delete;       \
  action& operator=(const action& copy) = delete

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

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
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiAction {
      friend class DigiKernel;

    protected:
      /// Reference to the Digi context
#if defined(G__ROOT) || defined(__CLING__) || defined(__ROOTCLING__)
      const DigiKernel*  m_kernel;
    public:
      const DigiKernel*  kernel()  const   {
	return m_kernel;
      }
    protected:
#else
      const DigiKernel&  m_kernel;
#endif
      /// Action name
      std::string        m_name;
      /// Property pool
      PropertyManager    m_properties;
      /// Reference count. Initial value: 1
      long               m_refCount    = 1;
      /// Default property: Output level
      int                m_outputLevel = 3;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiAction);

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
      /// Support for building formatted messages
      std::string format(const char* fmt, ...) const;
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

#endif // DDDIGI_DIGIACTION_H
