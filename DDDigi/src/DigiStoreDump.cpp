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
#include <DD4hep/InstanceCount.h>

#include <DDDigi/DigiData.h>
#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiStoreDump.h>

/// Standard constructor
dd4hep::digi::DigiStoreDump::DigiStoreDump(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  declareProperty("history",       m_dump_history  = false);
  declareProperty("data_segments", m_data_segments);
  m_data_segments.push_back("input");
  m_data_segments.push_back("deposits");
  m_data_segments.push_back("data");
  m_data_segments.push_back("counts");
  m_data_segments.push_back("output");
  InstanceCount::increment(this);
}

/// Default destructor
dd4hep::digi::DigiStoreDump::~DigiStoreDump() {
  InstanceCount::decrement(this);
}

/// Dump hit container
template <typename CONTAINER> 
void dd4hep::digi::DigiStoreDump::dump(const std::string& tag,
				       const DigiEvent& event,
				       const CONTAINER& data_segment)  const
{
  std::string str;
  std::vector<std::string> records;
  using detail::str_replace;
  info("%s+--- %-12s segment: %ld entries", event.id(), tag.c_str(), data_segment.size());
  std::lock_guard<std::mutex> lock(data_segment.lock);
  for ( const auto& e : data_segment )     {
    Key key {e.first};
    const std::any& data = e.second;
    std::string nam = Key::key_name(key);
    std::string typ = typeName(data.type());
    std::size_t idx = typ.find(", std::allocator<std::");
    if ( idx != std::string::npos ) typ = str_replace(typ, typ.substr(idx), ">");
    typ = str_replace(str_replace(typ,"std::",""),"dd4hep::","");
    typ = str_replace(str_replace(typ,"sim::",""),"digi::","");
    if ( const auto* mapping = std::any_cast<DepositMapping>(&data) )   {
      str = this->format("%s|----  %4X %08X %-32s: %6ld deposits   [%s]", 
			 event.id(), key.values.mask, key.values.item,
			 nam.c_str(), mapping->size(), typ.c_str());
    }
    else if ( const auto* vector = std::any_cast<DepositVector>(&data) )   {
      str = this->format("%s|----  %4X %08X %-32s: %6ld deposits   [%s]", 
			 event.id(), key.values.mask, key.values.item,
			 nam.c_str(), vector->size(), typ.c_str());
    }
    else if ( const auto* parts = std::any_cast<ParticleMapping>(&data) )   {
      str = this->format("%s|----  %4X %08X %-32s: %6ld particles  [%s]", 
			 event.id(), key.values.mask, key.values.item,
			 nam.c_str(), parts->size(), typ.c_str());
    }
    else if ( const auto* adcs = std::any_cast<DetectorResponse>(&data) )   {
      str = this->format("%s|----  %4X %08X %-32s: %6ld ADC values [%s]", 
			 event.id(), key.values.mask, key.values.item,
			 nam.c_str(), adcs->size(), typ.c_str());
    }
    else   {
      str = this->format("%s|----  %4X %08X %-32s: %s", 
			 event.id(), key.values.mask, key.values.item, 
			 nam.c_str(), typ.c_str());
    }
    records.push_back(str);
  }
  std::lock_guard<std::mutex> record_lock(m_kernel.global_output_lock());
  for(const auto& s : records)
    info(s.c_str());
}

/// Main functional callback
void dd4hep::digi::DigiStoreDump::execute(DigiContext& context)  const    {
  const auto& event = context.event;
  for(const auto& segment : this->m_data_segments)   {
    std::string seg = detail::str_upper(segment);
    dump(seg, *event, event->get_segment(segment));
  }
}
