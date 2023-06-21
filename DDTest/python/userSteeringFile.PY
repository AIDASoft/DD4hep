from DDSim.DD4hepSimulation import DD4hepSimulation
from g4units import mm, GeV, MeV
SIM = DD4hepSimulation()

## The compact XML file, or multiple compact files, if the last one is the closer.
SIM.compactFile = []
## Lorentz boost for the crossing angle, in radian!
SIM.crossingAngleBoost = 0.0
SIM.enableDetailedShowerMode = False
SIM.enableG4GPS = False
SIM.enableG4Gun = False
SIM.enableGun = False
## InputFiles for simulation .stdhep, .slcio, .HEPEvt, .hepevt, .pairs, .hepmc, .hepmc.gz, .hepmc.xz, .hepmc.bz2, .hepmc3, .hepmc3.gz, .hepmc3.xz, .hepmc3.bz2, .hepmc3.tree.root files are supported
SIM.inputFiles = []
## Macro file to execute for runType 'run' or 'vis'
SIM.macroFile = ""
## number of events to simulate, used in batch mode
SIM.numberOfEvents = 0
## Outputfile from the simulation: .slcio, edm4hep.root and .root output files are supported
SIM.outputFile = "dummyOutput.slcio"
## Physics list to use in simulation
SIM.physicsList = None
## Verbosity use integers from 1(most) to 7(least) verbose
## or strings: VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL, ALWAYS
SIM.printLevel = 3
## The type of action to do in this invocation
## batch: just simulate some events, needs numberOfEvents, and input file or gun
## vis: enable visualisation, run the macroFile if it is set
## qt: enable visualisation in Qt shell, run the macroFile if it is set
## run: run the macroFile and exit
## shell: enable interactive session
SIM.runType = "batch"
## Skip first N events when reading a file
SIM.skipNEvents = 0
## Steering file to change default behaviour
SIM.steeringFile = None
## FourVector of translation for the Smearing of the Vertex position: x y z t
SIM.vertexOffset = [0.0, 0.0, 0.0, 0.0]
## FourVector of the Sigma for the Smearing of the Vertex position: x y z t
SIM.vertexSigma = [0.0, 0.0, 0.0, 0.0]


################################################################################
## Helper holding sensitive detector actions.
## 
##   The default tracker and calorimeter actions can be set with
## 
##   >>> SIM = DD4hepSimulation()
##   >>> SIM.action.tracker=('Geant4TrackerWeightedAction', {'HitPositionCombination': 2, 'CollectSingleDeposits': False})
##   >>> SIM.action.calo = "Geant4CalorimeterAction"
## 
##   The default sensitive actions for calorimeters and trackers are applied based on the sensitive type.
##   The list of sensitive types can be changed with
## 
##   >>> SIM = DD4hepSimulation()
##   >>> SIM.action.trackerSDTypes = ['tracker', 'myTrackerSensType']
##   >>> SIM.calor.calorimeterSDTypes = ['calorimeter', 'myCaloSensType']
## 
##   For specific subdetectors specific sensitive detectors can be set based on patterns in the name of the subdetector.
## 
##   >>> SIM = DD4hepSimulation()
##   >>> SIM.action.mapActions['tpc'] = "TPCSDAction"
## 
##   and additional parameters for the sensitive detectors can be set when the map is given a tuple
## 
##   >>> SIM = DD4hepSimulation()
##   >>> SIM.action.mapActions['ecal'] =( "CaloPreShowerSDAction", {"FirstLayerNumber": 1} )
## 
##    
################################################################################

##  set the default calorimeter action 
SIM.action.calo = "Geant4ScintillatorCalorimeterAction"

## List of patterns matching sensitive detectors of type Calorimeter.
SIM.action.calorimeterSDTypes = ['calorimeter']

## Create a map of patterns and actions to be applied to sensitive detectors.
## 
##     Example: if the name of the detector matches 'tpc' the TPCSDAction is used.
## 
##       SIM.action.mapActions['tpc'] = "TPCSDAction"
##     
SIM.action.mapActions = {}

##  set the default tracker action 
SIM.action.tracker = ('Geant4TrackerWeightedAction', {'HitPositionCombination': 2, 'CollectSingleDeposits': False})

## List of patterns matching sensitive detectors of type Tracker.
SIM.action.trackerSDTypes = ['tracker']


################################################################################
## Configuration for the magnetic field (stepper) 
################################################################################
SIM.field.delta_chord = 0.25
SIM.field.delta_intersection = 0.001
SIM.field.delta_one_step = 0.01
SIM.field.eps_max = 0.001
SIM.field.eps_min = 5e-05
SIM.field.equation = "Mag_UsualEqRhs"
SIM.field.largest_step = 10000.0
SIM.field.min_chord_step = 0.01
SIM.field.stepper = "ClassicalRK4"


################################################################################
## Configuration for sensitive detector filters
## 
##   Set the default filter for 'tracker'
##   >>> SIM.filter.tracker = "edep1kev"
##   Use no filter for 'calorimeter' by default
##   >>> SIM.filter.calo = ""
## 
##   Assign a filter to a sensitive detector via pattern matching
##   >>> SIM.filter.mapDetFilter['FTD'] = "edep1kev"
## 
##   Or more than one filter:
##   >>> SIM.filter.mapDetFilter['FTD'] = ["edep1kev", "geantino"]
## 
##   Don't use the default filter or anything else:
##   >>> SIM.filter.mapDetFilter['TPC'] = None ## or "" or []
## 
##   Create a custom filter. The dictionary is used to instantiate the filter later on
##   >>> SIM.filter.filters['edep3kev'] = dict(name="EnergyDepositMinimumCut/3keV", parameter={"Cut": 3.0*keV} )
## 
##    
################################################################################

## 
##     default filter for calorimeter sensitive detectors;
##     this is applied if no other filter is used for a calorimeter
##     
SIM.filter.calo = "edep0"

##  list of filter objects: map between name and parameter dictionary 
SIM.filter.filters = {'geantino': {'name': 'GeantinoRejectFilter/GeantinoRejector', 'parameter': {}}, 'edep1kev': {'name': 'EnergyDepositMinimumCut', 'parameter': {'Cut': 0.001}}, 'edep0': {'name': 'EnergyDepositMinimumCut/Cut0', 'parameter': {'Cut': 0.0}}}

##  a map between patterns and filter objects, using patterns to attach filters to sensitive detector 
SIM.filter.mapDetFilter = {}

##  default filter for tracking sensitive detectors; this is applied if no other filter is used for a tracker
SIM.filter.tracker = "edep1kev"


################################################################################
## Configuration for the Detector Construction. 
################################################################################
SIM.geometry.dumpGDML = ""
SIM.geometry.dumpHierarchy = 0

## Print Debug information about Elements
SIM.geometry.enableDebugElements = False

## Print Debug information about Materials
SIM.geometry.enableDebugMaterials = False

## Print Debug information about Placements
SIM.geometry.enableDebugPlacements = False

## Print Debug information about Reflections
SIM.geometry.enableDebugReflections = False

## Print Debug information about Regions
SIM.geometry.enableDebugRegions = False

## Print Debug information about Shapes
SIM.geometry.enableDebugShapes = False

## Print Debug information about Surfaces
SIM.geometry.enableDebugSurfaces = False

## Print Debug information about Volumes
SIM.geometry.enableDebugVolumes = False

## Print information about placements
SIM.geometry.enablePrintPlacements = False

## Print information about Sensitives
SIM.geometry.enablePrintSensitives = False


################################################################################
## Configuration for the GuineaPig InputFiles 
################################################################################

## Set the number of pair particles to simulate per event.
##     Only used if inputFile ends with ".pairs"
##     If "-1" all particles will be simulated in a single event
##     
SIM.guineapig.particlesPerEvent = "-1"


################################################################################
## Configuration for the DDG4 ParticleGun 
################################################################################

##  direction of the particle gun, 3 vector 
SIM.gun.direction = (0, 0, 1)

## choose the distribution of the random direction for theta
## 
##     Options for random distributions:
## 
##     'uniform' is the default distribution, flat in theta
##     'cos(theta)' is flat in cos(theta)
##     'eta', or 'pseudorapidity' is flat in pseudorapity
##     'ffbar' is distributed according to 1+cos^2(theta)
## 
##     Setting a distribution will set isotrop = True
##     
SIM.gun.distribution = None

## The kinetic energy for the particle gun.
## 
## If not None, it will overwrite the setting of momentumMin and momentumMax
SIM.gun.energy = None

## Maximal pseudorapidity for random distibution (overrides thetaMin)
SIM.gun.etaMax = None

## Minimal pseudorapidity for random distibution (overrides thetaMax)
SIM.gun.etaMin = None

##  isotropic distribution for the particle gun
## 
##     use the options phiMin, phiMax, thetaMin, and thetaMax to limit the range of randomly distributed directions
##     if one of these options is not None the random distribution will be set to True and cannot be turned off!
##     
SIM.gun.isotrop = False

## Maximal momentum when using distribution (default = 0.0)
SIM.gun.momentumMax = 10000.0

## Minimal momentum when using distribution (default = 0.0)
SIM.gun.momentumMin = 0.0
SIM.gun.multiplicity = 1
SIM.gun.particle = "mu-"

## Maximal azimuthal angle for random distribution
SIM.gun.phiMax = None

## Minimal azimuthal angle for random distribution
SIM.gun.phiMin = None

##  position of the particle gun, 3 vector 
SIM.gun.position = (0.0, 0.0, 0.0)

## Maximal polar angle for random distribution
SIM.gun.thetaMax = None

## Minimal polar angle for random distribution
SIM.gun.thetaMin = None


################################################################################
## Configuration for the hepmc3 InputFiles 
################################################################################

## Set the name of the attribute contraining color flow information index 0.
SIM.hepmc3.Flow1 = "flow1"

## Set the name of the attribute contraining color flow information index 1.
SIM.hepmc3.Flow2 = "flow2"

## Set to false if the input should be opened with the hepmc2 ascii reader.
## 
##     If ``True`` a  '.hepmc' file will be opened with the HEPMC3 Reader Factory.
## 
##     Defaults to true if DD4hep was build with HEPMC3 support.
##     
SIM.hepmc3.useHepMC3 = False


################################################################################
## Configuration for Input Files. 
################################################################################

## Set one or more functions to configure input steps.
## 
##     The functions must take a ``DD4hepSimulation`` object as their only argument and return the created generatorAction
##     ``gen`` (for example).
## 
##     For example one can add this to the ddsim steering file:
## 
##       def exampleUserPlugin(dd4hepSimulation):
##         '''Example code for user created plugin.
## 
##         :param DD4hepSimulation dd4hepSimulation: The DD4hepSimulation instance, so all parameters can be accessed
##         :return: GeneratorAction
##         '''
##         from DDG4 import GeneratorAction, Kernel
##         # Geant4InputAction is the type of plugin, Cry1 just an identifier
##         gen = GeneratorAction(Kernel(), 'Geant4InputAction/Cry1' , True)
##         # CRYEventReader is the actual plugin, steeringFile its constructor parameter
##         gen.Input = 'CRYEventReader|' + 'steeringFile'
##         # we can give a dictionary of Parameters that has to be interpreted by the setParameters function of the plugin
##         gen.Parameters = {'DataFilePath': '/path/to/files/data'}
##         gen.enableUI()
##         return gen
## 
##       SIM.inputConfig.userInputPlugin = exampleUserPlugin
## 
##     Repeat function definition and assignment to add multiple input steps
## 
##     
SIM.inputConfig.userInputPlugin = []


################################################################################
## Configuration for the generator-level InputFiles 
################################################################################

## Set the name of the collection containing the MCParticle input.
##     Default is "MCParticle".
##     
SIM.lcio.mcParticleCollectionName = "MCParticle"


################################################################################
## Configuration for the LCIO output file settings 
################################################################################

## The event number offset to write in slcio output file. E.g setting it to 42 will start counting events from 42 instead of 0
SIM.meta.eventNumberOffset = 0

## Event parameters to write in every event. Use C/F/I ids to specify parameter type. E.g parameterName/F=0.42 to set a float parameter
SIM.meta.eventParameters = []

## The run number offset to write in slcio output file. E.g setting it to 42 will start counting runs from 42 instead of 0
SIM.meta.runNumberOffset = 0


################################################################################
## Configuration for the output levels of DDG4 components 
################################################################################

## Output level for geometry.
SIM.output.geometry = 2

## Output level for input sources
SIM.output.inputStage = 3

## Output level for Geant4 kernel
SIM.output.kernel = 3

## Output level for ParticleHandler
SIM.output.part = 3

## Output level for Random Number Generator setup
SIM.output.random = 6


################################################################################
## Configuration for Output Files. 
################################################################################

## Set a function to configure the outputFile.
## 
##     The function must take a ``DD4hepSimulation`` object as its only argument and return ``None``.
## 
##     For example one can add this to the ddsim steering file:
## 
##       def exampleUserPlugin(dd4hepSimulation):
##         '''Example code for user created plugin.
## 
##         :param DD4hepSimulation dd4hepSimulation: The DD4hepSimulation instance, so all parameters can be accessed
##         :return: None
##         '''
##         from DDG4 import EventAction, Kernel
##         dd = dd4hepSimulation  # just shorter variable name
##         evt_root = EventAction(Kernel(), 'Geant4Output2ROOT/' + dd.outputFile, True)
##         evt_root.HandleMCTruth = True or False
##         evt_root.Control = True
##         if not dd.outputFile.endswith(dd.outputConfig.myExtension):
##           output = dd.outputFile + dd.outputConfig.myExtension
##         evt_root.Output = output
##         evt_root.enableUI()
##         Kernel().eventAction().add(evt_root)
##         return None
## 
##       SIM.outputConfig.userOutputPlugin = exampleUserPlugin
##       # arbitrary options can be created and set via the steering file or command line
##       SIM.outputConfig.myExtension = '.csv'
##     

def outputPlugin(dd4hepSimulation):
  from DDG4 import EventAction, Kernel
  dd = dd4hepSimulation  # just shorter variable name
  evt_root = EventAction(Kernel(), 'Geant4Output2ROOT/' + dd.outputFile, True)
  evt_root.HandleMCTruth = True or False
  evt_root.Control = True
  output = dd.outputFile
  if not dd.outputFile.endswith(dd.outputConfig.myExtension):
    output = dd.outputFile + dd.outputConfig.myExtension
  evt_root.Output = output
  evt_root.enableUI()
  Kernel().eventAction().add(evt_root)
  return None

SIM.outputConfig.myExtension = '.root'
SIM.outputConfig.userOutputPlugin = outputPlugin


################################################################################
## Configuration for the Particle Handler/ MCTruth treatment 
################################################################################

## Enable lots of printout on simulated hits and MC-truth information
SIM.part.enableDetailedHitsAndParticleInfo = False

##  Keep all created particles 
SIM.part.keepAllParticles = False

## Minimal distance between particle vertex and endpoint of parent after
##     which the vertexIsNotEndpointOfParent flag is set
##     
SIM.part.minDistToParentVertex = 2.2e-14

## MinimalKineticEnergy to store particles created in the tracking region
SIM.part.minimalKineticEnergy = 1.0

##  Printout at End of Tracking 
SIM.part.printEndTracking = False

##  Printout at Start of Tracking 
SIM.part.printStartTracking = False

## List of processes to save, on command line give as whitespace separated string in quotation marks
SIM.part.saveProcesses = ['Decay']

## Optionally enable an extended Particle Handler
SIM.part.userParticleHandler = "Geant4TCUserParticleHandler"


################################################################################
## Configuration for the PhysicsList 
################################################################################

## If true, add decay processes for all particles.
## 
##     Only enable when creating a physics list not based on an existing Geant4 list!
##     
SIM.physics.decays = False

## The name of the Geant4 Physics list.
SIM.physics.list = "FTFP_BERT"

##  location of particle.tbl file containing extra particles and their lifetime information
## 
##     For example in $DD4HEP/examples/DDG4/examples/particle.tbl
##     
SIM.physics.pdgfile = None

##  The global geant4 rangecut for secondary production
## 
##     Default is 0.7 mm as is the case in geant4 10
## 
##     To disable this plugin and be absolutely sure to use the Geant4 default range cut use "None"
## 
##     Set printlevel to DEBUG to see a printout of all range cuts,
##     but this only works if range cut is not "None"
##     
SIM.physics.rangecut = 0.7

## Set of PDG IDs that will not be passed from the input record to Geant4.
## 
##     Quarks, gluons and W's Z's etc should not be treated by Geant4
##     
SIM.physics.rejectPDGs = {1, 2, 3, 4, 5, 6, 3201, 3203, 4101, 4103, 21, 23, 24, 5401, 25, 2203, 5403, 3101, 3103, 4403, 2101, 5301, 2103, 5303, 4301, 1103, 4303, 5201, 5203, 3303, 4201, 4203, 5101, 5103, 5503}

## Set of PDG IDs for particles that should not be passed to Geant4 if their properTime is 0.
## 
##     The properTime of 0 indicates a documentation to add FSR to a lepton for example.
##     
SIM.physics.zeroTimePDGs = {17, 11, 13, 15}

def setupCerenkovScint(kernel):
     from DDG4 import PhysicsList
     seq = kernel.physicsList()

     scint = PhysicsList(kernel, 'Geant4ScintillationPhysics/ScintillationPhys')
     scint.VerboseLevel = 0
     scint.TrackSecondariesFirst = True
     scint.enableUI()
     seq.adopt(scint)

     cerenkov = PhysicsList(kernel, 'Geant4CerenkovPhysics/CerenkovPhys')
     cerenkov.VerboseLevel = 0
     cerenkov.MaxNumPhotonsPerStep = 10
     cerenkov.MaxBetaChangePerStep = 10.0
     cerenkov.TrackSecondariesFirst = True
     cerenkov.enableUI()
     seq.adopt(cerenkov)

     ph = PhysicsList(kernel, 'Geant4OpticalPhotonPhysics/OpticalGammaPhys')
     ph.addParticleConstructor('G4OpticalPhoton')
     ph.VerboseLevel = 0
     ph.enableUI()
     seq.adopt(ph)

     return None


SIM.physics.setupUserPhysics(setupCerenkovScint)


################################################################################
## Properties for the random number generator 
################################################################################

## If True, calculate random seed for each event basedon eventID and runID
## Allows reproducibility even whenSkippingEvents
SIM.random.enableEventSeed = False
SIM.random.file = None
SIM.random.luxury = 1
SIM.random.replace_gRandom = True
SIM.random.seed = None
SIM.random.type = None
