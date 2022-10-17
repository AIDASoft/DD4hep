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
#include <DD4hep/Primitives.h>

/// C/C++ include files
#include <cstdint>
#include <memory>
#include <mutex>
#include <map>
#include <any>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    union Key;
    class Particle;
    class EnergyDeposit;
    class ParticleMapping;
    class DepositMapping;
    class DigiEvent;
    class DataSegment;

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
      
      /// First union entry used for fast initialization
      key_type key;
      /// Second union entry to use for discrimination
      struct {
	// Ordering is important here: 
	// We want to group the containers by item ie. by container name
	// and not by mask
        itemkey_type item;
        mask_type    mask;
        mask_type    spare;
      } values;

      /// Default constructor
      Key();
      /// Move constructor
      Key(Key&&);
      /// Copy constructor
      Key(const Key&);
      /// Initializaing constructor (fast)
      Key(key_type full_mask);
      /// Initializaing constructor with key generation using hash algorithm
      Key(mask_type mask, const std::string& item);
      /// Assignment operator
      Key& operator = (const Key&);
      /// Move assignment operator
      Key& operator = (Key&& copy);
      /// Equality operator
      bool operator == (const Key&)   const;
      /// Operator less
      bool operator < (const Key&)   const;
      /// Operator greator
      bool operator > (const Key&)   const;

      /// Conversion to uint64
      key_type toLong()  const  {
	return key;
      }
      /// Project the mask part of the key
      itemkey_type item()  {
	return this->values.item;
      }
      /// Project the item part of the key
      mask_type mask()  {
	return this->values.mask;
      }
      /// Generate key using hash algorithm
      void set(const std::string& name, int mask);
      
      /// Project the mask part of the key
      static itemkey_type item(key_type k)  {
	return Key(k).values.item;
      }
      /// Project the item part of the key
      static mask_type mask(key_type k)  {
	return Key(k).values.mask;
      }
      /// Project the mask part of the key
      static itemkey_type item(Key k)  {
	return k.values.item;
      }
      /// Project the item part of the key
      static mask_type mask(Key k)  {
	return k.values.mask;
      }
      /// Access key name (if registered properly)
      static std::string key_name(const Key& key);
    };

    /// Default constructor
    inline Key::Key()    {
      this->key = 0;
    }

    /// Move constructor
    inline Key::Key(Key&& copy)   {
      this->key = copy.key;
    }

    /// Copy constructor
    inline Key::Key(const Key& copy)   {
      this->key = copy.key;
    }

    /// Initializaing constructor (fast)
    inline Key::Key(key_type full_mask)   {
      this->key = full_mask;
    }

    /// Initializaing constructor with key generation using hash algorithm
    inline Key::Key(mask_type mask, const std::string& item)  {
      this->set(item, mask);
    }

    /// Move assignment operator
    inline Key& Key::operator = (Key&& copy)   {
      this->key = copy.key;
      return *this;
    }

    /// Assignment operator
    inline Key& Key::operator = (const Key& copy)   {
      this->key = copy.key;
      return *this;
    }

    /// Equality operator
    inline bool Key::operator == (const Key& other)   const    {
      return this->key == other.key;
    }

    /// Operator less
    inline bool Key::operator < (const Key& other)   const    {
      return this->key < other.key;
    }

    /// Operator greator
    inline bool Key::operator > (const Key& other)   const    {
      return this->key > other.key;
    }

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

      /// Merge new deposit map onto existing map (not thread safe!)
      std::size_t merge(ParticleMapping&& updates);
      /// Add new entry to the particle mapping (not thread safe!)
      void push(Key key, Particle&& particle);
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

    /// Energy deposit vector definition for digitization
    /** Energy deposit vector definition for digitization
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DepositVector : public std::vector<std::pair<CellID, EnergyDeposit> >  {
    public: 
      std::string    name { };
      Key            key  { 0x0 };

    public: 
      /// Initializing constructor
      DepositVector(const std::string& name, Key::mask_type mask);
      /// Default constructor
      DepositVector() = default;
      /// Disable move constructor
      DepositVector(DepositVector&& copy) = default;
      /// Disable copy constructor
      DepositVector(const DepositVector& copy) = default;      
      /// Default destructor
      virtual ~DepositVector() = default;
      /// Disable move assignment
      DepositVector& operator=(DepositVector&& copy) = default;
      /// Disable copy assignment
      DepositVector& operator=(const DepositVector& copy) = default;      
      /// Merge new deposit map onto existing map (not thread safe!)
      std::size_t merge(DepositVector&& updates);
    };

    /// Initializing constructor
    inline DepositVector::DepositVector(const std::string& n, Key::mask_type mask)
      : name(n), key(mask, n)
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
      Key            key  { 0x0 };

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
      /// Merge new deposit map onto existing map (not thread safe!)
      std::size_t merge(DepositMapping&& updates);
    };

    /// Initializing constructor
    inline DepositMapping::DepositMapping(const std::string& n, Key::mask_type mask)
      : name(n), key(mask, n)
    {
    }

    ///  Data segment definition (locked map)
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DataSegment   {
    public:
      using container_map_t = std::map<Key::key_type, std::any>;
      using iterator = container_map_t::iterator;
      using const_iterator = container_map_t::const_iterator;

    private:
      /// Call on failed any-casts
      std::string invalid_cast(Key key, const std::type_info& type)  const;
      /// Call on failed data requests during data requests
      std::string invalid_request(Key key)  const;

      /// Access data item by key
      std::any* get_item(Key key, bool exc);
      /// Access data item by key  (CONST)
      const std::any* get_item(Key key, bool exc)  const;

    public:
      container_map_t data;
      std::mutex&     lock;

    public:
      /// Initializing constructor
      DataSegment(std::mutex& lock);
      /// Default constructor
      DataSegment() = delete;
      /// Disable move constructor
      DataSegment(DataSegment&& copy) = delete;
      /// Disable copy constructor
      DataSegment(const DataSegment& copy) = delete;      
      /// Default destructor
      virtual ~DataSegment() = default;
      /// Disable move assignment
      DataSegment& operator=(DataSegment&& copy) = delete;
      /// Disable copy assignment
      DataSegment& operator=(const DataSegment& copy) = delete;      

      /** Locked operations */
      /// Emplace data item (locked)
      bool emplace(Key key, std::any&& data);
      /// Remove data item from segment (locked)
      bool erase(Key key);
      /// Remove data items from segment (locked)
      std::size_t erase(const std::vector<Key>& keys);
      /// Print segment keys
      void print_keys()   const;
      
      /** Unlocked operations */
      /// Access data as reference by key. If not existing, an exception is thrown
      template<typename T> T& get(Key key);
      /// Access data as reference by key. If not existing, an exception is thrown
      template<typename T> const T& get(Key key)  const;

      /// Access data as pointers by key. If not existing, nullptr is returned
      template<typename T> T* pointer(Key key);
      /// Access data as pointers by key. If not existing, nullptr is returned
      template<typename T> const T* pointer(Key key)  const;

      /// Access container size
      std::size_t size()  const           { return this->data.size();        }
      /// Check container if empty
      bool        empty() const           { return this->data.empty();       }
      /// Begin iteration
      iterator begin()                    { return this->data.begin();       }
      /// End iteration
      iterator end()                      { return this->data.end();         }
      /// Find entry by key
      iterator find(Key key)              { return this->data.find(key.key); }
      /// Begin iteration (CONST)
      const_iterator begin() const        { return this->data.begin();       }
      /// End iteration (CONST)
      const_iterator end()   const        { return this->data.end();         }
      /// Find entry by key
      const_iterator find(Key key) const  { return this->data.find(key.key); }
    };

    /// Access data as reference by key. If not existing, an exception is thrown
    template<typename T> inline T& DataSegment::get(Key key)     {
      if ( T* ptr = std::any_cast<T>(this->get_item(key, true)) )
	return *ptr;
      throw std::runtime_error(this->invalid_cast(key, typeid(T)));
    }
    /// Access data as reference by key. If not existing, an exception is thrown
    template<typename T> inline const T& DataSegment::get(Key key)  const   {
      if ( const T* ptr = std::any_cast<T>(this->get_item(key, true)) )
	return *ptr;
      throw std::runtime_error(this->invalid_cast(key, typeid(T)));
    }

    /// Access data as pointers by key. If not existing, nullptr is returned
    template<typename T> inline T* DataSegment::pointer(Key key)     {
      if ( T* ptr = std::any_cast<T>(this->get_item(key, false)) )
	return ptr;
      return nullptr;
    }
    /// Access data as pointers by key. If not existing, nullptr is returned
    template<typename T> inline const T* DataSegment::pointer(Key key)  const   {
      if ( const T* ptr = std::any_cast<T>(this->get_item(key, false)) )
	return ptr;
      return nullptr;
    }

    ///  User event data for DDDigi
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class  DigiEvent  {
    private:
      std::mutex  m_lock;
      std::string m_id;
      DataSegment m_data        { this->m_lock };
      DataSegment m_inputs      { this->m_lock };
      DataSegment m_deposits    { this->m_lock };

    public:
      int eventNumber  { 0 };

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
      /// Retrieve data segment from the event structure
      DataSegment& get_segment(const std::string& name);
    };
  }    // End namespace digi
}      // End namespace dd4hep

#endif // DDDIGI_DIGIDATA_H
