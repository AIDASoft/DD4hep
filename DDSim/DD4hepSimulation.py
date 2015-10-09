"""

DD4hep simulation with some argument parsing
Based on M. Frank and F. Gaede runSim.py
   @author  A.Sailer
   @version 0.1

"""
__RCSID__ = "$Id$"
from SystemOfUnits import *
import argparse
try:
  import argcomplete
  ARGCOMPLETEENABLED=True
except ImportError:
  ARGCOMPLETEENABLED=False

def outputLevel( level ):
  """return INT for outputlevel"""
  if isinstance(level, int):
    if level < 1 or 7 < level:
      raise KeyError
    return level
  outputlevels = { "VERBOSE": 1,
                   "DEBUG":   2,
                   "INFO":    3,
                   "WARNING": 4,
                   "ERROR":   5,
                   "FATAL":   6,
                   "ALWAYS":  7 }
  return outputlevels[level.upper()]


from DDSim.Helper.Gun import Gun
from DDSim.Helper.ParticleHandler import ParticleHandler
from DDSim.Helper.Output import Output
from DDSim.Helper.MagneticField import MagneticField
from DDSim.Helper.ConfigHelper import ConfigHelper
import os
import sys


class DD4hepSimulation(object):
  """Class to hold all the parameters and functions to run simulation"""

  def __init__(self):
    self.compactFile = ""
    self.inputFiles = []
    self.outputFile = "dummyOutput.slcio"
    self.runType = "batch"
    self.printLevel = 3

    self.numberOfEvents = 0
    self.skipNEvents = 0
    self.physicsList = "FTFP_BERT"
    self.crossingAngleBoost = 0.0
    self.macroFile = ''
    self.enableGun = False
    self.vertexSigma = [0.0, 0.0, 0.0, 0.0]
    self.vertexOffset = [0.0, 0.0, 0.0, 0.0]
    self.magneticFieldDict = {}
    self.detailedShowerMode = False

    self.errorMessages = []
    self.dumpParameter = False

    ## objects for extended configuration option
    self.output = Output()
    self.gun = Gun()
    self.part = ParticleHandler()
    self.field = MagneticField()

    ### use TCSH geant UI instead of QT
    os.environ['G4UI_USE_TCSH'] = "1"


  def readSteeringFile(self, steeringFile):
    """Reads a steering file and sets the parameters to that of the
    DD4hepSimulation object present in the steering file.
    """
    globs = {}
    locs  = {}
    if not steeringFile:
      return
    execfile(steeringFile, globs, locs)
    for _name, obj in locs.items():
      if isinstance(obj, DD4hepSimulation):
        self.__dict__ = obj.__dict__

  def parseOptions(self):
    """parse the command line options"""
    parser = argparse.ArgumentParser("Running DD4hep Simulations:",
                                     formatter_class=argparse.RawTextHelpFormatter)

    parser.add_argument("--steeringFile", "-S", action="store", default=None,
                        help="Steering file to change default behaviour")

    #first we parse just the steering file, but only if we don't want to see the help message
    if not any( opt in sys.argv for opt in ('-h','--help')):
      parsed, _unknown = parser.parse_known_args()
      self.readSteeringFile(parsed.steeringFile)

    parser.add_argument("--compactFile", action="store", default=self.compactFile,
                        help="The compact XML file")

    parser.add_argument("--runType", action="store", choices=("batch","vis","run","shell"), default=self.runType,
                        help="The type of action to do in this invocation"##Note: implicit string concatenation
                        "\nbatch: just simulate some events, needs numberOfEvents, and input file or gun"
                        "\nvis: enable visualisation"
                        "\nrun: enable run the macro"
                        "\nshell: enable interactive session")

    parser.add_argument("--inputFiles", "-I", nargs='+', action="store", default=self.inputFiles,
                        help="InputFiles for simulation, lcio, stdhep, HEPEvt, and hepevt files are supported")

    parser.add_argument("--outputFile","-O", action="store", default=self.outputFile,
                        help="Outputfile from the simulation,only lcio output is supported")

    parser.add_argument("-v", "--printLevel", action="store", default=self.printLevel, dest="printLevel",
                        choices=(1,2,3,4,5,6,7,'VERBOSE','DEBUG', 'INFO', 'WARNING', 'ERROR', 'FATAL', 'ALWAYS'),
                        help="Verbosity use integers from 1(most) to 7(least) verbose"
                        "\nor strings: VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL, ALWAYS")

    parser.add_argument("--numberOfEvents", "-N", action="store", dest="numberOfEvents", default=self.numberOfEvents,
                        type=int, help="number of events to simulate, used in batch mode")

    parser.add_argument("--skipNEvents", action="store", dest="skipNEvents", default=self.skipNEvents, type=int,
                        help="Skip first N events when reading a file")

    parser.add_argument("--physicsList", action="store", dest="physicsList", default=self.physicsList,
                        help="Physics list to use in simulation")

    parser.add_argument("--crossingAngleBoost", action="store", dest="crossingAngleBoost", default=self.crossingAngleBoost,
                        type=float, help="Lorentz boost for the crossing angle, in radian!")

    parser.add_argument("--vertexSigma", nargs=4, action="store", dest="vertexSigma", default=self.vertexSigma, metavar = ('X','Y','Z','T'),
                        type=float, help="FourVector of the Sigma for the Smearing of the Vertex position: x y z t")

    parser.add_argument("--vertexOffset", nargs=4, action="store", dest="vertexOffset", default=self.vertexOffset, metavar = ('X','Y','Z','T'),
                        type=float, help="FourVector of translation for the Smearing of the Vertex position: x y z t")

    parser.add_argument("--macroFile", "-M", action="store", dest="macroFile", default=self.macroFile,
                        help="Macro file to execute for runType 'run' or 'vis'")

    parser.add_argument("--enableGun", "-G", action="store_true", dest="enableGun", default=self.enableGun,
                        help="enable the DDG4 particle gun")

    parser.add_argument("--dumpParameter", "--dump", action="store_true", dest="dumpParameter", default=self.dumpParameter,
                        help="Print all configuration Parameters and exit")

    parser.add_argument("--enableDetailedShowerMode", action="store_true", dest="detailedShowerMode", default=self.detailedShowerMode,
                        help="use detailed shower mode")

    #FIXME: Add all the things here, then they will show up in the help usage
    #output, or do something smarter with fullHelp only for example
    self.__addAllHelper( parser )
    ## now parse everything. The default values are now taken from the
    ## steeringFile if they were set so that the steering file parameters can be
    ## overwritten from the command line
    if ARGCOMPLETEENABLED:
      argcomplete.autocomplete(parser)
    parsed = parser.parse_args()

    self.dumpParameter = parsed.dumpParameter

    self.compactFile = parsed.compactFile
    self.inputFiles = parsed.inputFiles
    self.__checkFileFormat( self.inputFiles, (".stdhep", ".slcio", ".HEPEvt", ".hepevt", ".hepmc"))
    self.outputFile = parsed.outputFile
    self.__checkFileFormat( self.outputFile, ('.root', '.slcio'))
    self.runType = parsed.runType
    self.printLevel = self.__checkOutputLevel(parsed.printLevel)

    self.numberOfEvents = parsed.numberOfEvents
    self.skipNEvents = parsed.skipNEvents
    self.physicsList = parsed.physicsList
    self.crossingAngleBoost = parsed.crossingAngleBoost
    self.macroFile = parsed.macroFile
    self.enableGun = parsed.enableGun
    self.detailedShowerMode = parsed.detailedShowerMode
    self.vertexOffset = parsed.vertexOffset
    self.vertexSigma = parsed.vertexSigma

    if not self.compactFile:
      self.errorMessages.append("ERROR: No geometry compact file provided")

    if self.runType == "batch":
      if not self.numberOfEvents:
        self.errorMessages.append("ERROR: Batch mode requested, but did not set number of events")
      if not self.inputFiles and not self.enableGun:
        self.errorMessages.append("ERROR: Batch mode requested, but did not set inputFile(s) or gun")

    #self.__treatUnknownArgs( parsed, unknown )
    self.__parseAllHelper( parsed )
    if self.errorMessages:
      parser.epilog = "\n".join(self.errorMessages)
      parser.print_help()
      exit(1)

  @staticmethod
  def getDetectorLists( lcdd ):
    ''' get lists of trackers and calorimeters that are defined in lcdd (the compact xml file)'''
    import DDG4
  #  if len(detectorList):
  #    print " subset list of detectors given - will only instantiate these: " , detectorList
    trackers,calos = [],[]
    for i in lcdd.detectors():
      det = DDG4.DetElement(i.second)
      name = det.name()
      sd =  lcdd.sensitiveDetector( name )
      if sd.isValid():
        detType = sd.type()
  #      if len(detectorList) and not(name in detectorList):
  #        continue
        print 'getDetectorLists - found active detctor ' ,  name , ' type: ' , type
        if detType == "tracker":
          trackers.append( det.name() )
        if detType == "calorimeter":
          calos.append( det.name() )

    return trackers,calos

#==================================================================================

  def run(self):
    """setup the geometry and dd4hep and geant4 and do what was asked to be done"""
    import ROOT
    ROOT.PyConfig.IgnoreCommandLineOptions = True

    import DDG4, DD4hep

    self.printLevel = getOutputLevel(self.printLevel)

    kernel = DDG4.Kernel()
    DD4hep.setPrintLevel(self.printLevel)
    #kernel.setOutputLevel('Compact',1)

    kernel.loadGeometry("file:"+ self.compactFile )
    lcdd = kernel.lcdd()

    DDG4.importConstants( lcdd )

  #----------------------------------------------------------------------------------

    #simple = DDG4.Geant4( kernel, tracker='Geant4TrackerAction',calo='Geant4CalorimeterAction')
    #simple = DDG4.Geant4( kernel, tracker='Geant4TrackerCombineAction',calo='Geant4ScintillatorCalorimeterAction')
    simple = DDG4.Geant4( kernel, tracker='Geant4TrackerAction',calo='Geant4ScintillatorCalorimeterAction')

    simple.printDetectors()

    if self.runType =="vis":
      simple.setupUI(typ="csh", vis=True, macro=self.macroFile)
    elif self.runType =="run":
      simple.setupUI(typ="csh", vis=False, macro=self.macroFile, ui=False)
    elif self.runType =="shell":
      simple.setupUI(typ="csh", vis=False, macro=None, ui=True)
    elif self.runType == "batch":
      simple.setupUI(typ="csh", vis=False, macro=None, ui=False)
    else:
      print "ERROR: unknown runType"
      exit(1)

    #kernel.UI="csh"
    kernel.NumEvents=self.numberOfEvents

    #-----------------------------------------------------------------------------------
    # setup the magnetic field:
    self.__setMagneticFieldOptions(simple)

    #----------------------------------------------------------------------------------

    # Configure Run actions
    run1 = DDG4.RunAction(kernel,'Geant4TestRunAction/RunInit')
    kernel.registerGlobalAction(run1)
    kernel.runAction().add(run1)

    # Configure I/O

    if self.outputFile.endswith(".slcio"):
      simple.setupLCIOOutput('LcioOutput', self.outputFile)
    elif self.outputFile.endswith(".root"):
      simple.setupROOTOutput('RootOutput', self.outputFile)

    actionList = []

    if self.enableGun:
      gun = DDG4.GeneratorAction(kernel,"Geant4ParticleGun/"+"Gun")
      self.__setGunOptions( gun )
      gun.Standalone = False
      gun.Mask = 1
      actionList.append(gun)
      self.__applyBoostOrSmear(kernel, actionList, 1)

    for index,inputFile in enumerate(self.inputFiles, start=2):
      if inputFile.endswith(".slcio"):
        gen = DDG4.GeneratorAction(kernel,"LCIOInputAction/LCIO1")
        gen.Input="LCIOFileReader|"+inputFile
      elif inputFile.endswith(".stdhep"):
        gen = DDG4.GeneratorAction(kernel,"LCIOInputAction/STDHEP1")
        gen.Input="LCIOStdHepReader|"+inputFile
      elif inputFile.endswith(".HEPEvt"):
        gen = DDG4.GeneratorAction(kernel,"LCIOInputAction/HEPEvt1")
        gen.Input="Geant4EventReaderHepEvtShort|"+inputFile
      elif inputFile.endswith(".hepevt"):
        gen = DDG4.GeneratorAction(kernel,"Geant4InputAction/hepevt1")
        gen.Input="Geant4EventReaderHepEvtLong|"+inputFile
      elif inputFile.endswith(".hepmc"):
        gen = DDG4.GeneratorAction(kernel,"Geant4InputAction/hepmc1")
        gen.Input="Geant4EventReaderHepMC|"+inputFile
      gen.Sync = self.skipNEvents
      gen.Mask = index
      actionList.append(gen)
      self.__applyBoostOrSmear(kernel, actionList, index)

    if actionList:
      simple.buildInputStage( actionList , output_level=self.output.inputStage )

    #================================================================================================

    # And handle the simulation particles.
    part = DDG4.GeneratorAction(kernel,"Geant4ParticleHandler/ParticleHandler")
    kernel.generatorAction().adopt(part)
    #part.SaveProcesses = ['conv','Decay']
    part.SaveProcesses        = self.part.saveProcesses
    part.MinimalKineticEnergy = self.part.minimalKineticEnergy
    part.KeepAllParticles     = self.part.keepAllParticles
    part.PrintEndTracking     = self.part.printEndTracking
    part.PrintStartTracking   = self.part.printStartTracking
    part.OutputLevel = self.output.part
    part.enableUI()
    user = DDG4.Action(kernel,"Geant4TCUserParticleHandler/UserParticleHandler")
    try:
      user.TrackingVolume_Zmax = DDG4.tracker_region_zmax
      user.TrackingVolume_Rmax = DDG4.tracker_region_rmax
    except AttributeError as e:
      print "No Attribute: ", str(e)

    #  user.enableUI()
    part.adopt(user)

    #=================================================================================


    # Setup global filters for use in sensitive detectors

    f1 = DDG4.Filter(kernel,'GeantinoRejectFilter/GeantinoRejector')
    kernel.registerGlobalFilter(f1)

    f4 = DDG4.Filter(kernel,'EnergyDepositMinimumCut')
    f4.Cut = 1.*keV
    kernel.registerGlobalFilter(f4)


    #=================================================================================
    # get lists of trackers and calorimeters in lcdd

    trk,cal = self.getDetectorLists( lcdd )

  # ---- add the trackers:
  # fixme: this assumes the same filters for all trackers ...

    for tracker in trk:
      print 'simple.setupTracker(  ' , tracker , ')'

      if 'tpc' in tracker.lower():
        seq,act = simple.setupTracker( tracker, type='TPCSDAction')
      else:
        seq,act = simple.setupTracker( tracker )

      seq.add(f1)
      if self.detailedShowerMode:
        act.HitCreationMode = 2

  # ---- add the calorimeters:

    for calo in cal:
      print 'simple.setupCalorimeter(  ' , calo , ')'
      seq,act = simple.setupCalorimeter( calo )
      if self.detailedShowerMode:
        act.HitCreationMode = 2


  #=================================================================================
    # Now build the physics list:
    _phys = simple.setupPhysics( self.physicsList )

    #fg: do we need these really ?
    #fg:  ph = DDG4.PhysicsList(kernel,'Geant4PhysicsList/Myphysics')
    #fg:  ph.addParticleConstructor('G4BosonConstructor')
    #fg:  ph.addParticleConstructor('G4LeptonConstructor')
    #fg:  ph.addParticleProcess('e[+-]','G4eMultipleScattering',-1,1,1)
    #fg:  ph.addPhysicsConstructor('G4OpticalPhysics')
    #fg:  ph.enableUI()
    #fg:  phys.add(ph)
    #fg:  phys.dump()

    DD4hep.setPrintLevel(self.printLevel)

    if self.dumpParameter:
      from pprint import pprint
      print "="*80
      pprint(vars(self))
      print "="*80
      exit(1)

    kernel.configure()
    kernel.initialize()

    #DDG4.setPrintLevel(Output.DEBUG)

    kernel.run()
    kernel.terminate()

  def __setGunOptions(self, gun):
    """set the starting properties of the DDG4 particle gun"""
    gun.energy       = self.gun.energy
    gun.particle     = self.gun.particle
    gun.multiplicity = self.gun.multiplicity
    gun.position     = self.gun.position
    gun.isotrop      = self.gun.isotrop
    gun.direction    = self.gun.direction

  def __setMagneticFieldOptions(self, simple):
    """ create and configure the magnetic tracking setup """
    field = simple.addConfig('Geant4FieldTrackingSetupAction/MagFieldTrackingSetup')
    field.stepper            = self.field.stepper
    field.equation           = self.field.equation
    field.eps_min            = self.field.eps_min
    field.eps_max            = self.field.eps_max
    field.min_chord_step     = self.field.min_chord_step
    field.delta_chord        = self.field.delta_chord
    field.delta_intersection = self.field.delta_intersection
    field.delta_one_step     = self.field.delta_one_step

  def __checkFileFormat(self, fileNames, extensions):
    """check if the fileName is allowed, note that the filenames are case
    sensitive, and in case of hepevt we depend on this to identify short and long versions of the content
    """
    if isinstance( fileNames, basestring ):
      fileNames = [fileNames]
    if not all( fileName.endswith( extensions ) for fileName in fileNames ):
      self.errorMessages.append("ERROR: Unknown fileformat for file: %s" % fileNames)
    return

  def __applyBoostOrSmear( self, kernel, actionList, mask ):
    """apply boost or smearing for given mask index"""
    import DDG4
    if self.crossingAngleBoost:
      lbo = DDG4.GeneratorAction(kernel, "Geant4InteractionVertexBoost")
      lbo.Angle = self.crossingAngleBoost
      lbo.Mask = mask
      actionList.append(lbo)

    if any(self.vertexSigma) or any(self.vertexOffset):
      vSmear = DDG4.GeneratorAction(kernel, "Geant4InteractionVertexSmear")
      vSmear.Offset = self.vertexOffset
      vSmear.Sigma = self.vertexSigma
      vSmear.Mask = mask
      actionList.append(vSmear)

  def __addAllHelper( self , parser ):
    """all configHelper objects to commandline args"""
    for name, obj in vars(self).iteritems():
      if isinstance( obj, ConfigHelper ):
        for var,valAndDoc in obj.getOptions().iteritems():
          parser.add_argument("--%s.%s" % (name, var),
                              action="store",
                              dest="%s.%s" % (name, var),
                              default = valAndDoc[0],
                              help = valAndDoc[1]
                              # type = type(val),
                             )

  def __parseAllHelper( self, parsed ):
    """ parse all the options for the helper """
    parsedDict = vars(parsed)
    for name, obj in vars(self).iteritems():
      if isinstance( obj, ConfigHelper ):
        for var in obj.getOptions():
          key = "%s.%s" %( name,var )
          if key in parsedDict:
            obj.setOption( var, parsedDict[key] )


  def __checkOutputLevel(self, level):
    """return outputlevel as int so we don't have to import anything for faster startup"""
    try:
      level = int(level)
      if level < 1 or 7 < level:
        raise KeyError
      return level
    except ValueError:
      try:
        return outputLevel[level.upper()]
      except ValueError:
        self.errorMessages.append( "ERROR: printLevel is neither integer nor string" )
        return -1
    except KeyError:
      self.errorMessages.append( "ERROR: printLevel '%s' unknown" % level )
      return -1


################################################################################
### MODULE FUNCTIONS GO HERE
################################################################################

def getOutputLevel(level):
  """return output.LEVEL"""
  from DDG4 import OutputLevel
  levels = { 1: OutputLevel.VERBOSE,
             2: OutputLevel.DEBUG,
             3: OutputLevel.INFO,
             4: OutputLevel.WARNING,
             5: OutputLevel.ERROR,
             6: OutputLevel.FATAL,
             7: OutputLevel.ALWAYS }
  return levels[level]
