// $Id: display.cpp 590 2013-06-03 17:02:43Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Generic ROOT based geometry display program
// 
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/Factories.h"
#include "DD4hep/LCDD.h"
#include "DDRec/SurfaceManager.h"


#include "run_plugin.h"
#include "TRint.h"
#include "TEveGeoNode.h"
//#include "TEveElement.h"
#include "TGLViewer.h"
#include "TGeoManager.h"
//#include "TGLUtil.h"
#include "TGLClip.h"
//#include "TSysEvtHandler.h"
#include "TMap.h"
#include "TObjString.h"

#define private public
#include "TEveManager.h"

#include "TEveStraightLineSet.h"
#include "TRandom.h"
#include "TGeoShape.h"

using namespace DD4hep ;
using namespace DDRec ;
using namespace Geometry ;
using namespace DDSurfaces ;

//=====================================================================================
TEveStraightLineSet* drawSurfaceVectors() {

  TEveStraightLineSet* ls = new TEveStraightLineSet("SurfaceVectors");

  LCDD& lcdd = LCDD::getInstance();

  DetElement world = lcdd.world() ;

  // create a list of all surfaces in the detector:
  SurfaceManager surfMan(  world ) ;

  const SurfaceList& sL = surfMan.surfaceList() ;

  for( SurfaceList::const_iterator it = sL.begin() ; it != sL.end() ; ++it ){

    ISurface* surf = *it ;
    const DDSurfaces::Vector3D& u = surf->u() ;
    const DDSurfaces::Vector3D& v = surf->v() ;
    const DDSurfaces::Vector3D& n = surf->normal() ;
    const DDSurfaces::Vector3D& o = surf->origin() ;

    DDSurfaces::Vector3D ou = o + u ;
    DDSurfaces::Vector3D ov = o + v ;
    DDSurfaces::Vector3D on = o + n ;
 
    ls->AddLine( o.x(), o.y(), o.z(), ou.x() , ou.y() , ou.z()  ) ;
    ls->AddLine( o.x(), o.y(), o.z(), ov.x() , ov.y() , ov.z()  ) ;
    ls->AddLine( o.x(), o.y(), o.z(), on.x() , on.y() , on.z()  ) ;

    ls->AddMarker(  o.x(), o.y(), o.z() );
  }
  ls->SetLineColor( kGreen ) ;
  ls->SetMarkerColor( kBlue ) ;
  ls->SetMarkerSize(1);
  ls->SetMarkerStyle(4);
  
  gEve->AddElement(ls);

  return ls;
}
//=====================================================================================
TEveStraightLineSet* drawSurfaces() {

  TEveStraightLineSet* ls = new TEveStraightLineSet("Surfaces");

  LCDD& lcdd = LCDD::getInstance();

  DetElement world = lcdd.world() ;

  // create a list of all surfaces in the detector:
  SurfaceManager surfMan(  world ) ;

  const SurfaceList& sL = surfMan.surfaceList() ;

  for( SurfaceList::const_iterator it = sL.begin() ; it != sL.end() ; ++it ){

    Surface* surf = *it ;

    const std::vector< Vector3D > vert = surf->getVertices() ;

    if( vert.empty() )
      std::cout << " **** drawSurfaces() : empty vertices for surface " << *surf << std::endl ;

    unsigned nV = vert.size() ;

    for( unsigned i=0 ; i<nV ; ++i){

      unsigned j = ( i < nV-1 ?  i+1 : 0 ) ;

      ls->AddLine( vert[i].x(), vert[i].y(), vert[i].z(), vert[j].x() , vert[j].y() , vert[j].z()  ) ;
    }
    

    // Volume vol = surf->volume() ; 
    // const TGeoShape* shape = vol->GetShape() ;

    // // get extend of surface volume along u and v
    // VolSurface volSurf = surf->volSurface() ;

    // const DDSurfaces::Vector3D& lu = volSurf.u() ;
    // const DDSurfaces::Vector3D& lv = volSurf.v() ;
    // const DDSurfaces::Vector3D& ln = volSurf.normal() ;
    // const DDSurfaces::Vector3D& lo = volSurf.origin() ;


    // // TGeoShape::DistFromInside(Double_t *point[3], Double_t *dir[3],
    // //                               Int_t iact, Double_t step, Double_t *safe)
 
    // double safe = 0. ; 
    // double lup = shape->DistFromInside( lo,  lu, 2, 0.1 , &safe ) ;
    // double lun = shape->DistFromInside( lo, -lu, 2, 0.1 , &safe ) ;
    // double lvp = shape->DistFromInside( lo,  lv, 2, 0.1 , &safe ) ;
    // double lvn = shape->DistFromInside( lo, -lv, 2, 0.1 , &safe ) ;
    
    
    // // std::cout << "*** lu : " << lu << " lv : " << lv << " ln : " << ln << " lo : " << lo << std::endl ; 
    // // std::cout << "*** lup : " << lup << " lun : " << lun << " lvp : " << lvp << " lvn : " << lvn << std::endl ; 

    // const DDSurfaces::Vector3D& u = surf->u() ;
    // const DDSurfaces::Vector3D& v = surf->v() ;
    // const DDSurfaces::Vector3D& n = surf->normal() ;
    // const DDSurfaces::Vector3D& o = surf->origin() ;
    

    // if( surf->type().isPlane() ) {
      
    //   DDSurfaces::Vector3D c0 = o + lup * u  + lvp * v ;
    //   DDSurfaces::Vector3D c1 = o + lup * u  - lvn * v ;
    //   DDSurfaces::Vector3D c2 = o - lun * u  - lvn * v ;
    //   DDSurfaces::Vector3D c3 = o - lun * u  + lvn * v ;
      
      
    //   ls->AddLine( c0.x(), c0.y(), c0.z(), c1.x() , c1.y() , c1.z()  ) ;
    //   ls->AddLine( c1.x(), c1.y(), c1.z(), c2.x() , c2.y() , c2.z()  ) ;
    //   ls->AddLine( c2.x(), c2.y(), c2.z(), c3.x() , c3.y() , c3.z()  ) ;
    //   ls->AddLine( c3.x(), c3.y(), c3.z(), c0.x() , c0.y() , c0.z()  ) ;
      
    //   //    ls->AddMarker(  o.x(), o.y(), o.z() );
    // }

    ls->SetLineColor( kRed ) ;
    ls->SetMarkerColor( kRed ) ;
    ls->SetMarkerSize(.1);
    ls->SetMarkerStyle(4);

  } // don't know what to do ...

  gEve->AddElement(ls);

  return ls;
}
//=====================================================================================


static long teve_display(LCDD& lcdd, int /* argc */, char** /* argv */) {
  TGeoManager* mgr = &lcdd.manager();
  TEveManager::Create();
  gEve->fGeometries->Add(new TObjString("DefaultGeometry"),mgr);
  TEveGeoTopNode* tn = new TEveGeoTopNode(mgr, mgr->GetTopNode());
  tn->SetVisLevel(4);


  // // ---- try to set transparency - does not seem to work ...
  // TGeoNode* node1 = gGeoManager->GetTopNode();
  // int k_nodes = node1->GetNdaughters();
  // for(int i=0; i<k_nodes; i++) {
  //   TGeoNode * CurrentNode = node1->GetDaughter(i);
  //   CurrentNode->GetMedium()->GetMaterial()->SetTransparency(80);
  //   // TEveGeoNode CurrentNode( node1->GetDaughter(i) ) ;
  //   // CurrentNode.SetMainTransparency( 80 ) ;
  // }
  // this code seems to have no effect either ...
  tn->CSCApplyMainTransparencyToAllChildren() ;
  tn->SetMainTransparency( 80 ) ;


  drawSurfaceVectors() ;
  drawSurfaces() ;

  gEve->AddGlobalElement(tn);
  gEve->FullRedraw3D(kTRUE);


  // EClipType not exported to CINT (see TGLUtil.h):
  // 0 - no clip, 1 - clip plane, 2 - clip box
  TGLViewer *v = gEve->GetDefaultGLViewer();

  //  v->GetClipSet()->SetClipType(TGLClip::kClipPlane);
  //  v->ColorSet().Background().SetColor(kMagenta+4);
  v->ColorSet().Background().SetColor(kWhite);

  v->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
  v->RefreshPadEditor(v);
  //  v->CurrentCamera().RotateRad(-1.2, 0.5);
  v->DoDraw();
  return 1;


}
DECLARE_APPLY(DD4hepTEveDisplay,teve_display)

//______________________________________________________________________________
int main(int argc,char** argv)  {
  return main_default("DD4hepTEveDisplay",argc,argv);
}

