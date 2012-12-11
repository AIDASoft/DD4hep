#include <iostream>
#include "XML/XMLElements.h"
#ifndef __TIXML__
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"

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
#else
namespace {
  struct __Init  {
    __Init() {
    }
  };
  __Init __In__;
}
#endif
#define TAG(x)  extern const Tag_t Tag_##x  (#x)
#define ATTR(x) extern const Tag_t Attr_##x (#x)

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

  TAG(element);
  TAG(material);
  TAG(atom);
  TAG(D);
  TAG(RL);
  TAG(NIL);
  ATTR(desnity);
  TAG(fraction);
  TAG(composite);
  TAG(sd);
  TAG(dipole_coeff);
  ATTR(funit);
  ATTR(global);
  ATTR(local);

  TAG(box);
  TAG(define);
  TAG(constant);
  TAG(detector);
  TAG(dimensions);
  TAG(first);
  TAG(id);
  TAG(idspecref);
  TAG(limit);
  TAG(limitset);
  TAG(limitsetref);
  TAG(materialref);
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
  TAG(solidref);
  TAG(tube);
  TAG(vis);
  TAG(visref);
  TAG(strength);
  TAG(volume);
  TAG(volumeref);
  TAG(zplane);

  TAG(alignment);
  TAG(alignments);
  TAG(properties);
  TAG(attributes);

  ATTR(overlap);
  ATTR(check);
  ATTR(InvisibleNoDaughters);
  ATTR(InvisibleWithDaughters);

  ATTR(1);
  ATTR(b);
  ATTR(barrel);
  ATTR(beampipe);
  ATTR(combineHits);
  ATTR(crossing_angle);
  ATTR(deltaphi);
  ATTR(dimensions);
  ATTR(ecut);
  ATTR(false);
  ATTR(strength);
  ATTR(field);
  ATTR(inner_field);
  ATTR(outer_field);
  ATTR(field_name);
  ATTR(g);
  ATTR(gridSizeX);
  ATTR(gridSizeY);
  ATTR(gridSizeZ);
  ATTR(gridSizePhi);
  ATTR(hits_collection);
  TAG(identity_rot);
  TAG(identity_pos);
  ATTR(layer);
  ATTR(limits);
  ATTR(material);
  ATTR(n);
  ATTR(name);
  ATTR(nphi);
  ATTR(ntheta);
  ATTR(position);
  ATTR(rbg);
  ATTR(readout);
  ATTR(ref);
  ATTR(reflect);
  ATTR(reflect_rot);
  ATTR(region);
  ATTR(repeat);

  // -- Boxes
  ATTR(width);
  ATTR(length);
  ATTR(height);
  ATTR(depth);

  // -- angles
  ATTR(angle);
  ATTR(alpha);
  ATTR(alpha1);
  ATTR(alpha2);
  ATTR(alpha3);
  ATTR(alpha4);
  ATTR(beta);
  ATTR(gamma);
  ATTR(psi);
  ATTR(phi);
  ATTR(startphi);
  ATTR(endphi);
  ATTR(theta);

  // -- Cartesian stuff
  ATTR(x);
  ATTR(X);
  ATTR(dx);
  ATTR(x0);
  ATTR(x1);
  ATTR(x2);
  ATTR(x3);
  ATTR(x4);
  ATTR(X1);
  ATTR(X2);
  ATTR(xmin);
  ATTR(xmax);
  ATTR(start_x);
  ATTR(end_x);
  ATTR(dim_x);
  ATTR(x_offset);

  ATTR(y);
  ATTR(Y);
  ATTR(dy);
  ATTR(y0);
  ATTR(y1);
  ATTR(y2);
  ATTR(y3);
  ATTR(y4);
  ATTR(Y1);
  ATTR(Y2);
  ATTR(ymin);
  ATTR(ymax);
  ATTR(start_y);
  ATTR(end_y);
  ATTR(dim_y);
  ATTR(y_offset);

  ATTR(z);
  ATTR(Z);
  ATTR(dz);
  ATTR(z0);
  ATTR(z1);
  ATTR(z2);
  ATTR(z3);
  ATTR(z4);
  ATTR(zmin);
  ATTR(zmax);
  ATTR(start_z);
  ATTR(end_z);
  ATTR(dim_z);
  ATTR(z_offset);
  ATTR(inner_z);
  ATTR(outer_z);

  // -- Radial stuff
  ATTR(r);
  ATTR(R);
  ATTR(dr);
  ATTR(r0);
  ATTR(rmin);
  ATTR(rmin1);
  ATTR(rmin2);
  ATTR(rmax);
  ATTR(rmax1);
  ATTR(rmax2);
  ATTR(radius);
  ATTR(inner_radius);
  ATTR(inner_r);
  ATTR(outer_radius);
  ATTR(outer_r);

  ATTR(rotation);
  ATTR(segmentation);
  ATTR(sensitive);
  ATTR(sensitive_detector);
  ATTR(system);
  ATTR(thickness);
  ATTR(true);
  ATTR(tubs);
  ATTR(unit);
  ATTR(useForHitPosition);
  ATTR(verbose);
  ATTR(version);
  ATTR(vis);
  ATTR(zhalf);

  // LCDD
  TAG(lcdd);
  TAG(lccdd);
  TAG(barrel);
  TAG(endcap);
  TAG(cartesian_grid_xy);
  TAG(global_grid_xy);
  TAG(color);
  TAG(detectors);
  TAG(display);
  TAG(field);
  TAG(fields);
  TAG(gdml);
  TAG(grid_xyz);
  TAG(iddict);
  TAG(idfield);
  TAG(idspec);
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
  ATTR(end);

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
  ATTR(B);
  ATTR(G);
  ATTR(visible);
  ATTR(combine_hits);
  TAG(trd);

  TAG(polyhedra);
  ATTR(numsides);

  // -- Detector building and composition
  TAG(slice);
  TAG(slices);
  TAG(layer);
  TAG(layers);
  TAG(stave);
  TAG(staves);
  TAG(module);
  TAG(modules);

  /// Compact XML
  TAG(sensor);
  TAG(wedge);
  ATTR(gap);
  ATTR(lineStyle);
  ATTR(drawingStyle);
  ATTR(showDaughters);
  ATTR(phiBins);
  ATTR(thetaBins);

  TAG(trap);
  ATTR(aunit);


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
  ATTR(status);
  ATTR(author);
  TAG(title);
  ATTR(title);
  TAG(name);
  ATTR(url);
  TAG(header);
  TAG(info);
  ATTR(file);
  ATTR(checksum);

  ATTR(incoming_r);
  ATTR(outgoing_r);

  TAG(beampipe);
  TAG(subtraction);

  TAG(cone);

  TAG(ladder);
  TAG(support);
  ATTR(number);
  ATTR(offset);

  TAG(module_envelope);
  TAG(barrel_envelope);
  TAG(rphi_layout);
  TAG(z_layout);
  TAG(ring);
  ATTR(nmodules);
  ATTR(phi0);
  ATTR(phi_tilt);
  ATTR(rc);
  ATTR(nz);
  ATTR(zstart);

  TAG(row);
  ATTR(nModules);
  ATTR(moduleHeight);
  ATTR(moduleWidth);
  ATTR(modulePosX);
  ATTR(modulePosY);
  ATTR(modulePitch);
  ATTR(rowPitch);
  ATTR(pads);
  ATTR(RowID);
  ATTR(nPads);
  ATTR(padPitch);
  ATTR(rowHeight);  
 
  void tags_init() {static __Init i; }

}}
