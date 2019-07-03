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
#include "DD4hep/Factories.h"
#include "DD4hep/Shapes.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Detector.h"
#include "DD4hep/DetElement.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Path.h"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DD4hep/detail/DetectorInterna.h"

// C/C++ include files
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <fstream>

// ROOT includes
#include "TClass.h"
#include "TGeoMatrix.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"

using namespace std;
using namespace dd4hep;

namespace {
  std::string prefix = "\t";
  std::string sep(",");

  struct Actor   {
    std::map<const TGeoNode*,std::string>     placements;
    std::map<const TGeoVolume*,std::string>   volumes;
    std::map<const TGeoShape*,std::string>    shapes;
    std::map<const TGeoMatrix*,std::string>   matrices;
    std::map<const TGeoMedium*,std::string>   materials;
    std::map<DetElement,std::string>          detelements;

    std::string function {"run_geometry"};
    bool dump_vis = false;
    bool dump_structure = false;
    Detector& detector;
    Actor(Detector& d) : detector(d) {}
    ~Actor() = default;
    ostream& handleHeader   (ostream& log);
    ostream& handleTrailer  (ostream& log);
    ostream& handleSolid    (ostream& log, const TGeoShape*  sh);
    ostream& handleMatrix   (ostream& log, TGeoMatrix* mat);
    ostream& handleMaterial (ostream& log, TGeoMedium* mat);
    ostream& handlePlacement(ostream& log, TGeoNode*   parent, TGeoNode* node);
    ostream& handleStructure(ostream& log, DetElement parent, DetElement de);
  };
  typedef void* pvoid_t;

  ostream& newline(ostream& log)    {
    return log << endl << prefix;
  }
  template <typename T> const void* pointer(const Handle<T>& h)   {
    return h.ptr();
  }
  template <typename T> const void* pointer(const T* h)   {
    return h;
  }
  template <typename T> inline string obj_name(const string& pref, const T* ptr)  {
    stringstream name;
    name << pref << "_" << pointer(ptr);
    return name.str();
  }


  ostream& Actor::handleHeader   (ostream& log)    {
    log << "#include \"TClass.h\"" << endl
        << "#include \"TGeoNode.h\"" << endl
        << "#include \"TGeoExtension.h\"" << endl
        << "#include \"TGeoShapeAssembly.h\"" << endl
        << "#include \"TGeoMedium.h\"" << endl
        << "#include \"TGeoVolume.h\"" << endl
        << "#include \"TGeoShape.h\"" << endl
        << "#include \"TGeoPhysicalNode.h\"" << endl
        << "#include \"TGeoCone.h\"" << endl
        << "#include \"TGeoParaboloid.h\"" << endl
        << "#include \"TGeoPgon.h\"" << endl
        << "#include \"TGeoPcon.h\"" << endl
        << "#include \"TGeoSphere.h\"" << endl
        << "#include \"TGeoArb8.h\"" << endl
        << "#include \"TGeoTrd1.h\"" << endl
        << "#include \"TGeoTrd2.h\"" << endl
        << "#include \"TGeoTube.h\"" << endl
        << "#include \"TGeoEltu.h\"" << endl
        << "#include \"TGeoXtru.h\"" << endl
        << "#include \"TGeoHype.h\"" << endl
        << "#include \"TGeoTorus.h\"" << endl
        << "#include \"TGeoHalfSpace.h\"" << endl
        << "#include \"TGeoCompositeShape.h\"" << endl
        << "#include \"TGeoShapeAssembly.h\"" << endl
        << "#include \"TGeoMatrix.h\"" << endl
        << "#include \"TGeoBoolNode.h\"" << endl
        << "#include \"TGeoCompositeShape.h\"" << endl
        << "#include \"TGeoManager.h\"" << endl << endl
        << "#include \"DD4hep/Factories.h\"" << endl
        << "#include \"DD4hep/Shapes.h\"" << endl
        << "#include \"DD4hep/Volumes.h\"" << endl
        << "#include \"DD4hep/Detector.h\"" << endl
        << "#include \"DD4hep/DetElement.h\"" << endl
        << "#include \"DD4hep/MatrixHelpers.h\"" << endl
        << "#include \"DD4hep/DD4hepUnits.h\"" << endl
        << "#include \"DD4hep/Printout.h\"" << endl
        << "#include \"DD4hep/Path.h\"" << endl
        << "#include \"DD4hep/detail/ObjectsInterna.h\"" << endl
        << "#include \"DD4hep/detail/DetectorInterna.h\"" << endl
        << "#include <vector>" << endl
        << "#include <map>" << endl
        << "#include <set>" << endl << endl << endl;
    log << "using namespace std;" << endl;
    log << "using namespace dd4hep;" << endl;
    log << "extern PlacedVolume _addNode(TGeoVolume* par, TGeoVolume* daughter, int id, TGeoMatrix* transform);" << endl;

    log << "namespace  {" << newline
        << "\t struct CodeGeo  {" << newline
        << "\t\t map<unsigned long, TGeoNode*>     placements;" << newline
        << "\t\t map<unsigned long, TGeoVolume*>   volumes;" << newline
        << "\t\t map<unsigned long, TGeoShape*>    shapes;" << newline
        << "\t\t map<unsigned long, TGeoMatrix*>   matrices;" << newline
        << "\t\t map<unsigned long, TGeoMedium*>   materials;" << newline
        << "\t\t map<unsigned long, DetElement>    structure;" << newline
        << "\t\t Detector& detector;" << newline
        << "\t\t CodeGeo(Detector& d) : detector(d) {}" << newline
        << "\t\t TGeoVolume* load_geometry();" << newline
        << "\t\t DetElement  load_structure();" << newline
        << "\t\t void add_vis(Detector& d, VisAttr& v)  {" << newline
        << "\t\t  try { d.add(v); } catch(...) {}" << newline
        << "\t\t }" << newline
        << "\t };" << endl
        << "}" << endl << endl << endl;
    log << "TGeoVolume* CodeGeo::load_geometry()   {" << newline;

    const auto& constants = detector.constants();
    log << "/// Handling " << constants.size() << " Constants" << newline;
    for(const auto& o : constants)    {
      const Constant& c = o.second;
      log << "detector.add(Constant(\"" << c.name() << "\",\""
          << c->type << "\",\"" << c->dataType << "\")); " << newline; 
    }

    // Copy visualization attributes and register them to the detector object
    const auto& vis = detector.visAttributes();
    log << "/// Handling " << vis.size() << " Visualization attributes" << newline;
    for(const auto& o : vis)    {
      float r, g, b;
      VisAttr v(o.second);
      v.rgb(r,g,b);
      log << "{ VisAttr v(\"" << o.first << "\"); "
          << "v.setColor(" << r << sep << g << sep << b << "); "
          << "v.setShowDaughters(" << v.showDaughters() << "); "
          << "v.setVisible(" << v.visible() << "); " << newline
          << "  v.setLineStyle(" << v.lineStyle() << "); "
          << "v.setDrawingStyle(" << v.drawingStyle() << "); "
          << "v.setAlpha(" << v.alpha() << "); "
          << "add_vis(detector,v);  }" << newline;
    }

    const auto& limits = detector.limitsets();
    log << "/// Handling " << limits.size() << " Limit Sets" << newline;
    for(const auto& o : limits)    {
      LimitSet ls = o.second;
      log << "{ LimitSet ls(string(\"" << ls.name() << "\")); " << newline;
      const set<Limit>& lims = ls.limits();
      const set<Limit>& cuts = ls.cuts();
      for(const auto& l : lims)   {
        log << "  { Limit l; l.particles = \"" << l.particles << "\";"
            << " l.name = \""    << l.name << "\";"
            << " l.unit = \""    << l.unit << "\";"
            << " l.content = \"" << l.content << "\";"
            << " l.value = " << l.value << ";"
            << " ls.addLimit(l); } " << newline;
      }
      for(const auto& l : cuts)   {
        log << "  { Limit l; l.particles = \"" << l.particles << "\";"
            << " l.name = \""    << l.name << "\";"
            << " l.unit = \""    << l.unit << "\";"
            << " l.content = \"" << l.content << "\";"
            << " l.value = " << l.value << ";"
            << " ls.addLimit(l); } " << newline;
      }
      log << "  detector.addLimitSet(ls); } " << newline;
    }

    const auto& regions = detector.regions();
    log << "/// Handling " << regions.size() << " Region settings " << newline;
    for(const auto& o : regions)    {
      Region r = o.second;
      log << "{ Region r(\"" << r.name() << "\")"
          << "; r->store_secondaries = " << r->store_secondaries
          << "; r->was_threshold_set = " << r->was_threshold_set
          << "; r->use_default_cut = " << r->use_default_cut
          << "; r->threshold = " << r->threshold
          << "; r->cut = " << r->cut << ";" << newline;
      if ( !r->user_limits.empty() )   {
        log << " vector<string> user_limits = {";
        for(size_t i=0, n=r->user_limits.size(); i<n; ++i)
          log << "r->user_limits.push_back(\"" << r->user_limits[i] << "\");" << newline;
      }
    }

    const auto& ids = detector.idSpecifications();
    log << "/// Handling " << ids.size() << " Id Specifications " << newline;
    for(const auto& o : ids)    {
      IDDescriptor i = o.second;
      log << "{ IDDescriptor i(\"" << i.name() << "\", \"" << i->description << "\");"
          << " detector.add(i); }" << newline;
    }

    const auto& segments = detector.readouts();
    log << "/// Handling " << segments.size() << " Segmentations " << newline
        << "list<Segmentation> segs; " << newline;
    for(const auto& o : segments)    {
      Readout      r = o.second;
      Segmentation s = r->segmentation;
      if ( s.isValid() )   {
        log << "{ Segmentation s(\"" << s.name() << "\");"
            << " segs.push_back(s); }" << newline;
      }
    }

    const auto& readouts = detector.readouts();
    log << "/// Handling " << readouts.size() << " Readout settings " << newline;
    for(const auto& o : readouts)    {
      Readout r = o.second;
      log << "{ Readout r(string(\"" << r.name() << "\"));"
          << " r->SetTitle(\"" << r->GetTitle() << "\"); ";
      if ( r->segmentation.isValid() )
        log << " r->segmentation = segs.front(); segs.pop_front(); ";
      if ( r->id.isValid() )
        log << " r->id = detector.idSpecification(\"" << r->id.name() << "\"); ";
      for(const auto& c : r->hits)   {
        log << newline;
        log << "{ HitCollection c(\"" << c.name << "\",\"" << c.key << "\", "
            << c.key_min << sep << c.key_max << "); "
            << "  r->hits.push_back(c); } ";
      }
      if ( !r->hits.empty() ) log << newline;
      log << " detector.add(r); }" << newline;
    }
    return log;
  }

  ostream& Actor::handleTrailer   (ostream& log)    {
    log << "static long generate_dd4hep(Detector& detector, int, char**)   {" << newline
        << "CodeGeo gen(detector);"                     << newline
        << "TGeoVolume* vol_top = gen.load_geometry();" << newline
        << "detector.manager().SetTopVolume(vol_top);"  << newline
        << "detector.init();"                           << newline;
    if ( dump_structure )  {
      TGeoManager& mgr = detector.manager();
      handleMaterial(log, mgr.GetMedium("Air"));
      handleMaterial(log, mgr.GetMedium("Vacuum"));
      log << "gen.structure[" << pointer(detector.world()) << "] = detector.world(); " << newline
          << "gen.load_structure();" << newline;
    }
    log << "return 1;"
        << endl << "}"  << endl     << endl
        << "DECLARE_APPLY(DD4hep_Run_" << function << ", generate_dd4hep)"
        << endl;
    return log;
  }
  
  ostream& Actor::handleStructure(ostream& log, DetElement parent, DetElement de)   {
    if ( de.isValid() && detelements.find(de) == detelements.end() )  {
      string name = obj_name("de", de.ptr());
      detelements.insert(make_pair(de,name));
      if ( !parent.isValid() )   {
        cout << "No parent: " << de.path() << " " << pointer(de) << " " << pointer(detector.world()) << endl;
        log << endl
            << "DetElement  CodeGeo::load_structure()   {" << newline;
      }
      else  {
        log << "{" << newline
            << "\t DetElement par = structure[" << pointer(parent) << "];"          << newline
            << "\t DetElement de(par,\"" << de.name() << "\"," << de.id() << ");"   << newline
            << "\t de->SetTitle(\""             << de->GetTitle() << "\");"         << newline
            << "\t de->combineHits = "          << de->combineHits << ";"           << newline
            << "\t de.setTypeFlag("             << de.typeFlag() << ");"            << newline;
        if ( de.placement().isValid() )  {
          log << "\t de.setPlacement(placements[" << pointer(de.placement()) << "]);" << newline;
        }
        else  {
          cout << "Placement od DetElement " << de.path() << " Is not valid! [Ignored]" << endl;
        }
        log << "\t structure[" << pointer(de) << "] = de; " << newline
            << "}"   << newline;
      }
      for(auto d : de.children() )  {
        handleStructure(log, de, d.second);
      }
      if ( !parent.isValid() )   {
        log << "return structure[" << pointer(de) << "];" << endl
            << "}" << endl << endl;
      }
    }
    return log;
  }

  ostream& Actor::handlePlacement(ostream& log, TGeoNode* parent, TGeoNode* node)  {
    if ( node && placements.find(node) == placements.end() )  {
      PlacedVolume pv(node);
      TGeoVolume* vol = node->GetVolume();
      TGeoMatrix* mat = node->GetMatrix();

      string name = obj_name("vol", vol);
      placements.insert(make_pair(node,name));

      handleMatrix(log, mat);

      if ( vol && volumes.find(vol) == volumes.end() )  {
        volumes.insert(make_pair(vol,name));
        if ( vol->IsA() == TGeoVolumeAssembly::Class() )    {
          log << "{" << newline;
          log << "\t Assembly vol(\"" << vol->GetName() << "\");" << newline;
        }
        else   {
          Volume v(vol);
          TGeoMedium* med = vol->GetMedium();
          TGeoShape*  sh  = vol->GetShape();
          handleSolid(log, sh);
          handleMaterial(log, med);
          log << "{" << newline;
          log << "\t Volume vol(\"" << vol->GetName() << "\", "
              << "Solid(shapes[" << pointer(sh) << "]), "
              << "Material(materials[" << pointer(med) << "]));" << newline;
          if ( ::strlen(vol->GetTitle()) != 0 )
            log << "\t vol->SetTitle(\"" << vol->GetTitle() << "\");" << newline;
          if ( !v.option().empty() )
            log << "\t vol.setOption(\"" << v.option() << "\"); " << newline;
          if ( v.region().isValid() )
            log << "\t vol.setRegion(detector, \"" << v.region().name() << "\");" << newline;
          if ( v.limitSet().isValid() )
            log << "\t vol.setLimitSet(detector, \"" << v.limitSet().name() << "\");" << newline;
          if ( v.sensitiveDetector().isValid() )
            log << "\t vol.setSensitiveDetector(detector.sensitiveDetector(\""
                << v.sensitiveDetector().name() << "\"));" << newline;
          if ( dump_vis && v.visAttributes().isValid() )
            log << "\t vol.setVisAttributes(detector, \"" << v.visAttributes().name() << "\");" << newline;
        }
        log << "\t volumes[" << pointer(vol) << "] = vol;" << newline;
      }
      else  {
        log << "{" << newline
            << "\t Volume vol = volumes[" << pointer(vol) << "];" << newline;
      }
      if ( parent )   {
        log << "\t PlacedVolume pv = _addNode(volumes[" << pointer(parent->GetVolume())
            << "],vol.ptr()," << pv.copyNumber() << sep
            << "matrices[" << pointer(mat) << "]);" << newline
            << "\t placements[" << pointer(node) << "] = pv.ptr(); " << newline;
        for(const auto& vid : pv.volIDs() )
          log << "\t pv.addPhysVolID(\"" << vid.first << "\", " << vid.second << ");" << newline;
      }
      log << "}" << newline;
      for (Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau) {
        TGeoNode* daughter = node->GetDaughter(idau);
        handlePlacement(log, node, daughter);
      }
      if ( !parent )  {
        log << "return volumes[" << pointer(vol) << "];" << endl
            << "}" << endl << endl << endl;
      }
    }
    return log;
  }

  ostream& Actor::handleMaterial(ostream& log, TGeoMedium* medium)   {
    if ( medium && materials.find(medium) == materials.end() )  {
      string name = obj_name("material",medium);
      materials.insert(make_pair(medium,name));
      TGeoMaterial* material = medium->GetMaterial();
      log << "{" << newline
          << "\t TGeoManager& mgr = detector.manager();" << newline
          << "\t TGeoMedium* med = mgr.GetMedium(\""<< medium->GetName() << "\");" << newline
          << "\t if ( 0 == med )  {" << newline
          << "\t TGeoMaterial* mat = mgr.GetMaterial(\"" << material->GetName() << "\");" << newline
          << "\t if ( 0 == mat )  {" << newline
          << "\t   mat = new TGeoMaterial(\"" << material->GetName() << "\", "
          << material->GetA() << sep << material->GetZ() << sep << material->GetDensity() << sep
          << material->GetRadLen() << sep << material->GetIntLen() << ");" << newline
          << "\t }" << newline
          << "\t med = new TGeoMedium(\""<< medium->GetName() << "\"," << medium->GetId() << ", mat);" << newline
          << "\t }" << newline
          << "\t materials[" << pointer(medium) << "] = med;" << newline
          << "}" << newline;
    }
    return log;
  }
  
  ostream& Actor::handleMatrix(ostream& log, TGeoMatrix* mat)   {
    if ( mat && matrices.find(mat) == matrices.end() )  {
      const Double_t*	rot = mat->GetRotationMatrix();
      const Double_t*	tra = mat->GetTranslation();
      const Double_t*	sca = mat->GetScale();
      string name = obj_name("matrix",mat);
      log << "{" << newline
          << "\t TGeoHMatrix* mat = new TGeoHMatrix(\"" << mat->GetName() << "\");" << newline
          << "\t matrices[" << pointer(mat) << "] = mat;" << newline;
      if ( mat->IsTranslation() )   {
        log << "\t Double_t trans[] = {";
        for(size_t i=0; tra && i<3; ++i)  {
          log << (tra[i]);
          log << ((i<2) ? sep : "};");
        }
        log << newline << "\t mat->SetTranslation(trans);" << newline;
      }
      if ( mat->IsRotation() )   {
        if ( rot[0] != 1e0 || rot[4] != 1e0 || rot[8] != 1e0)  {
          log << "\t Double_t rot[] = {";
          for(size_t i=0; rot && i<9; ++i)  {
            log << (rot ? rot[i] : 0e0);
            log << ((i<8) ? sep : "};");
          }
          log << newline << "\t mat->SetRotation(rot);" << newline;
        }
      }
      if ( mat->IsScale() )   {
        log << "\t Double_t scale[] = {";
        for(size_t i=0; sca && i<3; ++i)  {
          log << (sca ? sca[i] : 0e0);
          log << ((i<2) ? sep : "};");
        }
        log << newline << "\t mat->SetScale(scale);" << newline;
      }
      log << "}" << newline;
    }
    return log;
  }
  
  /// Pretty print of solid attributes
  ostream& Actor::handleSolid(ostream& log,  const TGeoShape* shape)    {
    string name = obj_name("solid", shape);

    if ( shapes.find(shape) != shapes.end() )  {
      return log;
    }
    else if (shape->IsA() == TGeoShapeAssembly::Class()) {
      // Nothing to do here: All handled in the handling of TGeoVolumeAssembly
      return log;
    }

    shapes.insert(make_pair(shape,name));

    TClass* cl = shape->IsA();
    log << "{" << newline;
    if ( cl == TGeoBBox::Class() )   {
      TGeoBBox* sh = (TGeoBBox*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep << sh->GetDX()
          << sep << sh->GetDY()
          << sep << sh->GetDZ() << "));" << newline;
    }
    else if (cl == TGeoHalfSpace::Class()) {
      TGeoHalfSpace* sh = (TGeoHalfSpace*)(const_cast<TGeoShape*>(shape));
      log << "\t Double_t* point = {"
          << sh->GetPoint()[0] << sep << sh->GetPoint()[1] << sep << sh->GetPoint()[2] << "}; " << newline
          << "\t Double_t* norm = {"
          << sh->GetNorm()[0] << sep << sh->GetNorm()[1] << sep << sh->GetNorm()[2] << "}; " << newline
          << "\t Solid " << name << " = Solid(new " << cl->GetName() << "(\""
          << sh->GetName() << "\", point, norm));" << newline;
    }
    else if (cl == TGeoTube::Class()) {
      const TGeoTube* sh = (const TGeoTube*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep << sh->GetRmin()
          << sep << sh->GetRmax()
          << sep << sh->GetDz()
          << "));" << newline;
    }
    else if (cl == TGeoTubeSeg::Class()) {
      const TGeoTubeSeg* sh = (const TGeoTubeSeg*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep << sh->GetRmin()
          << sep << sh->GetRmax()
          << sep << sh->GetDz()
          << sep << sh->GetPhi1()
          << sep << sh->GetPhi2()
          << "));" << newline;
    }
    else if (cl == TGeoCtub::Class()) {
      const TGeoCtub* sh = (const TGeoCtub*) shape;
      const Double_t*	hi = sh->GetNhigh();
      const Double_t*	lo = sh->GetNlow();
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep << sh->GetRmin()
          << sep << sh->GetRmax()
          << sep << sh->GetDz()
          << sep << sh->GetPhi1()
          << sep << sh->GetPhi2()
          << sep << lo[0]
          << sep << lo[1]
          << sep << lo[2]
          << sep << hi[0]
          << sep << hi[1]
          << sep << hi[2]
          << "));" << newline;
    }
    else if (cl == TGeoEltu::Class()) {
      const TGeoEltu* sh = (const TGeoEltu*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep   << sh->GetA()
          << sep   << sh->GetB()
          << sep   << sh->GetDz()
          << "));"   << newline;
    }
    else if (cl == TGeoTrd1::Class()) {
      const TGeoTrd1* sh = (const TGeoTrd1*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep     << sh->GetDx1()
          << sep     << sh->GetDx2()
          << sep     << sh->GetDy()
          << sep     << sh->GetDz()
          << "));"    << newline;
    }
    else if (cl == TGeoTrd2::Class()) {
      const TGeoTrd2* sh = (const TGeoTrd2*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep     << sh->GetDx1()
          << sep     << sh->GetDx2()
          << sep     << sh->GetDy1()
          << sep     << sh->GetDy2()
          << sep     << sh->GetDz()
          << "));"    << newline;
    }
    else if (cl == TGeoTrap::Class()) {
      const TGeoTrap* sh = (const TGeoTrap*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep     << sh->GetDz() << sep << sh->GetTheta() << sep << sh->GetPhi()
          << sep     << sh->GetH1() << sep << sh->GetBl1()   << sep << sh->GetTl1() << sep << sh->GetAlpha1()
          << sep     << sh->GetH2() << sep << sh->GetBl2()   << sep << sh->GetTl2() << sep << sh->GetAlpha2()
          << "));"    << newline;
    }
    else if (cl == TGeoHype::Class()) {
      const TGeoHype* sh = (const TGeoHype*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep     << sh->GetRmin() << sep  << sh->GetRmax() << sep << sh->GetDz()
          << sep     << sh->GetStIn() << sep << sh->GetStOut()
          << "));"    << newline;
    }
    else if (cl == TGeoPgon::Class()) {
      const TGeoPgon* sh = (const TGeoPgon*) shape;
      log << "double params[] = {" << sh->GetPhi1() << sep << sh->GetDphi() << sep
          << sh->GetNedges() << sep << sh->GetNz();
      for(int i=0, n=sh->GetNz(); i<n; ++i)
        log << sep << sh->GetZ(i) << sep << sh->GetRmin(i) << sep << sh->GetRmax(i);
      log << "};" << newline
          << "\t Solid " << name << " = Solid(new "    << cl->GetName() << "(params));" << newline
          << name << "->SetName(\"" << sh->GetName() << "\");" << newline;
    }
    else if (cl == TGeoPcon::Class()) {
      const TGeoPcon* sh = (const TGeoPcon*) shape;
      log << "double params[] = {" << sh->GetPhi1() << sep << sh->GetDphi() << sep << sh->GetNz();
      for(int i=0, n=sh->GetNz(); i<n; ++i)
        log << sep << sh->GetZ(i) << sep << sh->GetRmin(i) << sep << sh->GetRmax(i);
      log << "};" << newline
          << "\t Solid " << name << " = Solid(new "    << cl->GetName() << "(params));" << newline
          << name << "->SetName(\""     << sh->GetName() << "\");" << newline;
    }
    else if (cl == TGeoCone::Class()) {
      const TGeoCone* sh = (const TGeoCone*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep << sh->GetDz()
          << sep << sh->GetRmin1() << sep << sh->GetRmax1()
          << sep << sh->GetRmin2() << sep << sh->GetRmax2()
          << "));"   << newline;
    }
    else if (cl == TGeoConeSeg::Class()) {
      const TGeoConeSeg* sh = (const TGeoConeSeg*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep << sh->GetDz()
          << sep << sh->GetRmin1() << sep << sh->GetRmax1()
          << sep << sh->GetRmin2() << sep << sh->GetRmax2()
          << sep   << sh->GetPhi1()  << sep << sh->GetPhi2()
          << "));"   << newline;
    }
    else if (cl == TGeoParaboloid::Class()) {
      const TGeoParaboloid* sh = (const TGeoParaboloid*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep   << sh->GetRlo() << sep << sh->GetRhi() << sep << sh->GetDz() << "));" << newline;
    }
    else if (cl == TGeoSphere::Class()) {
      const TGeoSphere* sh = (const TGeoSphere*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep << sh->GetRmin()   << sep << sh->GetRmax()
          << sep << sh->GetPhi1()   << sep << sh->GetPhi2()
          << sep << sh->GetTheta1() << sep << sh->GetTheta2()
          << "));" << newline;
    }
    else if (cl == TGeoTorus::Class()) {
      const TGeoTorus* sh = (const TGeoTorus*) shape;
      log << "\t Solid " << name << "(new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << sep << sh->GetRmin()  << sep << sh->GetRmax() << sep << sh->GetR()
          << sep << sh->GetPhi1()  << sep << sh->GetDphi()
          << "));" << newline;
    }
    else if (cl == TGeoArb8::Class()) {
      TGeoArb8* sh = (TGeoArb8*) shape;
      const Double_t* v = sh->GetVertices();
      log << "double vertices[] = {";
      for(int i=0; i<8; ++i, v+=2)
        log << v[0] << sep << v[1] << ((i<7) ? ',' : ' ');
      log << "};" << newline
          << "\t Solid " << name << " = Solid(new " << cl->GetName()
          << "(\"" << sh->GetName() << "\"" << sep << sh->GetDz()  << sep << "vertices);" << newline;
    }
    else if (cl == TGeoXtru::Class()) {
      Solid sol(shape);
      const TGeoXtru* sh = (const TGeoXtru*) shape;
      vector<double> pars = sol.dimensions();
      log << "double param[] = {" << newline;
      for( size_t i=0; i < pars.size(); ++i )
        log << pars[i] << ((i+1<pars.size()) ? ',' : ' ');
      log << "};" << newline
          << "\t Solid " << name << " = Solid(new " << cl->GetName() << "(param));" << newline
          << "\t " << name << "->SetName(\"" << sh->GetName() << "\");" << newline;
    }
    else if (shape->IsA() == TGeoCompositeShape::Class()) {
      const TGeoCompositeShape* sh = (const TGeoCompositeShape*) shape;
      const TGeoBoolNode* boolean = sh->GetBoolNode();
      const TGeoShape* left  = boolean->GetLeftShape();
      const TGeoShape* right = boolean->GetRightShape();
      TGeoBoolNode::EGeoBoolType oper = boolean->GetBooleanOperator();
      handleSolid(log, left);
      handleSolid(log, right);
      handleMatrix(log, boolean->GetRightMatrix());
      if (oper == TGeoBoolNode::kGeoSubtraction)
        log << "\t TGeoSubtraction* boolean" << pointer(shape) << " = new TGeoSubtraction";
      else if (oper == TGeoBoolNode::kGeoUnion)
        log << "\t TGeoUnion* boolean" << pointer(shape) << " = new TGeoUnion";
      else if (oper == TGeoBoolNode::kGeoIntersection)
        log << "\t TGeoIntersection* boolean" << pointer(shape) << " = new TGeoIntersection";
      log << "(shapes[" << pointer(left)  << "], "
          << " shapes[" << pointer(right) << "], 0, ";
      log << " matrices[" << pointer(boolean->GetRightMatrix()) << "]);" << newline;
      log << "\t Solid " << name << " = Solid(new TGeoCompositeShape(\""
          << sh->GetName() << "\", boolean" << pointer(shape) << "));" << newline;
    }
    else   {
      except("CxxRootGenerator","++ Unknown shape transformation request: %s", shape->IsA()->GetName());
    }
    log << "\t shapes[" << pointer(shape) << "] = " << name << ";" << newline
        << "}" << newline;
    return log;
  }
}

static long generate_cxx(Detector& description, int argc, char** argv) {
  string output;
  Actor actor(description);

  for(int i=0; i<argc; ++i)  {
    char c = ::tolower(argv[i][0]);
    char* p = argv[i];
    if ( c == '-' ) { ++p; c = ::tolower(argv[i][1]); }
    if ( c == '-' ) { ++p; c = ::tolower(argv[i][1]); }
    if ( c == 'o' && i+1<argc )
      output = argv[++i];
    else if ( c == 'f' && i+1<argc )
      actor.function = argv[++i];
    else if ( c == 'v' )
      actor.dump_vis = true;
    else if ( c == 's' )
      actor.dump_structure = true;
    else   {
      cout <<
        "Usage: -plugin DD4hep_CxxRootGenerator -arg [-arg]                                  \n"
        "     -output   <string> Set output file for generated code. Default: stdout         \n"
        "     -help              Show thi help message                                       \n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
  }
  unique_ptr<ofstream> out;
  ostream* os = &cout;
  if ( !output.empty() )   {
    Path path(output);
    out.reset(new ofstream(path.c_str()));
    if ( !out->good() )   {
      out.reset();
      except("CxxRootGenerator",
             "++ Failed to open output files: %s [%s]",
             path.c_str(), ::strerror(errno));
    }
    os = out.get();
    actor.function = path.filename();
    if ( actor.function.rfind('.') != string::npos )
      actor.function = actor.function.substr(0, actor.function.rfind('.'));
    printout(INFO, "CxxRootGenerator",
             "++ Dump generated code to output files: %s [function: %s()]",
             path.c_str(), actor.function.c_str());
  }
  else if ( actor.function.empty() )   {
    actor.function = "run_geometry";
  }
  DetElement de = description.world();
  PlacedVolume pv = de.placement();
  actor.handleHeader(*os);
  actor.handlePlacement(*os, 0, pv.ptr());
  actor.handleStructure(*os, DetElement(), de);
  actor.handleTrailer(*os);
  out.reset();
  return 1;
}

DECLARE_APPLY(DD4hep_CxxGenerator,generate_cxx)
