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
#ifndef DD4HEP_DDDIGI_DIGIDATA_H
#define DD4HEP_DDDIGI_DIGIDATA_H

/// Framework include files
#include "DD4hep/Any.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/ObjectExtensions.h"

/// C/C++ include files
#include <functional>
#include <stdexcept>
#include <cstdint>
#include <memory>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    class DigiEvent;

    /// Container class to host energy deposits from simulation or noise processing
    /*
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    template <typename T> class DigiContainer : public std::vector<T>    {
      /// Unique name within the event
      std::string    name;

    public:
      /// Default constructor
      DigiContainer() = default;
      /// Disable move constructor
      DigiContainer(DigiContainer&& copy) = default;
      /// Disable copy constructor
      DigiContainer(const DigiContainer& copy) = default;      
      /// Default constructor
      DigiContainer(const std::string& nam) : name(nam) {}
      /// Default destructor
      virtual ~DigiContainer() = default;
      /// Disable move assignment
      DigiContainer& operator=(DigiContainer&& copy) = delete;
      /// Disable copy assignment
      DigiContainer& operator=(const DigiContainer& copy) = delete;      
    };
    
    /// 
    /*
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class EnergyDeposit   {
    public:
      class FunctionTable   {
        friend class EnergyDeposit;
      public:
        std::function<long long int(const void*)> cellID;
        std::function<long(const void*)>          flag;
        FunctionTable() = default;
        ~FunctionTable() = default;
      };
      template <typename Q, typename T> static const Q* vtable();
      std::pair<const void*, const FunctionTable*> object;

    public:
      /// Initializing constructor
      template <typename T> EnergyDeposit(const T* object);
      /// Default constructor
      EnergyDeposit() = delete;
      /// Disable move constructor
      EnergyDeposit(EnergyDeposit&& copy) = default;
      /// Disable copy constructor
      EnergyDeposit(const EnergyDeposit& copy) = default;      
      /// Default destructor
      virtual ~EnergyDeposit() = default;
      /// Disable move assignment
      EnergyDeposit& operator=(EnergyDeposit&& copy) = default;
      /// Disable copy assignment
      EnergyDeposit& operator=(const EnergyDeposit& copy) = default;      

      long long int cellID()  const    {   return object.second->cellID(object.first);     }
      long          flag()  const      {   return object.second->flag(object.first);       }
    };

    template <typename T> inline EnergyDeposit::EnergyDeposit(const T* ptr)
      : object(ptr, vtable<EnergyDeposit::FunctionTable,T>())
    {
    }

    /// 
    /*
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class TrackerDeposit : public EnergyDeposit   {
    public:
      class FunctionTable : public  EnergyDeposit::FunctionTable  {
        friend class TrackerDeposit;
        friend class EnergyDeposit;
      public:
        std::function<const Position& (const void*)>  position;
        std::function<const Direction& (const void*)> momentum;
        std::function<double(const void*)>            deposit;
        std::function<double(const void*)>            length;
        FunctionTable() = default;
        ~FunctionTable() = default;
      };
      std::pair<const void*, const FunctionTable*> object;

    public:
      /// Initializing constructor
      template <typename T> TrackerDeposit(const T* object);
      /// Default constructor
      TrackerDeposit() = delete;
      /// Disable move constructor
      TrackerDeposit(TrackerDeposit&& copy) = default;
      /// Disable copy constructor
      TrackerDeposit(const TrackerDeposit& copy) = default;      
      /// Default destructor
      virtual ~TrackerDeposit() = default;
      /// Disable move assignment
      TrackerDeposit& operator=(TrackerDeposit&& copy) = default;
      /// Disable copy assignment
      TrackerDeposit& operator=(const TrackerDeposit& copy) = default;      

      const Position&  position()  const  {   return object.second->position(object.first);     }
      const Direction& momentum()  const  {   return object.second->momentum(object.first);     }
      double           deposit()  const   {   return object.second->deposit(object.first);      }
      double           length()  const    {   return object.second->length(object.first);       }
    };

    template <typename T> inline TrackerDeposit::TrackerDeposit(const T* ptr)
      : object(ptr, vtable<TrackerDeposit::FunctionTable,T>())
    {
    }

    /// 
    /*
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class CaloDeposit : public EnergyDeposit   {
    public:
      class FunctionTable : public  EnergyDeposit::FunctionTable  {
        friend class CaloDeposit;
        friend class EnergyDeposit;
      public:
        std::function<const Position& (const void*)>  position;
        std::function<double(const void*)>            deposit;
        FunctionTable() = default;
        ~FunctionTable() = default;
      };
      std::pair<const void*, const FunctionTable*> object;

    public:
      /// Initializing constructor
      template <typename T> CaloDeposit(const T* object);
      /// Default constructor
      CaloDeposit() = delete;
      /// Disable move constructor
      CaloDeposit(CaloDeposit&& copy) = default;
      /// Disable copy constructor
      CaloDeposit(const CaloDeposit& copy) = default;      
      /// Default destructor
      virtual ~CaloDeposit() = default;
      /// Disable move assignment
      CaloDeposit& operator=(CaloDeposit&& copy) = default;
      /// Disable copy assignment
      CaloDeposit& operator=(const CaloDeposit& copy) = default;      

      const Position& position()  const {   return object.second->position(object.first);    }
      double          deposit()  const  {   return object.second->deposit(object.first);     }
    };

    template <typename T> inline CaloDeposit::CaloDeposit(const T* ptr)
      : object(ptr, vtable<CaloDeposit::FunctionTable,T>())
    {
    }
    
    /// 
    /*
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiCount   {
    public:
      /// Default constructor
      DigiCount() = default;
      /// Default move constructor
      DigiCount(DigiCount&& copy) = default;
      /// Default copy constructor
      DigiCount(const DigiCount& copy) = default;
      /// Default destructor
      virtual ~DigiCount() = default;
      /// Default move assignment
      DigiCount& operator=(DigiCount&& copy) = delete;
      /// Default copy assignment
      DigiCount& operator=(const DigiCount& copy) = delete;      
    };

    typedef DigiContainer<EnergyDeposit*> DigiEnergyDeposits;
    typedef DigiContainer<DigiCount*>     DigiCounts;

    ///  Key defintion to access the event data
    /**
     *  Helper to convert item and mask to a 64 bit integer
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    union Key   {
      typedef std::uint64_t key_type;
      typedef std::uint32_t itemkey_type;
      typedef std::uint8_t  mask_type;
      key_type key;
      struct {
        itemkey_type item;
        mask_type    mask;
        mask_type    spare[3];
      } values;
      Key();
      Key(const Key&);
      Key(mask_type mask, itemkey_type item);
      Key(mask_type mask, const std::string& item);
      Key& operator=(const Key&);
      key_type toLong()  const  {  return key; }
      void set(const std::string& name, int mask);
    };
    
    ///  User event data for DDDigi
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class  DigiEvent : public ObjectExtensions  {
    public:
      /// Forward definition of the key type
      typedef Key::key_type key_type;
      std::map<unsigned long, std::shared_ptr<DigiEnergyDeposits> >  energyDeposits;
      std::map<unsigned long, std::shared_ptr<DigiCounts> >          digitizations;

      int eventNumber = 0;
      std::map<key_type, dd4hep::any>  data;

    public:
#if defined(G__ROOT) || defined(__CLING__) || defined(__ROOTCLING__)
      /// Inhibit default constructor
      DigiEvent();
#endif
      /// Inhibit move constructor
      DigiEvent(DigiEvent&& copy) = delete;
      /// Inhibit copy constructor
      DigiEvent(const DigiEvent& copy) = delete;
      /// Intializing constructor
      DigiEvent(int num);
      /// Default destructor
      virtual ~DigiEvent();
      /// Add item by key to the data 
      template<typename T> bool put(const Key& key, T&& object)     {
        bool ret = data.emplace(key.toLong(),make_any<T>(object)).second;
        if ( ret ) return ret;
        except("DigiEvent","Invalid requested to store data in event container. Key:%ld",key.toLong());
        throw std::runtime_error("DigiEvent"); // Will never get here!
      }

      /// Retrieve item by key from the event data container
      template<typename T> T& get(const Key& key)     {
        auto iter = data.find(key.toLong());
        if ( iter != data.end() )  {
          T* ptr = dd4hep::any_cast<T>(&(*iter).second);
          if ( ptr ) return *ptr;
          except("DigiEvent","Invalid data requested from event container [cast failed]. Key:%ld",key.toLong());
        }
        except("DigiEvent","Invalid data requested from event container. Key:%ld",key.toLong());
        throw std::runtime_error("DigiEvent"); // Will never get here!
      }

      /// Retrieve item by key from the event data container
      template<typename T> const T& get(const Key& key)  const    {
        std::map<key_type, dd4hep::any>::const_iterator iter = data.find(key.toLong());
        if ( iter != data.end() )  {
          const T* ptr = dd4hep::any_cast<T>(&(*iter).second);
          if ( ptr ) return *ptr;
          except("DigiEvent","Invalid data requested from event container [cast failed]. Key:%ld",key.toLong());
        }
        except("DigiEvent","Invalid data requested from event container. Key:%ld",key.toLong());
        throw std::runtime_error("DigiEvent"); // Will never get here!
      }

      /// Add an extension object to the detector element
      void* addExtension(unsigned long long int k, ExtensionEntry* e)  {
        return ObjectExtensions::addExtension(k, e);
      }
      /// Add user extension object. Ownership is transferred and object deleted at the end of the event.
      template <typename T> T* addExtension(T* ptr, bool take_ownership=true)   {
        ExtensionEntry* e = take_ownership
          ? (ExtensionEntry*)new detail::DeleteExtension<T,T>(ptr)
          : (ExtensionEntry*)new detail::SimpleExtension<T,T>(ptr);
        return (T*)ObjectExtensions::addExtension(detail::typeHash64<T>(),e);
      }
      /// Access to type safe extension object. Exception is thrown if the object is invalid
      template <typename T> T* extension(bool alert=true) {
        return (T*)ObjectExtensions::extension(detail::typeHash64<T>(),alert);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#endif // DD4HEP_DDDIGI_DIGIDATA_H
