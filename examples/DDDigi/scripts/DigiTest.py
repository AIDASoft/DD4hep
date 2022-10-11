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
import dddigi
import logging
from g4units import ns

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

"""
  00C7A3C1 SiVertexEndcapHits               : map<long long, EnergyDeposit>
  00D16F45 EcalBarrelHits                   : map<long long, EnergyDeposit>
  0D3C3803 MCParticles                      : vector<shared_ptr<dd4hep::sim::Geant4Particle>, allocator<shared_ptr<dd4hep::sim::Geant4Particle> > >
  2E9082A9 HcalPlugHits                     : map<long long, EnergyDeposit>
  3A89E02E HcalEndcapHits                   : map<long long, EnergyDeposit>
  569C1C49 HcalBarrelHits                   : map<long long, EnergyDeposit>
  62AD0218 EcalEndcapHits                   : map<long long, EnergyDeposit>
  7FBC6131 SiTrackerBarrelHits              : map<long long, EnergyDeposit>
  A239CB2E LumiCalHits                      : map<long long, EnergyDeposit>
  B5CD88D8 MuonBarrelHits                   : map<long long, EnergyDeposit>
  BDB9AD3E SiVertexBarrelHits               : map<long long, EnergyDeposit>
  C84DE2C2 MuonEndcapHits                   : map<long long, EnergyDeposit>
  CB57C6D0 SiTrackerForwardHits             : map<long long, EnergyDeposit>
  D108063E BeamCalHits                      : map<long long, EnergyDeposit>
  F4183035 SiTrackerEndcapHits              : map<long long, EnergyDeposit>
"""

attenuation = { 'SiVertexEndcapHits': 15 * ns,
                'SiVertexBarrelHits': 15 * ns,
                'SiTrackerForwardHits': 15 * ns,
                'SiTrackerEndcapHits': 15 * ns,
                'SiTrackerBarrelHits': 15 * ns,
                'HcalPlugHits': 15 * ns,
                'HcalEndcapHits': 15 * ns,
                'HcalBarrelHits': 15 * ns,
                'EcalEndcapHits': 15 * ns,
                'MuonEndcapHits': 15 * ns,
                'MuonBarrelHits': 15 * ns,
                'BeamCalHits': 15 * ns,
                'LumiCalHits': 15 * ns,
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
        'CLICSiD_2022-10-05_13-21.root',
        'CLICSiD_2022-10-05_13-52.root',
        'CLICSiD_2022-10-05_14-16.root',
        'CLICSiD_2022-10-05_14-40.root'
    ]
    self.used_inputs = []

  def declare_input(self, name, input, parallel=True):
    if not self.input:
      self.input = dddigi.Synchronize(self.kernel(), 'DigiParallelActionSequence/READER')
      self.input.parallel = True

  def next_input(self):
    if len(self.used_inputs) == len(self.inputs):
      self.used_inputs = []
    next = self.inputs[len(self.used_inputs)]
    self.used_inputs.append(next)
    return next
