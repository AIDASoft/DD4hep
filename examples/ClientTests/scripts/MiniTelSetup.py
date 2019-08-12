import os, time, DDG4TestSetup
"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""
class Setup(DDG4TestSetup.Setup):
  def __init__(self):
    install_dir = os.environ['DD4hepExamplesINSTALL']
    DDG4TestSetup.Setup.__init__(self, "file:"+install_dir+"/examples/ClientTests/compact/MiniTel.xml")

  def configure(self,output_level=None):
    DDG4TestSetup.Setup.configure(self)
    # Now the calorimeters
    seq,act = self.geant4.setupTracker('MyLHCBdetector1')
    if output_level:  act.OutputLevel = output_level
    seq,act = self.geant4.setupTracker('MyLHCBdetector2')
    if output_level:  act.OutputLevel = output_level
    seq,act = self.geant4.setupTracker('MyLHCBdetector3')
    if output_level:  act.OutputLevel = output_level
    seq,act = self.geant4.setupTracker('MyLHCBdetector4')
    if output_level:  act.OutputLevel = output_level
    #act.OutputLevel = 4
    seq,act = self.geant4.setupTracker('MyLHCBdetector5')
    if output_level:  act.OutputLevel = output_level
    seq,act = self.geant4.setupTracker('MyLHCBdetector6')
    if output_level:  act.OutputLevel = output_level
    seq,act = self.geant4.setupTracker('MyLHCBdetector7')
    if output_level:  act.OutputLevel = output_level
    seq,act = self.geant4.setupTracker('MyLHCBdetector8')
    if output_level:  act.OutputLevel = output_level
    seq,act = self.geant4.setupTracker('MyLHCBdetector9')
    if output_level:  act.OutputLevel = output_level
    seq,act = self.geant4.setupTracker('MyLHCBdetector10') 
    if output_level:  act.OutputLevel = output_level
    return self

  def defineOutput(self,output='MiniTel_'+time.strftime('%Y-%m-%d_%H-%M')):
    return DDG4TestSetup.Setup.defineOutput(self,output)
