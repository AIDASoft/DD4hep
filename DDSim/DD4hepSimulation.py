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

POSSIBLEINPUTFILES = (".stdhep", ".slcio", ".HEPEvt", ".hepevt", ".hepmc", ".pairs")

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
from DDSim.Helper.Action import Action
from DDSim.Helper.Random import Random
from DDSim.Helper.Filter import Filter
from DDSim.Helper.Physics import Physics
from DDSim.Helper.GuineaPig import GuineaPig
from DDSim.Helper.LCIO import LCIO
from DDSim.Helper.Meta import Meta

import os
import sys
import datetime

class DD4hepSimulation(object):
  """Class to hold all the parameters and functions to run simulation"""

  def __init__(self):
    self.steeringFile = None
    self.compactFile = ""
    self.inputFiles = []
    self.outputFile = "dummyOutput.slcio"
    self.runType = "batch"
    self.printLevel = 3

    self.numberOfEvents = 0
    self.skipNEvents = 0
    self.physicsList = None ## deprecated use physics.list
    self.crossingAngleBoost = 0.0
    self.macroFile = ''
    self.enableGun = False
    self.enableG4GPS = False
    self.enableG4Gun = False
    self._g4gun = None
    self._g4gps = None
    self.vertexSigma = [0.0, 0.0, 0.0, 0.0]
    self.vertexOffset = [0.0, 0.0, 0.0, 0.0]
    self.enableDetailedShowerMode = False

    self._errorMessages = []
    self._dumpParameter = False
    self._dumpSteeringFile = False

    ## objects for extended configuration option
    self.output = Output()
    self.random = Random()
    self.gun = Gun()
    self.part = ParticleHandler()
    self.field = MagneticField()
    self.action = Action()
    self.guineapig = GuineaPig()
    self.lcio = LCIO()
    self.meta = Meta()

    self.filter = Filter()
    self.physics = Physics()

    self._argv = None

    ### use TCSH geant UI instead of QT
    os.environ['G4UI_USE_TCSH'] = "1"


  def readSteeringFile(self):
    """Reads a steering file and sets the parameters to that of the
    DD4hepSimulation object present in the steering file.
    """
    globs = {}
    locs  = {}
    if not self.steeringFile:
      return
    sFileTemp = self.steeringFile
    execfile(self.steeringFile, globs, locs)
    for _name, obj in locs.items():
      if isinstance(obj, DD4hepSimulation):
        self.__dict__ = obj.__dict__
    self.steeringFile = os.path.abspath(sFileTemp)

  def parseOptions(self, argv=None):
    """parse the command line options"""

    if argv is None:
      self._argv = list(sys.argv)

    parser = argparse.ArgumentParser("Running DD4hep Simulations:",
                                     formatter_class=argparse.RawTextHelpFormatter)

    parser.add_argument("--steeringFile", "-S", action="store", default=self.steeringFile,
                        help="Steering file to change default behaviour")

    #first we parse just the steering file, but only if we don't want to see the help message
    if not any( opt in self._argv for opt in ('-h','--help')):
      parsed, _unknown = parser.parse_known_args()
      self.steeringFile = parsed.steeringFile
      self.readSteeringFile()

    ## readSteeringFile will set self._argv to None if there is a steering file
    if self._argv is None:
      self._argv = list(argv) if argv else list(sys.argv)


    parser.add_argument("--compactFile", action="store", default=self.compactFile,
                        help="The compact XML file")

    parser.add_argument("--runType", action="store", choices=("batch","vis","run","shell"), default=self.runType,
                        help="The type of action to do in this invocation"##Note: implicit string concatenation
                        "\nbatch: just simulate some events, needs numberOfEvents, and input file or gun"
                        "\nvis: enable visualisation, run the macroFile if it is set"
                        "\nrun: run the macroFile and exit"
                        "\nshell: enable interactive session")

    parser.add_argument("--inputFiles", "-I", nargs='+', action="store", default=self.inputFiles,
                        help="InputFiles for simulation %s files are supported" % ", ".join(POSSIBLEINPUTFILES) )

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

    parser.add_argument("--enableG4GPS", action="store_true", dest="enableG4GPS", default=self.enableG4GPS,
                        help="enable the Geant4 GeneralParticleSource. Needs a macroFile (runType run) or use it with the shell (runType shell)")

    parser.add_argument("--enableG4Gun", action="store_true", dest="enableG4Gun", default=self.enableG4Gun,
                        help="enable the Geant4 particle gun. Needs a macroFile (runType run) or use it with the shell (runType shell)")

    parser.add_argument("--dumpParameter", "--dump", action="store_true", dest="dumpParameter", default=self._dumpParameter,
                        help="Print all configuration Parameters and exit")

    parser.add_argument("--enableDetailedShowerMode", action="store_true", dest="enableDetailedShowerMode", default=self.enableDetailedShowerMode,
                        help="use detailed shower mode")

    parser.add_argument("--dumpSteeringFile", action="store_true", dest="dumpSteeringFile", default=self._dumpSteeringFile,
                        help="print an example steering file to stdout")

    #output, or do something smarter with fullHelp only for example
    ConfigHelper.addAllHelper(self, parser)
    ## now parse everything. The default values are now taken from the
    ## steeringFile if they were set so that the steering file parameters can be
    ## overwritten from the command line
    if ARGCOMPLETEENABLED:
      argcomplete.autocomplete(parser)
    parsed = parser.parse_args()

    self._dumpParameter = parsed.dumpParameter
    self._dumpSteeringFile = parsed.dumpSteeringFile

    self.compactFile = parsed.compactFile
    self.inputFiles = parsed.inputFiles
    self.inputFiles = self.__checkFileFormat( self.inputFiles, POSSIBLEINPUTFILES)
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
    self.enableG4Gun = parsed.enableG4Gun
    self.enableG4GPS = parsed.enableG4GPS
    self.enableDetailedShowerMode = parsed.enableDetailedShowerMode
    self.vertexOffset = parsed.vertexOffset
    self.vertexSigma = parsed.vertexSigma

    self._consistencyChecks()

    #self.__treatUnknownArgs( parsed, unknown )
    self.__parseAllHelper( parsed )
    if self._errorMessages and not (self._dumpParameter or self._dumpSteeringFile):
      parser.epilog = "\n".join(self._errorMessages)
      parser.print_help()
      exit(1)

    if self._dumpParameter:
      from pprint import pprint
      print "="*80
      pprint(vars(self))
      print "="*80
      exit(1)

    if self._dumpSteeringFile:
      self.__printSteeringFile( parser )
      exit(1)

  @staticmethod
  def getDetectorLists( detectorDescription ):
    ''' get lists of trackers and calorimeters that are defined in detectorDescription (the compact xml file)'''
    import DDG4
  #  if len(detectorList):
  #    print " subset list of detectors given - will only instantiate these: " , detectorList
    trackers,calos = [],[]
    for i in detectorDescription.detectors():
      det = DDG4.DetElement(i.second.ptr())
      name = det.name()
      sd =  detectorDescription.sensitiveDetector( name )
      if sd.isValid():
        detType = sd.type()
  #      if len(detectorList) and not(name in detectorList):
  #        continue
        print 'getDetectorLists - found active detctor ' ,  name , ' type: ' , detType
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
    detectorDescription = kernel.detectorDescription()

    DDG4.importConstants( detectorDescription )

  #----------------------------------------------------------------------------------

    #simple = DDG4.Geant4( kernel, tracker='Geant4TrackerAction',calo='Geant4CalorimeterAction')
    #simple = DDG4.Geant4( kernel, tracker='Geant4TrackerCombineAction',calo='Geant4ScintillatorCalorimeterAction')
    simple = DDG4.Geant4( kernel, tracker=self.action.tracker, calo=self.action.calo )

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

    # Configure the random seed, do it before the I/O because we might change the seed!
    _rndm = self.random.initialize( DDG4, kernel, self.output.random )

    # Configure I/O
    if self.outputFile.endswith(".slcio"):
      lcOut = simple.setupLCIOOutput('LcioOutput', self.outputFile)
      lcOut.RunHeader = self.__addParametersToRunHeader()
      lcOut.EventParametersString, lcOut.EventParametersInt, lcOut.EventParametersFloat = self.meta.parseEventParameters()
      lcOut.RunNumberOffset = self.meta.runNumberOffset if self.meta.runNumberOffset > 0 else 0
      lcOut.EventNumberOffset = self.meta.eventNumberOffset if self.meta.eventNumberOffset > 0 else 0
    elif self.outputFile.endswith(".root"):
      simple.setupROOTOutput('RootOutput', self.outputFile)

    actionList = []

    if self.enableGun:
      gun = DDG4.GeneratorAction(kernel,"Geant4ParticleGun/"+"Gun")
      self.gun.setOptions( gun )
      gun.Standalone = False
      gun.Mask = 1
      actionList.append(gun)
      self.__applyBoostOrSmear(kernel, actionList, 1)
      print "++++ Adding DD4hep Particle Gun ++++"

    if self.enableG4Gun:
      ## GPS Create something
      self._g4gun = DDG4.GeneratorAction(kernel,"Geant4GeneratorWrapper/Gun")
      self._g4gun.Uses = 'G4ParticleGun'
      self._g4gun.Mask = 2
      print "++++ Adding Geant4 Particle Gun ++++"
      actionList.append(self._g4gun)

    if self.enableG4GPS:
      ## GPS Create something
      self._g4gps = DDG4.GeneratorAction(kernel,"Geant4GeneratorWrapper/GPS")
      self._g4gps.Uses = 'G4GeneralParticleSource'
      self._g4gps.Mask = 3
      print "++++ Adding Geant4 General Particle Source ++++"
      actionList.append(self._g4gps)

    for index,inputFile in enumerate(self.inputFiles, start=4):
      if inputFile.endswith(".slcio"):
        gen = DDG4.GeneratorAction(kernel,"LCIOInputAction/LCIO%d" % index)
        gen.Parameters = self.lcio.getParameters()
        gen.Input="LCIOFileReader|"+inputFile
      elif inputFile.endswith(".stdhep"):
        gen = DDG4.GeneratorAction(kernel,"LCIOInputAction/STDHEP%d" % index)
        gen.Input="LCIOStdHepReader|"+inputFile
      elif inputFile.endswith(".HEPEvt"):
        gen = DDG4.GeneratorAction(kernel,"Geant4InputAction/HEPEvt%d" % index)
        gen.Input="Geant4EventReaderHepEvtShort|"+inputFile
      elif inputFile.endswith(".hepevt"):
        gen = DDG4.GeneratorAction(kernel,"Geant4InputAction/hepevt%d" % index)
        gen.Input="Geant4EventReaderHepEvtLong|"+inputFile
      elif inputFile.endswith(".hepmc"):
        gen = DDG4.GeneratorAction(kernel,"Geant4InputAction/hepmc%d" % index)
        gen.Input="Geant4EventReaderHepMC|"+inputFile
      elif inputFile.endswith(".pairs"):
        gen = DDG4.GeneratorAction(kernel,"Geant4InputAction/GuineaPig%d" % index)
        gen.Input="Geant4EventReaderGuineaPig|"+inputFile
        gen.Parameters = self.guineapig.getParameters()
      else:
        ##this should never happen because we already check at the top, but in case of some LogicError...
        raise RuntimeError( "Unknown input file type: %s" % inputFile )
      gen.Sync = self.skipNEvents
      gen.Mask = index
      actionList.append(gen)
      self.__applyBoostOrSmear(kernel, actionList, index)

    if actionList:
      simple.buildInputStage( actionList , output_level=self.output.inputStage,
                              have_mctruth = self._enablePrimaryHandler() )

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
    part.MinDistToParentVertex= self.part.minDistToParentVertex
    part.OutputLevel = self.output.part
    part.enableUI()


    if self.part.enableDetailedHitsAndParticleInfo:
      self.part.setDumpDetailedParticleInfo( kernel, DDG4 )

    #----------------------------------




    user = DDG4.Action(kernel,"Geant4TCUserParticleHandler/UserParticleHandler")
    try:
      user.TrackingVolume_Zmax = DDG4.tracker_region_zmax
      user.TrackingVolume_Rmax = DDG4.tracker_region_rmax

      print " *** definition of tracker region *** "
      print "    tracker_region_zmax = " ,  user.TrackingVolume_Zmax
      print "    tracker_region_rmax = " ,  user.TrackingVolume_Rmax
      print " ************************************ "

    except AttributeError as e:
      print "ERROR - attribute of tracker region missing in detector model   ", str(e)
      print "   make sure to specify the global constants tracker_region_zmax and tracker_region_rmax "
      print "   this is needed for the MC-truth link of created sim-hits  !  "
      exit(1)
      
    #  user.enableUI()
    part.adopt(user)

    #=================================================================================

    # Setup global filters for use in sensitive detectors
    try:
      self.filter.setupFilters( kernel )
    except RuntimeError as e:
      print "ERROR",str(e)
      exit(1)

    #=================================================================================
    # get lists of trackers and calorimeters in detectorDescription

    trk,cal = self.getDetectorLists( detectorDescription )

    # ---- add the trackers:
    try:
      self.__setupSensitiveDetectors( trk, simple.setupTracker, self.filter.tracker)
    except Exception as e:
      print "ERROR setting up sensitive detector", str(e)
      raise

  # ---- add the calorimeters:
    try:
      self.__setupSensitiveDetectors( cal, simple.setupCalorimeter, self.filter.calo )
    except Exception as e:
      print "ERROR setting up sensitive detector", str(e)
      raise

  #=================================================================================
    # Now build the physics list:
    _phys = self.physics.setupPhysics( kernel, name=self.physicsList )

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

    kernel.configure()
    kernel.initialize()

    ## GPS
    if self._g4gun is not None:
      self._g4gun.generator()
    if self._g4gps is not None:
      self._g4gps.generator()

    #DDG4.setPrintLevel(Output.DEBUG)

    startUpTime, _sysTime, _cuTime, _csTime, _elapsedTime = os.times()

    kernel.run()
    kernel.terminate()

    totalTimeUser, totalTimeSys, _cuTime, _csTime, _elapsedTime = os.times()
    if self.printLevel <= 3:
      eventTime = totalTimeUser - startUpTime
      perEventTime =  eventTime / float(self.numberOfEvents)
      print "DDSim            INFO  Total Time:   %3.2f s (User), %3.2f s (System)"% (totalTimeUser, totalTimeSys)
      print "DDSim            INFO  StartUp Time: %3.2f s, Event Processing: %3.2f s (%3.2f s/Event) " \
        % (startUpTime, eventTime, perEventTime)


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
    field.largest_step       = self.field.largest_step

  def __checkFileFormat(self, fileNames, extensions):
    """check if the fileName is allowed, note that the filenames are case
    sensitive, and in case of hepevt we depend on this to identify short and long versions of the content
    """
    if isinstance( fileNames, basestring ):
      fileNames = [fileNames]
    if not all( fileName.endswith( extensions ) for fileName in fileNames ):
      self._errorMessages.append("ERROR: Unknown fileformat for file: %s" % fileNames)
    return fileNames

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


  def __parseAllHelper( self, parsed ):
    """ parse all the options for the helper """
    parsedDict = vars(parsed)
    for name, obj in vars(self).iteritems():
      if isinstance( obj, ConfigHelper ):
        for var in obj.getOptions():
          key = "%s.%s" %( name,var )
          if key in parsedDict:
            try:
              obj.setOption( var, parsedDict[key] )
            except RuntimeError as e:
              self._errorMessages.append( "ERROR: %s " % e )

  def __checkOutputLevel(self, level):
    """return outputlevel as int so we don't have to import anything for faster startup"""
    try:
      level = int(level)
      if level < 1 or 7 < level:
        raise KeyError
      return level
    except ValueError:
      try:
        return outputLevel(level.upper())
      except ValueError:
        self._errorMessages.append( "ERROR: printLevel is neither integer nor string" )
        return -1
    except KeyError:
      self._errorMessages.append( "ERROR: printLevel '%s' unknown" % level )
      return -1

  def __addParametersToRunHeader( self ):
    """add the parameters to the (lcio) run Header"""
    runHeader = {}
    parameters = vars(self)
    for parName, parameter in parameters.iteritems():
      if isinstance( parameter, ConfigHelper ):
        options = parameter.getOptions()
        for opt,optionsDict in options.iteritems():
          runHeader["%s.%s"%(parName, opt)] = str(optionsDict['default'])
      else:
        runHeader[parName] = str(parameter)

    ### steeringFile content
    if self.steeringFile and os.path.exists(self.steeringFile) and os.path.isfile(self.steeringFile):
      with open(self.steeringFile) as sFile:
        runHeader["SteeringFileContent"] = sFile.read()

    ### macroFile content
    if self.macroFile and os.path.exists(self.macroFile) and os.path.isfile(self.macroFile):
      with open(self.macroFile) as mFile:
        runHeader["MacroFileContent"] = mFile.read()

    ### add command line
    if self._argv:
      runHeader["CommandLine"] = " ".join(self._argv)

    ### add current working directory (where we call from)
    runHeader["WorkingDirectory"] = os.getcwd()

    ### ILCSoft, LCGEo location from environment variables, names from init_ilcsoft.sh
    runHeader["ILCSoft_location"] = os.environ.get("ILCSOFT", "Unknown")
    runHeader["lcgeo_location"] = os.environ.get("lcgeo_DIR", "Unknown")

    ### add date
    runHeader["DateUTC"] = str(datetime.datetime.utcnow())+" UTC"

    ### add User
    import getpass
    runHeader["User"] = getpass.getuser()

    return runHeader

  def __setupSensitiveDetectors(self, detectors, setupFuction, defaultFilter=None):
    """ attach sensitive detector actions for all subdetectors
    can be steered with the `Action` ConfigHelpers

    :param detectors: list of detectors
    :param setupFunction: function used to register the sensitive detector
    """
    for det in detectors:
      print 'Setting up SD for %s' % det
      action = None
      for pattern in self.action.mapActions:
        if pattern.lower() in det.lower():
          action = self.action.mapActions[pattern]
          print  '       replace default action with : ' , action 
          break
      seq,act = setupFuction( det, type=action )
      self.filter.applyFilters( seq, det, defaultFilter )

      ##set detailed hit creation mode for this
      if self.enableDetailedShowerMode:
        if isinstance(act, list):
          for a in act:
            a.HitCreationMode = 2
        else:
          act.HitCreationMode = 2

  def __printSteeringFile( self, parser):
    """print the parameters formated as a steering file"""

    steeringFileBase="""from DDSim.DD4hepSimulation import DD4hepSimulation
from SystemOfUnits import mm, GeV, MeV
SIM = DD4hepSimulation()

"""
    optionDict = parser._option_string_actions
    parameters = vars(self)
    for parName, parameter in sorted(parameters.items(), sortParameters ):
      if parName.startswith("_"):
        continue
      if isinstance( parameter, ConfigHelper ):
        steeringFileBase += "\n\n"
        steeringFileBase += "################################################################################\n"
        steeringFileBase += "## %s \n" % "\n## ".join( parameter.__doc__.splitlines() )
        steeringFileBase += "################################################################################\n"
        options = parameter.getOptions()
        for opt, optionsDict in sorted( options.iteritems(), sortParameters ):
          if opt.startswith("_"):
            continue
          parValue = optionsDict['default']
          if isinstance(optionsDict.get('help'), basestring):
            steeringFileBase += "\n## %s\n" % "\n## ".join(optionsDict.get('help').splitlines())
          ## add quotes if it is a string
          if isinstance( parValue, basestring ):
            steeringFileBase += "SIM.%s.%s = \"%s\"\n" %(parName, opt, parValue)
          else:
            steeringFileBase += "SIM.%s.%s = %s\n" %(parName, opt, parValue)
      else:
        ## get the docstring from the command line parameter
        optionObj = optionDict.get("--"+parName, None)
        if isinstance(optionObj, argparse._StoreAction ):
          steeringFileBase += "## %s\n" % "\n## ".join(optionObj.help.splitlines())
        ## add quotes if it is a string
        if isinstance(parameter, basestring):
          steeringFileBase += "SIM.%s = \"%s\"" %( parName, str(parameter))
        else:
          steeringFileBase += "SIM.%s = %s" %( parName, str(parameter))
        steeringFileBase += "\n"
    print steeringFileBase


  def _consistencyChecks( self ):
    """Check if the requested setup makes sense, or if there is something preventing it from working correctly

    Appends error messages to self._errorMessages

    :returns: None
    """

    if not self.compactFile:
      self._errorMessages.append("ERROR: No geometry compact file provided")

    if self.runType == "batch":
      if not self.numberOfEvents:
        self._errorMessages.append("ERROR: Batch mode requested, but did not set number of events")
      if not self.inputFiles and not self.enableGun:
        self._errorMessages.append("ERROR: Batch mode requested, but did not set inputFile(s) or gun")

    if self.inputFiles and ( self.enableG4Gun or self.enableG4GPS ):
      self._errorMessages.append("ERROR: Cannot use both inputFiles and Geant4Gun or GeneralParticleSource")

    if self.enableGun and ( self.enableG4Gun or self.enableG4GPS ):
      self._errorMessages.append("ERROR: Cannot use both DD4hepGun and Geant4 Gun or GeneralParticleSource")


  def _enablePrimaryHandler( self ):
    """ the geant4 Gun or GeneralParticleSource cannot be used together with the PrimaryHandler.
        Particles would be simulated multiple times

    :returns: True or False
    """
    enablePrimaryHandler = not (self.enableG4Gun or self.enableG4GPS)
    if enablePrimaryHandler:
      print "Enabling the PrimaryHandler"
    else:
      print "Disabling the PrimaryHandler"
    return enablePrimaryHandler

################################################################################
### MODULE FUNCTIONS GO HERE
################################################################################

def sortParameters( parA, parB ):
  """sort the parameters by name: first normal parameters, then set of
  parameters based on ConfigHelper objects
  """
  parTypeA = parA[1]
  parTypeB = parB[1]
  if isinstance(parTypeA, ConfigHelper) and isinstance(parTypeB, ConfigHelper):
    return 1 if str(parA[0]) > str(parB[0]) else -1
  elif isinstance(parTypeA, ConfigHelper):
    return 1
  elif isinstance(parTypeB, ConfigHelper):
    return -1
  else:
    return 1 if str(parA[0]) > str(parB[0]) else -1

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
