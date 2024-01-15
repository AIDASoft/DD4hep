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
#include <DD4hep/Printout.h>
#include <DDDigi/DigiData.h>
#include <DDDigi/DigiSegmentationTool.h>

#include <sstream>

using namespace std;
using namespace dd4hep::digi;

namespace  {
  void scan_detector(const DigiSegmentationTool& tool,
		     const string& split_by,
		     map<dd4hep::VolumeID, pair<dd4hep::DetElement, dd4hep::VolumeID> >& splits,
		     dd4hep::DetElement de, dd4hep::VolumeID vid, dd4hep::VolumeID mask)   {
    dd4hep::PlacedVolume plc = de.placement();
    const auto&      new_ids = plc.volIDs();
    dd4hep::VolumeID new_vid = vid;
    dd4hep::VolumeID new_msk = mask;
    if ( !new_ids.empty() )   {
      new_vid |= tool.iddescriptor.encode(new_ids);
      new_msk |= tool.iddescriptor.get_mask(new_ids);
      for (const auto& id : new_ids)   {
	if ( id.first == split_by )   {
	  splits.emplace(new_vid, make_pair(de, id.second));
	  return;
	}
      }
    }
    for ( const auto& c : de.children() )
      scan_detector(tool, split_by, splits, c.second, new_vid, new_msk);
  }
}

/// Split field name
const string& DigiSegmentContext::name()  const  {
  if ( this->field )  {
    return this->field->name();
  }
  throw std::runtime_error("Invalid field name!");
}

/// Split field name
const char* DigiSegmentContext::cname()  const  {
  return this->field ? this->field->name().c_str() : "";
}

/// Full identifier (field + id)
std::string DigiSegmentContext::identifier(uint32_t id)  const   {
  std::stringstream str;
  if ( this->field )   {
    str << this->field->name() << "." << id;
  }
  return str.str();
}

/// Initializing constructor
DigiSegmentationTool::DigiSegmentationTool(Detector& desc)
  : description(desc)
{
}

/// Setup tool to handle a given detector of the geometry
void DigiSegmentationTool::set_detector(const string& det_name)    {
  const char* det = det_name.c_str();
  this->detector = this->description.detector(det_name);
  if ( !this->detector.isValid() )   {
    except("DigiSegmentationTool",
	   "FAILED: Cannot access subdetector %s from the detector description.", det);
  }
  this->sensitive = this->description.sensitiveDetector(det_name);
  if ( !sensitive.isValid() )   {
    except("DigiSegmentationTool",
	   "FAILED: Cannot access sensitive detector for %s.", det);
  }
  this->iddescriptor = this->sensitive.idSpec();
  if ( !this->iddescriptor.isValid() )   {
    except("DigiSegmentationTool",
	   "FAILED: Cannot access ID descriptor for detector %s.", det);
  }
}

/// Access the readout collection keys
vector<string> DigiSegmentationTool::collection_names()   const   {
  if ( this->sensitive.isValid() )    {
    Readout rd = this->sensitive.readout();
    vector<string> names = rd.collectionNames();
    if ( names.empty() ) names.emplace_back(rd.name());
    return names;
  }
  except("DigiSegmentationTool",
	 "+++ collection_names: Readout not valid. Is the proper detector set ?");
  return {};
}

/// Access the readout collection keys
vector<Key> DigiSegmentationTool::collection_keys()   const   {
  return collection_keys(0x0);
}

/// Access the readout collection keys
vector<Key> DigiSegmentationTool::collection_keys(Key::mask_type mask)   const   {
  vector<Key> keys;
  vector<string> names = collection_names();
  for( const auto& collection : names )
    keys.emplace_back(Key(collection, mask));
  return keys;
}

/// Access the readout collection keys
vector<Key> 
DigiSegmentationTool::collection_keys(const vector<string>& detectors) const  {
  vector<Key> keys;
  for( const auto& det : detectors )   {
    DigiSegmentationTool tool(this->description);
    tool.set_detector(det);
    auto det_keys = tool.collection_keys(0x0);
    keys.insert(keys.end(), det_keys.begin(), det_keys.end());
  }
  return keys;
}

/// Access the readout collection keys
vector<Key> 
DigiSegmentationTool::collection_keys(const vector<string>& detectors,
				      Key::mask_type mask)   const
{
  vector<Key> keys;
  for( const auto& det : detectors )   {
    DigiSegmentationTool tool(this->description);
    tool.set_detector(det);
    auto det_keys = tool.collection_keys(mask);
    keys.insert(keys.end(), det_keys.begin(), det_keys.end());
  }
  return keys;
}

/// Create a split context depending on the segmentation field
DigiSegmentContext 
DigiSegmentationTool::split_context(const string& split_by)  const {
  DigiSegmentContext splitter;
  splitter.cell_mask = ~0x0UL;
  splitter.detector  = this->detector;
  splitter.idspec    = this->iddescriptor;
  for(const auto& f : this->iddescriptor.fields())   {
    const BitFieldElement* e = f.second;
    if ( e->name() == split_by )   {
      splitter.field      = e;
      splitter.split_mask = e->mask();
      splitter.width      = e->width();
      splitter.offset     = e->offset();
      splitter.max_split  = 1 << e->width();
    }
    else   {
      splitter.det_mask |= e->mask();
    }
    splitter.cell_mask = (splitter.cell_mask << e->width());
    printout(INFO,"DigiSegmentationTool",
	     "%-24s %-8s [%3d,%7d] width:%2d offset:%2d mask:%016lX Split:%016lX Det:%016lX Cells:%016lX",
	     this->iddescriptor.name(), e->name().c_str(),
	     e->minValue(), e->maxValue(), e->width(), e->offset(),
	     e->mask(), splitter.split_mask, splitter.det_mask, splitter.cell_mask);
    if ( splitter.field ) break;
  }
  if ( !splitter.field )   {
    except("DigiSegmentationTool",
	   "FAILED: The ID descriptor for detector %s has no field %s.",
	   this->detector.name(), split_by.c_str());
  }
  return splitter;
}

/// Create full set of detector segments which can be split according to the context
set<uint32_t> DigiSegmentationTool::split_segmentation(const string& split_by)  const  {
  map<VolumeID, pair<DetElement, VolumeID> > segmentation_splits;
  PlacedVolume     place = this->detector.placement();
  const auto& ids = place.volIDs();
  VolumeID    vid = this->iddescriptor.encode(ids);
  VolumeID    msk = this->iddescriptor.get_mask(ids);
  const auto* fld = this->iddescriptor.field(split_by);
  const char* det = this->detector.name();

  if ( !fld )   {
    except("DigiSegmentationTool","Field discriminator %s does not exist in ID descriptor %s",
	   split_by.c_str(), this->iddescriptor.name());
  }
  ::scan_detector(*this, split_by, segmentation_splits, this->detector, vid, msk);
  stringstream str;
  set<uint32_t> splits;
  for( const auto& id : segmentation_splits )  {
    auto val = ((id.first&fld->mask())>>fld->offset());
    splits.insert(val);
  }
  for( const auto& id : splits )  {
    str << setw(16) << hex << setfill(' ') << id << " ";
  }
  printout(INFO,"DigiSegmentationTool",
	   "%-24s has %ld entries and %ld parallel entries when splitting by \"%s\"",
	   det, segmentation_splits.size(), splits.size(), split_by.c_str());
  printout(INFO,"DigiSegmentationTool","%-24s --> %-12s ids: %s",
	   "", split_by.c_str(), str.str().c_str());
  return splits;
}
