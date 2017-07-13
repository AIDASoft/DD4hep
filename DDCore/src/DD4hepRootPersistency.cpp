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
#include "DD4hep/Printout.h"
#include "DD4hep/DD4hepRootPersistency.h"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DD4hep/detail/SegmentationsInterna.h"

// ROOT include files
#include "TFile.h"
#include "TTimeStamp.h"

ClassImp(DD4hepRootPersistency)

using namespace dd4hep;
using namespace std;


int DD4hepRootPersistency::save(Detector& description, const char* fname, const char* instance)   {
  TFile* f = TFile::Open(fname,"RECREATE");
  if ( f && !f->IsZombie()) {
    TTimeStamp start;
    DD4hepRootPersistency* persist = new DD4hepRootPersistency();
    persist->m_data = new dd4hep::DetectorData();
    persist->m_data->adoptData(dynamic_cast<DetectorData&>(description),false);
    for( const auto& s : persist->m_data->m_sensitive )  {
      dd4hep::SensitiveDetector sd = s.second;
      dd4hep::Readout ro = sd.readout();
      if ( ro.isValid() && ro.segmentation().isValid() )  {
        persist->m_segments[ro].first  = ro.idSpec();
        persist->m_segments[ro].second = ro.segmentation().segmentation();
      }
    }
    for( const auto& mgr : persist->m_data->m_volManager->managers )  {
      for( const auto& v : mgr.second->volumes )  {
        persist->nominals[v.second->element] = v.second->element.nominal();
      }
    }
    printout(ALWAYS,"DD4hepRootPersistency","+++ Saving %ld nominals....",persist->nominals.size());

    /// Now we write the object
    int nBytes = persist->Write(instance);
    f->Close();
    TTimeStamp stop;
    printout(ALWAYS,"DD4hepRootPersistency",
             "+++ Wrote %d Bytes of geometry data '%s' to '%s'  [%8.3f seconds].",
             nBytes, instance, fname, stop.AsDouble()-start.AsDouble());
    if ( nBytes > 0 )  {
      printout(ALWAYS,"DD4hepRootPersistency",
               "+++ Successfully saved geometry data to file.");
    }
    delete f;
    delete persist;
    return nBytes;
  }
  printout(ERROR,"DD4hepRootPersistency","+++ Cannot open file '%s'.",fname);
  return 0;
}

int DD4hepRootPersistency::load(Detector& description, const char* fname, const char* instance)  {
  TFile* f = TFile::Open(fname);
  if ( f && !f->IsZombie()) {
    TTimeStamp start;
    unique_ptr<DD4hepRootPersistency> persist((DD4hepRootPersistency*)f->Get(instance));
    if ( persist.get() )   {
      DetectorData* source = persist->m_data;
      const auto& iddesc = persist->idSpecifications();
      for( const auto& s : iddesc )  {
        IDDescriptor id = s.second;
        id.rebuild(id->description);
      }
      printout(ALWAYS,"DD4hepRootPersistency",
               "+++ Fixed %ld IDDescriptor objects.",iddesc.size());
      for( const auto& s : persist->m_segments )  {
        Readout ro = s.first;
        IDDescriptor id = s.second.first;
        DDSegmentation::Segmentation* seg = s.second.second;
        ro.setSegmentation(Segmentation(seg->type(),seg->name(),id.decoder()));
        delete seg;
      }
      printout(ALWAYS,"DD4hepRootPersistency",
               "+++ Fixed %ld segmentation objects.",persist->m_segments.size());
      persist->m_segments.clear();
      const auto& sdets = persist->volumeManager()->subdetectors;
      size_t num[3] = {0,0,0};
      for( const auto& vm : sdets )  {
        VolumeManager::Object* obj = vm.second.ptr();
        obj->system = obj->id.field("system");
        if ( 0 != obj->system )   {
          printout(ALWAYS,"DD4hepRootPersistency",
                   "+++ Fixed VolumeManager.system for %-24s  %6ld volumes %4ld sdets %4ld mgrs.",
                   obj->detector.path().c_str(), obj->volumes.size(),
                   obj->subdetectors.size(), obj->managers.size());
          num[0] += obj->volumes.size();
          num[1] += obj->subdetectors.size();
          num[2] += obj->managers.size();
          continue;
        }
        printout(ALWAYS,"DD4hepRootPersistency",
                 "+++ FAILED to fix VolumeManager.system for '%s: %s'.",
                 obj->detector.path().c_str(), "[No IDDescriptor field 'system']");
      }
      printout(ALWAYS,"DD4hepRootPersistency",
               "+++ Fixed VolumeManager TOTALS     %-24s  %6ld volumes %4ld sdets %4ld mgrs.","",num[0],num[1],num[2]);
      printout(ALWAYS,"DD4hepRootPersistency","+++ loaded %ld nominals....",persist->nominals.size());
      DetectorData* tar_data = dynamic_cast<DetectorData*>(&description);
      DetectorData* src_data = dynamic_cast<DetectorData*>(source);
      tar_data->adoptData(*src_data,false);
      TTimeStamp stop;
      printout(ALWAYS,"DD4hepRootPersistency",
               "+++ Successfully loaded detector description from file:%s  [%8.3f seconds]",
               fname, stop.AsDouble()-start.AsDouble());
      return 1;
    }
    printout(ERROR,"DD4hepRootPersistency",
             "+++ Cannot Cannot load instance '%s' from file '%s'.",
             instance, fname);
    f->ls();
    delete f;
    return 0;
  }
  printout(ERROR,"DD4hepRootPersistency","+++ Cannot open file '%s'.",fname);
  return 0;
}


#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/ConditionsInterna.h"
#include "DD4hep/detail/AlignmentsInterna.h"
namespace {

  class PersistencyChecks  {

  public:
    
    size_t errors = 0;

    /// Default constructor
    PersistencyChecks() = default;

    size_t checkConstant(const std::pair<string,Constant>& obj)  {
      if ( obj.first.empty() || obj.second->name.empty() )  {
        printout(ERROR,"chkConstant","+++ Invalid constant: key error %s <> %s [%s]",
                 obj.first.c_str(), obj.second->GetName(), obj.second->GetTitle());
        ++errors;
        return 0;
      }
      if ( obj.first != obj.second->GetName() )  {
        printout(ERROR,"chkConstant","+++ Invalid constant: key error %s <> %s [%s]",
                 obj.first.c_str(), obj.second->GetName(), obj.second->GetTitle());
        ++errors;
        return 0;
      }
      return 1;
    }
    
    size_t checkProperty(const std::pair<string,map<string,string> >& obj)  {
      if ( obj.first.empty() || obj.second.empty() )  {
        printout(ERROR,"chkProperty","+++ Emptty property set: %s",obj.first.c_str());
        ++errors;
        return 0;
      }
      return 1;
    }
    
    size_t printDetElement(DetElement d)  {
      const DetElement::Children& children = d.children();
      size_t count = 1;
      printout(INFO,"chkDetector","+++ %-40s Level:%3d Key:%08X VolID:%016llX",
               d.name(), d.level(), d.key(), d.volumeID());
      Alignment ideal = d.nominal();
      if ( ideal.isValid() )  {
        const Delta& delta = ideal.delta();
        printout(INFO,"chkDetector","+++     Ideal: %s  Delta:%s--%s--%s",
                 yes_no(ideal.isValid()),
                 delta.hasTranslation() ? "Translation" : "         ",
                 delta.hasRotation()    ? "Rotation"    : "        ",
                 delta.hasPivot()       ? "Pivot"       : "    "
                 );
        //ideal.worldTransformation().Print();
      }
      else  {
        printout(INFO,"chkDetector","+++     Ideal: %s",yes_no(ideal.isValid()));
      }
      for( const auto& c : children )
        count += printDetElement(c.second);
      return count;
    }

    size_t checkDetector(DetElement d)   {
      printout(INFO,"chkDetector","+++ Checking Sub-Detector: %-40s Key:%08X VolID:%016llX",
               d.name(), d.key(), d.volumeID());
      return printDetElement(d);
    }


    size_t checkSensitive(SensitiveDetector d)   {
      printout(INFO,"chkDetector","+++ Checking SensitiveDetector: %-30s %-16s CombineHits:%s ecut:%g Collection:%s",
               d.name(), ("["+d.type()+"]").c_str(),
               yes_no(d.combineHits()), d.energyCutoff(), d.hitsCollection().c_str());
      Readout ro = d.readout();
      if ( !ro.isValid() )  {
        printout(ERROR,"chkDetector",
                 "+++ FAILED   SensitiveDetector:%s No Readout Stricture attached.",
                 ro.name());
        ++errors;
        return 0;
      }
      if ( ro.isValid() )  {
        if ( !checkReadout(ro) )  {
          return 0;
        }
      }
      return 1;
    }

    size_t checkReadout(Readout ro)  {
      size_t ret = 1;
      printout(INFO,"chkReadOut","+++ Checking Readout: %s Collection No:%ld IDspec:%s",
               ro.name(), ro.numCollections(), yes_no(ro.idSpec().isValid()));
      IDDescriptor id = ro.idSpec();
      if ( id.isValid() )  {
        if ( !checkIDDescriptor(id) ) ret = 0;
      }
      else  {
        printout(ERROR,"chkReadOut",
                 "+++ FAILED   Segmentation: %s Found readout without ID descriptor!",
                 ro.name());
        ++errors;
        ret = 0;
      }
      Segmentation sg = ro.segmentation();
      if ( sg.isValid() )  {
        DDSegmentation::Segmentation* seg = sg.segmentation();
        if ( !checkSegmentation(sg) )  {
          ret = 0;
        }
        if ( id.isValid() && seg && id.decoder() != sg.decoder() )  {
          printout(ERROR,"chkReadOut","+++ FAILED   Decoder ERROR:%p != %p",
                   (void*)seg->decoder(), (void*)id.decoder());
          ++errors;
          ret = 0;
        }        
      }
      return ret;
    }

    size_t checkSegmentation(Segmentation sg)  {
      DDSegmentation::Segmentation* seg = sg.segmentation();
      if ( seg )  {
        size_t ret = 1;
        printout(INFO,"chkSegment","+++ Checking Segmentation: %-24s [%s] DDSegmentation:%p  Decoder:%p",
                 sg.name(), seg->type().c_str(),(void*)seg, seg->decoder());
        const auto& pars = seg->parameters();
        for ( const auto& p : pars )   {
          printout(INFO,"chkSegment","+++             Param:%-24s  -> %-16s  [%s] opt:%s",
                   p->name().c_str(), p->value().c_str(), p->type().c_str(),
                   yes_no(p->isOptional()));
            
        }
        if ( pars.empty() )   {
          printout(ERROR,"chkSegment",
                   "+++ FAILED   Segmentation: %s Found readout with invalid [EMPTY] ID descriptor!",
                   sg.name());
          ++errors;
          ret = 0;
        }
        if ( !seg->decoder() )   {
          printout(ERROR,"chkSegment","+++ FAILED   Segmentation: %s  NO Decoder!!!",
                   sg.name());
          ++errors;
          ret = 0;
        }
        return ret;
      }
      printout(ERROR,"chkSegment","+++ FAILED   Segmentation: %s  NO DDSegmentation",
               sg.name());
      ++errors;
      return 0;
    }

    size_t checkIDDescriptor(IDDescriptor id)   {
      const IDDescriptor::FieldMap& fields = id.fields();
      printout(INFO,"chkIDDescript","+++ Checking IDDesc:  %s decoder:%p",
               id->GetName(), (void*)id.decoder());
      printout(INFO,"chkIDDescript","+++             Specs:%s",id.fieldDescription().c_str());
      for( const auto& f : fields )
        printout(INFO,"chkIDDescript","+++             Field:%-24s  -> %016llX  %3d %3d [%d,%d]",
                 f.first.c_str(), f.second->mask(), f.second->offset(), f.second->width(),
                 f.second->minValue(), f.second->maxValue());
      if ( fields.empty() )   {
        printout(ERROR,"chkIDDescript",
                 "+++ FAILED   IDDescriptor: %s Found invalid [EMPTY] ID descriptor!",
                 id.name());
        ++errors;
        return 0;
      }
      return 1;
    }

    size_t checkLimitset(LimitSet ls)   {
      printout(INFO,"chkLimitSet","+++ Checking Limits:  %s Num.Limiits:%ld",
               ls.name(), ls.limits().size());
      return 1;
    }

    size_t checkRegion(Region ls)   {
      printout(INFO,"chkRegion","+++ Checking Limits:  %s Num.Limiits:%ld",
               ls.name(), ls.limits().size());
      return 1;
    }

    size_t checkField(CartesianField fld)   {
      printout(INFO,"chkField","+++ Checking Field:  %s",
               fld.name());
      return 1;
    }
    size_t checkAlignment(DetElement det)   {
      AlignmentCondition::Object* align = det->nominal.ptr();
      if ( 0 == align )  {
        printout(ERROR,"chkNominals",
                 "+++ ERROR +++ Detector element with invalid nominal:%s",
                 det.path().c_str());
        ++errors;
      }
      else if ( 0 == align->alignment_data )  {
        printout(ERROR,"chkNominals",
                 "+++ ERROR +++ Detector element with invalid nominal data:%s",
                 det.path().c_str());
        ++errors;
      }
      else if ( Condition(align).data().ptr() != align->alignment_data )  {
        printout(ERROR,"chkNominals",
                 "+++ ERROR +++ Detector element with inconsisten nominal data:%s [%p != %p]",
                 det.path().c_str(),Condition(align).data().ptr(),align->alignment_data);
        ++errors;
      }
      else  {
        return 1;
      }
      return 0;
    }
    
    /// Check nominal alignments of the volume manager
    size_t checkNominals(VolumeManager mgr)   {
      int count = 0;
      const auto& sdets = mgr->subdetectors;
      for( const auto& vm : sdets )  {
        VolumeManager::Object* obj   = vm.second.ptr();
        for( const auto& iv : obj->volumes )  {
          VolumeManagerContext* ctx   = iv.second;
          count += checkAlignment(ctx->element);
        }
      }
      return count;
    }
    size_t checkDetectorNominals(DetElement d)   {
      int count = 0;
      Volume v = d.placement().volume();
      if ( v.isSensitive() )   {
        count += checkAlignment(d);
      }
      for( const auto& c : d.children() )
        count += checkDetectorNominals(c.second);
      return count;
    }
  };
}

/// Check the collection of define statements
size_t DD4hepRootCheck::checkConstants()   const   {
  size_t count = 0;
  PersistencyChecks checks;
  for( const auto& obj : object->constants() )
    count += checks.checkConstant(obj);
  printout(ALWAYS,"chkProperty","+++ %s Checked %ld Constant objects. Num.Errors: %ld",
           checks.errors==0 ? "PASSED" : "FAILED", count, checks.errors); 
  return checks.errors;
}

/// Check detector description properties (string constants)
size_t DD4hepRootCheck::checkProperties()   const   { 
  size_t count = 0;
  PersistencyChecks checks;
  for( const auto& obj : object->properties() )
    count += checks.checkProperty(obj);
  printout(ALWAYS,"chkProperty","+++ %s Checked %ld Property objects. Num.Errors: %ld",
           checks.errors==0 ? "PASSED" : "FAILED", count, checks.errors); 
  return checks.errors;
}

/// Call to check a Material object
size_t DD4hepRootCheck::checkMaterials()  const   {
  size_t count = 0;
  printout(ALWAYS,"chkMaterials","+++ Check not implemented. Hence OK.");
  return count;
}

/// Call to check a Readout object
size_t DD4hepRootCheck::checkReadouts()   const   {
  size_t count = 0;
  PersistencyChecks checks;
  for( const auto& obj : object->readouts() )
    count += checks.checkReadout(obj.second);

  if ( object->sensitiveDetectors().size() != object->readouts().size() )   {
    printout(ERROR,"chkNominals",
             "+++ Number of sensitive detectors does NOT match number of readouts: %ld != %ld",
             object->sensitiveDetectors().size(),object->readouts().size());
    ++checks.errors;
  }
  printout(ALWAYS,"chkNominals","+++ %s Checked %ld readout objects. Num.Errors: %ld",
           checks.errors==0 ? "PASSED" : "FAILED", count, checks.errors);
  return count;
}

/// Call to theck the DD4hep fields
size_t DD4hepRootCheck::checkFields()   const   {
  size_t count = 0;
  PersistencyChecks checks;
  for( const auto& obj : object->fields() )
    count += checks.checkField(obj.second);
  printout(ALWAYS,"chkFields","+++ %s Checked %ld Field objects. Num.Errors: %ld",
           checks.errors==0 ? "PASSED" : "FAILED", count, checks.errors); 
  return checks.errors;
}

/// Call to check a Region object
size_t DD4hepRootCheck::checkRegions()   const   {
  size_t count = 0;
  PersistencyChecks checks;
  for( const auto& obj : object->regions() )
    count += checks.checkRegion(obj.second);
  printout(ALWAYS,"chkRegions","+++ %s Checked %ld Region objects. Num.Errors: %ld",
           checks.errors==0 ? "PASSED" : "FAILED", count, checks.errors); 
  return checks.errors;
}

/// Call to check an ID specification
size_t DD4hepRootCheck::checkIdSpecs()   const   {
  size_t count = 0;
  PersistencyChecks checks;
  for( const auto& obj : object->idSpecifications() )
    count += checks.checkIDDescriptor(obj.second);
  printout(ALWAYS,"chkReadouts","+++ %s Checked %ld Readout objects. Num.Errors: %ld",
           checks.errors==0 ? "PASSED" : "FAILED", count, checks.errors); 
  return checks.errors;
}

/// Call to check a top level Detector element (subdetector)
size_t DD4hepRootCheck::checkDetectors()  const   {
  size_t count = 0;
  PersistencyChecks checks;
  for( const auto& obj : object->detectors() )
    count += checks.checkDetector(obj.second);
  printout(ALWAYS,"chkDetectors","+++ %s Checked %ld DetElement objects. Num.Errors: %ld",
           checks.errors==0 ? "PASSED" : "FAILED", count, checks.errors); 
  return checks.errors;
}

/// Call to check a sensitive detector
size_t DD4hepRootCheck::checkSensitives()   const   {
  size_t count = 0;
  PersistencyChecks checks;
  for( const auto& obj : object->sensitiveDetectors() )
    count += checks.checkSensitive(obj.second);
  printout(ALWAYS,"chkSensitives","+++ %s Checked %ld SensitiveDetector objects. Num.Errors: %ld",
           checks.errors==0 ? "PASSED" : "FAILED", count, checks.errors); 
  return checks.errors;
}

/// Call to check a limit-set object
size_t DD4hepRootCheck::checkLimitSets()   const   {
  PersistencyChecks checks;
  size_t count = 0;
  for( const auto& obj : object->limitsets() )
    count += checks.checkLimitset(obj.second);
  printout(ALWAYS,"chkSensitives","+++ %s Checked %ld SensitiveDetector objects. Num.Errors: %ld",
           checks.errors==0 ? "PASSED" : "FAILED", count, checks.errors); 
  return checks.errors;
}

/// Call to check the volume manager hierarchy
size_t DD4hepRootCheck::checkVolManager()   const   {
  PersistencyChecks checks;
  size_t count = checks.checkNominals(object->volumeManager());
  printout(ALWAYS,"chkNominals","+++ %s Checked %ld VolumeManager contexts. Num.Errors: %ld",
           checks.errors==0 ? "PASSED" : "FAILED", count, checks.errors);
  return count;
}

/// Call to check the nominal alignments in the detector hierarchy (for sensitive detectors)
size_t DD4hepRootCheck::checkNominals()   const   {
  size_t count = 0;
  PersistencyChecks checks;
  const auto& dets = object->sensitiveDetectors();
  for( const auto& d : dets )
    count += checks.checkDetectorNominals(object->detector(d.first));
  printout(ALWAYS,"chkNominals","+++ %s Checked %ld DetElements. Num.Errors: %ld",
           checks.errors==0 ? "PASSED" : "FAILED", count, checks.errors);
  return count;
}

/// Call to check the segmentations starting from the top level detector
size_t DD4hepRootCheck::checkSegmentations()   const   {
  size_t count = 0;
  PersistencyChecks checks;
  const auto& dets = object->sensitiveDetectors();
  for( const auto& d : dets )   {
    Segmentation seg = SensitiveDetector(d.second).readout().segmentation();
    if ( seg.isValid() )  count += checks.checkSegmentation(seg);
  }
  printout(ALWAYS,"chkNominals","+++ %s Checked %ld readout segmentations. Num.Errors: %ld",
           checks.errors==0 ? "PASSED" : "FAILED", count, checks.errors);
  return count;
}

size_t DD4hepRootCheck::checkAll()   const   {
  size_t count = 0;
  PersistencyChecks checks;

  count += checkMaterials();
  for( const auto& obj : object->properties() )
    count += checks.checkProperty(obj);
  for( const auto& obj : object->constants() )
    count += checks.checkConstant(obj);
  for( const auto& obj : object->limitsets() )
    count += checks.checkLimitset(obj.second);
  for( const auto& obj : object->fields() )
    count += checks.checkField(obj.second);
  for( const auto& obj : object->regions() )
    count += checks.checkRegion(obj.second);
  for( const auto& obj : object->idSpecifications() )
    count += checks.checkIDDescriptor(obj.second);
  for( const auto& obj : object->detectors() )
    count += checks.checkDetector(obj.second);
  for( const auto& obj : object->sensitiveDetectors() )
    count += checks.checkSensitive(obj.second);

  count += checkVolManager();
  printout(ALWAYS,"chkAll","+++ Checked %ld objects. Num.Errors:%ld",
           count, checks.errors);
  return count;
}
