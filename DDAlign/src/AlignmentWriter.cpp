// $Id: $
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
#include "DDAlign/AlignmentWriter.h"

#include "DD4hep/Printout.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/DetFactoryHelper.h"
#include "XML/DocumentHandler.h"
#include "DDAlign/AlignmentTags.h"
#include "DDAlign/AlignmentCache.h"
#include "DDAlign/DetectorAlignment.h"

#include "TGeoMatrix.h"

// C/C++ include files
#include <stdexcept>

namespace DetectorTools = DD4hep::Geometry::DetectorTools;
using namespace DD4hep::Alignments;
using namespace DD4hep;
using namespace std;

/// Initializing Constructor
AlignmentWriter::AlignmentWriter(LCDD& lcdd)
  : m_lcdd(lcdd)
{
  m_cache = lcdd.extension<Alignments::AlignmentCache>();
}

/// Standard destructor
AlignmentWriter::~AlignmentWriter()  {
  if ( m_cache ) m_cache->release();
}

/// Create the element corresponding to one single detector element without children
XML::Element AlignmentWriter::createElement(XML::Document doc, DetElement element)  const  {
  XML::Element e(0), placement(0), elt = XML::Element(doc,_ALU(detelement));
  string path = element.placementPath();
  GlobalAlignment a = element->global_alignment;
  DetectorAlignment det(element);
  const vector<GlobalAlignment>& va = det.volumeAlignments();

  elt.setAttr(_ALU(path),element.path());
  if ( a.isValid() )  {
    addNode(elt,a);
  }
  for(vector<GlobalAlignment>::const_iterator i=va.begin(); i!=va.end();++i)  {
    e = XML::Element(doc,_U(volume));
    e.setAttr(_ALU(path),(*i)->GetName());
    addNode(e,*i);
    elt.append(e);
  }
  return elt;
}

/// Add single alignment node to the XML document
void AlignmentWriter::addNode(XML::Element elt, GlobalAlignment a)  const   {
  TGeoNode* n = a->GetNode();
  TGeoHMatrix mat(a->GetOriginalMatrix()->Inverse());
  mat.Multiply(n->GetMatrix());
  const Double_t* t = mat.GetTranslation();
  XML::Element placement = XML::Element(elt.document(),_U(comment));
  placement.setAttr(_ALU(placement),a->GetName());
  elt.append(placement);

  printout(INFO,"AlignmentWriter","Write Delta constants for %s",a->GetName());
  //mat.Print();
  if ( fabs(t[0]) > numeric_limits<double>::epsilon() ||
       fabs(t[1]) > numeric_limits<double>::epsilon() ||
       fabs(t[2]) > numeric_limits<double>::epsilon() ) {
    XML::Element e = XML::Element(elt.document(),_U(position));
    e.setAttr(_U(x),t[0]);
    e.setAttr(_U(y),t[1]);
    e.setAttr(_U(z),t[2]);
    elt.append(e);
  }
  if ( mat.IsRotation() )  {
    XYZAngles rot = _XYZangles(&mat);
    if ( fabs(rot.X()) > numeric_limits<double>::epsilon() ||
         fabs(rot.Y()) > numeric_limits<double>::epsilon() ||
         fabs(rot.Z()) > numeric_limits<double>::epsilon() )    {

      XML::Element e = XML::Element(elt.document(),_U(rotation));
      // Don't know why the angles have the wrong sign....
      rot *= -1;
      e.setAttr(_U(x),rot.X());
      e.setAttr(_U(y),rot.Y());
      e.setAttr(_U(z),rot.Z());
      elt.append(e);
    }
  }
}

/// Scan a DetElement subtree and add on the fly the XML entries
XML::Element AlignmentWriter::scan(XML::Document doc, DetElement element)  const  {
  XML::Element elt(0);
  if ( element.isValid() )   {
    const DetElement::Children& c = element.children();
    GlobalAlignment alignment = element->global_alignment;
    if ( alignment.isValid() ) elt = createElement(doc,element);
    for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i)   {
      XML::Element daughter = scan(doc, (*i).second);
      if ( daughter )   {
        (elt ? (elt) : (elt=createElement(doc,element))).append(daughter);
      }
    }
  }
  return elt;
}

/// Dump the tree content into a XML document structure
XML::Document AlignmentWriter::dump(DetElement top, bool enable_transactions)  const {
  const char comment[] = "\n"
    "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
    "      ++++   DD4hep generated alignment file using the         ++++\n"
    "      ++++   DD4hep Detector description XML generator.        ++++\n"
    "      ++++                                                     ++++\n"
    "      ++++   Parser:"
    XML_IMPLEMENTATION_TYPE
    "                ++++\n"
    "      ++++                                                     ++++\n"
    "      ++++                              M.Frank CERN/LHCb      ++++\n"
    "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n  ";
  XML::DocumentHandler docH;
  XML::Document doc = docH.create("alignment", comment);
  XML::Element elt(0), elements(0), root = doc.root();
  root.append(elements = XML::Element(doc, _ALU(detelements)));
  if ( enable_transactions ) root.append(XML::Element(doc,_ALU(open_transaction)));
  if ( (elt=scan(doc,top)) ) elements.append(elt);
  if ( enable_transactions ) root.append(XML::Element(doc,_ALU(close_transaction)));
  return doc;
}

/// Write the XML document structure to a file.
long AlignmentWriter::write(XML::Document doc, const string& output)   const {
  XML::DocumentHandler docH;
  return docH.output(doc, output);
}

static long create_alignment_file(LCDD& lcdd, int argc, char** argv)   {
  DetElement top;
  string output, path = "/world";
  bool enable_transactions = false;
  for(int i=1; i<argc;++i) {
    if ( argv[i][0]=='-' || argv[i][0]=='/' ) {
      const char* p = ::strchr(argv[i],'=');
      if ( p && strncmp(argv[i]+1,"-output",7)==0 )
        output = p+1;
      else if ( p && strncmp(argv[i]+1,"-path",5)==0 )
        path = p+1;
      else if ( strncmp(argv[i]+1,"-transactions",5)==0 )
        enable_transactions = true;
      else
        throw runtime_error("AlignmentWriter: Invalid argument:"+string(argv[i]));
    }
  }
  printout(ALWAYS,"AlignmentWriter",
           "++++ Writing DD4hep alignment constants of the \"%s\" DetElement tree to file \"%s\"",
           path.c_str(), output.c_str());
  top = DetectorTools::findDaughterElement(lcdd.world(),path);
  if ( top.isValid() )   {
    AlignmentWriter wr(lcdd);
    return wr.write(wr.dump(top,enable_transactions), output);
  }
  throw runtime_error("AlignmentWriter: Invalid top level element name:"+path);
}

DECLARE_APPLY(DDAlignmentWriter, create_alignment_file)
