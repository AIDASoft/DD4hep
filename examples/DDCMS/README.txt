Package DDCMS
=============

This is a small test development to demonstrate how to
convert a small part of the CMS geometry to DD4hep.
It is by no means complete.

         M.Frank,   September 2017
         

geoPluginRun -input file:checkout/examples/DDCMS/data/cms_tracker.xml \
             -ui -interpreter \
             -plugin DD4hep_PlacedVolumeProcessor -recursive -processor DDCMS_DetElementCreator \
             -plugin DD4hep_VolumeManager \
             -plugin DD4hep_DetectorDump -sensitive \
             -plugin DD4hep_VolumeDump   -sensitive -volids \
             -plugin DD4hep_GeometryDisplay
             -end-plugin
