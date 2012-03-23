#include <iostream>
#include "XML/XMLElements.h"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"

#define TAG(x)  extern const Tag_t Tag_##x  (#x)
#define ATTR(x) extern const Tag_t Attr_##x (#x)

namespace {
  struct __Init  {
    __Init() {
      try { xercesc::XMLPlatformUtils::Initialize();      }
      catch (const xercesc::XMLException& e)   {
        std::cout << "Xerces-c error in initialization:" 
          << xercesc::XMLString::transcode(e.getMessage())
          << std::endl;
      }
    }
    ~__Init() {
      xercesc::XMLPlatformUtils::Terminate();
    }
  };
  __Init __In__;
}

namespace DD4hep   { namespace XML  {

  extern const Tag_t Attr_id         ("id");
  extern const Tag_t Attr_type       ("type");
  extern const Tag_t Attr_value      ("value");
  extern const Tag_t Attr_formula    ("formula");
  extern const Tag_t Tag_NULL        ("0");
  extern const Tag_t Tag_empty       ("");
  extern const Tag_t Tag_star        ("*");
  extern const Tag_t Tag_PI          ("3.14159265358979323846");
  extern const Tag_t Tag_TWOPI       ("6.28318530717958647692");
  TAG(radian);
  TAG(mm);
  TAG(cm);
  TAG(MeV);
  TAG(n);
  TAG(ref);

  // RefElement parameters

  // Polycone parameters

  TAG(Air);
  TAG(Vacuum);
  TAG(D);
  TAG(fraction);

  TAG(atom);
  TAG(box);
  TAG(define);
  TAG(constant);
  TAG(detector);
  TAG(dimensions);
  TAG(element);
  TAG(first);
  TAG(id);
  TAG(idspecref);
  TAG(layer);
  TAG(limit);
  TAG(limitset);
  TAG(limitsetref);
  TAG(material);
  TAG(materialref);
  TAG(module);
  TAG(module_component);
  TAG(physvol);
  TAG(physvolid);
  TAG(polycone);
  TAG(position);
  TAG(positionref);
  TAG(nonprojective_cylinder);
  TAG(projective_cylinder);
  TAG(projective_zplane);
  TAG(region);
  TAG(regionref);
  TAG(rotation);
  TAG(rotationref);
  TAG(sdref);
  TAG(second);
  TAG(slice);
  TAG(solidref);
  TAG(tube);
  TAG(vis);
  TAG(visref);
  TAG(volume);
  TAG(volumeref);
  TAG(zplane);

  ATTR(InvisibleNoDaughters);
  ATTR(InvisibleWithDaughters);

  ATTR(1);
  ATTR(alpha);
  ATTR(b);
  ATTR(barrel);
  ATTR(beampipe);
  ATTR(combineHits);
  ATTR(crossing_angle);
  ATTR(deltaphi);
  ATTR(dimensions);
  ATTR(ecut);
  ATTR(false);
  ATTR(field_name);
  ATTR(g);
  ATTR(gridSizeX);
  ATTR(gridSizeY);
  ATTR(gridSizeZ);
  ATTR(gridSizePhi);
  ATTR(hits_collection);
  TAG(identity_rot);
  TAG(identity_pos);
  ATTR(inner_r);
  ATTR(inner_z);
  ATTR(layer);
  ATTR(limits);
  ATTR(material);
  ATTR(n);
  ATTR(name);
  ATTR(nphi);
  ATTR(ntheta);
  ATTR(outer_r);
  ATTR(outer_z);
  ATTR(position);
  ATTR(r);
  ATTR(rbg);
  ATTR(readout);
  ATTR(ref);
  ATTR(reflect);
  ATTR(reflect_rot);
  ATTR(region);
  ATTR(repeat);
  ATTR(zmin);
  ATTR(rmin);
  ATTR(rmax);
  ATTR(rotation);
  ATTR(segmentation);
  ATTR(sensitive);
  ATTR(startphi);
  ATTR(system);
  ATTR(thickness);
  ATTR(true);
  ATTR(tubs);
  ATTR(unit);
  ATTR(useForHitPosition);
  ATTR(verbose);
  ATTR(version);
  ATTR(vis);
  ATTR(x);
  ATTR(y);
  ATTR(z);
  ATTR(zhalf);

  // LCDD
  TAG(barrel);
  TAG(cartesian_grid_xy);
  TAG(global_grid_xy);
  TAG(color);
  TAG(detectors);
  TAG(display);
  TAG(fields);
  TAG(gdml);
  TAG(grid_xyz);
  TAG(header);
  TAG(iddict);
  TAG(idfield);
  TAG(idspec);
  TAG(lcdd);
  TAG(limits);
  TAG(materials);
  TAG(gdmlFile);
  TAG(includes);
  TAG(readout);
  TAG(readouts);
  TAG(reflect_rot);
  TAG(regions);
  TAG(segmentation);
  TAG(sensitive_detectors);
  TAG(setup);
  TAG(structure);
  TAG(solids);
  TAG(tracker);
  TAG(tubs);
  TAG(system);
  TAG(world);
  TAG(world_box);
  TAG(world_volume);
  TAG(calorimeter);
  TAG(tracking_cylinder);
  TAG(tracking_volume);

  ATTR(insideTrackingVolume);
  ATTR(signed);
  ATTR(label);
  ATTR(start);
  ATTR(width);
  ATTR(length);
  ATTR(z_length);
  ATTR(grid_size_x);
  ATTR(grid_size_y);
  ATTR(grid_size_z);
  ATTR(grid_size_phi);
  ATTR(line_style);
  ATTR(drawing_style);
  ATTR(module);
  ATTR(particles);
  ATTR(show_daughters);
  ATTR(R);
  ATTR(B);
  ATTR(G);
  ATTR(visible);
  ATTR(combine_hits);
  TAG(trd);
  ATTR(X1);
  ATTR(X2);
  ATTR(Y1);
  ATTR(Y2);
  ATTR(Z);

  TAG(polyhedra);
  ATTR(numsides);


  /// Compact XML
  TAG(slices);
  TAG(staves);
  TAG(stave);
  TAG(sensor);
  TAG(wedge);
  ATTR(gap);
  ATTR(lineStyle);
  ATTR(drawingStyle);
  ATTR(showDaughters);
  ATTR(phiBins);
  ATTR(thetaBins);

  TAG(trap);
  ATTR(phi);
  ATTR(theta);
  ATTR(aunit);

  ATTR(x1);
  ATTR(x2);
  ATTR(x3);
  ATTR(x4);

  ATTR(y1);
  ATTR(y2);
  ATTR(y3);
  ATTR(y4);

  ATTR(z1);
  ATTR(z2);
  ATTR(z3);
  ATTR(z4);

  ATTR(alpha1);
  ATTR(alpha2);
  ATTR(alpha3);
  ATTR(alpha4);

  ATTR(cut);
  ATTR(store_secondaries);
  ATTR(threshold);
  ATTR(lunit);
  ATTR(eunit);
  ATTR(r_size);
  ATTR(phi_size_max);

  TAG(component);
  TAG(generator);
  TAG(comment);
  TAG(author);
  TAG(info);
  ATTR(file);
  ATTR(checksum);

  ATTR(incoming_r);
  ATTR(outgoing_r);

  TAG(beampipe);
  TAG(subtraction);

  TAG(cone);
  ATTR(rmin1);
  ATTR(rmin2);
  ATTR(rmax1);
  ATTR(rmax2);

  TAG(ladder);
  TAG(support);
  ATTR(number);
  ATTR(offset);
  ATTR(radius);

  TAG(module_envelope);
  TAG(barrel_envelope);
  TAG(rphi_layout);
  TAG(z_layout);
  TAG(ring);
  ATTR(nmodules);
  ATTR(phi0);
  ATTR(phi_tilt);
  ATTR(rc);
  ATTR(dr);
  ATTR(z0);
  ATTR(nz);
  ATTR(dz);
  ATTR(zstart);

  TAG(modules);
  TAG(row);
  ATTR(nModules);
  ATTR(moduleHeight);
  ATTR(modulePitch);
  ATTR(rowPitch);
  ATTR(pads);
  ATTR(RowID);

  void tags_init() {static __Init i; }

}}
