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
#include "XML/Conversions.h"
#include "XML/XMLParsers.h"
#include "XML/DocumentHandler.h"
#include "DD4hep/Path.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/OpaqueDataBinder.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/detail/ConditionsInterna.h"

#include "DDCond/ConditionsTags.h"
#include "DDCond/ConditionsManager.h"

// C/C++ include files
#include <stdexcept>
#include <climits>

///   dd4hep namespace declaration
namespace dd4hep  {

  /// Ananymous local stuff only used in this module
  namespace {
    /// List of XML tags used by this parser
    class iov;
    class iov_type;
    class manager;
    class repository;
    class detelement;
    class conditions;
    class arbitrary;

    /// Conditions types
    class value;
    class pressure;
    class temperature;
    class mapping;
    class sequence;
    class alignment;
  }
  /// Forward declarations for all specialized converters
  template <> void Converter<iov>::operator()(xml_h seq)  const;
  template <> void Converter<iov_type>::operator()(xml_h seq)  const;
  template <> void Converter<repository>::operator()(xml_h seq)  const;
  template <> void Converter<manager>::operator()(xml_h seq)  const;
  template <> void Converter<value>::operator()(xml_h e) const;
  template <> void Converter<pressure>::operator()(xml_h e) const;
  template <> void Converter<temperature>::operator()(xml_h e) const;
  template <> void Converter<sequence>::operator()(xml_h e) const;
  template <> void Converter<mapping>::operator()(xml_h e) const;
  template <> void Converter<alignment>::operator()(xml_h e) const;
  template <> void Converter<conditions>::operator()(xml_h seq)  const;
  template <> void Converter<arbitrary>::operator()(xml_h seq)  const;
}  
  
using std::string;
using namespace dd4hep;
using namespace dd4hep::cond;

/// Anonymous local stuff only used in this module
namespace {

  /// Module print level
  static PrintLevel s_parseLevel = DEBUG;

  /// Local helper class to interprete XML conditions
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/11/2016
   */
  struct ConversionArg {
    DetElement         detector;
    ConditionsPool*    pool;
    ConditionsManager  manager;
    ConversionArg() = delete;
    ConversionArg(const ConversionArg&) = delete;
    ConversionArg(DetElement det, ConditionsManager m) : detector(det), pool(0), manager(m)
    { }
    ConversionArg& operator=(const ConversionArg&) = delete;
  };

  /// Local helper class to interprete XML conditions
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/11/2016
   */
  struct CurrentDetector {
    DetElement detector;
    ConversionArg* arg;
    CurrentDetector(ConversionArg* a) : arg(a) {
      detector = arg->detector;
    }
    ~CurrentDetector()  {
      arg->detector = detector;
    }
    void set(const string& path)  {
      if ( !path.empty() ) {
        arg->detector = detail::tools::findDaughterElement(detector,path);
      }
    }
  };
  /// Local helper class to interprete XML conditions
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/11/2016
   */
  struct CurrentPool {
    ConditionsPool* pool;
    ConversionArg* arg;
    CurrentPool(ConversionArg* a) : arg(a) {
      pool = arg->pool;
    }
    ~CurrentPool()  {
      arg->pool = pool;
    }
    void set(ConditionsPool* p)  {
      arg->pool = p;
    }
  };

  /// Local helper function to interprete XML conditions
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/11/2016
   */
  Condition create_condition(DetElement det, xml_h e)  {
    xml_dim_t elt(e);
    string tag = elt.tag();
    string typ = elt.hasAttr(_U(type)) ? elt.typeStr() : tag;
    string nam = elt.hasAttr(_U(name)) ? elt.nameStr() : tag;
    string add = xml::DocumentHandler::system_path(e);
    string cond_nam = det.path()+"#"+nam;
    Condition cond(cond_nam, typ);
    cond->hash = ConditionKey::hashCode(det, nam);
    printout(s_parseLevel,"XMLConditions","++ Processing condition tag:%s name:%s type:%s [%s] hash:%016X det:%p",
             tag.c_str(), cond.name(), typ.c_str(),
             Path(add).filename().c_str(), cond.key(), det.ptr());
#if !defined(DD4HEP_MINIMAL_CONDITIONS)
    cond->address  = add;
    cond->value    = "";
    cond->validity = "";
    if ( elt.hasAttr(_U(comment)) )  {
      cond->comment = elt.attr<string>(_U(comment));
    }
#endif
    //ConditionsKeyAssign(det).addKey(cond.name());
    return cond;
  }

  /// Local helper function to interprete XML conditions
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/11/2016
   */
  template <typename BINDER> Condition bind_condition(const BINDER& bnd,
                                                      DetElement det,
                                                      xml_h e,
                                                      const std::string& type="")
  {
    xml_dim_t elt(e);
    string    typ  = type.empty() ? elt.typeStr() : type;
    string    val  = elt.hasAttr(_U(value)) ? elt.valueStr() : elt.text();
    Condition con  = create_condition(det, e);
    string    unit = elt.hasAttr(_U(unit))  ? elt.attr<string>(_U(unit)) : string("");
    if ( !unit.empty() ) val += "*"+unit;
    con->value = val;
    detail::OpaqueDataBinder::bind(bnd, con, typ, val);
    return con;
  }
}

///   dd4hep namespace declaration
namespace dd4hep {

  /// Convert iov_type repository objects
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<iov_type>::operator()(xml_h element) const {
    xml_dim_t e   = element;
    string    nam = e.nameStr();
    size_t    id  = e.id() >= 0 ? e.id() : INT_MAX;
    ConversionArg* arg  = _param<ConversionArg>();
    printout(s_parseLevel,"XMLConditions","++ Registering IOV type: [%d]: %s",int(id),nam.c_str());
    const IOVType* iov_type = arg->manager.registerIOVType(id,nam).second;
    if ( !iov_type )   {
      except("XMLConditions","Failed to register iov type: [%d]: %s",int(id),nam.c_str());
    }
  }

  /// Convert iov repository objects
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<iov>::operator()(xml_h element) const {
    xml_dim_t e   = element;
    string    val = e.attr<string>(_UC(validity));
    ConversionArg* arg  = _param<ConversionArg>();
    CurrentPool pool(arg);

    pool.set(arg->manager.registerIOV(val));
    if ( e.hasAttr(_U(ref)) )  {
      string    ref = e.attr<string>(_U(ref));
      printout(s_parseLevel,"XMLConditions","++ Reading IOV file: %s -> %s",val.c_str(),ref.c_str());
      xml::DocumentHolder doc(xml::DocumentHandler().load(element, element.attr_value(_U(ref))));
      Converter<conditions>(description,param,optional)(doc.root());
      return;
    }
    xml_coll_t(e,_UC(detelement)).for_each(Converter<arbitrary>(description,param,optional));
  }

  /// Convert manager repository objects
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<manager>::operator()(xml_h element) const {
    ConversionArg* arg  = _param<ConversionArg>();
    if ( element.hasAttr(_U(ref)) )  {
      xml::DocumentHolder doc(xml::DocumentHandler().load(element, element.attr_value(_U(ref))));
      Converter<arbitrary>(description,param,optional)(doc.root());
    }
    for( xml_coll_t c(element,_UC(property)); c; ++c)  {
      xml_dim_t d = c;
      string nam = d.nameStr();
      string val = d.valueStr();
      try  {
        printout(s_parseLevel,"XMLConditions","++ Setup conditions Manager[%s] = %s",
                 nam.c_str(),val.c_str());
        arg->manager[nam].str(val);
      }
      catch(const std::exception& e)  {
        printout(ERROR,"XMLConditions","++ FAILED: conditions Manager[%s] = %s [%s]",
                 nam.c_str(),val.c_str(),e.what());
      }
    }
    arg->manager.initialize();
    printout(s_parseLevel,"XMLConditions","++ Conditions Manager successfully initialized.");
  }

  /// Convert conditions value objects (scalars)
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<value>::operator()(xml_h e) const {
    ConversionArg* arg = _param<ConversionArg>();
    Condition      con = bind_condition(detail::ValueBinder(), arg->detector, e);
    arg->manager.registerUnlocked(*arg->pool, con);
  }

  /// Convert conditions pressure objects (scalars with unit)
  /**
   *   <pressure value="980" unit="hPa"/>
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<pressure>::operator()(xml_h e) const {
    ConversionArg* arg = _param<ConversionArg>();
    Condition      con = bind_condition(detail::ValueBinder(), arg->detector, e, "double");
    con->setFlag(Condition::PRESSURE);
    arg->manager.registerUnlocked(*arg->pool, con);
  }

  /// Convert conditions temperature objects (scalars with unit)
  /**
   *   <temperature value="273.1" unit="kelvin"/>
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<temperature>::operator()(xml_h e) const {
    ConversionArg* arg = _param<ConversionArg>();
    Condition      con = bind_condition(detail::ValueBinder(), arg->detector, e, "double");
    con->setFlag(Condition::TEMPERATURE);
    arg->manager.registerUnlocked(*arg->pool, con);
  }

  /// Convert conditions sequence objects (unmapped containers). See XML/XMLParsers.h for details.
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<sequence>::operator()(xml_h e) const {
    ConversionArg* arg = _param<ConversionArg>();
    Condition      con = create_condition(arg->detector, e);
    xml::parse_sequence(e, con->data);
    arg->manager.registerUnlocked(*arg->pool, con);
  }

  /// Convert conditions STL maps. See XML/XMLParsers.h for details.
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<mapping>::operator()(xml_h e) const {
    ConversionArg* arg = _param<ConversionArg>();
    Condition      con = create_condition(arg->detector, e);
    xml::parse_mapping(e, con->data);
    arg->manager.registerUnlocked(*arg->pool, con);
  }

  /// Convert alignment delta objects. See XML/XMLParsers.h for details.
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<alignment>::operator()(xml_h e) const {
    xml_h              child_rot, child_pos, child_piv;
    ConversionArg*     arg = _param<ConversionArg>();
    Condition          con = create_condition(arg->detector, e);
    //Delta& del = con.bind<Delta>();
    xml::parse_delta(e, con->data);
    con->setFlag(Condition::ALIGNMENT_DELTA);
    arg->manager.registerUnlocked(*arg->pool, con);
  }

  /// Convert detelement objects
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<detelement>::operator()(xml_h e) const {
    xml_comp_t elt(e);
    ConversionArg* arg = _param<ConversionArg>();
    CurrentDetector detector(arg);
    if ( elt.hasAttr(_U(path)) )  {
      detector.set(e.attr<string>(_U(path)));
      printout(s_parseLevel,"XMLConditions","++ Processing condition for:%s",
               arg->detector.path().c_str());
    }
    if ( elt.hasAttr(_U(ref)) )  {
      xml::DocumentHolder doc(xml::DocumentHandler().load(e, e.attr_value(_U(ref))));
      (*this)(doc.root());
    }
    xml_coll_t(e,_U(value)).for_each(Converter<value>(description,param,optional));
    xml_coll_t(e,_UC(mapping)).for_each(Converter<mapping>(description,param,optional));
    xml_coll_t(e,_UC(sequence)).for_each(Converter<sequence>(description,param,optional));
    xml_coll_t(e,_UC(pressure)).for_each(Converter<pressure>(description,param,optional));
    xml_coll_t(e,_UC(alignment_delta)).for_each(Converter<alignment>(description,param,optional));
    xml_coll_t(e,_UC(temperature)).for_each(Converter<temperature>(description,param,optional));
    xml_coll_t(e,_UC(detelement)).for_each(Converter<detelement>(description,param,optional));
  }

  /// Convert repository objects
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<repository>::operator()(xml_h element) const {
    xml_coll_t(element,_UC(manager)).for_each(Converter<manager>(description,param,optional));
    xml_coll_t(element,_UC(iov_type)).for_each(Converter<iov_type>(description,param,optional));
    xml_coll_t(element,_UC(iov)).for_each(Converter<iov>(description,param,optional));
  }

  /// Convert any top level tag in the XML file
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<arbitrary>::operator()(xml_h e) const {
    xml_comp_t elt(e);
    string tag = elt.tag();
    if ( tag == "repository" )  
      Converter<repository>(description,param,optional)(e);
    else if ( tag == "manager" )  
      Converter<manager>(description,param,optional)(e);
    else if ( tag == "conditions" )  
      Converter<conditions>(description,param,optional)(e);
    else if ( tag == "detelement" )
      Converter<detelement>(description,param,optional)(e);
    else if ( tag == "iov_type" )
      Converter<iov_type>(description,param,optional)(e);
    else if ( tag == "iov" )         // Processing repository file
      Converter<iov>(description,param,optional)(e);
    else
      except("XMLConditions",
             "++ Failed to handle unknown tag: %s",tag.c_str());
  }

  /// Convert alignment conditions entries
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<conditions>::operator()(xml_h e) const {
    xml_coll_t(e,_U(star)).for_each(Converter<arbitrary>(description,param,optional));
  }
}

/// Basic entry point to set print level of this module.
/**
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long setup_repository_loglevel(Detector& /* description */, int argc, char** argv)  {
  if ( argc == 1 )  {
    s_parseLevel = printLevel(argv[0]);
    return 1;
  }
  except("ConditionsXMLRepositoryPrintLevel","++ Invalid plugin arguments: %s",
         arguments(argc,argv).c_str());
  return 0;
}
DECLARE_APPLY(DD4hep_ConditionsXMLRepositoryPrintLevel,setup_repository_loglevel)

#include "DD4hep/DD4hepUI.h"

/// Basic entry point to read alignment conditions files
/**
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long setup_repository_Conditions(Detector& description, int argc, char** argv)  {
  if ( argc == 1 )  {
    detail::DD4hepUI ui(description);
    string fname(argv[0]);
    ConditionsManager   mgr(ui.conditionsMgr());
    ConversionArg       arg(description.world(), mgr);
    xml::DocumentHolder doc(xml::DocumentHandler().load(fname));
    (dd4hep::Converter<conditions>(description,&arg))(doc.root());
    return 1;
  }
  except("XML_DOC_READER","Invalid number of arguments to interprete conditions: %d != %d.",argc,1);
  return 0;
}
DECLARE_APPLY(DD4hep_ConditionsXMLRepositoryParser,setup_repository_Conditions)
