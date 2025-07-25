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

// Framework includes
#include <Parsers/Parsers.h>
#include <DD4hep/Printout.h>
#include <DD4hep/AlignmentsPrinter.h>
#include <DD4hep/AlignmentsProcessor.h>
#include <DD4hep/detail/AlignmentsInterna.h>
#include <TClass.h>

// C/C++ include files
#include <sstream>

using namespace dd4hep;
using namespace dd4hep::align;

/// Initializing constructor
AlignmentsPrinter::AlignmentsPrinter(ConditionsMap* cond_map, const std::string& pref, int flg)
  : mapping(cond_map), name("Alignment"), prefix(pref), printLevel(INFO), m_flag(flg)
{
}

/// Callback to output alignments information
int AlignmentsPrinter::operator()(Alignment a)  const  {
  printAlignment(printLevel, name, a);
  return 1;
}

/// Callback to output alignments information of an entire DetElement
int AlignmentsPrinter::operator()(DetElement de, int level)  const   {
  if ( mapping )   {
    std::vector<Alignment> alignments;
    alignmentsCollector(*mapping,alignments)(de, level);
    printout(printLevel, name, "++ %s %-3ld Alignments for DE %s",
             prefix.c_str(), alignments.size(), de.path().c_str()); 
    for( auto alignment : alignments )
      (*this)(alignment);
    return int(alignments.size());
  }
  except(name,"Failed to dump conditions for DetElement:%s [No slice available]",
         de.path().c_str());
  return 0;
}

/// Initializing constructor
AlignedVolumePrinter::AlignedVolumePrinter(ConditionsMap* cond_map, const std::string& pref,int flg)
  : AlignmentsPrinter(cond_map, pref, flg)
{
  name = "Alignment";
}

/// Callback to output alignments information
int AlignedVolumePrinter::operator()(Alignment a)  const  {
  printAlignment(printLevel, name, a);
  return 1;
}

/// Default printout of an alignment entry
void dd4hep::align::printAlignment(PrintLevel lvl, const std::string& prefix, Alignment a)   {
  if ( a.isValid() )   {
    Alignment::Object* ptr = a.ptr();
    const AlignmentData& data = a.data();
    const Delta& D = data.delta;
    std::string new_prefix = prefix;
    new_prefix.assign(prefix.length(),' ');
    printout(lvl,prefix,"++ %s \t [%p] Typ:%s",
             new_prefix.c_str(), a.ptr(),
             typeName(typeid(*ptr)).c_str());
    printout(lvl,prefix,"++ %s \tData:(%11s-%8s-%5s)",
             new_prefix.c_str(), 
             D.hasTranslation() ? "Translation" : "",
             D.hasRotation() ? "Rotation" : "",
             D.hasPivot() ? "Pivot" : "");
    if ( isActivePrintLevel(lvl) )  {
      printf("WorldTrafo: "); data.worldTrafo.Print();
      printf("DetTrafo:   "); data.detectorTrafo.Print();
    }
  }
}

static std::string replace_all(const std::string& in, const std::string& from, const std::string& to)  {
  std::string res = in;
  std::size_t idx;
  while( std::string::npos != (idx=res.find(from)) )
    res.replace(idx,from.length(),to);
  return res;
}
static std::string _transformPoint2World(const AlignmentData& data, const Position& local)  {
  char text[256];
  Position world = data.localToWorld(local);
  ::snprintf(text,sizeof(text),"Local: (%7.3f , %7.3f , %7.3f )  -- > World:  (%7.3f , %7.3f , %7.3f )",
             local.x(), local.y(), local.z(), world.x(), world.y(), world.z());
  return text;
}

static std::string _transformPoint2Detector(const AlignmentData& data, const Position& local)  {
  char text[256];
  Position world = data.localToDetector(local);
  ::snprintf(text,sizeof(text),"Local: (%7.3f , %7.3f , %7.3f )  -- > Parent: (%7.3f , %7.3f , %7.3f )",
             local.x(), local.y(), local.z(), world.x(), world.y(), world.z());
  return text;
}

void dd4hep::align::printAlignment(PrintLevel lvl, const std::string& prefix,
                                   const std::string& opt, DetElement de, Alignment alignment)
{
  const std::string& tag = prefix;
  const AlignmentData& align_data = alignment.data();
  Condition  align_cond;// = align_data.condition;
  const Delta& align_delta = align_data.delta;
  std::string par = de.parent().isValid() ? de.parent().path() : std::string();
  Box bbox = de.placement().volume().solid();
  /// The edge positions of the bounding box:
  Position p1( bbox.x(), bbox.y(), bbox.z());
  Position p2( bbox.x(),-bbox.y(), bbox.z());
  Position p3(-bbox.x(), bbox.y(), bbox.z());
  Position p4(-bbox.x(),-bbox.y(), bbox.z());
  Position p5( bbox.x(), bbox.y(),-bbox.z());
  Position p6( bbox.x(),-bbox.y(),-bbox.z());
  Position p7(-bbox.x(), bbox.y(),-bbox.z());
  Position p8(-bbox.x(),-bbox.y(),-bbox.z());

  if ( align_cond.isValid() )  {
    printout(lvl,tag,"++ %s DATA: (%11s-%8s-%5s) %p IOV:%s", opt.c_str(), 
             align_delta.hasTranslation() ? "Translation" : "",
             align_delta.hasRotation() ? "Rotation" : "",
             align_delta.hasPivot() ? "Pivot" : "",
             alignment.ptr(),
             align_cond.iov().str().c_str());
  }
  else  {
    printout(lvl,tag,"++ %s DATA: (%11s-%8s-%5s) %p", opt.c_str(), 
             align_delta.hasTranslation() ? "Translation" : "",
             align_delta.hasRotation() ? "Rotation" : "",
             align_delta.hasPivot() ? "Pivot" : "",
             alignment.ptr());
  }
  if ( align_delta.hasTranslation() )  {
    std::stringstream str;
    Position copy(align_delta.translation * (1./dd4hep::cm));
    Parsers::toStream(copy, str);
    printout(lvl,tag,"++ %s DELTA Translation: %s [cm]",
             opt.c_str(), replace_all(str.str(),"\n","").c_str());
  }
  if ( align_delta.hasPivot() )  {
    std::stringstream str;
    Delta::Pivot copy(align_delta.pivot.Vect() * (1./dd4hep::cm));
    Parsers::toStream(copy, str);
    std::string res = replace_all(str.str(),"\n","");
    res = "( "+replace_all(res,"  "," , ")+" )";
    printout(lvl,tag,"++ %s DELTA Pivot:       %s [cm]", opt.c_str(), res.c_str());
  }
  if ( align_delta.hasRotation() )  {
    std::stringstream str;
    Parsers::toStream(align_delta.rotation, str);
    printout(lvl,tag,"++ %s DELTA Rotation:    %s [rad]", opt.c_str(), replace_all(str.str(),"\n","").c_str());
  }
  if ( isActivePrintLevel(lvl) )  {
    printf("%s %s WorldTrafo (to %s): ",opt.c_str(), tag.c_str(), de.world().path().c_str());
    align_data.worldTrafo.Print();
    printf("%s %s DetTrafo (to %s): ",opt.c_str(), tag.c_str(), par.c_str());
    align_data.detectorTrafo.Print();
  }
  printout(PrintLevel(lvl-1),tag,"++ %s: P1(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p1).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P2(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p2).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P3(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p3).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P4(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p4).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P5(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p5).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P6(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p6).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P7(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p7).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P8(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p8).c_str());

  printout(PrintLevel(lvl-1),tag,"++ %s: P1(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p1).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P2(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p2).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P3(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p3).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P4(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p4).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P5(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p5).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P6(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p6).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P7(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p7).c_str());
  printout(PrintLevel(lvl-1),tag,"++ %s: P8(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p8).c_str());
}

/// Default printout of a detector element entry
void dd4hep::align::printElement(PrintLevel prt_level, const std::string& prefix, DetElement de, ConditionsMap& pool)   {
  std::string tag = prefix+"Element";
  if ( de.isValid() )  {
    std::vector<Alignment> alignments;
    alignmentsCollector(pool,alignments)(de);
    printout(prt_level,tag,"++ Alignments of DE %s [%d entries]",
             de.path().c_str(), int(alignments.size()));
    for(const auto& align : alignments )
      printAlignment(prt_level, prefix, align);
    return;
  }
  except(tag,"Cannot process alignments of an invalid detector element");
}

/// PrintElement placement with/without alignment applied
void dd4hep::align::printElementPlacement(PrintLevel lvl, const std::string& prefix, DetElement de, ConditionsMap& pool)   {
  std::string tag = prefix+"Element";
  if ( de.isValid() )  {
    char text[132];
    Alignment    nominal = de.nominal();
    Box bbox = de.placement().volume().solid();
    std::vector<Alignment> alignments;

    alignmentsCollector(pool,alignments)(de);
    ::memset(text,'=',sizeof(text));
    text[sizeof(text)-1] = 0;
    printout(lvl, tag, text);
    printout(lvl, tag, "++ Alignments of DE %s [%d entries]",
             de.path().c_str(), int(alignments.size()));
    printout(lvl, tag, "++ Volume: %s  BBox: x=%7.3f y=%7.3f z=%7.3f",
             bbox.type(), bbox.x(), bbox.y(), bbox.z());
    printAlignment(lvl, tag, "NOMINAL", de, nominal);

    for(const auto& align : alignments )  {
      AlignmentCondition cond(align);
      try {
        printout(lvl, tag, "++ Alignment %p [%16llX]", align.ptr(), cond.key());
        printout(lvl, prefix, "++ \tPath:%s [%p]", cond.name(), align.ptr());
        printAlignment(lvl, tag, "ALIGNMENT", de, align);
      }
      catch(...)  {
        printout(ERROR, tag, "++ %s %s [%16llX]",
                 prefix.c_str(), "FAILED Alignment:", cond.key());
      }
    }
    return;
  }
  except(tag, "Cannot process alignments of an invalid detector element");
}
