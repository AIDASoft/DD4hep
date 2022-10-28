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
#ifndef DDDIGI_DIGISIGNALPROCESSOR_H
#define DDDIGI_DIGISIGNALPROCESSOR_H

/// Framework include files
#include <DDDigi/DigiAction.h>
#include <DDDigi/DigiData.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiAction;
    class DigiContext;
    class DigiCellData;
    class DigiSignalProcessor;

    /// 
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiCellContext  final  {
    public:
      DigiContext&  context;
      DigiCellData& data;
      DigiCellContext(DigiContext& c, DigiCellData& d) : context(c), data(d) {}
      ~DigiCellContext() = default;
    };

    /// Base class for signal processing actions to the digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiSignalProcessor : public DigiAction {
    protected:
      /// Flag to check if initialized was called
      bool  m_initialized = false;

      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiSignalProcessor);

    public:
      /// Standard constructor
      DigiSignalProcessor(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiSignalProcessor();
      /// Initialize the noise source
      virtual void initialize();
      /// Callback to read event signalprocessor
      virtual double operator()(DigiCellContext& context)  const = 0;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGISIGNALPROCESSOR_H
