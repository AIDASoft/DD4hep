###############################################################################
# File for running Brunel with default options (2008 real data,.mdf in,.dst out)
###############################################################################
# Syntax is:
#   gaudirun.py Brunel-Default.py <someDataFiles>.py
###############################################################################
from __future__ import absolute_import, unicode_literals
from Gaudi.Configuration  import *
from Configurables import LHCbConfigurableUser, LHCbApp, CondDB, ToolSvc, EventSelector
from Configurables import LoadDD4hepDet, LbDD4hepExample
import GaudiKernel.ProcessJobOptions

# import DD4hep
# DD4hep.setPrintLevel(DD4hep.OutputLevel.DEBUG)


class MyTest(LHCbConfigurableUser):
    # Steering options
    __slots__ = {
        "DDDBtag"         : ""
        ,"CondDBtag"       : ""
        ,"UseDBSnapshot" : False
        ,"PartitionName" : "LHCb"
        ,"DBSnapshotDirectory" : "/group/online/hlt/conditions"
        }

    _propertyDocDct = {
        'DDDBtag'      : """ Tag for DDDB """
        ,'CondDBtag'    : """ Tag for CondDB """
        ,"UseDBSnapshot" : """Use a snapshot for velo position and rich pressure"""
        ,"PartitionName" : """Name of the partition when running (needed to find DB: '', 'FEST', or 'LHCb'"""
        ,"DBSnapshotDirectory" : """Local Directory where the snapshot is"""
        }

    ## Apply the configuration
    def __apply_configuration__(self):
        actor = LoadDD4hepDet()
        actor.OutputLevel = DEBUG
        #actor.SetupPlugins    = ['DDCondDetElementMapping','DDDB_AssignConditions','DDDB_DetectorConditionKeysDump']
        actor.SetupPlugins    = ['DDDB_DetectorDump','DD4hep_InteractiveUI','DD4hep_Rint']
        actor.DumpDetElements = 0
        actor.DumpDetVolumes  = 0
        actor.DumpConditions  = ""
        #actor.DumpConditions  = "DDDB_DetElementConditionDump"
        #actor.DumpAlignments  = "DDDB_AlignmentDump"
        #actor.DumpAlignments  = "DDDB_DerivedCondTest"
        actor.ScanConditions  = 0
        pxml =   None
        try:
            pxml = os.path.join(os.path.dirname(os.path.realpath(__file__)), "Parameters.xml")
        except:
            pxml =  os.path.join(os.getcwd(), "Parameters.xml")
        actor.Parameters = "file://" + pxml
        actor.VisAttrs = os.path.join(os.getcwd(), "Visattrs.xml")
        #actor.Config     = [os.path.join(os.getcwd(), "apps/DD4hep/examples/DDDB/data/VPOnly.xml")]

        example_alg = LbDD4hepExample()
        ApplicationMgr().TopAlg = [actor, example_alg]

# Just instantiate the configurable...
theApp = MyTest()
ToolSvc.LogLevel=DEBUG
cdb = CondDB()
tag = { "DDDB":     ''
        , "LHCBCOND": 'default'
        #, "SIMCOND" : 'upgrade/dd4hep'
        , "SIMCOND" : ''
        , "ONLINE"  : 'fake'
        }
cdb.Tags = tag
cdb.setProp('IgnoreHeartBeat', True)
cdb.setProp('EnableRunChangeHandler', True)
#cdb.LogFile = "/tmp/cdb.log"
cdb.Upgrade = True
theApp.setOtherProps( cdb, [ 'UseDBSnapshot',
                             'DBSnapshotDirectory',
                             'PartitionName' ])

#-- Use latest database tags for real data
#LHCbApp().DDDBtag   = ""
#LHCbApp().CondDBtag = "default"
LHCbApp().EvtMax    = 5
LHCbApp().DataType = "Upgrade"
LHCbApp().Simulation = True
EventSelector().PrintFreq = 1

from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles([
    'PFN:Gauss/Boole.xdigi',
])
