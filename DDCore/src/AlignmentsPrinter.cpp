//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/Printout.h"
#include "DD4hep/DetAlign.h"
#include "DD4hep/AlignmentsPrinter.h"
#include "DD4hep/objects/AlignmentsInterna.h"

// C/C++ include files
#include <sstream>

using std::string;
using std::stringstream;
using namespace DD4hep;
using namespace DD4hep::Alignments;

/// Initializing constructor
AlignmentsPrinter::AlignmentsPrinter(const string& pref, int flg)
  : AlignmentsProcessor(0), name("Alignment"), prefix(pref), m_flag(flg)
{
}

/// Initializing constructor
AlignmentsPrinter::AlignmentsPrinter(UserPool* p, const std::string& pref,int flg)
  : AlignmentsProcessor(p), name("Alignment"), prefix(pref), m_flag(flg)
{
}

/// Callback to output alignments information
int AlignmentsPrinter::operator()(Alignment a)    {
  printAlignment(name, a);
  return 1;
}

/// Container callback for object processing
int AlignmentsPrinter::operator()(Container container)   {
  printContainer(name, container, m_pool);
  return 1;
}

/// Callback to output alignments information of an entire DetElement
int AlignmentsPrinter::processElement(DetElement de)  {
  printElement(name, de, m_pool);
  return 1;
}


/// Default printout of an alignment entry
void DD4hep::Alignments::printAlignment(const string& prefix, Alignment a)   {
  if ( a.isValid() )   {
    Alignment::Object* ptr = a.ptr();
    const Alignment::Data& data = a.data();
    Conditions::Condition  cond = data.condition;
    const Delta& D = data.delta;
    string new_prefix = prefix;
    new_prefix.assign(prefix.length(),' ');
    printout(INFO,prefix,"++ %s \tPath:%s [%p] Typ:%s",
             new_prefix.c_str(), cond.name(), a.ptr(),
             typeName(typeid(*ptr)).c_str());
    printout(INFO,prefix,"++ %s \tData:(%11s-%8s-%5s)",
             new_prefix.c_str(), 
             D.hasTranslation() ? "Translation" : "",
             D.hasRotation() ? "Rotation" : "",
             D.hasPivot() ? "Pivot" : "");
    printf("WorldTrafo: "); data.worldTrafo.Print();
    printf("DetTrafo:   "); data.detectorTrafo.Print();
  }
}

/// Default printout of an container entry
void DD4hep::Alignments::printContainer(const string& prefix, Container container, UserPool* pool)   {
  string tag = prefix+"Cont";
  if ( pool )  {
    for(const auto& k : container.keys() )  {
      try {
        Alignment align = container.get(k.first,*pool);
        printout(INFO,tag,"++ %s Alignment [%08X] -> [%08X] %s",
                 prefix.c_str(), k.first, k.second.first, k.second.second.c_str());
        printAlignment(prefix,align);
      }
      catch(...)  {
        printout(ERROR,tag,"++ %s %s [%08X] -> [%08X]",
                 prefix.c_str(), "FAILED Alignment:", k.first, k.second.first);
      }
    }
    return;
  }
  except(tag,"Cannot dump alignments container without valid user-pool.");
}

/// Default printout of a detector element entry
void DD4hep::Alignments::printElement(const string& prefix, DetElement de, UserPool* pool)   {
  string tag = prefix+"Element";
  if ( de.isValid() )  {
    if ( pool )  {
      DetAlign  a(de);
      Container c = a.alignments();
      printout(INFO,tag,"++ Alignments of DE %s [%d entries]",
               de.path().c_str(), int(c.keys().size()));
      printContainer(prefix, c, pool);
      return;
    }
    except(tag,"Cannot process DetElement alignments from '%s' without valid user-pool",de.name());
  }
  except(tag,"Cannot process alignments of an invalid detector element");
}


#include "TClass.h"
#include "DD4hep/ToStream.h"
static string replace_all(const string& in, const string& from, const string& to)  {
  string res = in;
  size_t idx;
  while( string::npos != (idx=res.find(from)) )
    res.replace(idx,from.length(),to);
  return res;
}
static string _transformPoint2World(const Alignment::Data& data, const Position& local)  {
  char text[256];
  Position world = data.localToWorld(local);
  ::snprintf(text,sizeof(text),"Local: (%7.3f , %7.3f , %7.3f )  -- > World:  (%7.3f , %7.3f , %7.3f )",
             local.x(), local.y(), local.z(), world.x(), world.y(), world.z());
  return text;
}

static string _transformPoint2Detector(const Alignment::Data& data, const Position& local)  {
  char text[256];
  Position world = data.localToDetector(local);
  ::snprintf(text,sizeof(text),"Local: (%7.3f , %7.3f , %7.3f )  -- > Parent: (%7.3f , %7.3f , %7.3f )",
             local.x(), local.y(), local.z(), world.x(), world.y(), world.z());
  return text;
}

static void printAlignmentEx(const string& prefix, const string& opt, DetElement de, Alignment alignment)  {
  using Geometry::Box;
  DetAlign      a(de);
  const string& tag = prefix;
  const Alignment::Data& align_data = alignment.data();
  Conditions::Condition  align_cond = align_data.condition;
  const Delta& align_delta = align_data.delta;
  string par = de.parent().isValid() ? de.parent().path() : string();
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
    printout(INFO,tag,"++ %s DATA: (%11s-%8s-%5s) %p IOV:%s", opt.c_str(), 
             align_delta.hasTranslation() ? "Translation" : "",
             align_delta.hasRotation() ? "Rotation" : "",
             align_delta.hasPivot() ? "Pivot" : "",
             alignment.ptr(),
             align_cond.iov().str().c_str());
  }
  else  {
    printout(INFO,tag,"++ %s DATA: (%11s-%8s-%5s) %p", opt.c_str(), 
             align_delta.hasTranslation() ? "Translation" : "",
             align_delta.hasRotation() ? "Rotation" : "",
             align_delta.hasPivot() ? "Pivot" : "",
             alignment.ptr());
  }
  if ( align_delta.hasTranslation() )  {
    stringstream str;
    Utils::toStream(align_delta.translation, str);
    printout(INFO,tag,"++ %s DELTA Translation: %s", opt.c_str(), replace_all(str.str(),"\n","").c_str());
  }
  if ( align_delta.hasPivot() )  {
    stringstream str;
    Utils::toStream(align_delta.pivot, str);
    string res = replace_all(str.str(),"\n","");
    res = "( "+replace_all(res,"  "," , ")+" )";
    printout(INFO,tag,"++ %s DELTA Pivot:       %s", opt.c_str(), res.c_str());
  }
  if ( align_delta.hasRotation() )  {
    stringstream str;
    Utils::toStream(align_delta.rotation, str);
    printout(INFO,tag,"++ %s DELTA Rotation:    %s", opt.c_str(), replace_all(str.str(),"\n","").c_str());
  }
  printf("%s %s WorldTrafo (to %s): ",opt.c_str(), tag.c_str(), de.world().path().c_str());
  align_data.worldTrafo.Print();
  printf("%s %s DetTrafo (to %s): ",opt.c_str(), tag.c_str(), par.c_str());
  align_data.detectorTrafo.Print();

  printout(INFO,tag,"++ %s: P1(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p1).c_str());
  printout(INFO,tag,"++ %s: P2(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p2).c_str());
  printout(INFO,tag,"++ %s: P3(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p3).c_str());
  printout(INFO,tag,"++ %s: P4(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p4).c_str());
  printout(INFO,tag,"++ %s: P5(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p5).c_str());
  printout(INFO,tag,"++ %s: P6(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p6).c_str());
  printout(INFO,tag,"++ %s: P7(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p7).c_str());
  printout(INFO,tag,"++ %s: P8(x,y,z) %s", opt.c_str(), _transformPoint2World(align_data, p8).c_str());

  printout(INFO,tag,"++ %s: P1(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p1).c_str());
  printout(INFO,tag,"++ %s: P2(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p2).c_str());
  printout(INFO,tag,"++ %s: P3(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p3).c_str());
  printout(INFO,tag,"++ %s: P4(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p4).c_str());
  printout(INFO,tag,"++ %s: P5(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p5).c_str());
  printout(INFO,tag,"++ %s: P6(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p6).c_str());
  printout(INFO,tag,"++ %s: P7(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p7).c_str());
  printout(INFO,tag,"++ %s: P8(x,y,z) %s", opt.c_str(), _transformPoint2Detector(align_data, p8).c_str());
}

/// PrintElement placement with/without alignment applied
void DD4hep::Alignments::printElementPlacement(const string& prefix, DetElement de, UserPool* pool)   {
  using Geometry::Box;
  using Geometry::Solid;
  using Geometry::Volume;
  using Geometry::PlacedVolume;
  string tag = prefix+"Element";
  if ( de.isValid() )  {
    if ( pool )  {
      char text[132];
      DetAlign     a(de);
      Container    container = a.alignments();
      Alignment    nominal = de.nominal();
      Box bbox = de.placement().volume().solid();
      ::memset(text,'=',sizeof(text));
      text[sizeof(text)-1] = 0;
      printout(INFO,tag,text);
      printout(INFO,tag,"++ Alignments of DE %s [%d entries]",
               de.path().c_str(), int(container.keys().size()));
      printout(INFO,tag,"++ Volume: %s  BBox: x=%7.3f y=%7.3f z=%7.3f",bbox.type(),bbox.x(),bbox.y(),bbox.z());
      printAlignmentEx(tag,"NOMINAL",de,nominal);

      for(const auto& k : container.keys() )  {
        try {
          Alignment align = container.get(k.first,*pool);
          const Alignment::Data& align_data = align.data();
          Conditions::Condition  align_cond = align_data.condition;
          if ( k.first != k.second.first )  {
            printout(INFO,tag,"++ Alignment %p [%08X] -> [%08X] %s (SYNONYM) ignored.",
                     a.ptr(), k.first, k.second.first, k.second.second.c_str());
            continue;
          }
          printout(INFO,tag,"++ Alignment %p [%08X] -> [%08X] %s",
                   a.ptr(), k.first, k.second.first, k.second.second.c_str());
          if ( k.second.second != align_cond.name() )  {
            printout(INFO,prefix,"++ \tPath:%s [%p]", align_cond.name(), a.ptr());
          }
          printAlignmentEx(tag,"ALIGNMENT",de,align);
        }
        catch(...)  {
          printout(ERROR,tag,"++ %s %s [%08X] -> [%08X]",
                   prefix.c_str(), "FAILED Alignment:", k.first, k.second.first);
        }
      }
      return;
    }
    except(tag,"Cannot process DetElement alignments from '%s' without valid user-pool",de.name());
  }
  except(tag,"Cannot process alignments of an invalid detector element");
}
