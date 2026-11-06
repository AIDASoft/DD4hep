![DDG3](../doc/logo_small.png)

DDG3: Geant3 gateway to DD4hep
======================================

DDG3 allows to import Geant3 geometries from a program to DD4hep.
This is useful if for example data of older experiments, where a
geometry description is only available in Geant3 code such as
the LEP experiments at CERN, where FCC developments try to use these
data for detector optimizations etc.

This procedure has 3 steps:

- Firstly the Geant3 program needs to be relinked and the files in the directory DDG3/geant3
  Ensure that the contained routines are used by the linker instead of the original
  subroutines from Geant3.
  You may need to call the subroutines explicitly from `Fortran`:
  ```
  CALL GPMATE(0)
  CALL GPROTM(0)
  CALL GPTMED(0)
  CALL GPVOLU(0)
  ```
- Run the program. It may exit after the geometry is instantiated.
  Capture the output in a file. Then analyse the output:
  ```
  $> python <path>/extract_geometry.py -h
usage: extract_geant3_geometry [-h] [-D] [-I GEOMETRY_INPUT] [-O GEOMETRY_OUTPUT]

Create DD4hep geometry

options:
  -h, --help            show this help message and exit
  -D, --debug           Enable python debugging (invoke pdb)
  -I, --input GEOMETRY_INPUT
                        Location of the geometry input file
  -O, --output GEOMETRY_OUTPUT
                        Location of the geometry output file
```
or explicitly:
```
python ../galeph/extract_geometry.py -I aleph.geometry.txt -O aleph.geom.cpp
+++ Successfully processed Geant3 input file: aleph.geometry.txt
+++ Parsed  146  material
+++         137  media
+++         313  volumes
+++         6  divisions
+++         1021  placements
```

The resulting file should contain lines like:
```
aleph_geometry.add_material( {  "index":       1, "name": "HYDROGEN            ", ... } )
...
aleph_geometry.add_placement( {   "index":    1, "name": "CDET", ... } )
...
aleph_geometry.add_medium( {   "index":      18, "name": "VDET KEVLARGLUE     ", ... } )
...
aleph_geometry.add_rotation( {  "index":    22, "flag":   633, "theta1": 0.9000E+02, ... } )
...
aleph_geometry.add_volume( {   "index":    1, "name": "ALEF", ... } )
...
aleph_geometry.add_division( {  "name": "EBAR", "volume": "EBAL", ... } )
```

The python script parses these lines and ejects the corresponding DD4hep C++ code.
Replace the file examples/DDG3/src/aleph_geometry.cpp.

- Compile and link the resulting geometry file in the DDG3 example.

You will need to inject
- visulisation attributes
- structure definitions (DetElement hierarchy)
- limits, readout structures and segmentation components by hand.

These cannot be automatically generated.
You can get inspired how it was done for the Aleph geometry or write directly
code by hand and inject the corresponding entities programmatically
when traversing the geometry hierarchy.


## License and Copyright

Copyright (C), Organisation européenne pour la recherche nucléaire (CERN)

DD4hep is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

DD4hep is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License long with this program. If not, see [https://www.gnu.org/licenses/](https://www.gnu.org/licenses/).

![HORIZON2020](../doc/usermanuals/DD4hep/figures/AIDA-2020.png)
