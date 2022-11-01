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
from __future__ import absolute_import
import os
import dddigi
import logging
from g4units import ns

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

attenuation = {'SiVertexEndcapHits': 50 * ns,
               'SiVertexBarrelHits': 50 * ns,
               'SiTrackerForwardHits': 50 * ns,
               'SiTrackerEndcapHits': 50 * ns,
               'SiTrackerBarrelHits': 50 * ns,
               'HcalPlugHits': 50 * ns,
               'HcalEndcapHits': 50 * ns,
               'HcalBarrelHits': 50 * ns,
               'EcalBarrelHits': 50 * ns,
               'EcalEndcapHits': 50 * ns,
               'MuonEndcapHits': 50 * ns,
               'MuonBarrelHits': 50 * ns,
               'BeamCalHits': 50 * ns,
               'LumiCalHits': 50 * ns,
           }

class Test(dddigi.Digitize):

  def __init__(self, geometry=None):
    global attenuation
    dddigi.Digitize.__init__(self, dddigi.Kernel())
    dddigi.setPrintFormat(str('%-32s %5s %s'))
    dddigi.setPrintLevel(dddigi.OutputLevel.INFO)
    self.kernel().printProperties()
    self.geometry = geometry
    self.input = None
    self.main_sequencer()
    self.attenuation = attenuation
    self.inputs = [
      'CLICSiD_2022-11-01_15-54.root',
      'CLICSiD_2022-11-01_15-10.root',
      'CLICSiD_2022-10-31_17-26.root',
      'CLICSiD_2022-10-31_17-55.root',
      'CLICSiD_2022-10-31_17-55.root',
      'CLICSiD_2022-10-31_18-20.root',
      'CLICSiD_2022-10-31_18-40.root',
      'CLICSiD_2022-10-31_18-59.root',
      'CLICSiD_2022-10-31_19-18.root',
      'CLICSiD_2022-10-31_19-36.root',
      'CLICSiD_2022-10-31_19-53.root',
      'CLICSiD_2022-10-31_20-11.root']
    self.used_inputs = []

  def segment_action(self, nam, **options):
    obj = dddigi.Interface.createSegmentAction(self.kernel(), str(nam))
    return obj

  def load_geo(self):
    fname = "file:" + os.environ['DD4hepINSTALL'] + "/DDDetectors/compact/SiD.xml"
    self.kernel().loadGeometry(str(fname))
    self.printDetectors()

  def data_containers(self):
    return list(self.attenuation.keys())

  def containers(self, count=None):
    if count:
      conts = []
      result = []
      for key in list(self.attenuation.keys()):
        conts.append(key)
        if len(conts) == count:
          result.append(conts)
          conts = []
      if len(conts) > 0:
        result.append(conts)
    else:
      result = list(self.attenuation.keys())
    return result

  def check_creation(self, objs):
    for o in objs:
      if o is None:
        self.error('FAILED  Failed to create object')

  def declare_input(self, name, input, parallel=True):
    if not self.input:
      self.input = dddigi.Synchronize(self.kernel(), 'DigiParallelActionSequence/READER')
      self.input.parallel = True

  def next_input(self):
    if len(self.used_inputs) == len(self.inputs):
      self.used_inputs = []
    next = self.inputs[len(self.used_inputs)]
    self.used_inputs.append(next)
    self.info('Prepariing next input file: ' + str(next))
    return next

  def run_checked(self, num_events=5, num_threads=5, parallel=3):
    result = "FAILED"
    evt_done = self.run(num_events=num_events, num_threads=num_threads, parallel=parallel)
    if evt_done == num_events:
        result = "PASSED"
    self.always('%s Test finished after processing %d events.' % (result, evt_done,))
    self.always('Test done. Exiting')
