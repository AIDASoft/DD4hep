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
#ifndef DD4HEP_DDCOND_CONDITIONSREPOSITORYWRITER_H
#define DD4HEP_DDCOND_CONDITIONSREPOSITORYWRITER_H

// Framework include files
#include <DD4hep/Detector.h>
#include <XML/XMLElements.h>
#include <DDCond/ConditionsManager.h>

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    // Forward declarations
    class ConditionsSlice;
    
    /// Conditions slice object. Defines which conditions should be loaded by the ConditionsManager.
    /**
     *  Object contains set of required conditions keys to be loaded to the user pool.
     *  It alkso contains the load information for the required conditions (conditions addresses).
     *  The address objects depend on the actual loader mechanism and must be specified the user.
     *  The information is then chained through the calls and made available to the loader object.
     *
     *  On return it contains the individual condition load information.
     *
     *  Referenced by: ConditionsUserPool, ConditionsManager
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsXMLRepositoryWriter  {
      /// Debug counters: converted conditions
      long m_numConverted = 0;
      /// Debug counters: unconverted conditions
      long m_numUnconverted = 0;
    public:
      /// Default constructor
      ConditionsXMLRepositoryWriter() = default;
      /// Copy constructor (Special, partial copy only. Hence no assignment!)
      ConditionsXMLRepositoryWriter(const ConditionsXMLRepositoryWriter& copy) = default;
      /// Default destructor. 
      virtual ~ConditionsXMLRepositoryWriter();
     
      /// Dump the tree content into a XML document structure
      xml::Document dump(ConditionsSlice& slice);
      /// Dump the ConditionsManager configuration properties into a XML document structure
      xml::Document dump(ConditionsManager manager);
      /// Dump the tree content into an existing XML document structure
      size_t collect(xml::Element root,ConditionsSlice& slice,DetElement detector);
      /// Dump the ConditionsManager configuration properties into an existing XML document structure
      size_t collect(xml::Element root, ConditionsManager manager);
      /// Dump the conditions tree content into a XML document structure
      size_t collect(xml::Element root, ConditionsSlice& slice);
      /// Write the XML document structure to a file.
      long write(xml::Document doc, const std::string& output)  const;
    };
    
  }        /* End namespace cond                          */
}          /* End namespace dd4hep                              */
#endif     /* DD4HEP_DDCOND_CONDITIONSREPOSITORYWRITER_H        */


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
#include <XML/XMLElements.h>
#include <XML/DocumentHandler.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Alignments.h>
#include <DD4hep/AlignmentData.h>
#include <DD4hep/OpaqueDataBinder.h>
#include <DD4hep/ConditionsProcessor.h>
#include <DD4hep/DetFactoryHelper.h>
#include <DD4hep/detail/ConditionsInterna.h>
#include <DD4hep/detail/AlignmentsInterna.h>

#include <DDCond/ConditionsTags.h>
#include <DDCond/ConditionsSlice.h>
#include <DDCond/ConditionsManagerObject.h>

// C/C++ include files
#include <stdexcept>
#include <memory>
#include <sstream>
#include <list>
#include <set>

using namespace dd4hep::cond;
namespace units = dd4hep;
namespace xml = dd4hep::xml;
using dd4hep::printout;
using dd4hep::_toString;

/// Anonymous local stuff only used in this module
namespace {

  /// Module print level
  static dd4hep::PrintLevel s_printLevel = dd4hep::INFO;

  class pressure;
  class temperature;
  class value;
  class sequence;

  class PropertyDumper {
    xml::Element root;
  public:
    PropertyDumper(xml::Element p) : root(p)  {}
    void operator()(const std::pair<std::string, dd4hep::Property>& p)  const  {
      xml::Element e = xml::Element(root.document(),_UC(property));
      std::string val = p.second.str();
      if ( val[0] == '\'' ) val = p.second.str().c_str()+1;
      if ( !val.empty() && val[val.length()-1] == '\'' ) val[val.length()-1] = 0;
      e.setAttr(_U(name), p.first);
      e.setAttr(_U(value), val);
      root.append(e);
    }
  };
  
  template <typename T> xml::Element _convert(xml::Element par, dd4hep::Condition c);
  
  xml::Element make(xml::Element e, dd4hep::Condition c)  {
    char hash[64];
    std::string nam = c.name();
    std::string cn = nam.substr(nam.find('#')+1);
    ::snprintf(hash,sizeof(hash),"%llX",c.key());
    e.setAttr(_U(name),cn);
    e.setAttr(_U(key),hash);
    return e;
  }
  xml::Element _convert(xml::Element par, const dd4hep::Translation3D& tr)  {
    xml::Element e = xml::Element(par.document(),_U(pivot));
    const dd4hep::Translation3D::Vector& v = tr.Vect();
    e.setAttr(_U(x),_toString(v.X()/dd4hep::mm,"%f*mm"));
    e.setAttr(_U(y),_toString(v.Y()/dd4hep::mm,"%f*mm"));
    e.setAttr(_U(z),_toString(v.Z()/dd4hep::mm,"%f*mm"));
    return e;
  }
  xml::Element _convert(xml::Element par, const dd4hep::Position& pos)  {
    xml::Element e = xml::Element(par.document(),_U(position));
    e.setAttr(_U(x),_toString(pos.X()/dd4hep::mm,"%f*mm"));
    e.setAttr(_U(y),_toString(pos.Y()/dd4hep::mm,"%f*mm"));
    e.setAttr(_U(z),_toString(pos.Z()/dd4hep::mm,"%f*mm"));
    return e;
  }
  xml::Element _convert(xml::Element par, const dd4hep::RotationZYX& rot)  {
    xml::Element e = xml::Element(par.document(),_U(rotation));
    double z, y, x;
    rot.GetComponents(z,y,x);
    e.setAttr(_U(x),_toString(x/dd4hep::rad,"%f*rad"));
    e.setAttr(_U(y),_toString(y/dd4hep::rad,"%f*rad"));
    e.setAttr(_U(z),_toString(z/dd4hep::rad,"%f*rad"));
    return e;
  }
  template <> xml::Element _convert<value>(xml::Element par, dd4hep::Condition c)  {
    xml::Element v = make(xml::Element(par.document(),_U(value)),c);
    dd4hep::OpaqueData& data = c.data();
    v.setAttr(_U(type),data.dataType());
    v.setAttr(_U(value),data.str());
    return v;
  }
  template <> xml::Element _convert<pressure>(xml::Element par, dd4hep::Condition c)  {
    xml::Element press = make(xml::Element(par.document(),_UC(pressure)),c);
    press.setAttr(_U(value),c.get<double>()/(100e0*units::pascal));
    press.setAttr(_U(unit),"hPa");
    return press;
  }
  template <> xml::Element _convert<temperature>(xml::Element par, dd4hep::Condition c)  {
    xml::Element temp = make(xml::Element(par.document(),_UC(temperature)),c);
    temp.setAttr(_U(value),c.get<double>()/units::kelvin);
    temp.setAttr(_U(unit),"kelvin");
    return temp;
  }
  template <> xml::Element _convert<dd4hep::Delta>(xml::Element par, dd4hep::Condition c)  {
    xml::Element         align = make(xml::Element(par.document(),_UC(alignment_delta)),c);
    const dd4hep::Delta& delta = c.get<dd4hep::Delta>();
    if ( delta.flags&dd4hep::Delta::HAVE_TRANSLATION )
      align.append(_convert(align,delta.translation));
    if ( delta.flags&dd4hep::Delta::HAVE_ROTATION )
      align.append(_convert(align,delta.rotation));
    if ( delta.flags&dd4hep::Delta::HAVE_PIVOT )
      align.append(_convert(align,delta.pivot));
    return align;
  }
  template <> xml::Element _convert<dd4hep::Alignment>(xml::Element par, dd4hep::Condition c)  {
    char hash[64];
    typedef dd4hep::ConditionKey::KeyMaker KM;
    dd4hep::AlignmentCondition   acond = c;
    KM                           km(c.key());
    const dd4hep::Delta&         delta = acond.data().delta;
    xml::Element                 align(xml::Element(par.document(),_UC(alignment_delta)));
    dd4hep::Condition::key_type  key = KM(km.values.det_key,dd4hep::align::Keys::deltaKey).hash;
    ::snprintf(hash,sizeof(hash),"%llX",key);
    align.setAttr(_U(name), dd4hep::align::Keys::deltaName);
    align.setAttr(_U(key),hash);
    if ( delta.flags&dd4hep::Delta::HAVE_TRANSLATION )
      align.append(_convert(align,delta.translation));
    if ( delta.flags&dd4hep::Delta::HAVE_ROTATION )
      align.append(_convert(align,delta.rotation));
    if ( delta.flags&dd4hep::Delta::HAVE_PIVOT )
      align.append(_convert(align,delta.pivot));
    return align;
  }
  xml::Element _seq(xml::Element v, dd4hep::Condition c, const char* tag, const char* match)  {
    dd4hep::OpaqueData& data = c.data();
    std::string typ = data.dataType();
    size_t len = ::strlen(match);
    size_t idx = typ.find(match);
    size_t idq = typ.find(',',idx+len);
    if ( idx != std::string::npos && idq != std::string::npos )   {
      std::string subtyp = tag;
      subtyp += "["+typ.substr(idx+len,idq-idx-len)+"]";
      v.setAttr(_U(type),subtyp);
      xml::Handle_t(v.ptr()).setText(data.str());
      return v;
    }
    dd4hep::except("Writer","++ Unknwon XML conversion to type: %s",typ.c_str());
    return v;
  }
  template <> xml::Element _convert<std::vector<void*> >(xml::Element par, dd4hep::Condition c)  {
    xml::Element v = make(xml::Element(par.document(),_UC(sequence)),c);
    return _seq(v,c,"vector","::vector<");
  }
  template <> xml::Element _convert<std::list<void*> >(xml::Element par, dd4hep::Condition c)  {
    xml::Element v = make(xml::Element(par.document(),_UC(sequence)),c);
    return _seq(v,c,"list","::list<");
  }
  template <> xml::Element _convert<std::set<void*> >(xml::Element par, dd4hep::Condition c)  {
    xml::Element v = make(xml::Element(par.document(),_UC(sequence)),c);
    return _seq(v,c,"set","::set<");
  }
}

/// Default destructor. 
ConditionsXMLRepositoryWriter::~ConditionsXMLRepositoryWriter()    {
  printout(INFO,"ConditionsXMLWriter","++ Summary: Converted %ld conditions. %ld conditions without recipe.",
           m_numConverted, m_numUnconverted);
}

/// Dump the tree content into a XML document structure
xml::Document ConditionsXMLRepositoryWriter::dump(ConditionsSlice& slice) {
  xml::DocumentHandler docH;
  xml::Document doc = docH.create("conditions", docH.defaultComment());
  collect(doc.root(),slice);
  return doc;
}

/// Dump the ConditionsManager configuration properties into a XML document structure
xml::Document ConditionsXMLRepositoryWriter::dump(ConditionsManager manager)  {
  xml::DocumentHandler docH;
  xml::Document doc  = docH.create("conditions", docH.defaultComment());
  xml::Element  root = doc.root();
  collect(root,manager);
  return doc;
}

/// Dump the conditions tree content into a XML document structure
size_t ConditionsXMLRepositoryWriter::collect(xml::Element root, ConditionsSlice& slice)    {
  xml::Element  repo(root.document(),_UC(repository));
  xml::Element  iov (repo.document(),_UC(iov));
  const IOV&    validity = slice.pool->validity();
  char text[128];

  root.append(repo);
  repo.append(iov);
  std::snprintf(text,sizeof(text),"%ld,%ld#%s",
                long(validity.keyData.first), long(validity.keyData.second),
                validity.iovType->name.c_str());
  iov.setAttr(_UC(validity),text);
  return collect(iov,slice,slice.manager->detectorDescription().world());
}

/// Dump the ConditionsManager configuration properties into an existing XML document structure
size_t ConditionsXMLRepositoryWriter::collect(xml::Element root,
                                              ConditionsManager manager)  
{
  size_t count = 0;
  if ( manager.isValid() )  {
    /// Access to the property manager
    PropertyManager& prp = manager.properties();
    xml::Element     rep(root.document(),_UC(repository));
    xml::Element     mgr(rep.document(),_UC(manager));
    const auto iovs = manager.iovTypesUsed();

    prp.for_each(PropertyDumper(mgr));
    rep.append(mgr);
    count += prp.size();
    for ( const auto t : iovs )  {
      xml::Element iov_typ(rep.document(),_UC(iov_type));
      iov_typ.setAttr(_U(name),t->name);
      iov_typ.setAttr(_U(id),int(t->type));
      rep.append(iov_typ);
    }
    root.append(rep);
  }
  return count;
}

/// Dump the conditions tree content into a XML document structure
size_t ConditionsXMLRepositoryWriter::collect(xml::Element root,
                                              ConditionsSlice& slice,
                                              DetElement detector)  
{
  size_t count = 0;
  if ( detector.isValid() )  {
    std::vector<Condition> conds;
    conditionsCollector(slice,conds)(detector);
    if ( !conds.empty() )   {
      std::stringstream comment;
      Condition cond_align, cond_delta;
      xml::Element conditions = xml::Element(root.document(),_UC(detelement));
      conditions.setAttr(_U(path),detector.path());
      printout(s_printLevel,"Writer","++ Conditions of DE %s [%d entries]",
               detector.path().c_str(), int(conds.size()));
      comment << " DDCond conditions for DetElement " << detector.path()
              << " Total of " << int(conds.size()) << " Entries.  ";
      root.addComment(comment.str());
      root.append(conditions);
      for(const auto& c : conds )   {
        std::string nam = c.name();
        std::string cn  = nam.substr(nam.find('#')+1);
        detail::ReferenceBitMask<Condition::mask_type> msk(c->flags);

        printout(s_printLevel,"Writer","++ Condition %s [%16llX] -> %s",
                 cn.c_str(), c.key(), c.name());
        if ( msk.isSet(Condition::TEMPERATURE) )  {
          conditions.append(_convert<temperature>(conditions,c));
          ++m_numConverted;
        }
        else if ( msk.isSet(Condition::PRESSURE) )  {
          conditions.append(_convert<pressure>(conditions,c));
          ++m_numConverted;
        }
        else if ( msk.isSet(Condition::ALIGNMENT_DERIVED) )  {
          cond_align = c;
        }
        else if ( msk.isSet(Condition::ALIGNMENT_DELTA) )  {
          cond_delta = c;
        }
        else {
          const dd4hep::OpaqueData& data = c.data();
          const std::type_info& typ = data.typeInfo();
#if defined(DD4HEP_HAVE_ALL_PARSERS)
          if ( typ == typeid(char)   || typ == typeid(unsigned char)  ||
               typ == typeid(short)  || typ == typeid(unsigned short) ||
               typ == typeid(int)    || typ == typeid(unsigned int)   ||
               typ == typeid(long)   || typ == typeid(unsigned long)  ||
               typ == typeid(float)  || typ == typeid(double)         ||
               typ == typeid(bool)   || typ == typeid(std::string) )
#else
            if ( typ == typeid(int)    || typ == typeid(long)           ||
                 typ == typeid(float)  || typ == typeid(double)         ||
                 typ == typeid(bool)   || typ == typeid(std::string) )
#endif
            {
              conditions.append(_convert<value>(conditions,c));
              ++m_numConverted;
            }
            else if ( ::strstr(data.dataType().c_str(),"::vector<") )  {
              conditions.append(_convert<std::vector<void*> >(conditions,c));
              ++m_numConverted;
            }
            else if ( ::strstr(data.dataType().c_str(),"::list<") )  {
              conditions.append(_convert<std::list<void*> >(conditions,c));
              ++m_numConverted;
            }
            else if ( ::strstr(data.dataType().c_str(),"::set<") )  {
              conditions.append(_convert<std::set<void*> >(conditions,c));
              ++m_numConverted;
            }
            else   {
              comment.str("");
              comment << "\n ** Unconverted condition: "
                      << "Unknown data type of condition: " << cn
                      << " [" << (void*)c.key() << "] -> "
                      << c.name() << "  Flags:" << (unsigned int)c->flags << "\n";
              conditions.addComment(comment.str());
              printout(ERROR,"Writer",comment.str());
              comment.str("");
              comment << c.data().str() << " [" << c.data().dataType() << "]\n";
              conditions.addComment(comment.str());
              ++m_numUnconverted;
            }
        }
      }
      if ( cond_align.isValid() )  {
        conditions.append(_convert<Alignment>(conditions,cond_align));
        ++m_numConverted;
      }
      else if ( cond_delta.isValid() )   {
        conditions.append(_convert<dd4hep::Delta>(conditions,cond_delta));
        ++m_numConverted;
      }
    }
    for (const auto& i : detector.children())
      count += collect(root,slice,i.second);
  }
  return count;
}
// ======================================================================================

/// Write the XML document structure to a file.
long ConditionsXMLRepositoryWriter::write(xml::Document doc, const std::string& output)   const {
  xml_handler_t docH;
  long ret = docH.output(doc, output);
  if ( !output.empty() )  {
    printout(INFO,"Writer","++ Successfully wrote %ld conditions (%ld unconverted) to file: %s",
             m_numConverted,m_numUnconverted,output.c_str());
  }
  return ret;
}

/// Basic entry point to read alignment conditions files
/**
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long write_repository_conditions(dd4hep::Detector& description, int argc, char** argv)  {
  ConditionsManager manager  =  ConditionsManager::from(description);
  const dd4hep::IOVType* iovtype  =  0;
  long                   iovvalue = -1;
  long                   mgr_prop = 0;
  std::string            output;

  for(int i=0; i<argc; ++i)  {
    if ( ::strncmp(argv[i],"-iov_type",7) == 0 )
      iovtype = manager.iovType(argv[++i]);
    else if ( ::strncmp(argv[i],"-iov_value",7) == 0 )
      iovvalue = ::atol(argv[++i]);
    else if ( ::strncmp(argv[i],"-output",4) == 0 && argc>i+1)
      output = argv[++i];
    else if ( ::strncmp(argv[i],"-manager",4) == 0 )
      mgr_prop = 1;
    else if ( ::strncmp(argv[i],"-help",2) == 0 )  {
      printout(dd4hep::ALWAYS,"Plugin-Help","Usage: dd4hep_XMLConditionsRepositoryWriter --opt [--opt]           ");
      printout(dd4hep::ALWAYS,"Plugin-Help","  -output    <string>   Output file name. Default: stdout           ");
      printout(dd4hep::ALWAYS,"Plugin-Help","  -manager   <string>   Add manager properties to the output.       ");
      printout(dd4hep::ALWAYS,"Plugin-Help","  -iov_type  <string>   IOV type to be selected.                    ");
      printout(dd4hep::ALWAYS,"Plugin-Help","  -iov_value <string>   IOV value to create the conditions snapshot.");
      ::exit(EINVAL);
    }
  }
  if ( 0 == iovtype )
    dd4hep::except("ConditionsPrepare","++ Unknown IOV type supplied.");
  if ( 0 > iovvalue )
    dd4hep::except("ConditionsPrepare",
                   "++ Unknown IOV value supplied for iov type %s.",iovtype->str().c_str());

  dd4hep::IOV iov(iovtype,iovvalue);
  std::shared_ptr<ConditionsContent> content(new ConditionsContent());
  std::shared_ptr<ConditionsSlice>   slice(new ConditionsSlice(manager,content));
  dd4hep::cond::fill_content(manager,*content,*iovtype);
  ConditionsManager::Result cres = manager.prepare(iov, *slice);
  printout(dd4hep::INFO, "Conditions",
           "++ Selected conditions: %7ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) for IOV:%-12s",
           cres.total(), cres.selected, cres.loaded, cres.computed, cres.missing,
           iovtype ? iov.str().c_str() : "???");
  
  ConditionsXMLRepositoryWriter writer;  
  xml::Document doc(0);
  if ( mgr_prop )  {
    doc = writer.dump(manager);
    writer.collect(doc.root(), *slice);
  }
  else  {
    doc = writer.dump(*slice);
  }
  writer.write(doc, output);
  return 1;
}
DECLARE_APPLY(DD4hep_ConditionsXMLRepositoryWriter,write_repository_conditions)


// ======================================================================================
/// Basic entry point to read alignment conditions files
/**
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long write_repository_manager(dd4hep::Detector& description, int argc, char** argv)  {
  ConditionsManager manager  =  ConditionsManager::from(description);
  std::string       output;

  for(int i=0; i<argc; ++i)  {
    if ( ::strncmp(argv[i],"-output",4) == 0 && argc>i+1)
      output = argv[++i];
    else if ( ::strncmp(argv[i],"-help",2) == 0 )  {
      printout(dd4hep::ALWAYS,"Plugin-Help","Usage: dd4hep_XMLConditionsManagerWriter --opt [--opt]           ");
      printout(dd4hep::ALWAYS,"Plugin-Help","  -output    <string>   Output file name. Default: stdout        ");
      ::exit(EINVAL);
    }
  }
  ConditionsXMLRepositoryWriter writer;
  xml::Document doc = writer.dump(manager);
  writer.write(doc, output);
  return 1;
}
DECLARE_APPLY(DD4hep_ConditionsXMLManagerWriter,write_repository_manager)
// ======================================================================================
