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
#include "DD4hep/Primitives.h"
#include "DD4hep/ObjectExtensions.h"

/// C/C++ include files
#include <memory>

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
    template <typename T> class DigiContainer  {
    public:
      /// Default constructor
      DigiContainer() = default;
      /// Disable move constructor
      DigiContainer(DigiContainer&& copy) = delete;
      /// Disable copy constructor
      DigiContainer(const DigiContainer& copy) = delete;      
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
    class DigiDeposit   {
    public:
      /// Default constructor
      DigiDeposit() = default;
      /// Disable move constructor
      DigiDeposit(DigiDeposit&& copy) = delete;
      /// Disable copy constructor
      DigiDeposit(const DigiDeposit& copy) = delete;      
      /// Default destructor
      virtual ~DigiDeposit() = default;
      /// Disable move assignment
      DigiDeposit& operator=(DigiDeposit&& copy) = delete;
      /// Disable copy assignment
      DigiDeposit& operator=(const DigiDeposit& copy) = delete;      
    };

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
      /// Disable move constructor
      DigiCount(DigiCount&& copy) = delete;
      /// Disable copy constructor
      DigiCount(const DigiCount& copy) = delete;      
      /// Default destructor
      virtual ~DigiCount() = default;
      /// Disable move assignment
      DigiCount& operator=(DigiCount&& copy) = delete;
      /// Disable copy assignment
      DigiCount& operator=(const DigiCount& copy) = delete;      
    };

    typedef DigiContainer<DigiDeposit> DigiEnergyDeposits;
    typedef DigiContainer<DigiCount>   DigiCounts;

    ///  User event data for DDDigi
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class  DigiEvent : public ObjectExtensions  {
    public:
      std::map<unsigned long, std::shared_ptr<DigiEnergyDeposits> >  energyDeposits;
      std::map<unsigned long, std::shared_ptr<DigiCounts> >          digitizations;
      int eventNumber = 0;
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
