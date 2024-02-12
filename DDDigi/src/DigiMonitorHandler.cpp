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

/// Framework include files
#include <DD4hep/Printout.h>
#include <DDDigi/DigiMonitorHandler.h>

/// ROOT include files
#include <TFile.h>

/// C/C++ include files

using namespace dd4hep::digi;

/// Standard constructor
DigiMonitorHandler::DigiMonitorHandler(const DigiKernel& kernel, const std::string& nam)
  : DigiAction(kernel, nam)
{
  declareProperty("MonitorOutput", m_output_file);
}

/// Default destructor
DigiMonitorHandler::~DigiMonitorHandler()    {
  for( auto& m : m_monitors )   {
    m.first->release();
    for( auto* itm : m.second )   {
      detail::deletePtr(itm);
    }
    m.second.clear();
  }
  m_monitors.clear();
}

/// Adopt monitor and keep reference for saving
void DigiMonitorHandler::adopt(DigiAction* source, TNamed* object)    {
  source->addRef();
  m_monitors[source].insert(object);
}

/// Save monitors
void DigiMonitorHandler::save()    {
  if ( !m_output_file.empty() )    {
    TFile* output = TFile::Open(m_output_file.c_str(),
                                "DD4hep Digitization monitoring information",
                                "RECREATE");
    TDirectory::TContext top_context(output);
    if ( output && !output->IsZombie() )    {
      for( const auto& m : m_monitors )   {
        const auto* act   = m.first;
        const auto& items = m.second;
        const auto& nam   = act->name();
        std::string title = "Monitor items of digitization action "+nam;
        TDirectory* direc = output->mkdir(nam.c_str(), title.c_str(), kTRUE);
        TDirectory::TContext action_context(direc);
        for( const auto* itm : items )   {
          Int_t nbytes = direc->WriteTObject(itm);
          if ( nbytes <= 0 )  {
            error("+++ Failed to write object: %s -> %s", nam.c_str(), itm->GetName());
          }
        }
        direc->Write();
      }
      output->Write();
      output->Close();
      delete output;
      return;
    }
    except("+++ Failed to open monitoring output file: %s", m_output_file.c_str());
  }
}

