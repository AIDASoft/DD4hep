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

#ifndef DD4HEP_DDG4_GEANT4SETUP_H
#define DD4HEP_DDG4_GEANT4SETUP_H

// Framework include files
#include "DD4hep/ComponentProperties.h"
#include "DD4hep/LCDD.h"

// C/C++ include files
#include <string>
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Forward declarations
    class Geant4Kernel;
    class Geant4Action;

    /// Handle to Geant4 actions with built-in creation mechanism
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    template <typename TYPE> class Geant4Handle {
    protected:
      void checked_assign(TYPE* p);
    public:
      typedef TYPE handled_type;
      /// Pointer to referenced object
      mutable handled_type* value = 0;
      /// Default constructor
      explicit Geant4Handle() = default;
      /// Construction initialized with object pointer
      Geant4Handle(handled_type* typ);
      /// Cross type initialization
      template <typename T> Geant4Handle(T* typ) : value(0) {
        checked_assign(dynamic_cast<handled_type*>(typ));
      }
      /// Copy constructor
      Geant4Handle(const Geant4Handle& handle);
      /// Move constructor
      Geant4Handle(Geant4Handle&& handle);
      /// Initializing constructor
      Geant4Handle(Geant4Kernel&, const char* type_name, bool shared=false);
      /// Initializing constructor
      Geant4Handle(Geant4Kernel&, const std::string& type_name, bool shared=false);
      /// Constructor only implemented for sensitive objects
      Geant4Handle(Geant4Kernel& ctxt, const std::string& type_name, const std::string& detector, bool shared=false);
      /// Default destructor
      ~Geant4Handle();
      /// Property accessor
      Property& operator[](const std::string& property_name) const;
      /// Assignment operator
      Geant4Handle& operator=(const Geant4Handle& handle);
      /// Move assignment operator
      Geant4Handle& operator=(Geant4Handle&& handle);
      /// Assignment operator
      Geant4Handle& operator=(handled_type* ptr);
      /// Validity check
      bool operator!() const;
      /// Access to the underlying object
      Geant4Action* action() const;
      /// Access to the underlying object
      handled_type* operator->() const;
      /// Conversion operator
      operator handled_type*() const;
      /// Access to the underlying object
      handled_type* get() const;
      /// Release the underlying object
      handled_type* release();
    };

    /// Handle to Geant4 actions with built-in creation mechanism
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class KernelHandle {
    public:
      typedef Geant4Kernel handled_type;
      /// Pointer to referenced object
      mutable handled_type* value;
      /// Default constructor
      explicit KernelHandle();
      /// Construction initialized with object pointer
      explicit KernelHandle(Geant4Kernel* k);
      /// Copy constructor
      KernelHandle(const KernelHandle& k) : value(k.value) {}
      /// Default destructor
      ~KernelHandle()                  {               }
      /// Conversion operator
      operator handled_type*() const   { return value; }
      /// Access to the underlying object
      handled_type* get() const        { return value; }
      /// Access to the underlying object
      handled_type* operator->() const { return value; }
      /// Access to worker thread
      KernelHandle worker();
      /// Destroy referenced object (program termination)
      void destroy();
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4SETUP_H
