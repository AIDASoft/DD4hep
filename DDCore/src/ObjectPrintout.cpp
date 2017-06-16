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
#include "DD4hep/Detector.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DD4hep/Printout.h"

// C/C++ include files
#include <cstdarg>
#include <stdexcept>

using namespace std;

#include "TMap.h"
#include "TROOT.h"
#include "TColor.h"
using namespace std;
namespace dd4hep {
  using namespace detail;

  template <typename T> void PrintMap<T>::operator()() const {
    Printer < T > p(description, os);
    os << "++" << endl << "++          " << text << endl << "++" << endl;
    for (Detector::HandleMap::const_iterator i = cont.begin(); i != cont.end(); ++i)
      p((*i).second);
  }

  template <> void Printer<Handle<NamedObject> >::operator()(const Handle<NamedObject>& val) const {
    printout(INFO, "Printer", "++ %s Handle:%s %s", prefix.c_str(), val->GetName(), val->GetTitle());
  }
  template <> void Printer<Handle<TNamed> >::operator()(const Handle<TNamed>& val) const {
    printout(INFO, "Printer", "++ %s Handle:%s %s", prefix.c_str(), val->GetName(), val->GetTitle());
  }

  template <> void Printer<Constant>::operator()(const Constant& val) const {
    printout(INFO, "Printer", "++ %s Constant:%s %s", prefix.c_str(), val->GetName(), val.toString().c_str());
  }

  template <> void Printer<Material>::operator()(const Material& val) const {
    printout(INFO, "Printer", "++ %s Material:%s %s", prefix.c_str(), val->GetName(), val.toString().c_str());
  }

  template <> void Printer<VisAttr>::operator()(const VisAttr& val) const {
    printout(INFO, "Printer", "++ %s VisAttr: %s", prefix.c_str(), val.toString().c_str());
  }

  template <> void Printer<Readout>::operator()(const Readout& val) const {
    printout(INFO, "Printer", "++ %s Readout: %s of type %s", prefix.c_str(), val->GetName(), val->GetTitle());
  }

  template <> void Printer<Region>::operator()(const Region& val) const {
    printout(INFO, "Printer", "++ %s Region:  %s of type %s", prefix.c_str(), val->GetName(), val->GetTitle());
  }

  template <> void Printer<RotationZYX>::operator()(const RotationZYX& val) const {
    printout(INFO, "Printer", "++ %s ZYXRotation: phi: %7.3 rad theta: %7.3 rad psi: %7.3 rad", prefix.c_str(), val.Phi(),
             val.Theta(), val.Psi());
  }

  template <> void Printer<Position>::operator()(const Position& val) const {
    printout(INFO, "Printer", "++ %s Position:    x: %9.3 mm y: %9.3 mm z: %9.3 mm", prefix.c_str(), val.X(), val.Y(), val.Z());
  }
  template <> void Printer<Condition>::operator()(const Condition& val) const {
    int flg = Condition::WITH_IOV|Condition::WITH_ADDRESS;
    printout(INFO, "Printer", "++ %s %s", prefix.c_str(), val.str(flg).c_str());
  }
#if 0
  template <> void Printer<LimitSet>::operator()(const LimitSet& val) const {
    const set<Limit>& o = val.limits();
    printout(INFO, "Printer", "++ %s LimitSet: %s", prefix.c_str(), val.name());
    val->TNamed::Print();
    for (set<Limit>::const_iterator i = o.begin(); i != o.end(); ++i) {
      os << "++    Limit:" << (*i).name << " " << (*i).particles << " [" << (*i).unit << "] " << (*i).content << " "
         << (*i).value << endl;
    }
  }

  template <> void Printer<DetElement>::operator()(const DetElement& val) const {
    DetElement::Object* obj = val.data<DetElement::Object>();
    if (obj) {
      char text[256];
      const DetElement& sd = val;
      PlacedVolume plc = sd.placement();
      bool vis = plc.isValid();
      bool env = plc.isValid();
      bool mat = plc.isValid();
      ::snprintf(text, sizeof(text), "ID:%-3d Combine Hits:%3s Material:%s Envelope:%s VisAttr:%s", sd.id(),
                 yes_no(sd.combineHits()), mat ? plc.material().name() : yes_no(mat),
                 env ? plc.motherVol()->GetName() : yes_no(env), yes_no(vis));
      os << prefix << "+= DetElement: " << val.name() << " " << val.type() << endl;
      os << prefix << "|               " << text << endl;

      if (vis) {
        VisAttr attr = plc.volume().visAttributes();
        VisAttr::Object* v = attr.data<VisAttr::Object>();
        TColor* col = gROOT->GetColor(v->color);
        char text[256];
        ::snprintf(text, sizeof(text), " RGB:%-8s [%d] %7.2f  Style:%d %d ShowDaughters:%3s Visible:%3s", col->AsHexString(),
                   v->color, col->GetAlpha(), int(v->drawingStyle),
                   int(v->lineStyle), yes_no(v->showDaughters),
                   yes_no(v->visible));
        os << prefix << "|               VisAttr:  " << setw(32) << left << attr.name() << text << endl;
      }
      if (plc.isValid()) {
        Volume vol = plc.volume();
        Solid s = vol.solid();
        Material m = vol.material();
        ::snprintf(text, sizeof(text), "Volume:%s Shape:%s Material:%s", vol->GetName(), s.isValid() ? s.name() : "Unknonw",
                   m.isValid() ? m->GetName() : "Unknown");
        os << prefix << "+-------------  " << text << endl;
      }
      const DetElement::Children& ch = sd.children();
      for (DetElement::Children::const_iterator i = ch.begin(); i != ch.end(); ++i)
        Printer < DetElement > (description, os, prefix + "| ")((*i).second);
      return;
    }
  }
#endif
  template <> void Printer<const Detector*>::operator()(const Detector* const &) const {
    //Header(description.header()).fromCompact(doc,compact.child(Tag_info),Strng_t("In memory"));
    PrintMap < Constant > (description, os, description->constants(), "List of Constants")();
    //PrintMap < VisAttr > (description, os, description->visAttributes(), "List of Visualization attributes")();
    //PrintMap < LimitSet > (description, os, description->readouts(), "List of Readouts")();
    //PrintMap < Region > (description, os, description->regions(), "List of Regions")();
    //PrintMap < DetElement > (description, os, description->detectors(), "List of DetElements")();
  }
}
