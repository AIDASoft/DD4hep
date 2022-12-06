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

using namespace dd4hep::digi;

/// Standard constructor
DigiStoreDump::DigiStoreDump(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  declareProperty("dump_history",  m_dump_history  = false);
  declareProperty("segments", m_segments);
  m_segments.push_back("input");
  m_segments.push_back("deposits");
  m_segments.push_back("data");
  m_segments.push_back("counts");
  m_segments.push_back("output");
  declareProperty("containers", m_containers);
  declareProperty("masks",      m_masks);
  m_kernel.register_initialize(std::bind(&DigiStoreDump::initialize,this));
  InstanceCount::increment(this);
}

/// Default destructor
DigiStoreDump::~DigiStoreDump() {
  InstanceCount::decrement(this);
}

/// Initialize the action
void DigiStoreDump::initialize()   {
  m_container_items.clear();
  for( const auto& cont : m_containers )   {
    Key key(cont, 0x0);
    m_container_items.emplace_back(key.item());
  }
}

template <typename T> std::string DigiStoreDump::data_header(Key key, const std::string& tag, const T& container)  const  {
  return this->format("%04X %04X %08X %-32s: %6ld %-12s [%s]", 
		      key.segment(), key.mask(), key.item(),
		      Key::key_name(key).c_str(), container.size(), tag.c_str(),
		      digiTypeName(typeid(T)).c_str());
}

template <> std::string DigiStoreDump::data_header(Key key, const std::string& tag, const std::type_info& info)  const  {
  return this->format("%04X %04X %08X %-32s: %-12s %s",
		      key.segment(), key.mask(), key.item(),
		      Key::key_name(key).c_str(), tag.c_str(),
		      digiTypeName(info).c_str());
}

template <> std::string DigiStoreDump::data_header(Key key, const std::string& tag, const std::any& data)  const  {
  return data_header(key, tag, data.type());
}

template <> std::vector<std::string>
DigiStoreDump::dump_history(DigiContext& context, 
			    Key key,
			    const std::pair<const Key, Particle>& data,
			    std::size_t seq_no)  const
{
  std::stringstream str;
  auto& ev = *(context.event);
  const auto& item = data.second;
  Key particle_key = data.first;
  Key history_key  = item.history;
  std::vector<std::string> records;
  History::hist_entry_t hist { particle_key, 1.0 };
  const Particle&  par = hist.get_particle(ev);
  const Direction& mom = par.momentum;
  const Position&  vtx = par.start_position;

  str << Key::key_name(key) << "[" << seq_no << "]:";
  std::string line =
    format("|----  %-18s Segment:%04X Mask:%04X Item:%04X History: %s Segment:%04X Mask:%04X Item:%04X %016lX",
	   str.str().c_str(), particle_key.segment(), particle_key.mask(), particle_key.item(),
	   Key::key_name(history_key).c_str(), history_key.segment(), history_key.mask(), history_key.item(),
	   long(&data.second));
  records.emplace_back(line);
  line = format("|        PDG:%6d Charge:%-2d Mass:%7.2f v:%7.5g %7.5g %7.5g  p:%12g %12g %12g      %016lX",
		par.pdgID, int(par.charge), par.mass, vtx.X(), vtx.Y(), vtx.Z(), mom.X(), mom.Y(), mom.Z(), long(&par));
  records.emplace_back(line);
  return records;
}

template <> std::vector<std::string> 
DigiStoreDump::dump_history(DigiContext& context,
			    Key container_key,
			    const std::pair<const CellID, EnergyDeposit>& data,
			    std::size_t seq_no)  const
{
  std::string line;
  std::stringstream str;
  auto& ev = *(context.event);
  const auto& item = data.second;
  const CellID cell = data.first;
  std::vector<std::string> records;

  str << Key::key_name(container_key) << "[" << seq_no << "]:";
  line = format("+----- %-30s Container: Segment:%04X Mask:%04X Item:%08X Cell:%016X Hist: Hits:%ld Parts:%ld",
		str.str().c_str(), container_key.segment(), container_key.mask(), container_key.item(),
		cell, item.history.hits.size(), item.history.particles.size());
  records.emplace_back(line);
  for( std::size_t i=0; i<item.history.hits.size(); ++i )   {
    const auto& entry = item.history.hits[i];
    const EnergyDeposit&  dep = entry.get_deposit(ev, container_key.item());
    const Position& pos = dep.position;
    const Position& mom = dep.momentum;
    Key k = entry.source;
    str.str("");
    str << "|        Hit-history[" << i << "]:";
    line = format("%-30s Segment:%04X Mask:%04X Cell:%08X  Weight:%.8g",
		  str.str().c_str(), k.segment(), k.mask(), k.item(), entry.weight);
    records.emplace_back(line);
    line = format("|              pos: %7.3f %7.3f %7.3f   p: %7.3f %7.3f %7.3f deposit: %7.3f",
		  pos.X(), pos.Y(), pos.Z(), mom.X(), mom.Y(), mom.Z(), dep.deposit);
    records.emplace_back(line);
  }
  for( std::size_t i=0; i<item.history.particles.size(); ++i )   {
    const auto&      ent = item.history.particles[i];
    const Particle&  par = ent.get_particle(ev);
    const Direction& mom = par.momentum;
    const Position&  vtx = par.start_position;
    Key key = ent.source;
    str.str("");
    str << "|        Part-history[" << i << "]:";
    line = format("%-30s Segment:%04X Mask:%04X Key: %08X  %.8g",
		  str.str().c_str(), key.segment(), key.mask(), key.item(), ent.weight);
    records.emplace_back(line);
    line = format("|              PDG:%6d Charge:%-2d Mass:%7.3f v:%7.3f %7.3f %7.3f   p:%7.3f %7.3f %7.3f",
		  par.pdgID, int(par.charge), par.mass, vtx.X(), vtx.Y(), vtx.Z(), mom.X(), mom.Y(), mom.Z());
    records.emplace_back(line);
  }
  return records;
}

template <> std::vector<std::string> 
DigiStoreDump::dump_history(DigiContext& context,
			    Key container_key,
			    const std::pair<const CellID, History>& data,
			    std::size_t seq_no)  const
{
  std::string line;
  std::stringstream str;
  auto& ev = *(context.event);
  const auto& history = data.second;
  const CellID cell = data.first;
  std::vector<std::string> records;

  str << Key::key_name(container_key) << "[" << seq_no << "]:";
  line = format("+----- %-30s Container: Segment:%04X Mask:%04X Item:%08X Cell:%016lX Hist: Hits:%ld Parts:%ld",
		str.str().c_str(), container_key.segment(), container_key.mask(), container_key.item(),
		cell, history.hits.size(), history.particles.size());
  records.emplace_back(line);
  for( std::size_t i=0; i<history.hits.size(); ++i )   {
    const auto& entry = history.hits[i];
    const EnergyDeposit&  dep = entry.get_deposit(ev, container_key.item());
    const Position& pos = dep.position;
    const Position& mom = dep.momentum;
    Key k = entry.source;
    str.str("");
    str << "|        Hit-history[" << i << "]:";
    line = format("%-30s Segment:%04X Mask:%04X Cell:%08X  Weight:%.8g",
		  str.str().c_str(), k.segment(), k.mask(), k.item(), entry.weight);
    records.emplace_back(line);
    line = format("|              pos: %7.3f %7.3f %7.3f   p: %7.3f %7.3f %7.3f deposit: %7.3f",
		  pos.X(), pos.Y(), pos.Z(), mom.X(), mom.Y(), mom.Z(), dep.deposit);
    records.emplace_back(line);
  }
  for( std::size_t i=0; i<history.particles.size(); ++i )   {
    const auto&      ent = history.particles[i];
    const Particle&  par = ent.get_particle(ev);
    const Direction& mom = par.momentum;
    const Position&  vtx = par.start_position;
    Key key = ent.source;
    str.str("");
    str << "|        Part-history[" << i << "]:";
    line = format("%-30s Segment:%04X Mask:%04X Key: %08X  %.8g",
		  str.str().c_str(), key.segment(), key.mask(), key.item(), ent.weight);
    records.emplace_back(line);
    line = format("|              PDG:%6d Charge:%-2d Mass:%7.3f v:%7.3f %7.3f %7.3f   p:%7.3f %7.3f %7.3f",
		  par.pdgID, int(par.charge), par.mass, vtx.X(), vtx.Y(), vtx.Z(), mom.X(), mom.Y(), mom.Z());
    records.emplace_back(line);
  }
  return records;
}

template <typename T> std::vector<std::string>
DigiStoreDump::dump_history(DigiContext& context, Key container_key, const T& container)  const {
  std::size_t count = 0;
  std::vector<std::string> records;
  for( const auto& item : container )  {
    auto rec = dump_history(context, container_key, item, count++);
    records.insert(records.end(), rec.begin(), rec.end());
  }
  return records;
}

template <typename T> std::vector<std::string>
DigiStoreDump::dump_deposit_history(DigiContext& context, Key container_key, const T& container)  const {
  std::vector<std::string> records;
  auto line = format("|----  %s", data_header(container_key, "deposits", container).c_str());
  records.emplace_back(line);
  std::size_t count = 0;
  for( const auto& item : container )   {
    auto rec = dump_history(context, container_key, item, count++);
    records.insert(records.end(), rec.begin(), rec.end());
  }
  return records;
}
template std::vector<std::string>
DigiStoreDump::dump_deposit_history(DigiContext& context, Key container_key, const DepositMapping& container)  const;

template std::vector<std::string>
DigiStoreDump::dump_deposit_history(DigiContext& context, Key container_key, const DepositVector& container)  const;

std::vector<std::string>
DigiStoreDump::dump_particle_history(DigiContext& context, Key container_key, const ParticleMapping& container)  const {
  std::size_t count = 0;
  std::vector<std::string> records;
  auto line = format("|----  %s", data_header(container_key, "particles", container).c_str());
  records.emplace_back(line);
  for( const auto& item : container )   {
    auto rec = dump_history(context, container_key, item, count++);
    records.insert(records.end(), rec.begin(), rec.end());
  }
  return records;
}

/// Dump hit container
void DigiStoreDump::dump_history(DigiContext& context,
				 const std::string& tag,
				 const DigiEvent& event,
				 const DataSegment& segment)  const
{
  std::vector<std::string> records;
  std::lock_guard<std::mutex> lock(segment.lock);

  records.emplace_back(format("+--- %-12s segment: %ld entries", tag.c_str(), segment.size()));
  for ( const auto& entry : segment )     {
    Key key {entry.first};
    const std::any& data = entry.second;
    bool use = m_containers.empty() ||
      std::find(m_container_items.begin(), m_container_items.end(), key.item()) != m_container_items.end();
    use &= m_masks.empty() || 
      std::find(m_masks.begin(), m_masks.end(), key.mask()) != m_masks.end();

    if ( use )   {
      std::vector<std::string> rec;
      if ( const auto* mapping = std::any_cast<DepositMapping>(&data) )   {
	rec = dump_deposit_history(context, key, *mapping);
      }
      else if ( const auto* vector = std::any_cast<DepositVector>(&data) )   {
	rec = dump_deposit_history(context, key, *vector);
      }
      else if ( const auto* parts = std::any_cast<ParticleMapping>(&data) )   {
	rec = dump_particle_history(context, key, *parts);
      }
      else if ( const auto* adcs = std::any_cast<DetectorResponse>(&data) )   {
	rec = { format("|----  %s", data_header(key, "ADC values", *adcs).c_str()) };
      }
      else if ( const auto* hist = std::any_cast<DetectorHistory>(&data) )   {
	rec = { format("|----  %s", data_header(key, "histories", *hist).c_str()) };
      }
      else   {
	rec = { format("|----  %s", data_header(key, "", data).c_str()) };
      }
      records.insert(records.end(), rec.begin(), rec.end());
    }
  }
  std::lock_guard<std::mutex> record_lock(m_kernel.global_output_lock());
  for(const auto& s : records)
    info("%s%s", event.id(), s.c_str());
}

/// Dump hit container
void DigiStoreDump::dump_headers(const std::string& tag,
				 const DigiEvent&   event,
				 const DataSegment& segment)  const
{
  std::string str;
  std::vector<std::string> records;
  info("%s+--- %-12s segment: %ld entries", event.id(), tag.c_str(), segment.size());
  std::lock_guard<std::mutex> lock(segment.lock);
  for ( const auto& entry : segment )     {
    Key key {entry.first};
    const std::any& data = entry.second;
    if ( const auto* mapping = std::any_cast<DepositMapping>(&data) )
      str = data_header(key, "deposits", *mapping);
    else if ( const auto* vector = std::any_cast<DepositVector>(&data) )
      str = data_header(key, "deposits", *vector);
    else if ( const auto* parts = std::any_cast<ParticleMapping>(&data) )
      str = data_header(key, "particles", *parts);
    else if ( const auto* adcs = std::any_cast<DetectorResponse>(&data) )
      str = data_header(key, "ADC values", *adcs);
    else if ( const auto* hist = std::any_cast<DetectorHistory>(&data) )
      str = data_header(key, "histories", *hist);
    else if ( data.type() == typeid(void) )
      str = data_header(key, "void data", data);
    else
      str = data_header(key, "", data);
    records.push_back(str);
  }
  std::lock_guard<std::mutex> record_lock(m_kernel.global_output_lock());
  for(const auto& s : records)
    info("%s|----  %s", event.id(), s.c_str());
}

/// Main functional callback
void DigiStoreDump::execute(DigiContext& context)  const    {
  const auto& event = context.event;
  for( const auto& segment : this->m_segments )    {
    std::string seg = detail::str_upper(segment);
    if ( m_dump_history )    {
      dump_history(context, seg, *event, event->get_segment(segment));
      continue;
    }
    dump_headers(seg, *event, event->get_segment(segment));
  }
}
