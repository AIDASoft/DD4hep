# ==========================================================================
#  AIDA Detector description implementation
# --------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
# ==========================================================================
#
# IronCylinder_python_geo.py — Python port of IronCylinder_geo.cpp.
#
# Demonstrates the DD4hep_PythonDetector factory:
#   - Reads detector dimensions from the compact XML.
#   - Builds an iron tube sensitive calorimeter volume.
#   - Places it inside the world volume.
#
# ==========================================================================

from dd4hep_geo_plugin import Assembly, DetElement, Tube, Volume, TMath, xml_det_t


def create_detector(description, xml_h, sens):
    """Build an iron cylindrical calorimeter.

    Parameters
    ----------
    description : dd4hep.Detector
        The top-level detector description object.
    xml_h : dd4hep.xml.Handle_t
        Handle to the ``<detector>`` XML element.
    sens : dd4hep.SensitiveDetector
        Sensitive detector assigned to this sub-detector.

    Returns
    -------
    dd4hep.DetElement
        The fully placed detector element.
    """
    x_det = xml_det_t(xml_h)
    name = x_det.nameStr()

    d_det = DetElement(name, x_det.id())

    # Read dimensions from the compact XML.
    x_dim = x_det.dimensions()
    inner_r = x_dim.rmin()
    outer_r = x_dim.rmax()
    z_half = x_dim.z() / 2.0

    # Envelope assembly volume.
    calo_vol = Assembly(name + "_envelope")
    calo_vol.setAttributes(description,
                           x_det.regionStr(),
                           x_det.limitsStr(),
                           x_det.visStr())

    # Sensitive iron tube.
    tub = Tube(inner_r, outer_r, z_half, 0.0, TMath.TwoPi())
    tub_vol = Volume(name + "_tube", tub,
                     description.material("Iron"))
    calo_vol.placeVolume(tub_vol).addPhysVolID("module", 1)

    sens.setType("calorimeter")
    tub_vol.setSensitiveDetector(sens)
    d_det.setAttributes(description, tub_vol,
                        x_det.regionStr(),
                        x_det.limitsStr(),
                        x_det.visStr())

    # Place the envelope inside the world / mother volume.
    mother = description.pickMotherVolume(d_det)
    calo_plv = mother.placeVolume(calo_vol)
    calo_plv.addPhysVolID("system", x_det.id())
    calo_plv.addPhysVolID("barrel", 0)
    d_det.setPlacement(calo_plv)

    return d_det
