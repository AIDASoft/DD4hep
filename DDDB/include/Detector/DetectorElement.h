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
#ifndef DETECTOR_DETECTORELEMENT_H 
#define DETECTOR_DETECTORELEMENT_H 1

// Framework include files
#include "DD4hep/Objects.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetElement.h"
#include "DD4hep/detail/ConditionsInterna.h"
#include "Math/Vector3D.h"
#include "Math/Point3D.h"
#include "Detector/ParameterMap.h"

// C/C++ include files


namespace dd4hep { namespace DDDB { class DDDBCatalog; }}


/// Gaudi namespace declaration
namespace gaudi   {

  typedef ROOT::Math::XYZPoint      XYZPoint;
  typedef ROOT::Math::XYZVector     XYZVector;
  typedef ROOT::Math::Translation3D Translation3D;

#define DE_CONDITIONS_TYPEDEFS                              \
  public:                                                   \
  typedef dd4hep::DDDB::DDDBCatalog        Catalog;         \
  typedef ParameterMap::Parameter          Parameter;       \
  typedef dd4hep::DetElement               DetElement;      \
  typedef dd4hep::Condition                Condition;       \
  typedef dd4hep::ConditionsMap            ConditionsMap;   \
  typedef dd4hep::PlacedVolume             PlacedVolume;    \
  typedef dd4hep::Alignment                Alignment;       \
  typedef Condition::itemkey_type          itemkey_type;    \
  typedef std::map<itemkey_type,Condition> Conditions;      \
  typedef std::map<PlacedVolume,Alignment> VolumeAlignments

  using dd4hep::yes_no;
  using dd4hep::except;
  using dd4hep::printout;
  using dd4hep::DEBUG;
  using dd4hep::INFO;
  using dd4hep::ERROR;
  
#define DE_CTORS_DEFAULT(X)                     \
  X() = default;                                \
  X(const X& c) = delete;                       \
  virtual ~X() = default;                       \
  X& operator=(const X& c) = delete

#define DE_CTORS_HANDLE(X,B)                                    \
  X() = default;                                                \
  X(const X& c) = default;                                      \
  explicit X(Object* p) : B(p) {}                               \
  template <typename Q> X(const dd4hep::Handle<Q>& e) : B(e) {} \
  X& operator=(const X& c) = default;                           \
  X& operator=(Object* p)  { return (*this = X(p));  }          \
  ~X() = default
  
  namespace DE  {
    std::string indent(int level);
  }
  namespace DePrint {
    enum PrintFlags { BASICS     = 1,
                      PARAMS     = 2,
                      DETAIL     = 4,
                      SPECIFIC   = 5,
                      STATIC     = 6,
                      ALIGNMENTS = 7,
                      ALL        = BASICS|PARAMS|DETAIL|SPECIFIC|STATIC
    };
  }
  namespace DeInit {
    enum InitFlags  { FILLCACHE = 1,
                      INITIALIZED = 1<<31
    };
  }
  struct DeHelpers {
    DE_CONDITIONS_TYPEDEFS;
    enum { ALL = 1<<31 };
    template<typename T> static std::map<DetElement, T*>
    getChildConditions(ConditionsMap& m, DetElement de, itemkey_type key, int flags);
  };
  
  /// Gaudi::detail namespace declaration
  namespace detail   {

    /// We want to have the base in the local namespace
    typedef dd4hep::detail::ConditionObject ConditionObject;
    
    /// Generic Detector element data object combining static and iov data
    /**
     *
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    template <typename STATIC, typename IOV>
    class DeObject : public ConditionObject    {
      DE_CONDITIONS_TYPEDEFS;
      typedef STATIC   static_t;
      typedef IOV      iov_t;
      static_t         de_static;
      iov_t            de_iov;
    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeObject);
    };
  }

  /// Static identifiers computed once. Use the hash code whenever possible!
  struct Keys  {
    typedef dd4hep::Condition::key_type     key_type;
    typedef dd4hep::Condition::itemkey_type itemkey_type;
    
    /// Static key name: "DetElement-Info-Static"
    static const std::string  staticKeyName;
    /// Static key: 32 bit hash of "DetElement-Info-Static". Must be unique for one DetElement
    static const itemkey_type staticKey;

    /// Static key name: "DetElement-Info-IOV"
    static const std::string  deKeyName;
    /// Static key: 32 bit hash of "DetElement-Info-IOV". Must be unique for one DetElement
    static const itemkey_type deKey;

    /// Static key name: "Alignments-Computed"
    static const std::string alignmentsComputedKeyName;
    /// Static key: 32 bit hash of "Alignments-Computed". Must be unique for the world
    static const key_type alignmentsComputedKey;
  };
}      // End namespace gaudi

#include "Detector/DeStatic.h"
#include "Detector/DeIOV.h"

/// Gaudi::detail namespace declaration
namespace gaudi    {
  
  /// Main detector element class
  /**
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   *
   */
  template<typename TYPE>
  class DetectorElement : public TYPE {
    DE_CONDITIONS_TYPEDEFS;
    typedef DetectorElement<TYPE>   self_t;
    typedef TYPE                    base_t;
    typedef typename TYPE::Object            iov_t;
    typedef typename TYPE::static_t          static_t;
    typedef typename DetElement::Children    children_t;

  private:
    /// Static detelement accessor. Used internally - may be specialized for optimization
    const static_t& static_data()  const;
    /// Access the time dependent data block. Used internally - may be specialized for optimization
    const iov_t& iovData()  const;

    base_t& base()  { return *this; }
  public: 
    /// Standard constructor
    DetectorElement() = default;
    /// Copy constructoe
    DetectorElement(const DetectorElement&) = default;
    /// Copy assignment
    DetectorElement& operator=(const DetectorElement&) = default;
    /// Constructor from base class pointer
    DetectorElement(const typename base_t::Object* p) : base_t(p) {}
    /// Constructor from other polymorph pointers
    template <typename Q> DetectorElement(Q* p) : base_t(dd4hep::Handle<Q>(p)) {}
    /// Constructor from other polymorph handle
    template <typename Q> DetectorElement(const dd4hep::Handle<Q>& p) : base_t() {
      base() = p;
      if ( p.ptr() && !this->ptr() ) { this->bad_assignment(typeid(p),typeid(self_t)); }
    }
    /// Copy assignment from polymorph handle
    template <typename Q> DetectorElement& operator=(const dd4hep::Handle<Q>& p) {
      base() = p;
      if ( p.ptr() && !this->ptr() ) { this->bad_assignment(typeid(p),typeid(self_t)); }
      return *this;
    }
    
    /** Access to static detector element data                                */
    /// Detector element Class ID
    int classID()  const
    {  return static_data().classID;                                            }
    /// Accessor to detector structure
    DetElement   detector() const
    {  return static_data().detector;                                           }
    /// Accessor to the geometry structure of this detector element
    PlacedVolume geometry() const
    {  return static_data().geometry;                                           }
    /// Accessor to the parameter map
    const ParameterMap& params()   const
    {  return static_data().parameters;                                         }
    /// Access single parameter
    const Parameter& parameter(const std::string& nam, bool throw_if_not_present=true)   const
    {   return params().parameter(nam, throw_if_not_present);                   }
    /// Type dependent accessor to a named parameter
    template <typename T> T param(const std::string& nam, bool throw_if_not_present=true)   const
    {   return parameter(nam, throw_if_not_present).template get<T>();          }
    /// Access the parent detector element
    DetElement parent()  const
    {   return static_data().detector.parent();                                 }
    /// Access the children detector elements
    children_t children()  const
    {   return static_data().detector.children();                               }

    /** Access to IOV dependent data                                           */
    /// Check if the condition called 'name' is in the list of conditionrefs.
    bool hasCondition(const std::string& nam) const
    {   return iovData().condition(nam).isValid();                              }
    /// Access all conditions which belong to this detector element
    const Conditions& conditions()  const
    {  return iovData().conditions;                                             }
    /// Return the SmartRef for the condition called 'name'. Throw exception on failure
    Condition condition(const std::string& nam) const
    {  return iovData().condition(nam, true);                                   }
    /// Return the SmartRef for the condition called 'name'. Throw exception if requested.
    Condition condition(const std::string& nam, bool throw_if) const
    {  return iovData().condition(nam, throw_if);                               }
    /// Access to the alignmant object to transformideal coordinates 
    Alignment detectorAlignment()   const
    {  return iovData().detectorAlignment;                                      }
    /// Access the volume alignments
    const VolumeAlignments& volumeAlignments()  const
    {  return iovData().volumeAlignments();                                     }

    /// helper member using IGeometryInfo::isInside
    bool isInside(const XYZPoint& globalPoint) const
    {  return iovData().isInside(static_data(), globalPoint);                   } 

    /** Access to more sophisticated geometry information                      */
    /// Check if the geometry is connected to a logical volume
    bool hasLVolume() const
    {  return geometry().volume().isValid();                                    }
    /// Check if the geometry is connected to a supporting parent detector element
    bool hasSupport() const
    {  return detector().parent().isValid();                                    }

    /// Access to transformation matrices
    const TGeoHMatrix& toLocalMatrix() const
    {  return iovData().toLocalMatrix();                                        }
    const TGeoHMatrix& toGlobalMatrix() const 
    {  return iovData().toGlobalMatrix();                                       }
    const TGeoHMatrix& toLocalMatrixNominal() const
    {  return iovData().toLocalMatrixNominal();                                 }

    /// Local -> Global and Global -> Local transformations
    XYZPoint toLocal( const XYZPoint& global ) const
    {  return iovData().toLocal(global);                                        }
    XYZPoint toGlobal( const XYZPoint& local ) const
    {  return iovData().toGlobal(local);                                        }
    XYZVector toLocal( const XYZVector& globalDirection ) const
    {  return iovData().toLocal(globalDirection);                               }
    XYZVector toGlobal( const XYZVector& localDirection  ) const
    {  return iovData().toGlobal(localDirection);                               }
  };
  
  /// Static detelement accessor. Used internally - may be specialized for optimization
  template <typename TYPE> inline
  const typename DetectorElement<TYPE>::static_t&
  DetectorElement<TYPE>::static_data()  const
  {  return this->TYPE::staticData();                                           }

  /// Access the time dependent data block. Used internally - may be specialized for optimization
  template <typename TYPE> inline
  const typename DetectorElement<TYPE>::iov_t&
  DetectorElement<TYPE>::iovData()  const
  {  return *(this->TYPE::access());                                            }
  
}      // End namespace gaudi
#endif
