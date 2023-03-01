//----------------------------------
//  pfRICH: Proximity Focusing RICH
//  Author: C. Dilks
//----------------------------------

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/OpticalSurfaces.h"
#include "DD4hep/Printout.h"
#include "DDRec/DetectorData.h"
#include <XML/Helper.h>

using namespace dd4hep;
using namespace dd4hep::rec;

// create the detector
static Ref_t createDetector(Detector& desc, xml::Handle_t handle, SensitiveDetector sens)
{
  xml::DetElement       detElem = handle;
  std::string           detName = detElem.nameStr();
  int                   detID   = detElem.id();
  xml::Component        dims    = detElem.dimensions();
  OpticalSurfaceManager surfMgr = desc.surfaceManager();
  DetElement            det(detName, detID);

  // constant attributes -----------------------------------------------------------
  // - vessel
  double vesselLength       = dims.attr<double>(_Unicode(length));
  double vesselZmin         = dims.attr<double>(_Unicode(zmin));
  double vesselRmin0        = dims.attr<double>(_Unicode(rmin0));
  double vesselRmin1        = dims.attr<double>(_Unicode(rmin1));
  double vesselRmax0        = dims.attr<double>(_Unicode(rmax0));
  double vesselRmax1        = dims.attr<double>(_Unicode(rmax1));
  double wallThickness      = dims.attr<double>(_Unicode(wall_thickness));
  double windowThickness    = dims.attr<double>(_Unicode(window_thickness));
  auto   vesselMat          = desc.material(detElem.attr<std::string>(_Unicode(material)));
  auto   gasvolMat          = desc.material(detElem.attr<std::string>(_Unicode(gas)));
  auto   vesselVis          = desc.visAttributes(detElem.attr<std::string>(_Unicode(vis_vessel)));
  auto   gasvolVis          = desc.visAttributes(detElem.attr<std::string>(_Unicode(vis_gas)));
  // - radiator (applies to aerogel and filter)
  auto   radiatorElem       = detElem.child(_Unicode(radiator));
  double radiatorRmin       = radiatorElem.attr<double>(_Unicode(rmin));
  double radiatorRmax       = radiatorElem.attr<double>(_Unicode(rmax));
  double radiatorFrontplane = radiatorElem.attr<double>(_Unicode(frontplane));
  // - aerogel
  auto   aerogelElem        = radiatorElem.child(_Unicode(aerogel));
  auto   aerogelMat         = desc.material(aerogelElem.attr<std::string>(_Unicode(material)));
  auto   aerogelVis         = desc.visAttributes(aerogelElem.attr<std::string>(_Unicode(vis)));
  double aerogelThickness   = aerogelElem.attr<double>(_Unicode(thickness));
  // - filter
  auto   filterElem         = radiatorElem.child(_Unicode(filter));
  auto   filterMat          = desc.material(filterElem.attr<std::string>(_Unicode(material)));
  auto   filterVis          = desc.visAttributes(filterElem.attr<std::string>(_Unicode(vis)));
  double filterThickness    = filterElem.attr<double>(_Unicode(thickness));
  // - sensor module
  auto   sensorElem         = detElem.child(_Unicode(sensors)).child(_Unicode(module));
  auto   sensorMat          = desc.material(sensorElem.attr<std::string>(_Unicode(material)));
  auto   sensorVis          = desc.visAttributes(sensorElem.attr<std::string>(_Unicode(vis)));
  auto   sensorSurf         = surfMgr.opticalSurface(sensorElem.attr<std::string>(_Unicode(surface)));
  double sensorSide         = sensorElem.attr<double>(_Unicode(side));
  double sensorGap          = sensorElem.attr<double>(_Unicode(gap));
  double sensorThickness    = sensorElem.attr<double>(_Unicode(thickness));
  // - sensor plane
  auto   sensorPlaneElem    = detElem.child(_Unicode(sensors)).child(_Unicode(plane));
  double sensorPlaneDist    = sensorPlaneElem.attr<double>(_Unicode(sensordist));
  double sensorPlaneRmin    = sensorPlaneElem.attr<double>(_Unicode(rmin));
  double sensorPlaneRmax    = sensorPlaneElem.attr<double>(_Unicode(rmax));

  // BUILD VESSEL //////////////////////////////////////
  /* - `vessel`: aluminum enclosure, the mother volume of the pfRICH
   * - `gasvol`: gas volume, which fills `vessel`; all other volumes defined below
   *   are children of `gasvol`
   */

  // tank solids
  double boreDelta = vesselRmin1 - vesselRmin0;
  Cone vesselSolid(
      vesselLength / 2.0,
      vesselRmin1,
      vesselRmax1,
      vesselRmin0,
      vesselRmax0
      );
  Cone gasvolSolid(
      vesselLength / 2.0 - windowThickness,
      vesselRmin1 + wallThickness,
      vesselRmax1 - wallThickness,
      vesselRmin0 + wallThickness,
      vesselRmax0 - wallThickness
      );

  // volumes
  Volume vesselVol(detName,        vesselSolid, vesselMat);
  Volume gasvolVol(detName+"_gas", gasvolSolid, gasvolMat);
  vesselVol.setVisAttributes(vesselVis);
  gasvolVol.setVisAttributes(gasvolVis);

  // reference positions
  /* - the vessel is created such that the center of the cylindrical tank volume
   *   coincides with the origin; this is called the "origin position" of the vessel
   * - when the vessel (and its children volumes) is placed, it is translated in
   *   the z-direction to be in the proper full-detector integration location
   * - these reference positions are for the frontplane and backplane of the vessel,
   *   with respect to the vessel origin position
   */
  auto originFront = Position(0., 0., vesselLength / 2.0);

  // sensitive detector type
  sens.setType("tracker");

  // BUILD RADIATOR //////////////////////////////////////

  // attributes
  double airGap = 0.01 * mm; // air gap between aerogel and filter (FIXME? actually it's currently a gas gap)

  // solid and volume: create aerogel and filter
  Cone aerogelSolid(
      aerogelThickness / 2,
      radiatorRmin + boreDelta * aerogelThickness / vesselLength, // at backplane
      radiatorRmax,
      radiatorRmin, // at frontplane
      radiatorRmax
      );
  Cone filterSolid(
      filterThickness / 2,
      radiatorRmin + boreDelta * (aerogelThickness + airGap + filterThickness) / vesselLength, // at backplane
      radiatorRmax,
      radiatorRmin + boreDelta * (aerogelThickness + airGap) / vesselLength, // at frontplane
      radiatorRmax
      );
  Volume aerogelVol(detName + "_aerogel", aerogelSolid, aerogelMat);
  Volume filterVol(detName  + "_filter",  filterSolid,  filterMat);
  aerogelVol.setVisAttributes(aerogelVis);
  filterVol.setVisAttributes(filterVis);

  // aerogel placement and surface properties
  // FIXME: define skin properties for aerogel and filter
  auto radiatorPos = Position(0., 0., radiatorFrontplane - 0.5 * aerogelThickness) + originFront;
  auto aerogelPV = gasvolVol.placeVolume(
      aerogelVol,
      Transform3D(Translation3D(radiatorPos.x(), radiatorPos.y(), radiatorPos.z())) // re-center to originFront
      );
  DetElement aerogelDE(det, "aerogel_de", 0);
  aerogelDE.setPlacement(aerogelPV);

  // filter placement and surface properties
  auto filterPV = gasvolVol.placeVolume(
      filterVol,
      Transform3D(
        Translation3D(0., 0., -airGap) // add an airgap (FIXME: actually a gas gap)
        *
        Translation3D(radiatorPos.x(), radiatorPos.y(), radiatorPos.z())  // re-center to originFront
        *
        Translation3D(0., 0., -(aerogelThickness + filterThickness) / 2.) // move to aerogel backplane
        )
      );
  DetElement filterDE(det, "filter_de", 0);
  filterDE.setPlacement(filterPV);

  // BUILD SENSORS ///////////////////////

  // solid and volume: single sensor module
  Box    sensorSolid(sensorSide / 2., sensorSide / 2., sensorThickness / 2.);
  Volume sensorVol(detName + "_sensor", sensorSolid, sensorMat);
  sensorVol.setVisAttributes(sensorVis);

  // sensitivity
  sensorVol.setSensitiveDetector(sens);

  // sensor plane positioning: we want `sensorPlaneDist` to be the distance between the
  // aerogel backplane (i.e., aerogel/filter boundary) and the sensor active surface (e.g, photocathode)
  double sensorZpos     = radiatorFrontplane - aerogelThickness - sensorPlaneDist - 0.5 * sensorThickness;
  auto   sensorPlanePos = Position(0., 0., sensorZpos) + originFront; // reference position
  // miscellaneous
  int    imod    = 0;           // module number
  double tBoxMax = vesselRmax1; // sensors will be tiled in tBox, within annular limits

  // SENSOR MODULE LOOP ------------------------
  /* cartesian tiling loop
   * - start at (x=0,y=0), to center the grid
   * - loop over positive-x positions; for each, place the corresponding negative-x sensor too
   * - nested similar loop over y positions
   */
  double sx, sy;
  for (double usx = 0; usx <= tBoxMax; usx += sensorSide + sensorGap) {
    for (int sgnx = 1; sgnx >= (usx > 0 ? -1 : 1); sgnx -= 2) {
      for (double usy = 0; usy <= tBoxMax; usy += sensorSide + sensorGap) {
        for (int sgny = 1; sgny >= (usy > 0 ? -1 : 1); sgny -= 2) {

          // sensor (x,y) center
          sx = sgnx * usx;
          sy = sgny * usy;

          // annular cut
          if (std::hypot(sx, sy) < sensorPlaneRmin || std::hypot(sx, sy) > sensorPlaneRmax)
            continue;

          // placement (note: transformations are in reverse order)
          auto sensorPV = gasvolVol.placeVolume(
              sensorVol,
              Transform3D(
                Translation3D(sensorPlanePos.x(), sensorPlanePos.y(), sensorPlanePos.z()) // move to reference position
                *
                Translation3D(sx, sy, 0.) // move to grid position
                )
              );

          // generate LUT for module number -> sensor position, for readout mapping tests
          // printf("%d %f %f\n",imod,sensorPV.position().x(),sensorPV.position().y());

          // properties
          sensorPV.addPhysVolID("module", imod);
          DetElement sensorDE(det, Form("sensor_de_%d", imod), imod);
          sensorDE.setPlacement(sensorPV);
          SkinSurface sensorSkin(desc, sensorDE, "sensor_optical_surface", sensorSurf, sensorVol); // FIXME: 3rd arg needs `imod`?
          sensorSkin.isValid();

          // increment sensor module number
          imod++;
        }
      }
    }
  }
  // END SENSOR MODULE LOOP ------------------------

  // Add service material if desired (added by Sylvester Joosten) ////////////////
  if (detElem.child("sensors").hasChild(_Unicode(services))) {
    xml_comp_t x_service = detElem.child("sensors").child(_Unicode(services));
    Assembly   service_vol("services");
    service_vol.setVisAttributes(desc, x_service.visStr());

    // Compute service total thickness from components
    double total_thickness = 0;
    for (xml_coll_t ci(x_service, _Unicode(component)); ci; ++ci) {
      total_thickness += xml_comp_t(ci).thickness();
    }

    int    ncomponents   = 0;
    double thickness_sum = -total_thickness / 2.0;
    for (xml_coll_t ci(x_service, _Unicode(component)); ci; ++ci, ncomponents++) {
      xml_comp_t x_comp    = ci;
      double     thickness = x_comp.thickness();
      Tube       c_tube{sensorPlaneRmin, sensorPlaneRmax, thickness / 2};
      Volume     c_vol{_toString(ncomponents, "component%d"), c_tube, desc.material(x_comp.materialStr())};
      c_vol.setVisAttributes(desc, x_comp.visStr());
      service_vol.placeVolume(c_vol, Position(0, 0, thickness_sum + thickness / 2.0));
      thickness_sum += thickness;
    }
    gasvolVol.placeVolume(service_vol,
        Transform3D(Translation3D(sensorPlanePos.x(), sensorPlanePos.y(),
            sensorPlanePos.z() - sensorThickness - total_thickness)));
  }

  // VESSEL PLACEMENT /////////////////////////////////////////////////////////////

  // place gas volume
  PlacedVolume gasvolPV = vesselVol.placeVolume(gasvolVol, Position(0, 0, 0));
  DetElement   gasvolDE(det, "gasvol_de", 0);
  gasvolDE.setPlacement(gasvolPV);

  // place mother volume (vessel)
  Volume       motherVol = desc.pickMotherVolume(det);
  PlacedVolume vesselPV  = motherVol.placeVolume(vesselVol, Position(0, 0, vesselZmin) - originFront);
  vesselPV.addPhysVolID("system", detID);
  det.setPlacement(vesselPV);

  return det;
}

// clang-format off
DECLARE_DETELEMENT(PFRICH, createDetector)
