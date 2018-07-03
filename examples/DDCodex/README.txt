CODEX starting kit
==================


Small geometry driver for CODEX-b sketch.


First save a snapshot of the upgrade geometry in DD4hep
-------------------------------------------------------
You will need a proper checkout of the git repository of Ben provided for testing the
upgrade with DD4hep.

Comamnds:
$> `which python` `which gaudirun.py` GeoExtract.py
root [0] gDD4hepUI->saveROOT("Upgrade.root")
root [1] .q

Output:
DetElement-Info  INFO  ++            ->  +++++++++++++ Printout summary:
DetElement-Info  INFO  ++            ->  Number of conditions:              0  [  dto. empty:0]
DetElement-Info  INFO  ++            ->  Total Number of parameters:        0  [  0.000 Parameters/Condition]
DD4hepUI               Use the ROOT interpreter variable gDD4hepUI to interact with the detector description.
   ------------------------------------------------------------
  | Welcome to ROOT 6.10/06                http://root.cern.ch |
  |                               (c) 1995-2017, The ROOT Team |
  | Built for linuxx8664gcc                                    |
  | From tag v6-10-06, 19 September 2017                       |
  | Try '.help', '.demo', '.license', '.credits', '.quit'/'.q' |
   ------------------------------------------------------------

DD4hepRint       INFO  ++ Created ROOT interpreter instance for DD4hepUI.
root [0] gDD4hepUI->saveROOT("VPGeo.root")
Geometry2ROOT    INFO  +++ Dump geometry to root file:VPGeo.root
DD4hepRootPersistency       +++ No valid Volume manager. No nominals saved.
DD4hepRootPersistency       +++ Wrote 992746 Bytes of geometry data 'Geometry' to 'VPGeo.root'  [   0.772 seconds].
DD4hepRootPersistency       +++ Successfully saved geometry data to file.
(long) 1
root [1] .q


Then to execute:
----------------------------------------------------------------------------
$> geoPluginRun -ui -inter
...
root [0] gDD4hepUI->importROOT("Upgrade.root")
root [1] gDD4hepUI->fromXML("checkout/examples/DDCodex/compact/CODEX-b.xml")
root [2] gDD4hepUI->draw()


Output:
----------------------------------------------------------------------------
17:25:30-frankm~/SW/DD4hep_head_dbg.root_v6.12.06.g4_10.04.p01_MT/build$ geoPluginRun -ui -inter
PersistencyIO    INFO  +++ Set Streamer to dd4hep::OpaqueDataBlock
PersistencyIO    INFO  +++ Patching TGeoVolume.fUserExtension to persistent
PersistencyIO    INFO  +++ Patching TGeoNode.fUserExtension to persistent
Info in <TGeoManager::TGeoManager>: Geometry world, Detector Geometry created
geoPluginRun: No geometry input supplied. No geometry will be loaded.
DD4hepUI               Use the ROOT interpreter variable gDD4hepUI to interact with the detector description.
   ------------------------------------------------------------
  | Welcome to ROOT 6.12/06                http://root.cern.ch |
  |                               (c) 1995-2017, The ROOT Team |
  | Built for linuxx8664gcc                                    |
  | From tag v6-12-06, 9 February 2018                         |
  | Try '.help', '.demo', '.license', '.credits', '.quit'/'.q' |
   ------------------------------------------------------------

root [0] gDD4hepUI->importROOT("Upgrade.root")
DD4hepRootLoader INFO  +++ Read geometry from root file:Upgrade.root
Warning in <TGeoManager::Init>: Deleting previous geometry: world/Detector Geometry
Info in <TGeoManager::CloseGeometry>: Geometry loaded from file...
Info in <TGeoManager::SetTopVolume>: Top volume is world_volume. Master volume is world_volume
Info in <TGeoNavigator::BuildCache>: --- Maximum geometry depth set to 100
Info in <TGeoManager::Voxelize>: Voxelizing...
Error in <TGeoVoxelFinder::SortAll>: Volume lvAsicGroup16: Cannot make slices on any axis
Error in <TGeoVoxelFinder::SortAll>: Volume lvAsicGroup4: Cannot make slices on any axis
Error in <TGeoVoxelFinder::SortAll>: Volume lvAsicGroup8: Cannot make slices on any axis
Info in <TGeoManager::CountLevels>: max level = 11, max placements = 171
Info in <TGeoManager::CloseGeometry>: 18553873 nodes/ 2754 volume UID's in Detector Geometry
Info in <TGeoManager::CloseGeometry>: ----------------modeler ready----------------
DD4hepRootPersistency       +++ Fixed 0 segmentation objects.
DD4hepRootPersistency       +++ Volume manager NOT restored. [Was it ever up when saved?]
DD4hepRootPersistency       +++ Successfully loaded detector description from file:Upgrade.root  [   7.322 seconds]
(long) 1
root [1] gDD4hepUI->fromXML("checkout/examples/DDCodex/compact/CODEX-b.xml")
CODEX-b          INFO  COBEXb Envelope: dz=2000 r1=0 r2=400 beam-angle=1.0472 atan(cone)=0.0996687
CODEX-b          INFO  COBEXb Shield: Pb-shield-1  [Lead]       z=    700 dz=    150 r1=69.6526 r2=84.5782
CODEX-b          INFO  COBEXb Shield: Shield-veto  [Si]         z=    850 dz=     10 r1=84.5782 r2=85.5732
CODEX-b          INFO  COBEXb Shield: Pb-shield-2  [Lead]       z=    860 dz=     50 r1=85.5732 r2=90.5484
CODEX-b          INFO  COBEXb X_tot= 532
CODEX-b          INFO  COBEXb Module:  0 [Si]         x=-307.15 y=0 z=   1732 Dist:22 dx:25.4034 dz:44
CODEX-b          INFO  COBEXb Module:  1 [Si]         x=-281.747 y=0 z=   1776 Dist:22 dx:25.4034 dz:44
CODEX-b          INFO  COBEXb Module:  2 [Si]         x=-256.344 y=0 z=   1820 Dist:22 dx:25.4034 dz:44
CODEX-b          INFO  COBEXb Module:  3 [Si]         x=-230.94 y=0 z=   1864 Dist:32 dx:36.9504 dz:64
CODEX-b          INFO  COBEXb Module:  4 [Si]         x=-193.99 y=0 z=   1928 Dist:32 dx:36.9504 dz:64
CODEX-b          INFO  COBEXb Module:  5 [Si]         x=-157.039 y=0 z=   1992 Dist:32 dx:36.9504 dz:64
CODEX-b          INFO  COBEXb Module:  6 [Si]         x=-120.089 y=0 z=   2056 Dist:32 dx:36.9504 dz:64
CODEX-b          INFO  COBEXb Module:  7 [Si]         x=-83.1384 y=0 z=   2120 Dist:32 dx:36.9504 dz:64
CODEX-b          INFO  COBEXb Module:  8 [Si]         x=-46.188 y=0 z=   2184 Dist:32 dx:36.9504 dz:64
CODEX-b          INFO  COBEXb Module:  9 [Si]         x=-9.2376 y=0 z=   2248 Dist:32 dx:36.9504 dz:64
CODEX-b          INFO  COBEXb Module: 10 [Si]         x=27.7128 y=0 z=   2312 Dist:32 dx:36.9504 dz:64
CODEX-b          INFO  COBEXb Module: 11 [Si]         x=64.6632 y=0 z=   2376 Dist:42 dx:48.4974 dz:84
CODEX-b          INFO  COBEXb Module: 12 [Si]         x=113.161 y=0 z=   2460 Dist:42 dx:48.4974 dz:84
CODEX-b          INFO  COBEXb Module: 13 [Si]         x=161.658 y=0 z=   2544 Dist:42 dx:48.4974 dz:84
CODEX-b          INFO  COBEXb Module: 14 [Si]         x=210.155 y=0 z=   2628 Dist:42 dx:48.4974 dz:84
CODEX-b          INFO  COBEXb Module: 15 [Si]         x=258.653 y=0 z=   2712 Dist:42 dx:48.4974 dz:84
Compact          INFO  ++ Converted subdetector:COBEXb of type DD4hep_CODEXb 
root [2] gDD4hepUI->draw()
Info in <TGeoManager::SetVisLevel>: Automatic visible depth disabled
Info in <TCanvas::MakeDefCanvas>:  created default TCanvas with name c1
root [3] 



Running Standalone
==================

Display:

$> geoDisplay -input file:checkout/examples/DDCodex/compact/CODEX-b-alone.xml

DDG4 simulation:

$> python ../DD4hep//examples/DDCodex/python/CODEX-b-alone.py

Visualisation of the hits:

$> root.exe ../DD4hep/examples/DDEve/DDEve.C


