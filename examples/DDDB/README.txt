
Remarks:
Transformation computation is ambiguous:
Either the old magnet is correct or the upgrade magnet.
See src/plugins/CondDB2DDDB.cpp apply_trafo(....)

To show the display:
-----------------
$> geoPluginRun -destroy \
   -plugin DDDB_Executor -loader DDDB_FileReader \
   -params file:/tmp/frankm/DDDB/Parameters.xml \
   -vis -attr file:/tmp/frankm/DDDB/Visattrs.xml \
   -input file:/tmp/frankm/DDDB/DDDB/lhcb.xml \
   -config DD4hep_ConditionsManagerInstaller

Optional:
   -config DD4hep_XMLLoader file:checkout/examples/DDDB/data/Debug.xml
