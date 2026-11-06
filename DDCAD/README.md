![DDCond](../doc/logo_small.png)

DDCAD: CAD support in DD4hep
============================

onsistent detector description is an integral part of all modern experiments and also the main motivation
behind the creation of DD4hep, which tries to address detector description in a broad sense including for,
example, geometry and the materials used in the device, additional parameters describing e.g. the detection
techniques, constants required for alignment and calibration, description of the readout structures and
conditions data. A central component of DD4hep is DDG4 which is a mechanism that converts arbitrary DD4hep
detector geometries to Geant4 and provides access to all Geant4 action stages. In addition to that DDG4
also offers a comprehensive plugins suite that includes handling of different IO formats, Monte Carlo
truth linking and a large set of segmentation and sensitive detector classes, allowing the simulation
of a wide variety of detector technologies. One of the last remaining open issues of detector description
was support for drawings from civil engineers for passive detector components. In this proceedings we
highlight recent developments in DD4hep/DDG4 that enable support for CAD drawings and generic tessellated
shapes and through the help of the library ASSIMP enable the import of a wide variety of
CAD formats, thus eliminating the need for writing complex re-implementations of CAD drawings in source code.

The corresponding publication can be found here
[EPJ Web of Conferences 251, 03015 (2021)](https://doi.org/10.1051/epjconf/202125103015)  or
see the [presentation](https://indico.cern.ch/event/948465/contributions/4324167/attachments/2245068/3809632/2021-05-18-CHEP21-DD4hep-Developments.pdf).

Some usage examples for DDCond can be found [here](../examples/DDCAD)


![HORIZON2020](../doc/usermanuals/DD4hep/figures/AIDA-2020.png)
