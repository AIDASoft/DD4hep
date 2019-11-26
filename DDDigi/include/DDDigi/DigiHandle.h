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
#ifndef DD4HEP_DDDIGI_DIGIHANDLE_H
#define DD4HEP_DDDIGI_DIGIHANDLE_H

// Framework include files
#include "DD4hep/ComponentProperties.h"
#include "DD4hep/Detector.h"

// C/C++ include files
#include <string>
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi  {

    /// Forward declarations
    class DigiKernel;
    class DigiAction;
    class DigiEventAction;
    class DigiSignalProcessor;

    /// Handle to Digi actions with built-in creation mechanism
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    template <typename TYPE> class DigiHandle {
    protected:
      void checked_assign(TYPE* p);
      TYPE* null()  { return 0; }
    public:
      /// Pointer to referenced object
      mutable TYPE* value = 0;
      /// Default constructor
      explicit DigiHandle() = default;
      /// Construction initialized with object pointer
      DigiHandle(TYPE* typ);
      /// Cross type initialization
      template <typename T> DigiHandle(T* typ) : value(0) {
        checked_assign(dynamic_cast<TYPE*>(typ));
      }
      /// Copy constructor
      DigiHandle(const DigiHandle& handle);
      /// Move constructor
      DigiHandle(DigiHandle&& handle);
      /// Initializing constructor
      DigiHandle(const DigiKernel&, const char* type_name);
      /// Initializing constructor
      DigiHandle(const DigiKernel&, const std::string& type_name);
      /// Default destructor
      ~DigiHandle();
      /// Property accessor
      Property& operator[](const std::string& property_name) const;
      /// Assignment operator
      DigiHandle& operator=(const DigiHandle& handle);
      /// Move assignment operator
      DigiHandle& operator=(DigiHandle&& handle);
      /// Assignment operator
      DigiHandle& operator=(TYPE* ptr);
      /// Validity check
      bool operator!() const;
      /// Access to the underlying object
      DigiAction* action() const;
      /// Access to the underlying object
      TYPE* operator->() const;
      /// Conversion operator
      operator TYPE*() const;
      /// Access to the underlying object
      TYPE* get() const;
      /// Release the underlying object
      TYPE* release();
    };

    /// Handle to Digi actions with built-in creation mechanism
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class KernelHandle {
    public:
      /// Pointer to referenced object
      mutable DigiKernel* value;
      /// Default constructor
      explicit KernelHandle();
      /// Construction initialized with object pointer
      explicit KernelHandle(DigiKernel* k);
      /// Copy constructor
      KernelHandle(const KernelHandle& k) : value(k.value) {}
      /// Default destructor
      ~KernelHandle()                  {               }
      /// Conversion operator
      operator DigiKernel*() const   { return value; }
      /// Access to the underlying object
      DigiKernel* get() const        { return value; }
      /// Access to the underlying object
      DigiKernel* operator->() const { return value; }
      /// Access to worker thread
      KernelHandle worker();
      /// Destroy referenced object (program termination)
      void destroy();
    };

  }    // End namespace digi
}      // End namespace dd4hep

#endif // DD4HEP_DDDIGI_DIGIHANDLE_H
