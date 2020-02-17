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
from __future__ import absolute_import, unicode_literals
import dd4hep as core
import logging

logger = logging.getLogger(__name__)


def loadDDRec():
  from ROOT import gSystem
  result = gSystem.Load("libDDRec")
  if result < 0:
    raise Exception('DDG4.py: Failed to load the DDG4 library libDDRec: ' + gSystem.GetErrorStr())
  from ROOT import dd4hep as module
  core.rec = module.rec
  return module.rec


# We are nearly there ....
name_space = __import__(__name__)


def import_namespace_item(ns, nam):
  scope = getattr(name_space, ns)
  attr = getattr(scope, nam)
  setattr(name_space, nam, attr)
  return attr


# ---------------------------------------------------------------------------
#
try:
  rec = loadDDRec()
except Exception as X:
  logger.error('+--%-100s--+', 100 * '-')
  logger.error('|  %-100s  |', 'Failed to load DDRec library:')
  logger.error('|  %-100s  |', str(X))
  logger.error('+--%-100s--+', 100 * '-')
  exit(1)


def import_rec():
  import_namespace_item('rec', 'CellIDPositionConverter')

  import_namespace_item('rec', 'FixedPadSizeTPCStruct')

  import_namespace_item('rec', 'ZPlanarStruct')
  import_namespace_item('rec', 'ZPlanarStruct::LayerLayout')

  import_namespace_item('rec', 'ZDiskPetalsStruct')
  import_namespace_item('rec', 'ZDiskPetalsStruct::LayerLayout')

  import_namespace_item('rec', 'ConicalSupportStruct')

  import_namespace_item('rec', 'LayeredCalorimeterStruct')
  import_namespace_item('rec', 'LayeredCalorimeterStruct::Layer')

  import_namespace_item('rec', 'NeighbourSurfacesStruct')
  import_namespace_item('rec', 'DetectorSurfaces')

  import_namespace_item('rec', 'IMaterial')
  import_namespace_item('rec', 'ISurface')
  import_namespace_item('rec', 'ICylinder')
  import_namespace_item('rec', 'ICone')
  import_namespace_item('rec', 'SurfaceType')
  import_namespace_item('rec', 'MaterialData')
  import_namespace_item('rec', 'MaterialManager')
  import_namespace_item('rec', 'VolSurfaceBase')
  import_namespace_item('rec', 'VolSurface')
  import_namespace_item('rec', 'VolSurfaceList')
  import_namespace_item('rec', 'VolPlaneImpl')
  import_namespace_item('rec', 'VolCylinderImpl')
  import_namespace_item('rec', 'VolConeImpl')
  import_namespace_item('rec', 'Surface')
  import_namespace_item('rec', 'CylinderSurface')
  import_namespace_item('rec', 'ConeSurface')
  import_namespace_item('rec', 'SurfaceList')
  import_namespace_item('rec', 'Vector2D')
  import_namespace_item('rec', 'Vector3D')
  import_namespace_item('rec', 'SurfaceManager')

  import_namespace_item('rec', 'FixedPadSizeTPCData')
  import_namespace_item('rec', 'ZPlanarData')
  import_namespace_item('rec', 'ZDiskPetalsData')
  import_namespace_item('rec', 'ConicalSupportData')
  import_namespace_item('rec', 'LayeredCalorimeterData')
  import_namespace_item('rec', 'NeighbourSurfacesData')


# Now instantiate the entire thing
import_rec()
std_list_ISurface = core.std_list(str('ISurface*'))
std_list_VolSurface = core.std_list(str('VolSurface'))
