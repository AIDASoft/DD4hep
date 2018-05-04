import os, time, DDG4TestSetup
"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""
class Setup(DDG4TestSetup.Setup):
  def __init__(self):
    install_dir = os.environ['DD4hepINSTALL']
    DDG4TestSetup.Setup.__init__(self, "file:"+install_dir+"/examples/ClientTests/compact/MiniTel.xml")

  def configure(self):
    DDG4TestSetup.Setup.configure(self)
    # Now the calorimeters
    seq,act = self.geant4.setupTracker('MyLHCBdetector1')
    seq,act = self.geant4.setupTracker('MyLHCBdetector2')
    seq,act = self.geant4.setupTracker('MyLHCBdetector3')
    seq,act = self.geant4.setupTracker('MyLHCBdetector4')
    act.OutputLevel = 4
    seq,act = self.geant4.setupTracker('MyLHCBdetector5')
    seq,act = self.geant4.setupTracker('MyLHCBdetector6')
    seq,act = self.geant4.setupTracker('MyLHCBdetector7')
    seq,act = self.geant4.setupTracker('MyLHCBdetector8')
    seq,act = self.geant4.setupTracker('MyLHCBdetector9')
    seq,act = self.geant4.setupTracker('MyLHCBdetector10')
    return self

  def defineOutput(self,output='MiniTel_'+time.strftime('%Y-%m-%d_%H-%M')):
    return DDG4TestSetup.Setup.configure(self,output)
