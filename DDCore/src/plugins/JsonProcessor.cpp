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

/**
   Note:
   This is experimental playground.

   It should only proof the concept, that actually input parsers may be mixed:
   The MiniTel_json example starts with xml, then inputs a json file, which
   is processed here....

   If json is supposed to be really used, then the corresponding machinery
   first has to be implemented, similar to simething like Compact or DDDB.

   M. Frank
*/

// Framework inlcude files
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <JSON/Helper.h>
#include <JSON/DocumentHandler.h>
#include <JSON/Conversions.h>
#include <DD4hep/Plugins.h>
#include <DD4hep/Printout.h>
#include <DD4hep/IDDescriptor.h>
#include <DD4hep/detail/SegmentationsInterna.h>
#include <DD4hep/detail/DetectorInterna.h>
#include <DD4hep/detail/ObjectsInterna.h>

// C/C++ include files
#include <boost/property_tree/json_parser.hpp>

namespace {
  class Json;
  class detector;
}

using namespace dd4hep;

static void setChildTitles(const std::pair<std::string, DetElement>& e) {
  DetElement parent = e.second.parent();
  const DetElement::Children& children = e.second.children();
  if (::strlen(e.second->GetTitle()) == 0) {
    e.second->SetTitle(parent.isValid() ? parent.type().c_str() : e.first.c_str());
  }
  for_each(children.begin(), children.end(), setChildTitles);
}

namespace dd4hep  {
    template <> void Converter<detector>::operator()(json_h element) const;
}

/// This is the identical converter as it is used for the XML compact!
template <> void Converter<detector>::operator()(json_h element) const {
  std::string type = element.attr<std::string>(_U(type));
  std::string name = element.attr<std::string>(_U(name));
  std::string name_match = ":" + name + ":";
  std::string type_match = ":" + type + ":";

  try {
    json_attr_t attr_par = element.attr_nothrow(_U(parent));
    if (attr_par) {
      // We have here a nested detector. If the mother volume is not yet registered
      // it must be done here, so that the detector constructor gets the correct answer from
      // the call to Detector::pickMotherVolume(DetElement).
      std::string par_name = element.attr<std::string>(attr_par);
      DetElement parent_detector = description.detector(par_name);
      if ( !parent_detector.isValid() )  {
        except("Compact","Failed to access valid parent detector of %s",name.c_str());
      }
      description.declareParent(name, parent_detector);
    }
    json_attr_t attr_ro  = element.attr_nothrow(_U(readout));
    SensitiveDetector sd;
    Segmentation seg;
    if ( attr_ro )   {
      Readout ro = description.readout(element.attr<std::string>(attr_ro));
      if (!ro.isValid()) {
        throw std::runtime_error("No Readout structure present for detector:" + name);
      }
      seg = ro.segmentation();
      sd = SensitiveDetector(name, "sensitive");
      sd.setHitsCollection(ro.name());
      sd.setReadout(ro);
      description.addSensitiveDetector(sd);
    }
    Handle<NamedObject> sens = sd;
    DetElement det(PluginService::Create<NamedObject*>(type, &description, &element, &sens));
    if (det.isValid()) {
      setChildTitles(std::make_pair(name, det));
      if ( sd.isValid() )  {
        det->flag |= DetElement::Object::HAVE_SENSITIVE_DETECTOR;
      }
      if ( seg.isValid() )  {
        seg->sensitive = sd;
        seg->detector  = det;
      }
    }
    printout(det.isValid() ? INFO : ERROR, "Compact", "%s subdetector:%s of type %s %s",
             (det.isValid() ? "++ Converted" : "FAILED    "), name.c_str(), type.c_str(),
             (sd.isValid() ? ("[" + sd.type() + "]").c_str() : ""));

    if (!det.isValid()) {
      PluginDebug dbg;
      PluginService::Create<NamedObject*>(type, &description, &element, &sens);
      throw std::runtime_error("Failed to execute subdetector creation plugin. " + dbg.missingFactory(type));
    }
    description.addDetector(det);
    return;
  }
  catch (const std::exception& e) {
    printout(ERROR, "Compact", "++ FAILED    to convert subdetector: %s: %s", name.c_str(), e.what());
    std::terminate();
  }
  catch (...) {
    printout(ERROR, "Compact", "++ FAILED    to convert subdetector: %s: %s", name.c_str(), "UNKNONW Exception");
    std::terminate();
  }
}

static long handle_json(Detector& description, int argc, char** argv) {
  if ( argc < 1 || (argc<2 && argv[0][0] != '/') )  {
    ::printf("DD4hep_JsonProcessor <file> <directory>                \n"
             "  If file is an absolute path (does NOT start with '/')\n"
             "  the directory path is mandatory.                     \n"
             "  The file name is then assumed to be relative.        \n"
             "\n");
    exit(EINVAL);
  }
  std::string file = argv[0];
  if ( file[0] != '/' ) file = std::string(argv[1]) + "/" + file;

  printout(INFO,"JsonProcessor","++ Processing JSON input: %s",file.c_str());
  json::DocumentHolder doc(json::DocumentHandler().load(file.c_str()));
  json::Element elt = doc.root();

  json_coll_t(elt,_U(detector)).for_each(Converter<detector>(description));
  printout(INFO,"JsonProcessor","++ ... Successfully processed JSON input: %s",file.c_str());
  return 1;
}
DECLARE_APPLY(DD4hep_JsonProcessor,handle_json)
