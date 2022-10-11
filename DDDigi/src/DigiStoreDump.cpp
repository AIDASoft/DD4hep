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
#include "DD4hep/InstanceCount.h"

#include "DDDigi/DigiData.h"
#include "DDDigi/DigiKernel.h"
#include "DDDigi/DigiContext.h"
#include "DDDigi/DigiStoreDump.h"

/// Standard constructor
dd4hep::digi::DigiStoreDump::DigiStoreDump(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  declareProperty("history",       this->m_dump_history  = false);
  declareProperty("data_segments", this->m_data_segments);
  this->m_data_segments.push_back("input");
  this->m_data_segments.push_back("deposits");
  this->m_data_segments.push_back("data");
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
  info("%s+---- %s segment: %ld entries", event.id(), tag.c_str(), data_segment.size());
  for ( const auto& e : data_segment )     {
    Key key {e.first};
    const std::any& data = e.second;
    std::string nam = Key::key_name(key);
    std::string typ = typeName(data.type());
    std::size_t idx = typ.find(", std::less<long long>, std::allocator<std::pair");
    if ( idx != std::string::npos ) typ = str_replace(typ, typ.substr(idx), ">");
    typ = str_replace(str_replace(typ,"std::",""),"dd4hep::digi::","");
    if ( const auto* hits = std::any_cast<DepositMapping>(&data) )   {
      str = this->format("%s|---- %4X %08X %-32s: %6ld hits      [%s]", 
			 event.id(), key.values.mask, key.values.item,
			 nam.c_str(), hits->size(), typ.c_str());
    }
    else if ( const auto* parts = std::any_cast<ParticleMapping>(&data) )   {
      str = this->format("%s|---- %4X %08X %-32s: %6ld particles [%s]", 
			 event.id(), key.values.mask, key.values.item,
			 nam.c_str(), parts->size(), typ.c_str());
    }
    else   {
      str = this->format("%s|---- %4X %08X %-32s: %s", 
			 event.id(), key.values.mask, key.values.item, 
			 nam.c_str(), typ.c_str());
    }
    records.push_back(str);
  }
  std::lock_guard<std::mutex> record_lock(m_kernel.global_output_lock());
  for(const auto& s : records)
    this->info(s.c_str());
}

/// Main functional callback
void dd4hep::digi::DigiStoreDump::execute(DigiContext& context)  const    {
  const auto& event = context.event;
  for(const auto& segment : this->m_data_segments)   {
    std::string seg = detail::str_upper(segment);
    this->dump(seg, *event, event->get_segment(segment));
  }
}
