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

// Framework include files
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiSegmentSplitter.h>
#include <DDDigi/DigiContainerProcessor.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Helper class to print energy deposits pre-selected by the segment splitter
    /** Helper class to print energy deposits pre-selected by the segment splitter
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class DigiSegmentDepositPrint : public DigiContainerProcessor   {
    public:
      /// Standard constructor
      DigiSegmentDepositPrint(const DigiKernel& kernel, const std::string& nam)
        : DigiContainerProcessor(kernel, nam) {}

      /// Single container printout
      template <typename T> void
      print(const char* fmt, const T& cont, const predicate_t& predicate)  const   {
        for( const auto& dep : cont )   {
          if( predicate(dep) )   {
            info(fmt, predicate.segmentation->split_id(dep.first), dep.first,
                 dep.second.history.hits.size(), 
                 dep.second.history.particles.size(),
                 dep.second.deposit);
          }
        }
      }

      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work, const predicate_t& predicate)  const override final  {
        using ulonglong = unsigned long long;
        char format[256];
        ::snprintf(format, sizeof(format), 
                   "%s[%s] %s-id: %%d [processor:%d] Cell: %%016lX mask: %016llX  "
                   "hist:%%4ld hits %%4ld parts. "
                   "entries deposit: %%f", 
                   context.event->id(),
                   predicate.segmentation->idspec.name(), predicate.segmentation->cname(),
                   predicate.id, ulonglong(predicate.segmentation->split_mask));
        if ( const auto* m = work.get_input<DepositMapping>() )
          print(format, *m, predicate);
        else if ( const auto* v = work.get_input<DepositVector>() )
          print(format, *v, predicate);
        else
          error("+++ Request to dump an invalid container %s", Key::key_name(work.input.key).c_str());
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

///        Factory definition
#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiSegmentDepositPrint)
