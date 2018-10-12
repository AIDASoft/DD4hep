
geoPluginRun -input file:../DD4hep/examples/DDUpgrade/data/load_LHCb.xml -ui -destroy -plugin DD4hep_ROOTGDMLExtract -output VP.gdml     -volname -path _dd_Structure_LHCb_BeforeMagnetRegion_UT
geoPluginRun -input file:../DD4hep/examples/DDUpgrade/data/load_LHCb.xml -ui -destroy -plugin DD4hep_ROOTGDMLExtract -output UT.gdml     -volname -path _dd_Structure_LHCb_BeforeMagnetRegion_UT
geoPluginRun -input file:../DD4hep/examples/DDUpgrade/data/load_LHCb.xml -ui -destroy -plugin DD4hep_ROOTGDMLExtract -output VP.gdml     -volname -path _dd_Structure_LHCb_BeforeMagnetRegion_VP
geoPluginRun -input file:../DD4hep/examples/DDUpgrade/data/load_LHCb.xml -ui -destroy -plugin DD4hep_ROOTGDMLExtract -output Rich1.gdml  -volname -path _dd_Structure_LHCb_BeforeMagnetRegion_Rich1
geoPluginRun -input file:../DD4hep/examples/DDUpgrade/data/load_LHCb.xml -ui -destroy -plugin DD4hep_ROOTGDMLExtract -output Rich2.gdml  -volname -path _dd_Structure_LHCb_AfterMagnetRegion_Rich2
geoPluginRun -input file:../DD4hep/examples/DDUpgrade/data/load_LHCb.xml -ui -destroy -plugin DD4hep_ROOTGDMLExtract -output Ecal.gdml   -volname -path _dd_Structure_LHCb_DownstreamRegion_Ecal
geoPluginRun -input file:../DD4hep/examples/DDUpgrade/data/load_LHCb.xml -ui -destroy -plugin DD4hep_ROOTGDMLExtract -output Hcal.gdml   -volname -path _dd_Structure_LHCb_DownstreamRegion_Hcal
geoPluginRun -input file:../DD4hep/examples/DDUpgrade/data/load_LHCb.xml -ui -destroy -plugin DD4hep_ROOTGDMLExtract -output FT.gdml     -volname -path _dd_Structure_LHCb_AfterMagnetRegion_T_FT
geoPluginRun -input file:../DD4hep/examples/DDUpgrade/data/load_LHCb.xml -ui -destroy -plugin DD4hep_ROOTGDMLExtract -output Muon.gdml   -volname -path _dd_Structure_LHCb_DownStreamRegion_Muon

