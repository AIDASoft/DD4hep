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

#ifndef DETECTOR_DEIOV_H
#define DETECTOR_DEIOV_H

/// Gaudi namespace declaration
namespace gaudi   {

  /// Gaudi::detail namespace declaration
  namespace detail   {

    // Forward declarations
    class DeIOVObject;
    
    /// Base class for intervall of validity dependent DetectorElement data
    /**
     *  Need to store pointers in the child chache - 
     *  Handles are only defined below and cannot be used here.
     *  Though: The pointer can directly be assigned to handles.
     *
     *  Note:
     *  1) When populating the child chache it is the USER's responsibility
     *     To use ConditionsMaps with appropriate IOVs!
     *  2) The child chache may ONLY be populated while the condition is not
     *     active ie. during creation/initialization time. Otherwise
     *     MT races occur!
     *
     *  Concurrentcy notice:
     *  Except during filling, which is performed by the framework code,
     *  instances of this class are assumed to the read-only! 
     *  Thread safety hence is no issue.
     *
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    class DeIOVObject : public detail::ConditionObject    {
      DE_CONDITIONS_TYPEDEFS;
      typedef DeStatic::Object static_t;

    public:
      /// Helper to initialize the basic information
      DeIOVObject* fill_info(DetElement de, Catalog* cat);
      
    public: 
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeIOVObject);

      /// Initialization of sub-classes. May only be called while the condition is NOT active
      virtual void initialize();
      /// Fill the child cache. May only be called while the condition is NOT active
      void fillCache(ConditionsMap& m);

      /// Printout method to stdout
      virtual void print(int indent, int flags)  const;
      /// Check (enforce) the validity of the alignment object if required
      void checkAlignment()  const;
      /// Access daughter elements: IOV dependent part
      DeIOVObject* child(DetElement de)   const;

    public:
      /// The static part of the detector element
      DeStatic          de_static;
      /// Cache of static information of the children.
      std::map<DetElement,DeIOVObject*> childCache;
      /// The dd4hep detector element reference of this gaudi detector element
      DetElement        detector;
      /// All the time dependent conditions
      Conditions        conditions;
      /// The alignment object, which belongs to THIS detectoreleemnt
      Alignment         detectorAlignment;
      /// Alignments for daughter volumes not directly identified by a DetElement
      VolumeAlignments  volumeAlignments;

      /// We want to cache here the matrix from world to local (=inverse world2local)
      TGeoHMatrix       toLocalMatrix;
      /// We want to cache here the delta matrix
      TGeoHMatrix       deltaMatrix;
      /// Initialization flags to steer actions
      unsigned short    de_flags = 0;
      unsigned short    de_user  = 0;
      /// Item key
      itemkey_type      item_key = 0;

    public:

      /// Compute key value for caching
      static itemkey_type key(const std::string& value)
      {  return dd4hep::ConditionKey::itemCode(value);         }
      /// Compute key value for caching
      static itemkey_type key(const char* value)
      {  return dd4hep::ConditionKey::itemCode(value);         }

      /// Check if the condition identified by 'key' is in the list of conditionrefs.
      bool hasCondition(itemkey_type key) const
      {   return this->condition(key, false).isValid();        }
      /// Access condition by hash-key (fast)
      Condition condition(itemkey_type key)  const;
      /// Access condition by hash-key (fast)
      Condition condition(itemkey_type key, bool throw_if)  const;

      /// Check if the condition called 'name' is in the list of conditionrefs.
      bool hasCondition(const std::string& nam) const
      {   return this->condition(nam, false).isValid();        }
      /// Access condition by name (slow)
      Condition condition(const std::string& name)  const;
      /// Access condition by name (slow)
      Condition condition(const std::string& name, bool throw_if)  const;
     
      /// Local -> Global and Global -> Local transformations
      XYZPoint toLocal( const XYZPoint& global ) const
      {  return XYZPoint(detectorAlignment.worldToLocal(XYZVector(global)));     }
      XYZPoint toGlobal( const XYZPoint& local ) const
      {  return XYZPoint(detectorAlignment.localToWorld(XYZVector(local)));      }
      XYZVector toLocal( const XYZVector& globalDirection ) const
      {  return detectorAlignment.worldToLocal(globalDirection);                 }
      XYZVector toGlobal( const XYZVector& localDirection  ) const
      {  return detectorAlignment.localToWorld(localDirection);                  }
    };
  }    // End namespace detail

  /// Geometry access
  /**
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeIOVElement : public dd4hep::Handle<detail::DeIOVObject>  {
    DE_CONDITIONS_TYPEDEFS;
    /// Forward definition of the static type for facades
    typedef detail::DeStaticObject static_t;
    typedef detail::DeIOVObject    iov_t;

  public:
    /// Standard handle assignments and constructors
    DE_CTORS_HANDLE(DeIOVElement,Base);    
    /// Access to the static data
    static_t& staticData()  const  {  return access()->de_static;                }
  };
}      // End namespace gaudi

#endif // DETECTOR_DEIOV_H
