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
#ifndef DD4HEP_DDCOND_CONDITIONSREPOSITORYWRITER_H
#define DD4HEP_DDCOND_CONDITIONSREPOSITORYWRITER_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DDCond/ConditionsManager.h"
#include "XML/XMLElements.h"

// C/C++ include files



/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {
    
    /// Conditions slice object. Defines which conditions should be loaded by the ConditionsManager.
    /**
     *  Object contains set of required conditions keys to be loaded to the user pool.
     *  It alkso contains the load information for the required conditions (conditions addresses).
     *  The address objects depend on the actual loader mechanism and must be specified the user.
     *  The information is then chained through the calls and made availible to the loader object.
     *
     *  On return it contains the individual condition load information.
     *
     *  Referenced by: ConditonsUserPool, ConditionsManager
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsXMLRepositoryWriter  {
      ConditionsManager manager;
    public:
      /// Default constructor
      ConditionsXMLRepositoryWriter() = delete;
      /// Copy constructor (Special, partial copy only. Hence no assignment!)
      ConditionsXMLRepositoryWriter(const ConditionsXMLRepositoryWriter& copy) = delete;
      /// Initializing constructor
      ConditionsXMLRepositoryWriter(ConditionsManager& mgr);
      /// Default destructor. 
      virtual ~ConditionsXMLRepositoryWriter();
     
      /// Dump the tree content into a XML document structure
      size_t collectConditions(XML::Element root,
                               ConditionsSlice& slice,
                               DetElement detector)  const;
      /// Write the XML document structure to a file.
      long write(XML::Document doc, const std::string& output)   const;
      /// Dump the tree content into a XML document structure
      XML::Document dump(ConditionsSlice& slice, DetElement element)  const;
    };
    
  }        /* End namespace Conditions                          */
}          /* End namespace DD4hep                              */
#endif     /* DD4HEP_DDCOND_CONDITIONSREPOSITORYWRITER_H        */


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
#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetConditions.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/OpaqueDataBinder.h"
#include "DD4hep/ConditionsKeyAssign.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/objects/ConditionsInterna.h"

#include "DDCond/ConditionsTags.h"
#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsManager.h"

// C/C++ include files
#include <stdexcept>

using std::string;
using namespace DD4hep;
using namespace DD4hep::Conditions;
using Geometry::RotationZYX;
using Geometry::Transform3D;
using Geometry::Translation3D;
using Geometry::Position;

/// Anonymous local stuff only used in this module
namespace {

  /// Module print level
  static PrintLevel s_printLevel = INFO;

  class condition;
  class pressure;
  class temperature;
  class alignment;
  class position;
  class rotation;
  class pivot;

  template <typename T> XML::Element _convert(XML::Element par, Condition c);

  XML::Element make(XML::Element e, Condition c)  {
    std::string nam = c.name();
    std::string cn = nam.substr(nam.find('#')+1);
    e.setAttr(_U(name),cn);
    return e;
  }
  XML::Element _convert(XML::Element par, const Translation3D& tr)  {
    XML::Element e = XML::Element(par.document(),_U(pivot));
    const Translation3D::Vector& v = tr.Vect();
    e.setAttr(_U(x),v.X());
    e.setAttr(_U(y),v.Y());
    e.setAttr(_U(z),v.Z());
    return e;
  }
  XML::Element _convert(XML::Element par, const Position& pos)  {
    XML::Element e = XML::Element(par.document(),_U(position));
    e.setAttr(_U(x),pos.X());
    e.setAttr(_U(y),pos.Y());
    e.setAttr(_U(z),pos.Z());
    return e;
  }
  XML::Element _convert(XML::Element par, const RotationZYX& rot)  {
    XML::Element e = XML::Element(par.document(),_U(rotation));
    double z, y, x;
    rot.GetComponents(z,y,x);
    e.setAttr(_U(x),x);
    e.setAttr(_U(y),y);
    e.setAttr(_U(z),z);
    return e;
  }
  template <> XML::Element _convert<pressure>(XML::Element par, Condition c)  {
    XML::Element press = make(XML::Element(par.document(),_UC(pressure)),c);
    press.setAttr(_U(value),c.get<float>()/(100e0*dd4hep::pascal));
    press.setAttr(_U(unit),"hPa");
    return press;
  }
  template <> XML::Element _convert<temperature>(XML::Element par, Condition c)  {
    XML::Element temp = make(XML::Element(par.document(),_UC(temperature)),c);
    temp.setAttr(_U(value),c.get<float>()/dd4hep::kelvin);
    temp.setAttr(_U(unit),"kelvin");
    return temp;
  }
  template <> XML::Element _convert<alignment>(XML::Element par, Condition c)  {
    using Alignments::Delta;
    XML::Element align = make(XML::Element(par.document(),_UC(alignment)),c);
    const Delta& delta = c.get<Delta>();
    if ( delta.flags&Delta::HAVE_TRANSLATION )
      align.append(_convert(align,delta.translation));
    if ( delta.flags&Delta::HAVE_ROTATION )
      align.append(_convert(align,delta.rotation));
    if ( delta.flags&Delta::HAVE_PIVOT )
      align.append(_convert(align,delta.pivot));
    return align;
  }
}

/// Initializing constructor
ConditionsXMLRepositoryWriter::ConditionsXMLRepositoryWriter(ConditionsManager& mgr)
  : manager(mgr)
{
}

/// Default destructor. 
ConditionsXMLRepositoryWriter::~ConditionsXMLRepositoryWriter()   {
}

/// Dump the tree content into a XML document structure
XML::Document ConditionsXMLRepositoryWriter::dump(ConditionsSlice& slice, DetElement element)  const {
  XML::DocumentHandler docH;
  XML::Document doc  = docH.create("conditions", docH.defaultComment());
  XML::Element  root = doc.root();
  collectConditions(root,slice,element);
  return doc;
}

/// Dump the tree content into a XML document structure
size_t ConditionsXMLRepositoryWriter::collectConditions(XML::Element root,
                                                        ConditionsSlice& slice,
                                                        DetElement detector)  const
{
  size_t count = 0;
  if ( detector.isValid() )  {
    if ( detector.hasConditions() )   {
      DetConditions det(detector);
      Container     cont = det.conditions();
      if ( cont.numKeys() > 0 )   {
        XML::Element conditions = XML::Element(root.document(),_U(detelement));
        conditions.setAttr(_U(path),detector.path());
        printout(s_printLevel,"Writer","++ Conditions of DE %s [%d entries]",
                 detector.path().c_str(), int(cont.keys().size()));
        root.append(conditions);
        for(const auto& k : cont.keys() )   {
          if ( k.first == k.second.first )  {
            Condition   c   = cont.get(k.first,*slice.pool);
            std::string nam = c.name();
            std::string cn  = nam.substr(nam.find('#')+1);
            ReferenceBitMask<Condition::mask_type> msk(c->flags);
          
            printout(s_printLevel,"Writer","++ Condition %s [%16llX] -> %s",
                     cn.c_str(), k.first, c.name());
            if ( msk.isSet(Condition::TEMPERATURE) )  {
              conditions.append(_convert<temperature>(conditions,c));
            }
            else if ( msk.isSet(Condition::PRESSURE) )  {
              conditions.append(_convert<pressure>(conditions,c));
            }
            else if ( msk.isSet(Condition::ALIGNMENT) )  {
              conditions.append(_convert<alignment>(conditions,c));
            }
            else   {
              printout(ERROR,"Writer","Unknown data type of condition: %s [%16llX] -> %s Flags:0x%08X",
                       cn.c_str(), k.first, c.name(), c->flags);
            }
          }
        }
      }
    }
    for (const auto& i : detector.children())
      count += collectConditions(root,slice,i.second);
  }
  return count;
}

/// Write the XML document structure to a file.
long ConditionsXMLRepositoryWriter::write(XML::Document doc, const string& output)   const {
  XML::DocumentHandler docH;
  return docH.output(doc, output);
}

/// Basic entry point to read alignment conditions files
/**
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long write_repository_conditions(lcdd_t& lcdd, int argc, char** argv)  {
  ConditionsManager manager  =  ConditionsManager::from(lcdd);
  const IOVType*    iovtype  =  0;
  long              iovvalue = -1;
  string            output;

  for(int i=0; i<argc; ++i)  {
    if ( ::strncmp(argv[i],"-iov_type",7) == 0 )
      iovtype = manager.iovType(argv[++i]);
    else if ( ::strncmp(argv[i],"-iov_value",7) == 0 )
      iovvalue = ::atol(argv[++i]);
    else if ( ::strncmp(argv[i],"-output",4) == 0 && argc>i+1)
      output = argv[++i];
    else if ( ::strncmp(argv[i],"-help",2) == 0 )  {
      printout(ALWAYS,"Plugin-Help","Usage: DD4hep_XMLConditionsRepositoryWriter --opt [--opt]           ");
      printout(ALWAYS,"Plugin-Help","  -output    <string>   Output file name. Default: stdout           ");
      printout(ALWAYS,"Plugin-Help","  -iov_type  <string>   IOV type to be selected.                    ");
      printout(ALWAYS,"Plugin-Help","  -iov_value <string>   IOV value to create the conditions snapshot.");
      ::exit(EINVAL);
    }
  }
  if ( 0 == iovtype )
    except("ConditionsPrepare","++ Unknown IOV type supplied.");
  if ( 0 > iovvalue )
    except("ConditionsPrepare",
           "++ Unknown IOV value supplied for iov type %s.",iovtype->str().c_str());

  IOV iov(iovtype,iovvalue);
  dd4hep_ptr<ConditionsSlice> slice(Conditions::createSlice(manager,*iovtype));
  ConditionsManager::Result cres = manager.prepare(iov, *slice);
  printout(INFO,"Conditions",
           "++ Selected conditions: %7ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) for IOV:%-12s",
           cres.total(), cres.selected, cres.loaded, cres.computed, cres.missing,
           iovtype ? iov.str().c_str() : "???");
  
  ConditionsXMLRepositoryWriter writer(manager);
  XML::Document doc = writer.dump(*slice,lcdd.world());
  writer.write(doc, output);
  return 1;
}
DECLARE_APPLY(DD4hep_XMLConditionsRepositoryWriter,write_repository_conditions)
