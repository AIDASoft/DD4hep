// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4SETUP_H
#define DD4HEP_DDG4_GEANT4SETUP_H

// Framework include files
#include "DDG4/ComponentProperties.h"
#include "DD4hep/LCDD.h"

// C/C++ include files
#include <string>
#include <memory>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /// Forward declarations
    class Geant4Kernel;
    class Geant4Action;

    /** @class Geant4Handle Geant4Handle.h DDG4/Geant4Handle.h
     *
     * Handle to Geant4 actions with built-in creation mechanism
     *
     * @author  M.Frank
     * @version 1.0
     */
    template <typename TYPE> struct Geant4Handle {
    protected:
      void checked_assign(TYPE* p);
    public:
      typedef TYPE handled_type;
      mutable handled_type* value;
      explicit Geant4Handle();
      Geant4Handle(handled_type* typ);
      template <typename T> Geant4Handle(T* typ)
          : value(0) {
        checked_assign(dynamic_cast<handled_type*>(typ));
      }
      Geant4Handle(const Geant4Handle& handle);
      Geant4Handle(const Geant4Kernel&, const std::string& type_name);
      /// Constructor only implemented for sensitive objects
      Geant4Handle(const Geant4Kernel& ctxt, const std::string& type_name, const std::string& detector);
      ~Geant4Handle();
      Property& operator[](const std::string& property_name) const;
      Geant4Handle& operator=(const Geant4Handle& handle);
      Geant4Handle& operator=(handled_type* ptr);
      bool operator!() const;
      operator handled_type*() const;
      handled_type* get() const;
      handled_type* release();
      handled_type* operator->() const;
      Geant4Action* action() const;
    };
    /** @class Geant4Handle Geant4Handle.h DDG4/Geant4Handle.h
     *
     * Handle to Geant4 actions with built-in creation mechanism
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct KernelHandle {
    public:
      typedef Geant4Kernel handled_type;
      mutable handled_type* value;
      explicit KernelHandle();
      ~KernelHandle()  {}
      operator handled_type*() const   { return value; }
      handled_type* get() const        { return value; }
      handled_type* operator->() const { return value; }
      void destroy();
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4SETUP_H
