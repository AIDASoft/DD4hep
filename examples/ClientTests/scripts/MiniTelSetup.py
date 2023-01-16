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
import os
import time
import DDG4TestSetup
"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""


class Setup(DDG4TestSetup.Setup):
  def __init__(self, geometry="/examples/ClientTests/compact/MiniTel.xml", macro=None, vis=None):
    install_dir = os.environ['DD4hepExamplesINSTALL']
    DDG4TestSetup.Setup.__init__(self, "file:" + install_dir + geometry, macro=macro, vis=vis)

  def configure(self, output_level=None):
    from dd4hep import DetElement
    DDG4TestSetup.Setup.configure(self)
    for i in self.geant4.description.detectors():
      det = DetElement(i.second.ptr())
      sd = self.description.sensitiveDetector(str(det.name()))
      if sd.isValid():
        seq, act = self.geant4.setupTracker(det.name())
        if output_level:
          act.OutputLevel = output_level
    return self

  def defineOutput(self, output='MiniTel_' + time.strftime('%Y-%m-%d_%H-%M')):
    return DDG4TestSetup.Setup.defineOutput(self, output)

  def defineEdm4hepOutput(self, output='MiniTel_' + time.strftime('%Y-%m-%d_%H-%M')):
    return DDG4TestSetup.Setup.defineEdm4hepOutput(self, output)
