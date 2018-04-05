//==============================================================================
//  AIDA Detector description implementation for LHCb
//------------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2018-03-08
//  \version  1.0
//
//==============================================================================
#ifndef DE_CTORS_DEFAULT
#pragma error("This header should never be included directly. Inlcude Detector/DetectorElement.h instead!")
#endif

#ifndef DETECTOR_DESTATIC_H
#define DETECTOR_DESTATIC_H

/// Framework include files
#include "Detector/DetectorElement.h"

/// gaudi namespace declaration
namespace gaudi   {

  /// gaudi::detail namespace declaration
  namespace detail   {

    /// Forward declarations
    class DeStaticObject;

    /// Base class for static DetectorElement data
    /**
     *  The static data support all requests to a generic detector element
     *  if no time-dependent information is required. The time dependent
     *  information is stored in the IOV dependent part of the detector element.
     *
     *  Note:
     *  1) When populating the child chache it is the USER's responsibility
     *     To use ConditionsMaps with appropriate IOVs!
     *  2) The child chache may ONLY be populated while the condition is not
     *     active ie. during creation/initialization time. Otherwise
     *     MT races occur!
     *
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    class DeStaticObject : public detail::ConditionObject  {
      DE_CONDITIONS_TYPEDEFS;

    public:
      /// Helper to initialize the basic information
      DeStaticObject* fill_info(DetElement de, Catalog* cat);
        
    public: 
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeStaticObject);

      /// Initialization of sub-classes. May only be called while the condition is NOT active
      virtual void initialize();
      
      /// Printout method to stdout
      virtual void print(int indent, int flags)  const;

      /// Fill the child cache. May only be called while the condition is NOT active
      void fillCache(ConditionsMap& m);

      /// Access daughter elements: Static part
      DeStaticObject* child(DetElement de)  const;

      /** Simplification accessors. Do not check validity here   */
      /// Access parameters directory
      const ParameterMap::Parameters& params()  const;
      
      /// Access single parameter
      const ParameterMap::Parameter& parameter(const std::string& nam, bool throw_if_not_present=true)   const;

      /// Type dependent accessor to a named parameter
      template <typename T> T param(const std::string& nam, bool throw_if_not_present=true)   const
      {  return parameters.parameter(nam,throw_if_not_present).template get<T>();     }

    public:
      /// Cache of static information of the children.
      std::map<DetElement,DeStaticObject*> childCache;
      /// Pointer to parent
      DeStaticObject*      parent = 0;
      /// The dd4hep detector element reference of this gaudi detector element
      DetElement           detector;
      /// Reference to the geometry information of this gaudi detector element
      PlacedVolume         geometry;
      /// The parameter map of this detector element
      ParameterMap         parameters;
      /// Detector element Class ID
      int                  clsID = 0;
      /// Initialization flags to steer actions
      unsigned short       de_flags = 0;
      unsigned short       de_user  = 0;
      /// Item key
      itemkey_type         key = 0;

      /// Not really necessary and no code should depend on it, but helps for debugging!
      Catalog*             catalog = 0;
    };
  }    // End namespace detail

  /// Handle definition to an instance of a handle to static detector element data
  /**
   *  This object defines the behaviour of the objects's data.
   *  We implement here only the behaviour of the object specific
   *  stuff. The geometry interactions are then combined with this
   *  implementation and the specialized detector element
   *  DetectorElementStatic<TYPE> to the real data accessor.
   *  The DetectorElementStatic<TYPE> by non-virtual inheritance
   *  automatically exposes the specific stuff here.
   *
   *  See the corresponding typedef below.
   *
   *  Note: in this class the is no big deal of specialization!
   *        this for the time being is only for illustration about the mechanism.
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeStaticElement : public dd4hep::Handle<detail::DeStaticObject>  {
    DE_CONDITIONS_TYPEDEFS;
    /// This is needed by the DetectorElement<TYPE> to properly forward requests.
    typedef detail::DeStaticObject static_t;
  public:
    /// Standard handle assignments and constructors
    DE_CTORS_HANDLE(DeStaticElement,Base);
  };

}      // End namespace gaudi
#endif // DETECTOR_DESTATIC_H
