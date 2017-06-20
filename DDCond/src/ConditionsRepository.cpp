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
#include "DDCond/ConditionsRepository.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsTags.h"
#include "DD4hep/detail/ConditionsInterna.h"
#include "DD4hep/Printout.h"
#include "XML/DocumentHandler.h"
#include "XML/XMLTags.h"

// C/C++ include files
#include <cstring>
#include <fstream>
#include <climits>
#include <cerrno>
#include <map>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cond;
typedef xml::Handle_t xml_h;
typedef xml::Element xml_elt_t;
typedef xml::Document xml_doc_t;
typedef xml::Collection_t xml_coll_t;

typedef map<Condition::key_type,Condition> AllConditions;

/// Default constructor
ConditionsRepository::ConditionsRepository()  {
}

/// Default destructor
ConditionsRepository::~ConditionsRepository()   {
}

namespace {

  int createXML(const string& output, const AllConditions& all) {
    const char comment[] = "\n"
      "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
      "      ++++   Linear collider detector description Detector in C++  ++++\n"
      "      ++++   dd4hep Detector description generator.            ++++\n"
      "      ++++                                                     ++++\n"
      "      ++++                                                     ++++\n"
      "      ++++                              M.Frank CERN/LHCb      ++++\n"
      "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n  ";
    dd4hep::xml::DocumentHandler docH;
    xml_doc_t doc = docH.create("collection", comment);
    xml_elt_t root = doc.root(), cond(0);
    for( const auto& i : all )  {
      char text[32];
      Condition c = i.second;
      ::snprintf(text,sizeof(text),"0x%16llX",c.key());
      root.append(cond = xml_elt_t(doc, _U(ref)));
      cond.setAttr(_U(key), text);
      cond.setAttr(_U(name), c.name());
      cond.setAttr(_U(ref), c.address());
    }
    printout(ALWAYS,"ConditionsRepository","++ Handled %ld conditions.",all.size());
    if ( !output.empty() )  {
      return docH.output(doc, output);
    }
    return 1;
  }

  /// Load the repository from file and fill user passed data structory
  int readXML(const string& input, ConditionsRepository::Data& data)    {
    struct Conv {
      /// Reference to optional user defined parameter
      ConditionsRepository::Data& data;
      /// Initializing constructor of the functor
      Conv(ConditionsRepository::Data& p) : data(p) {}
      /// Callback operator to be specialized depending on the element type
      void operator()(xml_h element) const   {
        string key = element.attr<string>(_U(key));
        size_t cap = data.capacity();
        ConditionsRepository::Entry e;
        ::sscanf(key.c_str(),"0x%16llX",&e.key);
        e.name = element.attr<string>(_U(name));
        e.address = element.attr<string>(_U(ref));
        if ( data.size() == cap ) data.reserve(cap+500);
        data.push_back(e);
      }
    };
    xml::DocumentHolder doc(xml::DocumentHandler().load(input));
    xml_h root = doc.root();
    xml_coll_t(root, _U(ref)).for_each(Conv(data));
    return 1;
  }
  
  int createText(const string& output, const AllConditions& all, char sep)   {
    size_t siz_nam=0, siz_add=0, siz_tot=0;
    char fmt[64], text[2*PATH_MAX+64];
    ofstream out(output);
    if ( !out.good() )  {
      except("ConditionsRepository",
             "++ Failed to open output file:%s [errno:%d %s]",
             output.c_str(), errno, ::strerror(errno));
    }
    else if ( sep )  {
      ::snprintf(fmt,sizeof(fmt),"%%16llX%c%%s%c%%s%c",sep,sep,sep);
    }
    else   {
      for( const auto& i : all )  {
        Condition::Object* c = i.second.ptr();
        size_t siz_n = c->name.length();
        size_t siz_a = c->address.length();
        if ( siz_nam < siz_n ) siz_nam = siz_n;
        if ( siz_add < siz_a ) siz_add = siz_a;
        if ( siz_tot < (siz_n+siz_a) ) siz_tot = siz_n+siz_a;
      }
      siz_tot += 8+2+1;
      ::snprintf(fmt,sizeof(fmt),"%%16llX %%-%lds %%-%lds",long(siz_nam),long(siz_add));
    }
    out << "dd4hep." << char(sep ? sep : '-')
        << "." << long(siz_nam)
        << "." << long(siz_add)
        << "." << long(siz_tot) << endl;
    for( const auto& i : all )  {
      Condition c = i.second;
      ::snprintf(text, sizeof(text), fmt, c.key(), c.name(), c.address().c_str());
      out << text << endl;
    }
    out.close();
    return 1;
  }

  /// Load the repository from file and fill user passed data structory
  int readText(const string& input, ConditionsRepository::Data& data)    {
    size_t idx;
    ConditionsRepository::Entry e;
    long siz_nam, siz_add, siz_tot;
    char sep, c, text[2*PATH_MAX+64];
    ifstream in(input);
    in >> c >> c >> c >> c >> c >> c >> c >> sep 
       >> c >> siz_nam
       >> c >> siz_add
       >> c >> siz_tot;
    text[0] = 0;
    in.getline(text,sizeof(text),'\n');
    do {
      text[0] = 0;
      in.getline(text,sizeof(text),'\n');
      if ( in.good() )  {
        if ( siz_tot )  {
          // Direct access mode with fixed record size
          text[8] = text[9+siz_nam] = text[10+siz_nam+siz_add] = 0;
          e.name = text+9;
          e.address = text+10+siz_nam;  
          if ( (idx=e.name.find(' ')) != string::npos )
            e.name[idx] = 0;
          if ( (idx=e.address.find(' ')) != string::npos )
            e.address[idx] = 0;
        }
        else  {
          // Variable record size
          e.name=text+9;
          if ( (idx=e.name.find(sep)) != string::npos )
            text[9+idx] = 0, e.address=text+idx+10, e.name=text+9;
          if ( (idx=e.address.find(sep)) != string::npos )
            e.address[idx] = 0;
          else if ( (idx=e.address.find('\n')) != string::npos )
            e.address[idx] = 0;
        }
        size_t cap = data.capacity();
        ::sscanf(text,"%16llX",&e.key);
        if ( data.size() == cap ) data.reserve(cap+500);
        data.push_back(e);
      }
    } while(in.good() && !in.eof() );
    in.close();
    return 1;
  }
}

/// Save the repository to file
int ConditionsRepository::save(ConditionsManager manager, const string& output)  const  {
  AllConditions all;
  const auto types = manager.iovTypesUsed();
  for( const IOVType* type : types )  {
    if ( type )   {
      ConditionsIOVPool* pool = manager.iovPool(*type);
      if ( pool )  {
        for( const auto& cp : pool->elements )   {
          RangeConditions rc;
          cp.second->select_all(rc);
          for( const auto cond : rc )
            all[cond.key()] = cond;
        }
      }
    }
  }

  if ( output.find(".xml") != string::npos )   {
    /// Write XML file with conditions addresses
    return createXML(output, all);
  }
  else if ( output.find(".txt") != string::npos )   {
    /// Write fixed records with conditions addresses
    return createText(output, all, 0);
  }
  else if ( output.find(".daf") != string::npos )   {
    /// Write fixed records with conditions addresses
    return createText(output, all, 0);
  }
  else if ( output.find(".csv") != string::npos )   {
    /// Write records separated by ';' with conditions addresses
    return createText(output, all, ';');
  }
  return 0;
}

/// Load the repository from file and fill user passed data structory
int ConditionsRepository::load(const string& input, Data& data)  const  {
  if ( input.find(".xml") != string::npos )   {
    return readXML(input, data);
  }
  else if ( input.find(".txt") != string::npos )   {
    return readText(input, data);
  }
  else if ( input.find(".daf") != string::npos )   {
    return readText(input, data);
  }
  else if ( input.find(".csv") != string::npos )   {
    return readText(input, data);
  }
  return 0;
}
