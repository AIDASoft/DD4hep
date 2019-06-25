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
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Path.h"

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

  /// Debug utility to generate pure TGeo code from DD4hep detector setups.
  /** 
   *  This utility can be used to reproduce problems with TGeo ONLY.
   * 
   *  \author  M.Frank
   *  \version 1.0
   */
  struct Actor   {
    std::set<const TGeoNode*>     nodes;
    std::set<const TGeoVolume*>   volumes;
    std::set<const TGeoShape*>    solids;
    std::set<const TGeoMatrix*>   matrices;
    std::set<const TGeoMedium*>   materials;
    std::set<const TGeoElement*>  elements;
    std::string function {"run_geometry"};
    bool dump_vis = false;
    bool dump_mat = false;

    Actor() = default;
    ~Actor() = default;
    ostream& handleHeader   (ostream& log);
    ostream& handleTrailer  (ostream& log);
    ostream& handleSolid    (ostream& log, const TGeoShape*  sh);
    ostream& handleMatrix   (ostream& log, TGeoMatrix* mat);
    ostream& handleElement  (ostream& log, TGeoElement* elt);
    ostream& handleMaterial (ostream& log, TGeoMedium* mat);
    ostream& handlePlacement(ostream& log, TGeoNode*   parent, TGeoNode* node);
  };
  typedef void* pvoid_t;

  ostream& newline(ostream& log)    {
    return log << endl << prefix;
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
        << "#include \"TGeoManager.h\"" << endl
        << "#include <vector>" << endl
        << "#include <map>" << endl
        << "#include <set>" << endl << endl << endl;
    log << "TGeoVolume* generate_geometry()   {" << newline;
    return log;
  }

  ostream& Actor::handleTrailer   (ostream& log)    {
    log << endl << "}" << endl << endl;
    log << "void " << function << "() {" << newline
        << "if ( !gGeoManager ) gGeoManager = new TGeoManager();" << newline
        << "TGeoVolume* vol_top = generate_geometry();" << newline
        << "gGeoManager->SetTopVolume(vol_top);" << newline
        << "vol_top->Draw(\"ogl\");" << newline
        << endl << "}" << endl;
    return log;
  }

  ostream& Actor::handlePlacement(ostream& log, TGeoNode* parent, TGeoNode* node)  {
    if ( node && nodes.find(node) == nodes.end() )  {
      TGeoVolume* vol = node->GetVolume();
      TGeoMatrix* mat = node->GetMatrix();
      nodes.insert(node);
      handleMatrix(log, mat);

      if ( vol && volumes.find(vol) == volumes.end() )  {
        volumes.insert(vol);
        if ( vol->IsA() == TGeoVolumeAssembly::Class() )    {
          log << "TGeoVolume* vol_" << pvoid_t(vol)
              << " = new TGeoVolumeAssembly(\"" << vol->GetName() << "\");" << newline;          
        }
        else   {
          TGeoMedium* med = vol->GetMedium();
          TGeoShape*  sh  = vol->GetShape();
          handleSolid(log, sh);
          handleMaterial(log, med);
          log << "TGeoVolume* vol_" << pvoid_t(vol) << " = new TGeoVolume(\"" << vol->GetName() << "\", "
              << "shape_" << pvoid_t(sh) << ", material_" << pvoid_t(med) << ");" << newline;
          if ( dump_vis )    {
            log << "vol_" << pvoid_t(vol) << "->SetLineColor(Color_t(" << int(vol->GetLineColor()) << "));" << newline;
            log << "vol_" << pvoid_t(vol) << "->SetLineStyle(" << vol->GetLineStyle() << ");" << newline;
            log << "vol_" << pvoid_t(vol) << "->SetLineWidth(" << vol->GetLineWidth() << ");" << newline;
            log << "vol_" << pvoid_t(vol) << "->SetFillColor(Color_t(" << int(vol->GetFillColor()) << "));" << newline;
            log << "vol_" << pvoid_t(vol) << "->SetFillStyle(" << vol->GetFillStyle() << ");" << newline;
          }
        }
      }

      for (Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau) {
        TGeoNode* daughter = node->GetDaughter(idau);
        handlePlacement(log, node, daughter);
      }

      if ( parent )   {
        Int_t ndau       = parent->GetNdaughters();
        TGeoVolume* pvol = parent->GetVolume();
        log << "vol_" << pvoid_t(pvol)
            << "->AddNode(vol_" << pvoid_t(vol) << ", " << ndau << ", matrix_" << pvoid_t(mat)
            << ");" << newline;
        log << "TGeoNode* node_" << pvoid_t(node) << " = vol_" << pvoid_t(pvol)
            << "->GetNode(" << ndau << ");" << newline;
      }
      else   {
        log << "return vol_" << pvoid_t(vol) << ";" << endl;
      }
    }
    return log;
  }

  ostream& Actor::handleElement  (ostream& log, TGeoElement* elt)   {
    if ( elt && elements.find(elt) == elements.end() )  {
      elements.insert(elt);
      log << "TGeoElement* elt_" << pvoid_t(elt) << " = new TGeoElement(\""
          << elt->GetName() << "\", \"" << elt->GetTitle() << "\", ";
      if ( elt->GetNisotopes() > 0 )   {
        log << elt->GetNisotopes() << ");" << newline;
        for(Int_t i=0; i<elt->GetNisotopes(); ++i)   {
          TGeoIsotope* iso = elt->GetIsotope(i);
          log << "elt_" << pvoid_t(elt) << "->AddIsotope("
              << "new TGeoIsotope(\"" << elt->GetName() << "_" << iso->GetN()
              << "\", " << iso->GetZ() << ", " << iso->GetN() << ", " << iso->GetA() << "));"
              << newline;
        }
      }
      else   {
        log << elt->Z() << ", " << elt->N() << ", " << elt->A() << ");" << newline;
      }
    }
    return log;
  }

  ostream& Actor::handleMaterial(ostream& log, TGeoMedium* medium)   {
    if ( medium && materials.find(medium) == materials.end() )  {
      materials.insert(medium);
      if ( !dump_mat )    {
        log << "TGeoMedium* material_" << pvoid_t(medium) << " = gGeoManager->GetMedium(\"IRON\");"
            << newline;
        return log;
      }
      TGeoMaterial* mat = medium->GetMaterial();
      if ( mat->IsMixture() )  {
        TGeoMixture* mix = (TGeoMixture*) mat;
        int nElements = mix->GetNelements();
        double W_total = 0.0;
        for (int i = 0; i < nElements; ++i)   {
          handleElement(log, mix->GetElement(i));
          W_total += (mix->GetWmixt())[i];
        }
        log << "TGeoMixture* mat_" << pvoid_t(mat) << " = new TGeoMixture(\""
            << mix->GetName() << "\", " << nElements << ", " << mix->GetDensity() << ");"
            << newline;
        for (int i = 0; i < nElements; ++i)   {
          TGeoElement* e = mix->GetElement(i);
          log << "mat_" << pvoid_t(mat) << "->AddElement(elt_" << pvoid_t(e)
              << ", " << ((mix->GetWmixt())[i]/W_total) << ");"
              << newline;
        }
        mix->SetRadLen(0e0);
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,12,0)
        mix->ComputeDerivedQuantities();
#endif
      }
      else {
        double z = mat->GetZ(), a = mat->GetA();
        if ( z < 1.0000001 ) z = 1.0;
        if ( a < 0.5000001 ) a = 1.0;
        log << "TGeoMaterial* mat_" << pvoid_t(mat) << " = new TGeoMaterial(\""
            << mat->GetName() << "\", " << a << ", " << z
            << ", " << mat->GetDensity()
            << ", " << mat->GetRadLen()
            << ", " << mat->GetIntLen() << ");"
            << newline;
      }
      log << "mat_" << pvoid_t(mat) << "->SetState(TGeoMaterial::EGeoMaterialState("
          << mat->GetState() << "));" << newline
          << "mat_" << pvoid_t(mat) << "->SetPressure(" << mat->GetPressure() << ");" << newline
          << "mat_" << pvoid_t(mat) << "->SetTemperature(" << mat->GetTemperature() << ");" << newline
          << "mat_" << pvoid_t(mat) << "->SetTransparency(" << int(mat->GetTransparency()) << ");" << newline;
      log << "TGeoMedium* material_" << pvoid_t(medium) << " = new TGeoMedium(\""
          << medium->GetName() << "\", " << medium->GetId()
          << ", mat_" << pvoid_t(mat) << ");" << newline;
    }
    return log;
  }
  
  ostream& Actor::handleMatrix(ostream& log, TGeoMatrix* mat)   {
    if ( mat && matrices.find(mat) == matrices.end() )  {
      const Double_t*	rot = mat->GetRotationMatrix();
      const Double_t*	tra = mat->GetTranslation();
      const Double_t*	sca = mat->GetScale();
      log << "TGeoHMatrix* matrix_" << pvoid_t(mat) << " = new TGeoHMatrix(\"" << mat->GetName() << "\");"
          << newline << "{" << newline;
      if ( mat->IsTranslation() )   {
        log << "\t Double_t trans[] = {";
        for(size_t i=0; tra && i<3; ++i)  {
          log << (tra ? tra[i] : 0e0);
          log << ((i<2) ? ", " : "};");
        }
        log << newline << "\t matrix_" << pvoid_t(mat) << "->SetTranslation(trans);" << newline;
      }
      if ( mat->IsRotation() )   {
        log << "\t Double_t rot[] = {";
        for(size_t i=0; rot && i<9; ++i)  {
          log << (rot ? rot[i] : 0e0);
          log << ((i<8) ? ", " : "};");
        }
        log << newline << "\t matrix_" << pvoid_t(mat) << "->SetRotation(rot);" << newline;
      }
      if ( mat->IsScale() )   {
        log << "\t Double_t scale[] = {";
        for(size_t i=0; sca && i<3; ++i)  {
          log << (sca ? sca[i] : 0e0);
          log << ((i<2) ? ", " : "};");
        }
        log << newline << "\t matrix_" << pvoid_t(mat) << "->SetScale(scale);" << newline;
      }
      log << "}" << newline;
    }
    return log;
  }
  
  /// Pretty print of solid attributes
  ostream& Actor::handleSolid(ostream& log,  const TGeoShape* shape)    {
    if ( !shape || solids.find(shape) != solids.end() )  {
      return log;
    }
    solids.insert(shape);
    
    TClass* cl = shape->IsA();
    void* pvoid = (void*)shape;
    if ( cl == TGeoBBox::Class() )   {
      TGeoBBox* sh = (TGeoBBox*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << ", " << sh->GetDX()
          << ", " << sh->GetDY()
          << ", " << sh->GetDZ() << ");" << newline;
    }
    else if (cl == TGeoHalfSpace::Class()) {
      TGeoHalfSpace* sh = (TGeoHalfSpace*)(const_cast<TGeoShape*>(shape));
      log << cl->GetName() << "* shape_" << (void*)shape << " = 0;" << newline
          << "{" << newline
          << "\t Double_t* point_ << " << pvoid << " = {"
          << sh->GetPoint()[0] << ", " << sh->GetPoint()[1] << ", " << sh->GetPoint()[2] << "}; " << newline
          << "\t Double_t* norm_ << " << pvoid << " = {"
          << sh->GetNorm()[0] << ", " << sh->GetNorm()[1] << ", " << sh->GetNorm()[2] << "}; " << newline
          << "shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(\"" << sh->GetName() << "\" "
          << ", point_" << pvoid << ", norm_" << pvoid << ");" << newline
          << "}" << newline;
    }
    else if (cl == TGeoTube::Class()) {
      const TGeoTube* sh = (const TGeoTube*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << ", " << sh->GetRmin()
          << ", " << sh->GetRmax()
          << ", " << sh->GetDz()
          << ");" << newline;
    }
    else if (cl == TGeoTubeSeg::Class()) {
      const TGeoTubeSeg* sh = (const TGeoTubeSeg*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << ", " << sh->GetRmin()
          << ", " << sh->GetRmax()
          << ", " << sh->GetDz()
          << ", " << sh->GetPhi1()
          << ", " << sh->GetPhi2()
          << ");" << newline;
    }
    else if (cl == TGeoCtub::Class()) {
      const TGeoCtub* sh = (const TGeoCtub*) shape;
      const Double_t*	hi = sh->GetNhigh();
      const Double_t*	lo = sh->GetNlow();
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << ", " << sh->GetRmin()
          << ", " << sh->GetRmax()
          << ", " << sh->GetDz()
          << ", " << sh->GetPhi1()
          << ", " << sh->GetPhi2()
          << ", " << lo[0]
          << ", " << lo[1]
          << ", " << lo[2]
          << ", " << hi[0]
          << ", " << hi[1]
          << ", " << hi[2]
          << ");" << newline;
    }
    else if (cl == TGeoEltu::Class()) {
      const TGeoEltu* sh = (const TGeoEltu*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << ", "   << sh->GetA()
          << ", "   << sh->GetB()
          << ", "   << sh->GetDz()
          << ");"   << newline;
    }
    else if (cl == TGeoTrd1::Class()) {
      const TGeoTrd1* sh = (const TGeoTrd1*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << ", "   << sh->GetDx1()
          << ", "   << sh->GetDx2()
          << ", "   << sh->GetDy()
          << ", "   << sh->GetDz()
          << ");"   << newline;
    }
    else if (cl == TGeoTrd2::Class()) {
      const TGeoTrd2* sh = (const TGeoTrd2*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << ", "   << sh->GetDx1()
          << ", "   << sh->GetDx2()
          << ", "   << sh->GetDy1()
          << ", "   << sh->GetDy2()
          << ", "   << sh->GetDz()
          << ");"   << newline;
    }
    else if (cl == TGeoTrap::Class()) {
      const TGeoTrap* sh = (const TGeoTrap*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << ", "   << sh->GetDz() << ", " << sh->GetTheta() << ", " << sh->GetPhi()
          << ", "   << sh->GetH1() << ", " << sh->GetBl1()   << ", " << sh->GetTl1() << ", " << sh->GetAlpha1()
          << ", "   << sh->GetH2() << ", " << sh->GetBl2()   << ", " << sh->GetTl2() << ", " << sh->GetAlpha2()
          << ");"   << newline;
    }
    else if (cl == TGeoHype::Class()) {
      const TGeoHype* sh = (const TGeoHype*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << ", "   << sh->GetRmin() << ", "  << sh->GetRmax() << ", " << sh->GetDz()
          << ", "   << sh->GetStIn() << ", " << sh->GetStOut()
          << ");"   << newline;
    }
    else if (cl == TGeoPgon::Class()) {
      const TGeoPgon* sh = (const TGeoPgon*) shape;
      vector<double> params;
      params.push_back(sh->GetPhi1());
      params.push_back(sh->GetDphi());
      params.push_back(double(sh->GetNedges()));
      params.push_back(double(sh->GetNz()));
      for(int i=0, n=sh->GetNz(); i<n; ++i)  {
        params.push_back(sh->GetZ(i));
        params.push_back(sh->GetRmin(i));
        params.push_back(sh->GetRmax(i));
      }
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(" << &params[0] << ");" << newline;
      log << "shape_" << pvoid << "->SetName(\"" << sh->GetName() << "\");" << newline;
    }
    else if (cl == TGeoPcon::Class()) {
      const TGeoPcon* sh = (const TGeoPcon*) shape;
      vector<double> params;
      params.push_back(sh->GetPhi1());
      params.push_back(sh->GetDphi());
      params.push_back(double(sh->GetNz()));
      for(int i=0, n=sh->GetNz(); i<n; ++i)  {
        params.push_back(sh->GetZ(i));
        params.push_back(sh->GetRmin(i));
        params.push_back(sh->GetRmax(i));
      }
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(" << &params[0] << ");" << newline;
      log << "shape_" << pvoid << "->SetName(\"" << sh->GetName() << "\");" << newline;
    }
    else if (cl == TGeoCone::Class()) {
      const TGeoCone* sh = (const TGeoCone*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName() << "(\"" << sh->GetName() << '"'
          << ", "   << sh->GetRmin1() << ", " << sh->GetRmin2() << ", "
          << ", "   << sh->GetRmax1() << ", " << sh->GetRmax2()
          << ", " << sh->GetDz()
          << ");"   << newline;
    }
    else if (cl == TGeoConeSeg::Class()) {
      const TGeoConeSeg* sh = (const TGeoConeSeg*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName()  << "(\"" << sh->GetName() << '"'
          << ", "   << sh->GetRmin1() << ", " << sh->GetRmin2() << ", "
          << ", "   << sh->GetRmax1() << ", " << sh->GetRmax2()
          << ", "   << sh->GetDz()
          << ", "   << sh->GetPhi1()  << ", " << sh->GetPhi2()
          << ");"   << newline;
    }
    else if (cl == TGeoParaboloid::Class()) {
      const TGeoParaboloid* sh = (const TGeoParaboloid*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName()  << "(\"" << sh->GetName() << '"'
          << ", "   << sh->GetRlo() << ", " << sh->GetRhi() << ", " << sh->GetDz() << ");" << newline;
    }
    else if (cl == TGeoSphere::Class()) {
      const TGeoSphere* sh = (const TGeoSphere*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName()  << "(\"" << sh->GetName() << '"'
          << ", "   << sh->GetRmin() << ", " << sh->GetRmax()
          << ", "   << sh->GetPhi1()  << ", " << sh->GetPhi2()
          << ", "   << sh->GetTheta1()  << ", " << sh->GetTheta2()
          << ");" << newline;
    }
    else if (cl == TGeoTorus::Class()) {
      const TGeoTorus* sh = (const TGeoTorus*) shape;
      log << cl->GetName() << "* shape_" << pvoid << " = "
          << "new " << cl->GetName()  << "(\"" << sh->GetName() << '"'
          << ", "   << sh->GetRmin()  << ", " << sh->GetRmax() << ", " << sh->GetR()
          << ", "   << sh->GetPhi1()  << ", " << sh->GetDphi()
          << ");" << newline;
    }
    else if (cl == TGeoArb8::Class()) {
      TGeoArb8* sh = (TGeoArb8*) shape;
      const Double_t* v = sh->GetVertices();
      log << cl->GetName() << "* shape_" << pvoid << " = 0;" << newline
          << "{" << newline
          << "\tstd::vector<double> vertices_" << pvoid << ";" << newline;
      for(int i=0; i<8; ++i) {
        log << "\tvertices_" << pvoid << ".push_back(" << *v << ");" << newline; ++v;
        log << "\tvertices_" << pvoid << ".push_back(" << *v << ");" << newline; ++v;
      }
      log << "\tshape_" << pvoid << " = new " << cl->GetName()
          << "(\"" << sh->GetName() << '"'
          << ", "  << sh->GetDz()  << ", &vertices_" << pvoid << "[0]);" << newline
          << "}" << newline;
    }
    else if (cl == TGeoXtru::Class()) {
      Solid sol(shape);
      const TGeoXtru* sh = (const TGeoXtru*) shape;
      std::vector<double> pars = sol.dimensions();
      log << cl->GetName() << "* shape_" << pvoid << " = 0;" << newline
          << "{" << newline
          << "\tstd::vector<double> param_" << pvoid << ";" << newline;
      for( auto p : pars)
        log << "\tparam_" << pvoid << ".push_back(" << p << ");" << newline;
      log << "\tshape_" << pvoid << " = new " << cl->GetName()
          << "( &param_" << pvoid << "[0]);" << newline;
      log << "shape_" << pvoid << "->SetName(\"" << sh->GetName() << "\");" << newline;
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
      log << "TGeoCompositeShape* shape_" << pvoid_t(sh) << " = 0;" << newline;
      log << "{" << newline;
      if (oper == TGeoBoolNode::kGeoSubtraction)
        log << "\t TGeoSubtraction* boolean = new TGeoSubtraction";
      else if (oper == TGeoBoolNode::kGeoUnion)
        log << "\t TGeoUnion* boolean = new TGeoUnion";
      else if (oper == TGeoBoolNode::kGeoIntersection)
        log << "\t TGeoIntersection* boolean = new TGeoIntersection";
      log << "(shape_"   << pvoid_t(left) << ", shape_" << pvoid_t(right) << ", 0, ";
      log << "matrix_"   << pvoid_t(boolean->GetRightMatrix()) << ");" << newline;
      log << "\t shape_" << pvoid_t(sh)
          << " = new TGeoCompositeShape(\"" << sh->GetName() << "\", boolean);" << newline;
      log << "}" << newline;
    }
    else if (shape->IsA() == TGeoShapeAssembly::Class()) {
      //const TGeoShapeAssembly* sh = (const TGeoShapeAssembly*)shape;
      // Nothing to do here: All handled in the handling of TGeoVolumeAssembly
    }
    else   {
      except("CxxRootGenerator","++ Unknown shape transformation request: %s", shape->IsA()->GetName());
    }
    return log;
  }

}

static long generate_cxx(Detector& description, int argc, char** argv) {
  string output;
  Actor actor;

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
    else if ( c == 'm' )
      actor.dump_mat = true;
    else   {
      cout <<
        "Usage: -plugin DD4hep_CxxRootGenerator -arg [-arg]                                  \n"
        "     -output   <string> Set output file for generated code. Default: stdout         \n"
        "     -visualization     Also dump visualization attributes of volumes               \n"
        "     -materials         Also dump proper materials. Default to IRON                 \n"
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
  actor.handleTrailer(*os);
  out.reset();
  return 1;
}

DECLARE_APPLY(DD4hep_TGeoCxxGenerator,generate_cxx)
