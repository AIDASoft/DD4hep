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

// Framework include files
#include "DD4hep/LCDD.h"
#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"
#include "DD4hep/Path.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/OpaqueDataBinder.h"
#include "DD4hep/ConditionsKeyAssign.h"
#include "DD4hep/DetFactoryHelper.h"

#include "DDCond/ConditionsTags.h"
#include "DDCond/ConditionsManager.h"

// C/C++ include files
#include <stdexcept>

///   DD4hep namespace declaration
namespace DD4hep  {

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
    class mapping;
    class sequence;
    class alignment;
    class position;
    class rotation;
    class pivot;
  }
  /// Forward declarations for all specialized converters
  template <> void Converter<iov>::operator()(xml_h seq)  const;
  template <> void Converter<iov_type>::operator()(xml_h seq)  const;
  template <> void Converter<repository>::operator()(xml_h seq)  const;
  template <> void Converter<manager>::operator()(xml_h seq)  const;
  template <> void Converter<rotation>::operator()(xml_h e) const;
  template <> void Converter<position>::operator()(xml_h e) const;
  template <> void Converter<pivot>::operator()(xml_h e) const;
  template <> void Converter<value>::operator()(xml_h e) const;
  template <> void Converter<sequence>::operator()(xml_h e) const;
  template <> void Converter<mapping>::operator()(xml_h e) const;
  template <> void Converter<alignment>::operator()(xml_h e) const;
  template <> void Converter<conditions>::operator()(xml_h seq)  const;
  template <> void Converter<arbitrary>::operator()(xml_h seq)  const;
}  
  
using std::string;
using namespace DD4hep;
using namespace DD4hep::Conditions;
using Geometry::RotationZYX;
using Geometry::Transform3D;
using Geometry::Translation3D;
using Geometry::Position;
using Geometry::DetElement;

/// Ananymous local stuff only used in this module
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
        arg->detector = Geometry::DetectorTools::findDaughterElement(detector,path);
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
    string add = XML::DocumentHandler::system_path(e);
    Condition cond(det.path()+"#"+nam, typ);

    printout(s_parseLevel,"XMLConditions","++ Processing condition tag:%s name:%s type:%s [%s]",
             tag.c_str(), nam.c_str(), typ.c_str(),
             Path(add).filename().c_str());
    cond->address  = add;
    cond->value    = "";
    cond->validity = "";
    cond->hash     = Conditions::ConditionKey::hashCode(cond->name);
    cond->setFlag(Condition::ACTIVE);
    if ( elt.hasAttr(_U(comment)) )  {
      cond->comment = elt.attr<string>(_U(comment));
    }
    ConditionsKeyAssign(det).addKey(cond.name());//.addKey(nam,cond.name());
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
    string    typ = type.empty() ? elt.typeStr() : type;
    string    val = elt.hasAttr(_U(value)) ? elt.valueStr() : elt.text();
    Condition con = create_condition(det, e);
    con->value = val;
    OpaqueDataBinder::bind(bnd, con, typ, val);
    return con;
  }
}

///   DD4hep namespace declaration
namespace DD4hep {

  /// Convert iov_type repository objects
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<iov_type>::operator()(xml_h element) const {
    xml_dim_t e  = element;
    size_t id    = e.id();
    string nam   = e.nameStr();
    ConversionArg* arg  = _param<ConversionArg>();
    printout(s_parseLevel,"XMLConditions","++ Registering IOV type: [%d]: %s",id,nam.c_str());
    const IOVType* iov_type = arg->manager.registerIOVType(id,nam).second;
    if ( !iov_type )  {
      except("XMLConditions","Failed to register iov type: [%d]: %s",id,nam.c_str());
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
    string    ref = e.attr<string>(_U(ref));
    string    val = e.attr<string>(_U(validity));
    ConversionArg* arg  = _param<ConversionArg>();
    CurrentPool pool(arg);
    printout(s_parseLevel,"XMLConditions","++ Reading IOV file: %s -> %s", val.c_str(), ref.c_str());
    pool.set(arg->manager.registerIOV(val));
    XML::DocumentHolder doc(XML::DocumentHandler().load(element, element.attr_value(_U(ref))));
    Converter<conditions>(lcdd,param,optional)(doc.root());
  }

  /// Convert manager repository objects
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<manager>::operator()(xml_h element) const {
    ConversionArg* arg  = _param<ConversionArg>();
    for( xml_coll_t c(element,_Unicode(property)); c; ++c)  {
      xml_dim_t d = c;
      string nam = d.nameStr();
      string val = d.valueStr();
      try  {
        printout(s_parseLevel,"XMLConditions","++ Setup conditions Manager[%s] = %s",nam.c_str(),val.c_str());
        arg->manager[nam] = val;
      }
      catch(const std::exception& e)  {
        printout(ERROR,"XMLConditions","++ FAILED: conditions Manager[%s] = %s [%s]",nam.c_str(),val.c_str(),e.what());
      }
    }
    arg->manager.initialize();
    printout(s_parseLevel,"XMLConditions","++ Conditions Manager successfully initialized.");
  }

  /// Convert rotation objects
  /**
   *    <rotation x="0.5" y="0"  z="0"/>
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<rotation>::operator()(xml_h e) const {
    xml_comp_t r(e);
    RotationZYX* v = (RotationZYX*)param;
    v->SetComponents(r.z(), r.y(), r.x());
    printout(s_parseLevel,"XMLConditions",
             "++ Rotation:   x=%9.3f y=%9.3f   z=%9.3f  phi=%7.4f psi=%7.4f theta=%7.4f",
             r.x(), r.y(), r.z(), v->Phi(), v->Psi(), v->Theta());
  }

  /// Convert position objects
  /**
   *    <position x="0.5" y="0"  z="0"/>
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<position>::operator()(xml_h e) const {
    xml_comp_t p(e);
    Position* v = (Position*)param;
    v->SetXYZ(p.x(), p.y(), p.z());
    printout(s_parseLevel,"XMLConditions","++ Position:   x=%9.3f y=%9.3f   z=%9.3f",
             v->X(), v->Y(), v->Z());
  }

  /// Convert pivot objects
  /**
   *    <pivot x="0.5" y="0"  z="0"/>
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<pivot>::operator()(xml_h e) const {
    xml_comp_t p(e);
    double x,y,z;
    Translation3D* v = (Translation3D*)param;
    v->SetXYZ(x=p.x(), y=p.y(), z=p.z());
    printout(s_parseLevel,"XMLConditions","++ Pivot:      x=%9.3f y=%9.3f   z=%9.3f",x,y,z);
  }

  /// Convert conditions value objects (scalars)
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<value>::operator()(xml_h e) const {
    ConversionArg* arg = _param<ConversionArg>();
    Condition      con = bind_condition(ValueBinder(), arg->detector, e);
    arg->manager.registerUnlocked(arg->pool, con);
  }

  /// Convert conditions sequence objects (unmapped containers)
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<sequence>::operator()(xml_h e) const {
    xml_dim_t      elt(e);
    Condition      con(0);
    string         typ = elt.typeStr();
    ConversionArg* arg = _param<ConversionArg>();
    size_t idx = typ.find('[');
    size_t idq = typ.find(']');
    string value_type = typ.substr(idx+1,idq-idx-1);
    if ( typ.substr(0,6) == "vector" )
      con = bind_condition(VectorBinder(), arg->detector, e, value_type);
    else if ( typ.substr(0,4) == "list" )
      con = bind_condition(ListBinder(), arg->detector, e, value_type);
    else if ( typ.substr(0,3) == "set" )
      con = bind_condition(SetBinder(), arg->detector, e, value_type);
    else
      except("XMLConditions",
             "++ Failed to convert unknown sequence conditions type: %s",typ.c_str());
    arg->manager.registerUnlocked(arg->pool, con);
  }

  /// Convert conditions STL maps
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<mapping>::operator()(xml_h e) const {
    xml_comp_t elt(e);
    ConversionArg* arg = _param<ConversionArg>();
    string    key_type = e.attr<string>(_U(key));
    string    val_type = e.attr<string>(_U(value));
    Condition      con = create_condition(arg->detector, e);
    OpaqueDataBlock& b = con->data;
    MapBinder binder;

    OpaqueDataBinder::bind_map(binder, b, key_type, val_type);
    for(xml_coll_t i(e,_U(item)); i; ++i)  {
      // If explicit key, value data are present in attributes:
      if ( i.hasAttr(_U(key)) && i.hasAttr(_U(value)) )  {
        string key = i.attr<string>(_U(key));
        string val = i.attr<string>(_U(value));
        OpaqueDataBinder::insert_map(binder, b, key_type, key, val_type, val);
        continue;
      }
      // Otherwise interprete the data directly from the data content
      OpaqueDataBinder::insert_map(binder, b, key_type, val_type, i.text());
    }
    arg->manager.registerUnlocked(arg->pool, con);
  }

  /// Convert alignment delta objects
  /**
   *     A generic alignment transformation is defined by
   *     - a translation in 3D space identified in XML as a
   *         <position/> element
   *       - a rotation in 3D space around a pivot point specified in XML by
   *         2 elements: the <rotation/> and the <pivot/> element.
   *       The specification of any of the elements is optional:
   *     - The absence of a translation implies the origine (0,0,0)
   *     - The absence of a pivot point implies the origine (0,0,0)
   *       - The absence of a rotation implies the identity rotation.
   *         Any supplied pivot point in this case is ignored.
   *
   *      <xx>
   *        <position x="0" y="0"  z="0.0001*mm"/>
   *        <rotation x="0" y="0"  z="0"/>
   *        <pivot    x="0" y="0"  z="100"/>
   *      </xx>
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<alignment>::operator()(xml_h e) const {
    using Alignments::Delta;
    Position       pos;
    RotationZYX    rot;
    Translation3D  piv;
    xml_h          child_rot, child_pos, child_piv;
    ConversionArg* arg = _param<ConversionArg>();
    Condition      con = create_condition(arg->detector, e);
    Delta&         del = con.bind<Delta>();

    if ( (child_pos=e.child(_U(position),false)) )
      Converter<position>(lcdd,&del.translation)(child_pos);
    if ( (child_rot=e.child(_U(rotation),false)) )   {
      Converter<rotation>(lcdd,&del.rotation)(child_rot);
      if ( (child_piv=e.child(_U(pivot),false)) )
        Converter<pivot>(lcdd,&del.pivot)(child_piv);
    }
    if ( child_rot && child_pos && child_piv )
      del.flags |= Delta::HAVE_ROTATION|Delta::HAVE_PIVOT|Delta::HAVE_TRANSLATION;
    else if ( child_rot && child_pos )
      del.flags |= Delta::HAVE_ROTATION|Delta::HAVE_TRANSLATION;
    else if ( child_rot && child_piv )
      del.flags |= Delta::HAVE_ROTATION|Delta::HAVE_PIVOT;
    else if ( child_rot )
      del.flags |= Delta::HAVE_ROTATION;
    else if ( child_pos )
      del.flags |= Delta::HAVE_TRANSLATION;

    con->setFlag(Condition::ALIGNMENT);
    arg->manager.registerUnlocked(arg->pool, con);
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
      XML::DocumentHolder doc(XML::DocumentHandler().load(e, e.attr_value(_U(ref))));
      (*this)(doc.root());
    }
    xml_coll_t(e,_U(value)).for_each(Converter<value>(lcdd,param,optional));
    xml_coll_t(e,_UC(mapping)).for_each(Converter<mapping>(lcdd,param,optional));
    xml_coll_t(e,_UC(sequence)).for_each(Converter<sequence>(lcdd,param,optional));
    xml_coll_t(e,_UC(alignment)).for_each(Converter<alignment>(lcdd,param,optional));
    xml_coll_t(e,_UC(detelement)).for_each(Converter<detelement>(lcdd,param,optional));
  }

  /// Convert repository objects
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  template <> void Converter<repository>::operator()(xml_h element) const {
    xml_coll_t(element,_UC(manager)).for_each(Converter<manager>(lcdd,param,optional));
    xml_coll_t(element,_UC(iov_type)).for_each(Converter<iov_type>(lcdd,param,optional));
    xml_coll_t(element,_UC(iov)).for_each(Converter<iov>(lcdd,param,optional));
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
      Converter<repository>(lcdd,param,optional)(e);
    else if ( tag == "manager" )  
      Converter<manager>(lcdd,param,optional)(e);
    else if ( tag == "detelement" )
      Converter<detelement>(lcdd,param,optional)(e);
    else if ( tag == "iov" )         // Processing repository file
      Converter<iov>(lcdd,param,optional)(e);
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
    xml_coll_t(e,_U(star)).for_each(Converter<arbitrary>(lcdd,param,optional));
  }
}

/// Basic entry point to set print level of this module.
/**
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long setup_repository_loglevel(lcdd_t& /* lcdd */, int argc, char** argv)  {
  if ( argc == 1 )  {
    s_parseLevel = printLevel(argv[1]);
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
static long setup_repository_Conditions(lcdd_t& lcdd, int argc, char** argv)  {
  if ( argc == 1 )  {
    DD4hepUI ui(lcdd);
    string fname = argv[0];
    ConditionsManager mgr = ui.conditionsMgr();
    ConversionArg args(lcdd.world(), mgr);
    XML::DocumentHolder doc(XML::DocumentHandler().load(fname));
    (DD4hep::Converter<conditions>(lcdd,&args))(doc.root());
    return 1;
  }
  except("XML_DOC_READER","Invalid number of arguments to interprete conditions: %d != %d.",argc,1);
  return 0;
}
DECLARE_APPLY(DD4hep_ConditionsXMLRepositoryParser,setup_repository_Conditions)
