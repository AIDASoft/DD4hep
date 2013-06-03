// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "TGeoTube.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace DD4hep { namespace Geometry {
  struct BeamCal01Data : public DetElement::Object  {
    typedef Position Dimension;

    vector<pair<Material,pair<double,double> > > rpc_layers;
    pair<double,double> rpc_last_layer;

    double dSensor;
    double dAirgap;
    double dElboard;
    double dElectrMet;
    double dAbsorber;
    double dGraphite;

    double dLayer;
    double inner_r;
    double outer_r;
    double segmentation_r;
    int    nLayers;
    double nWafers;
    double start_z;

    Material materialAir;
    Material materialTungsten;
    Material materialKapton;
    Material materialGold;
    Material materialDiamond;
    Material materialSilicon;
    Material materialGraphite;

    struct SubComponent : public DetElement  {
      SensitiveDetector sensDet;
      /// Helper function to allow assignment
      DetElement& operator=(const DetElement& d)  { return this->DetElement::operator=(d); }
    } m_barrel, m_endcap;

    LCDD*       lcdd;
    std::string name;
    DetElement  self;
  };

  struct BeamCal01 : public BeamCal01Data  {
    /// Standard constructor
    BeamCal01() : BeamCal01Data() {}
    /// Detector construction function
    DetElement construct(LCDD& lcdd, xml_det_t e, SensitiveDetector sens_det);
    /// 
    static DetElement instance(LCDD& lcdd, xml_det_t e, SensitiveDetector sd) { 
      BeamCal01* p = new BeamCal01(); 
      p->self.assign(p,e.nameStr(),e.typeStr());
      return p->construct(lcdd,e,sd);
    }
  };
}}

static const double TwoPI = 2*M_PI;
static const double mrad = 1e-3;
#define BEAMCAL_MAX_LAYERS 1023

inline double split_segm(double totLength, double initSegm)   
{  return totLength/(double(int(totLength/initSegm)+1));      }

inline int split_n(double totLength, double initSegm)   
{  return int(ceil(totLength/initSegm));                      }

/// Detector construction function
DetElement BeamCal01::construct(LCDD& l, xml_det_t x_det, SensitiveDetector sens_det)  {
  int det_id = x_det.id();
  lcdd     = &l;
  name     = x_det.nameStr();
  self._data().id = det_id;
  Assembly     assembly(name);
  PlacedVolume pv;

  // Global Parameters
  // inner radius of the calorimeter. 10 microns has been added to ensure no overlap with the beampipe due to numerical instability
  inner_r            = lcdd->constant<double>("BCal_rInner") + 0.01;
  outer_r            = lcdd->constant<double>("BCal_rOuter");
  dAbsorber          = lcdd->constant<double>("BCal_dAbsorber");
  dGraphite          = lcdd->constant<double>("BCal_dGraphite");
  segmentation_r     = lcdd->constant<double>("BCal_rSegmentation");
  nLayers            = lcdd->constant<int>   ("BCal_nLayers");
  nWafers            = lcdd->constant<double>("BCal_nWafers");
  start_z            = lcdd->constant<double>("LHcal_zend") + lcdd->constant<double>("LHcal_BCal_clearance");

  double r_tube             = lcdd->constant<double>("BCal_TubeIncomingRadius");
  int    pairmonitorOnOff   = lcdd->constant<int>   ("BCal_PairMonitor");
  double dPhi               = lcdd->constant<double>("BCal_SpanningPhi");
  double x_angle            = lcdd->constant<double>("ILC_Main_Crossing_Angle"); //the xAngle
  double sPhi               = 2*M_PI - dPhi/2.;
  // This is always true, even if crossing anlge is 0
  double tiltForw           = x_angle/2.;
  double tiltBack           = M_PI - x_angle/2.;

  if (nLayers > BEAMCAL_MAX_LAYERS)   {
    throw runtime_error("You are using too many Layers in BeamCal, maximum is 1023 (one is reserved for PairMonitor)!");
  }

  for(xml_coll_t c(x_det.child(_U(params)),_U(param)); c; ++c)  {
    string nam = c.attr<string>(_U(name));
    if ( nam == "dSensor"    ) dSensor    = c.attr<double>(_U(value)); // Diamond thickness
    if ( nam == "dAirgap"    ) dAirgap    = c.attr<double>(_U(value)); // Layer gap (air)
    if ( nam == "dElboard"   ) dElboard   = c.attr<double>(_U(value)); // PCB (Kapton)
    if ( nam == "dElectrMet" ) dElectrMet = c.attr<double>(_U(value)); // Sensor Electrode Metalisation (gold)
  }
  dLayer = dAbsorber+dSensor+dElectrMet+dElboard+dAirgap; //layer thickness
  double length = dLayer*nLayers; //total length of the calorimeter

  // create and overall envelope volume which will include the pair monitor + 3mm clearance + graphite shield + 6mm clearance + calorimeter    
  double envVolumeLength  = dSensor + 3 + dGraphite + 6 + length ;
  double PMLength         = dSensor + 3 + dGraphite;
  double BCLength         = length;
  double GlobalStart      = start_z - (PMLength +6);
  double BCStart          = start_z; //This is were BeamCal starts!
  double envVolumeCenterZ = GlobalStart + envVolumeLength/2;
  double PMVolumeCenterZ  = GlobalStart + PMLength/2;
  //
  // Dead Area Calculations
  double BPMaxRCalc = RotateY(Position(-r_tube,r_tube,BCStart+BCLength),-x_angle).Rho()+0.1;

  // inline functions from BeamCalSD01.hh
  double dR  = split_segm((outer_r-inner_r),segmentation_r); //distance between 2 rings
  int nRs = split_n((outer_r-inner_r),segmentation_r);    //number of rings
  //DA is for DEAD AREA
  double DAStart=0, DArinner=-1;

  if ( x_angle >= 14*mrad )  {
    for(int j = 0; j < nRs; j++){
      double r = inner_r + j*dR;
      if(r >= BPMaxRCalc){
        DArinner = r; //inner radius of the DA
        DAStart  = j; //number where rings are complete
        break;
      }
    }
  }//if the crossing angle is above 14mrad
  
  //materials to be used
  materialAir      = lcdd->air();
  materialTungsten = lcdd->material("tungsten_19.3gccm");
  materialKapton   = lcdd->material("Kapton");
  materialGold     = lcdd->material("Gold");
  materialDiamond  = lcdd->material("Diamond");
  materialSilicon  = lcdd->material("Silicon");
  materialGraphite = lcdd->material("Graphite"); 


  //visualisation attributes
  VisAttr caloVisAttributes      = lcdd->visAttributes("BeamCalCaloVis");
  VisAttr sensVisAttributes      = lcdd->visAttributes("BeamCalSensorVis");
  VisAttr absVisAttributes       = lcdd->visAttributes("BeamCalAbsorberVis");
  VisAttr electrodeVisAttributes = lcdd->visAttributes("BeamCalElectrodeVis");
  VisAttr pcbVisAttributes       = lcdd->visAttributes("BeamCalPcbVis");
  VisAttr grVisAttributes        = lcdd->visAttributes("BeamCalGraphiteVis");

  double pairsMonitorZ = GlobalStart + dSensor/2.0 ;

#if 0
  //================================= Sensitive Detector 
  BeamCalSD01*  bcSD =  new BeamCalSD01("BeamCal", Rinner,Router,crossingAngle,GlobalStart,sPhi,dPhi,nWafers,DAStart,dLayer,nLayers,dSensor,dAbsorber,RSegmentation,pairsMonitorZ,envVolumeLength,envVolumeCenterZ);
  RegisterSensitiveDetector(bcSD);
#endif

  Position pos, crossing_pos(std::sin(-x_angle),0,0);
  Rotation rot, crossing_rot(0,0,-x_angle);
  SubtractionSolid solid;
  Volume vol;

  //-------------------------------------------
  // Build the PairMonitor and Graphite Layer
  //-------------------------------------------

  // The Space for the incoming beampipe
  Tube CutOutGr(0, r_tube+0.2, envVolumeLength);
  Tube CutOutGr2(0, r_tube+0.3, envVolumeLength);

  // The PairMonitor will be placed in its own volume together with the graphite 6 mm before BeamCal
  Tube monitorTube(inner_r, outer_r, PMLength/2, sPhi, TwoPI+sPhi);
  // Must use full crossing angle, because we are centering beamcal on outgoing beamline
  SubtractionSolid monitorSolid(monitorTube,CutOutGr,Position((GlobalStart+PMLength/2)*crossing_pos.X(),0,0),crossing_rot);
  Volume           monitorVol(name+"_pair_monitor",monitorSolid,materialAir);
  monitorVol.setVisAttributes(caloVisAttributes);

  if ( pairmonitorOnOff == 1 )   {
    pos = Position((dSensor+PMLength/2)*crossing_pos.X(),0,0);
    solid = SubtractionSolid(Tube(inner_r,outer_r,dSensor/2,sPhi,TwoPI+sPhi),CutOutGr2,pos,crossing_rot);
    vol = Volume(name+"_pair_monitor2",solid, materialSilicon);
    vol.setVisAttributes(sensVisAttributes);
    vol.setSensitiveDetector(sens_det);
    pv = monitorVol.placeVolume(vol,Position(0,0,(-PMLength+dSensor)/2));
    pv.addPhysVolID("layer",nLayers+1);
  }
  // the graphite layer in the forward region
  if( dGraphite > 0 )   {
    pos = Position((GlobalStart+PMLength-dGraphite/2)*crossing_pos.X(),0,0);
    solid = SubtractionSolid(Tube(inner_r,outer_r,dGraphite/2,sPhi,TwoPI+sPhi),CutOutGr2,pos,crossing_rot);
    vol = Volume(name+"_pair_monitor2",solid, materialGraphite);
    vol.setVisAttributes(grVisAttributes);
    monitorVol.placeVolume(vol,Position(0,0,(-PMLength+dGraphite)/2));
  }
  if(dGraphite > 0 || pairmonitorOnOff == 1) {
    pos = Position(0,0,PMVolumeCenterZ);
    pv = assembly.placeVolume(monitorVol,Rotation(0,0,tiltForw),RotateY(pos,tiltForw));
    pv.addPhysVolID("side",1);
    pos = Position(0,0,PMVolumeCenterZ);
    pv = assembly.placeVolume(monitorVol,Rotation(M_PI,0,tiltBack),RotateY(pos,tiltBack));
    pv.addPhysVolID("side",2);
  }

  //-------------------------------------------
  //build the calorimeter in the forward region
  //-------------------------------------------

  //Check that the spanning angle is not too large and overlaps with the cutout for the incoming beampipe
  double bpminrcalc = (-1*RotateY(Position(0,0,BCStart),-x_angle)).X();
  double cutoutangle = std::atan(r_tube/bpminrcalc);

  if( 2*M_PI-2*cutoutangle < dPhi) {
    cout << "ERROR: Spanning Angle too large! Maximum possible angle is " << 2*(M_PI-cutoutangle) << " rad" << endl;
    throw runtime_error("Spanning Angle is too large!");
  }

  //G4Transform3D  BCtransform(G4RotationMatrix().RotateY(-x_angle), G4ThreeVector(G4ThreeVector(0, 0, BCStart+BCLength/2.).RotateY(-x_angle)[0],0,0));

  //Don't need the space for the airgap in BeamCalLayer, because we don't place anything there! Taking Tungsten out of beamCalLayer
  double dLayerBC = (dLayer-dAirgap-dAbsorber)/2;
  Tube   layerTube(inner_r-1, DArinner, dLayerBC,TwoPI-sPhi,TwoPI-dPhi);
  solid = SubtractionSolid(Tube(inner_r,outer_r,dLayerBC,sPhi,TwoPI+sPhi),layerTube);
  Volume layerVol(name+"_layer",solid,lcdd->air());
  layerVol.setVisAttributes(lcdd->invisible());
  
  // Sensor-->nLayers layers of diamond + the Pairs Monitor in front of the graphite block
  solid = SubtractionSolid(Tube(inner_r,outer_r,dSensor/2,sPhi,TwoPI+sPhi),layerTube);
  vol = Volume(name+"_sensor",solid,materialDiamond);
  vol.setVisAttributes(sensVisAttributes);
  vol.setSensitiveDetector(sens_det);
  pv = layerVol.placeVolume(vol,Position(0,0,-dLayerBC+dSensor/2));
  pv.addPhysVolID("sensor",nLayers+1);

  // Electrode metalisation of gold
  solid = SubtractionSolid(Tube(inner_r,outer_r,dElectrMet/2,sPhi,TwoPI+sPhi),layerTube);
  vol = Volume(name+"_electrode",solid,materialGold);
  vol.setVisAttributes(electrodeVisAttributes);
  layerVol.placeVolume(vol,Position(0,0,-dLayerBC+dSensor+dElectrMet/2));
  
  // KaptonBoard
  solid = SubtractionSolid(Tube(inner_r,outer_r,dElboard/2,sPhi,TwoPI+sPhi),layerTube);
  vol = Volume(name+"_pcb",solid,materialKapton);
  vol.setVisAttributes(pcbVisAttributes);
  layerVol.placeVolume(vol,Position(0,0,-dLayerBC+dSensor+dElectrMet+dElboard/2));

  // TungstenLayer
  Tube absorberTube(inner_r,outer_r,dAbsorber/2,sPhi,TwoPI);

  // Calorimeter volume - a tube filled with air that contains all the layers
  pos = Position((BCStart+BCLength/2)*crossing_pos.X(),0,0);
  solid = SubtractionSolid(Tube(inner_r,outer_r,BCLength/2,sPhi,TwoPI+sPhi),CutOutGr,pos,crossing_rot);
  Volume caloVol(name+"_calo", solid, materialAir);
  caloVol.setVisAttributes(caloVisAttributes);

  // Place the BeamCalLayer and the absorber plates in the CaloVolumeLog
  for(int i = 1; i <= nLayers; i++) {
    char index[5]; 
    ::snprintf(index,sizeof(index),"%i",i);
    // Position of the hole depends on layer position!
    Position abs_pos((BCStart + dAbsorber/2. + dLayer*double(i-1))*crossing_pos.X(),0,0);
    solid = SubtractionSolid(absorberTube,CutOutGr2,abs_pos,crossing_rot);
    vol = Volume(name+_toString(i,"_absober%d"),solid,materialTungsten);
    vol.setVisAttributes(absVisAttributes);  

    // Place TunsgtenLayer as daughter to CaloVolumeLog
    pos = Position(0,0,-BCLength/2+dAbsorber/2+dLayer*double(i-1));
    caloVol.placeVolume(vol,pos).addPhysVolID("absorber_layer",i);
    pos = Position(0,0,-BCLength/2+dAbsorber+dLayerBC+dLayer*double(i-1));
    caloVol.placeVolume(layerVol,pos).addPhysVolID("layer",i);
  }
  
  // Place the Forward calorimeter
  // use copy number to decide which side we are on in SD 1 for forward, 2 for backward
  pos = RotateY(Position(0,0,BCStart+BCLength/2),tiltForw);
  assembly.placeVolume(caloVol,pos,Rotation(0,0,tiltForw)).addPhysVolID("side",0);
  pos = RotateY(Position(0,0,BCStart+BCLength/2),tiltBack);
  assembly.placeVolume(caloVol,pos,Rotation(M_PI,0,tiltBack)).addPhysVolID("side",1);

  // now place the full assembly
  assembly.setVisAttributes(lcdd->visAttributes(x_det.visStr()));
  pv = lcdd->pickMotherVolume(self).placeVolume(assembly);
  pv.addPhysVolID("system",det_id);
  self.setPlacement(pv);
  return self;
}

static Ref_t create_detector(LCDD& lcdd, xml_h element, SensitiveDetector sens)  {
  return BeamCal01::instance(lcdd,element,sens);
}

DECLARE_DETELEMENT(Tesla_BeamCal01,create_detector);
