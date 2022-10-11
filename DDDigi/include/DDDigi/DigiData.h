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
#ifndef DDDIGI_DIGIDATA_H
#define DDDIGI_DIGIDATA_H

/// Framework include files
#include "DD4hep/config.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/ObjectExtensions.h"

/// C/C++ include files
#include <functional>
#include <stdexcept>
#include <cstdint>
#include <memory>
#include <mutex>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <any>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class DigiEvent;
    class Particle;
    class ParticleMapping;
    class EnergyDeposit;
    class DepositMapping;

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
      typedef std::uint16_t mask_type;
      
      key_type key;
      struct {
        itemkey_type item;
        mask_type    mask;
        mask_type    spare;
      } values;
      Key();
      Key(const Key&);
      Key(key_type full_mask);
      Key(mask_type mask, const std::string& item);
      Key& operator=(const Key&);
      key_type toLong()  const  {  return key; }
      void set(const std::string& name, int mask);
      /// Project the mask part of the key
      static itemkey_type item(key_type k)  {
	return Key(k).values.item;
      }
      /// Project the item part of the key
      static mask_type mask(key_type k)  {
	return Key(k).values.mask;
      }
      /// Access key name (if registered properly)
      static std::string key_name(const Key& key);
    };

    /// Particle definition for digitization
    /** Particle definition for digitization
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class Particle   {
    public:
      /// Source contributing
      std::any history;

    public:
      /// Initializing constructor
      Particle(std::any&& history);
      /// Default constructor
      Particle() = default;
      /// Disable move constructor
      Particle(Particle&& copy) = default;
      /// Disable copy constructor
      Particle(const Particle& copy) = default;
      /// Default destructor
      virtual ~Particle() = default;
      /// Disable move assignment
      Particle& operator=(Particle&& copy) = default;
      /// Disable copy assignment
      Particle& operator=(const Particle& copy) = default;
    };

    /// Initializing constructor
    inline Particle::Particle(std::any&& h)
      : history(h)
    {
    }

    /// Particle mapping definition for digitization
    /** Particle mapping definition for digitization
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class ParticleMapping : public std::map<Key::key_type, Particle>   {
    public:
      std::string      name { };
      Key::mask_type   mask { 0x0 };

    public: 
      /// Initializing constructor
      ParticleMapping(const std::string& name, Key::mask_type mask);
      /// Default constructor
      ParticleMapping() = default;
      /// Disable move constructor
      ParticleMapping(ParticleMapping&& copy) = default;
      /// Disable copy constructor
      ParticleMapping(const ParticleMapping& copy) = default;
      /// Default destructor
      virtual ~ParticleMapping() = default;
      /// Disable move assignment
      ParticleMapping& operator=(ParticleMapping&& copy) = default;
      /// Disable copy assignment
      ParticleMapping& operator=(const ParticleMapping& copy) = default;

      /// Merge new deposit map onto existing map
      std::size_t merge(ParticleMapping&& updates);
      void push(Key::key_type key, Particle&& particle);
    };

    /// Initializing constructor
    inline ParticleMapping::ParticleMapping(const std::string& n, Key::mask_type m)
      : name(n), mask(m)
    {
    }

    /// Energy deposit definition for digitization
    /** Energy deposit definition for digitization
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class EnergyDeposit   {
    public:
      /// Total energy deposit
      double deposit  { 0 };
      /// Sources contributing to this deposit
      std::vector<std::pair<std::any, Key::mask_type> > history;

    public:
      /// Initializing constructor
      EnergyDeposit(double ene);
      /// Default constructor
      EnergyDeposit() = default;
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
    };


    /// Initializing constructor
    inline EnergyDeposit::EnergyDeposit(double ene)
      : deposit(ene)
    {
    }

    /// Energy deposit mapping definition for digitization
    /** Energy deposit mapping definition for digitization
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DepositMapping : public std::map<CellID, EnergyDeposit>   {
    public: 
      std::string    name { };
      Key::mask_type mask { 0x0 };

    public: 
      /// Initializing constructor
      DepositMapping(const std::string& name, Key::mask_type mask);
      /// Default constructor
      DepositMapping() = default;
      /// Disable move constructor
      DepositMapping(DepositMapping&& copy) = default;
      /// Disable copy constructor
      DepositMapping(const DepositMapping& copy) = default;      
      /// Default destructor
      virtual ~DepositMapping() = default;
      /// Disable move assignment
      DepositMapping& operator=(DepositMapping&& copy) = default;
      /// Disable copy assignment
      DepositMapping& operator=(const DepositMapping& copy) = default;      

      /// Merge new deposit map onto existing map
      std::size_t merge(DepositMapping&& updates);
    };

    /// Initializing constructor
    inline DepositMapping::DepositMapping(const std::string& n, Key::mask_type m)
      : name(n), mask(m)
    {
    }

  }    // End namespace digi
}      // End namespace dd4hep
    

/// <any> is not properly processed by cling.
/// We need to exclude any reference to it.
#if !defined(DD4HEP_INTERPRETER_MODE)
#include "DD4hep/Any.h"
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    ///  User event data for DDDigi
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class  DigiEvent : public ObjectExtensions  {
    private:
      std::mutex  m_lock;
      std::string m_id;
    public:
      /// Forward definition of the key type
      typedef Key::key_type key_type;

      int eventNumber = 0;
#if defined(DD4HEP_INTERPRETER_MODE)
      std::map<key_type, long>  data;
      std::map<key_type, long>  inputs;
      std::map<key_type, long>  deposits;
#else
      typedef std::map<key_type, dd4hep::any> container_map_t;
    protected:
      container_map_t data;
      container_map_t inputs;
      container_map_t deposits;
#endif
    public:
#if defined(DD4HEP_INTERPRETER_MODE) || defined(G__ROOT)
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

      /// String identifier of this event
      const char* id()   const    {   return this->m_id.c_str();   }

#if !defined(DD4HEP_INTERPRETER_MODE)
      /// Add item by key to the data segment
      bool put_data(unsigned long key, std::any&& object);

      /// Add item by key to the input segment
      bool put_input(unsigned long key, std::any&& object);

      /// Retrieve data segment from the event structure
      container_map_t& get_segment(const std::string& name);

      /// Add item by key to the data 
      template<typename T> bool put(const Key& key, T&& object)     {
        bool ret = this->inputs.emplace(key.toLong(),make_any<T>(object)).second;
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
#endif
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

#endif // DDDIGI_DIGIDATA_H
