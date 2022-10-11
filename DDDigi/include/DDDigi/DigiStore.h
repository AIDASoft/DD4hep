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
#ifndef DDDIGI_DIGISTORE_H
#define DDDIGI_DIGISTORE_H

// Framework incloude files
#include <DDDigi/DigiAction.h>
#include <DDDigi/DigiData.h>

#include <mutex>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// 
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiInputs   {
    public:
      friend class DigiStore;

      /// Forward definition of the key type
      typedef Key::key_type key_type;
      
    protected:
#if defined(DD4HEP_INTERPRETER_MODE)
      std::map<key_type, long>  data;
#else
      std::map<key_type, dd4hep::any>  data;
#endif
    public:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiInputs);

#if !defined(DD4HEP_INTERPRETER_MODE)
      /// Retrieve item by key from the data container
      template <typename T> const T& get(const Key& key)   const  {
        auto iter = data.find(key.toLong());
        if ( iter != data.end() )  {
          T* ptr = dd4hep::any_cast<T>(&(*iter).second);
          if ( ptr ) return *ptr;
          except("DigiEvent","Invalid data requested from store [cast failed]. Key:%ld",key.toLong());
        }
        except("DigiEvent","Invalid data requested from store. Key:%ld",key.toLong());
        throw std::runtime_error("DigiEvent"); // Will never get here!
      }
#endif
    };
    
    /// 
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiOutputs   {
    public:
      friend class DigiStore;

      /// Forward definition of the key type
      typedef Key::key_type key_type;
      
    protected:
#if defined(DD4HEP_INTERPRETER_MODE)
      std::map<key_type, long>  data;
#else
      std::map<key_type, dd4hep::any>  data;
#endif
      std::mutex    lock;
    public:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiOutputs);

#if !defined(DD4HEP_INTERPRETER_MODE)
      /// Add item by key to the data 
      template<typename T> bool put(const Key& key, T&& object)     {
        std::lock_guard<std::mutex> guard(lock);
        bool ret = data.emplace(key.toLong(),make_any<T>(object)).second;
        if ( ret ) return ret;
        except("DigiEvent","Invalid request to store data in store. Key:%ld",key.toLong());
        return false;
      }
#endif
    };
    
    /// 
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiStore {
    public:
      /// Forward definition of the key type
      typedef Key::key_type key_type;

      /// Container of input data
      DigiInputs  inputs;
      /// Container of output data
      DigiOutputs outputs;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiStore);

    public:
      /// Standard constructor
      DigiStore(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiStore();
      /// Add a set of input data to the store
      void add_inputs(const std::map<key_type,dd4hep::any>& items);
      /// Move produced results out of the store
      std::map<key_type,dd4hep::any> move_outputs();
    };

  }    // End namespace digi
}      // End namespace dd4hep

#endif // DDDIGI_DIGISTORE_H
