
examples/ClientTests
====================

A set of example detectors used to debug special conditions
or problematic assemblies from users.

LHeD_tracker
------------
by Ercan Pilicier
Detector constructors:  LheD_tracker_BeamPipe_geo.cpp  LheD_tracker_SiVertexBarrel_geo.cpp
Compact XML:            compact/LheD_tracker.xml
Script:                 scripts/LheD_tracker.py

Example of the LHeC vertex detector and the beampipe using elliptical tubes.
(TGeoEltu).


FCC_Hcal:
--------
by Carlos Solans
Detector constructor:  src/FCC_Hcal.cpp
Compact XML:           compact/FCC_HcalBarrel.xml

FCC Hadron calorimeter
Comes together with the Geant4 simulation script scripts/FCC_Hcal.py
If you want to run Geant4 bootstrapped by a ROOT-AClick, have a look at 
scripts/FCC_Hcal.C

IronCylinder:
-------------
by Markus Frank
Detector constructor:  src/IronCylinder_geo.cpp
Compact XML:           compact/IronCylinder.xml

Simple iron cylinder

MiniTel:
--------
Marco Clemecic & Anastasia Karachaliou
Detector constructor:  src/MiniTel.cpp
Compact XML:           compact/MiniTel.xml

Simple telescope like detector setup with a number of silicon pixel detectors.
Comes together with the Geant4 simulation script scripts/MiniTel.py

Assemblies_VXD:
--------------
Frank Gade
Detector constructor:  src/Assemblies_VXD_geo.cpp
Compact XML:           compact/Assemblies.xml

Test for nested assemblies using a mockup of the ILD vertex detector.
Test for the conversion of nested assemblies to Geant4 volumes.

