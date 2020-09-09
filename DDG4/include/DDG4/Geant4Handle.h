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

#ifndef DDG4_GEANT4HANDLE_H
#define DDG4_GEANT4HANDLE_H

// Framework include files
#include "DD4hep/ComponentProperties.h"
#include "DD4hep/Detector.h"

// C/C++ include files
#include <string>
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

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
      TYPE* null()  { return 0; }
    public:
      /// Pointer to referenced object
      mutable TYPE* value = 0;
      /// Default constructor
      explicit Geant4Handle() = default;
      /// Construction initialized with object pointer
      Geant4Handle(TYPE* typ);
      /// Cross type initialization
      template <typename T> Geant4Handle(T* typ) : value(0) {
        checked_assign(dynamic_cast<TYPE*>(typ));
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
      Geant4Handle& operator=(TYPE* ptr);
      /// Validity check
      bool operator!() const;
      /// Access to the underlying object
      Geant4Action* action() const;
      /// Access to the underlying object
      TYPE* operator->() const;
      /// Conversion operator
      operator TYPE*() const;
      /// Access to the underlying object
      TYPE* get() const;
      /// Release the underlying object
      TYPE* release();
    };

    /// Handle to Geant4 actions with built-in creation mechanism
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class KernelHandle {
    public:
      /// Pointer to referenced object
      mutable Geant4Kernel* value;
      /// Default constructor
      explicit KernelHandle();
      /// Construction initialized with object pointer
      explicit KernelHandle(Geant4Kernel* k);
      /// Copy constructor
      KernelHandle(const KernelHandle& k) : value(k.value) {}
      /// Default destructor
      ~KernelHandle()                  {               }
      /// Conversion operator
      operator Geant4Kernel*() const   { return value; }
      /// Access to the underlying object
      Geant4Kernel* get() const        { return value; }
      /// Access to the underlying object
      Geant4Kernel* operator->() const { return value; }
      /// Access to worker thread
      KernelHandle worker();
      /// Destroy referenced object (program termination)
      void destroy();
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif // DDG4_GEANT4HANDLE_H
