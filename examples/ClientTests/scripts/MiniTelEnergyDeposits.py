from __future__ import absolute_import
import sys, time, DDG4, MiniTelSetup
from DDG4 import OutputLevel as Output
#
#
"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""
def run():
  m = MiniTelSetup.Setup()
  if len(sys.argv) >= 2 and sys.argv[1] =="batch":
    m.kernel.NumEvents = 200
    m.kernel.UI = ''
    DDG4.setPrintLevel(Output.WARNING)

  m.configure()
  gun = m.setupGun()
  part = m.setupGenerator()
  part.OutputLevel = Output.DEBUG
  # This is the actual test:
  hit_tuple = DDG4.EventAction(m.kernel,'HitTupleAction/MiniTelTuple',True)
  hit_tuple.OutputFile = 'MiniTel_EnergyDeposits_'+time.strftime('%Y-%m-%d_%H-%M')+'.root'
  hit_tuple.Collections = ['*']
  m.kernel.eventAction().add(hit_tuple)
  # Setup physics
  m.setupPhysics()
  # ... and run
  m.geant4.execute()

if __name__ == "__main__":
  run()
