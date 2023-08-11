#include "TROOT.h"
#include "TColor.h"
#include "TGeoCone.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TGeoElement.h"
#include "TGeoMaterial.h"

#include <cstdio>


class Test    {
public:
  TGeoManager*   m_manager = nullptr;
  TGeoMaterial*  m_vacuum  = nullptr;
  TGeoMedium*    m_air     = nullptr;

  TGeoBBox*      m_worldShape   = nullptr;
  TGeoVolume*    m_world = nullptr;
  
  struct Cone   {
    TGeoMaterial* material = nullptr;
    TGeoMedium*  medium = nullptr;
    TGeoConeSeg* shape = nullptr;
    TGeoVolume* volume = nullptr;
    TGeoNode* place = nullptr;
    void set_color(const char* typ, Float_t alpha, Float_t red, Float_t green, Float_t blue)   {
      Int_t col = TColor::GetColor(red, green, blue);
      TColor* color = gROOT->GetColor(col);
      if ( !color )    {
        ::printf("+++ %s Failed to allocate Color: r:%02X g:%02X b:%02X\n",
                 typ, int(red*255.), int(green*255.), int(blue*255));
      }
      char text[64];
      ::snprintf(text, sizeof(text), "%p", this);
      TColor* colortr = new TColor(gROOT->GetListOfColors()->GetLast()+1,
                                   color->GetRed(), color->GetGreen(), color->GetBlue(), text, alpha);
      //colortr->SetAlpha(alpha);

      ::printf("Plot cone %s (col: %d) with transparency %8.3f r:%02X g:%02X b:%02X\n",
               typ, col, (1.0-alpha)*100, int(red*255.), int(green*255.), int(blue*255));

      /// Propagate values to the volume:
      int col_num    = color->GetNumber();
      int col_tr_num = colortr->GetNumber();
      volume->SetVisibility(kTRUE);
      volume->SetVisContainers(kTRUE);
      volume->SetVisDaughters(kTRUE);
      volume->SetLineWidth(10);
      volume->SetFillStyle(1001);
      volume->ResetTransparency((1.0-alpha)*100);
      volume->SetLineStyle(1);

      /// Set line color
      volume->SetLineColor(col_num);
      /// Set fill color
      volume->SetFillColor(col_tr_num);
    }
  } cone1, cone2;
  
public:
  Test()   {
  }
  virtual ~Test()   {
  }
  void build(int version);
};

void Test::build(int version)    {
  m_manager = new TGeoManager("geom","Tube test");
  m_vacuum = new TGeoMaterial("vacuum", 0, 0, 0);
  m_air = new TGeoMedium("Vacuum", 0, m_vacuum);

  m_worldShape = new TGeoBBox("WorldBox", 100, 100, 100);
  m_world = new TGeoVolume("World", m_worldShape, m_air);

  cone1.material = new TGeoMaterial("Fe1", 55.845, 26, 7.87);
  cone1.medium   = new TGeoMedium("Iron1", 1, cone1.material);
  cone1.shape    = new TGeoConeSeg("Cone1Shape", 40, 0e0, 20e0, 0e0, 20e0, 0e0, 360e0);
  cone1.volume   = new TGeoVolume("Cone1", cone1.shape, cone1.medium);
  cone1.set_color("Cone1",
                  version ? 0.1 : 0.9,
                  version ? 1e0 : 0e0,
                  0e0,
                  version ? 0e0 : 1e0);
  m_world->AddNode(cone1.volume, 1, new TGeoTranslation(-30, -30, 0));

  cone2.material = version ? new TGeoMaterial("Fe2", 55.845, 26, 7.87) : cone1.material;
  cone2.medium   = version ? new TGeoMedium("Iron2", 1, cone2.material) : cone1.medium;
  cone2.shape    = new TGeoConeSeg("Cone2Shape", 40, 0e0, 20e0, 0e0, 20e0, 0e0, 360e0);
  cone2.volume   = new TGeoVolume("Cone2", cone2.shape, cone2.medium);
  cone2.set_color("Cone2",
                  version ? 0.9 : 0.1,
                  version ? 0e0 : 0.9,
                  0e0,
                  version ? 0.9 : 0e0);
  m_world->AddNode(cone2.volume, 1, new TGeoTranslation(30, 30, 0));

  m_manager->SetTopVolume(m_world);
  m_manager->SetTopVisible(0);

  m_manager->GetTopVolume()->Draw("ogl");
}


Test* visTest(int version=0)   {
  Test* obj = new Test();
  obj->build(version);
  return obj;
}
