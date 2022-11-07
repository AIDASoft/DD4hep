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
#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiContainerProcessor.h>

#include <DD4hep/Detector.h>
#include <DD4hep/Readout.h>
#include <DD4hep/DetElement.h>
#include <DD4hep/IDDescriptor.h>
#include <DD4hep/Segmentations.h>
#include <DD4hep/DetectorLoad.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Actor to select energy deposits according to the supplied segmentation
    /** Actor to select energy deposits according to the supplied segmentation
     *
     *  The selected deposits are placed in the output container
     *  supplied by the arguments.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiResegment : public DigiContainerProcessor   {
    protected:
      std::string  m_detector_name { };
      std::string  m_readout_name  { };
      std::string  m_readout_descriptor { };
      bool         m_debug              { false };

      Readout      m_new_readout  { };
      Segmentation m_new_segment  { };
      IDDescriptor m_new_id_desc  { };
      Readout      m_org_readout  { };
      Segmentation m_org_segment  { };
      IDDescriptor m_org_id_desc  { };
      DetElement   m_detector { };
      VolumeManager m_volmgr  { };
    public:
      /// Standard constructor
      DigiResegment(const DigiKernel& krnl, const std::string& nam)
	: DigiContainerProcessor(krnl, nam)
      {
	declareProperty("debug",      m_debug);
	declareProperty("detector",   m_detector_name);
	declareProperty("readout",    m_readout_name);
	declareProperty("descriptor", m_readout_descriptor);
	m_kernel.register_initialize(std::bind(&DigiResegment::initialize, this));
      }

      void initialize()   {
	auto& detector = m_kernel.detectorDescription();
	if ( m_readout_descriptor.empty() )   {
	  except("+++ Invalid ID descriptor %s", m_readout_descriptor.c_str());
	}
	DetectorLoad loader(detector);
	loader.processXMLString(m_readout_descriptor.c_str());
	m_new_readout = detector.readout(m_readout_name);
	if ( !m_new_readout.isValid() )  {
	  except("+++ Invalid resegmentation readout for %s", m_readout_name.c_str());
	}
	m_new_id_desc = m_new_readout.idSpec();
	m_new_segment = m_new_readout.segmentation();


	m_detector = detector.detector(m_detector_name);
	if ( !m_detector.isValid() )   {
	  except("+++ Cannot locate subdetector: %s", m_detector_name.c_str());
	}
	m_org_readout = detector.sensitiveDetector(m_detector_name).readout();
	if ( !m_org_readout.isValid() )  {
	  except("+++ Invalid resegmentation readout for %s", m_readout_name.c_str());
	}
	m_org_id_desc = m_org_readout.idSpec();
	m_org_segment = m_org_readout.segmentation();

	m_volmgr = detector.volumeManager();
	if ( !m_volmgr.isValid() )   {
	  detector.apply("DD4hepVolumeManager",0,nullptr);
	}
	m_volmgr = detector.volumeManager();
	if ( !m_volmgr.isValid() )   {
	  except("+++ Cannot locate volume manager!");
	}
	info("+++ Successfully initialized resegmentation action.");
      }

      template <typename T> void
      resegment_deposits(const T& cont, work_t& work, const predicate_t& predicate)  const  {
	Key key(cont.name, work.output.mask);
	DepositVector m(cont.name, work.output.mask);
	std::size_t start = m.size();
	for( const auto& dep : cont )   {
	  if( predicate(dep) )   {
	    CellID cell = dep.first;
	    auto*  ctxt = m_volmgr.lookupContext(cell);
	    if ( !ctxt )   {
	      error("+++ Cannot locate volume context for cell %016lX", cell);
	    }
	    else   {
	      VolumeID volID     = m_org_segment.volumeID(cell);
	      Position org_local = m_org_segment.position(cell);
	      Position global    = ctxt->localToWorld(org_local);
	      CellID   new_cell  = m_new_segment.cellID(org_local, global, volID);
	      Position new_local = m_new_segment.position(new_cell);
	      if ( m_debug )   {
		info("+++ Cell: %016lX -> %016lX DE: %-20s "
		     "Pos global: %8.2f %8.2f %8.2f  local: %8.2f %8.2f %8.2f -> %8.2f %8.2f %8.2f",
		     cell, new_cell, ctxt->element.name(), 
		     global.X(), global.Y(), global.Z(),
		     org_local.X(), org_local.Y(), org_local.Z(),
		     new_local.X(), new_local.Y(), new_local.Z()
		     );
	      }
	      EnergyDeposit d(dep.second);
	      d.position = global;
	      d.momentum = dep.second.momentum;
	      m.emplace(new_cell, std::move(d));
	    }
	  }
	}
	std::size_t end   = m.size();
	work.output.data.put(m.key, std::move(m));
	info("+++ %-32s added %6ld entries (now: %6ld) from mask: %04X to mask: %04X",
	     cont.name.c_str(), end-start, end, cont.key.mask(), m.key.mask());
      }

      /// Main functional callback
      virtual void execute(DigiContext&, work_t& work, const predicate_t& predicate)  const override final  {
	if ( const auto* m = work.get_input<DepositMapping>() )
	  resegment_deposits(*m, work, predicate);
	else if ( const auto* v = work.get_input<DepositVector>() )
	  resegment_deposits(*v, work, predicate);
	else
	  except("Request to handle unknown data type: %s", work.input_type_name().c_str());
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiResegment)
