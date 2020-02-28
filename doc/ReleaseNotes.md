# v01-12

* 2020-02-26 Andre Sailer ([PR#637](https://github.com/aidasoft/dd4hep/pull/637))
  - LcioEventReader: fix setting of color flow, second coordinate was never set

* 2020-02-25 Markus Frank ([PR#633](https://github.com/aidasoft/dd4hep/pull/633))
  - Added basic implementation for CAD interface to load shapes from CAD files. There is an open issue how to best embed the volume/shape loading from CAD files into the existing infrastructure. See for [presentation](https://indico.cern.ch/event/885083/contributions/3758180/attachments/1990793/3318984/2020-02-20-DD4hep-Tessellated-Shapes.pdf). This feature is only available if build against ROOT 6.22 or higher. 
  - DDCAD used the [assimp](https://github.com/assimp/assimp) librray to interprete the CAD files.
  - Simplify grammar instantiation. 
    - For grammars to be parsed with `boost::spirit` simply include
      `#include "DD4hep/detail/Grammar_parsed.h"`
    - For Grammars which should **not be parsed** with `boost::spirit` include
      `#include "DD4hep/detail/Grammar_unparsed.h"`
    - To instantiate the code and to register the instantiation call:
       `template <> dd4hep::Grammar<my-class>;`
    - If the Grammar is supposed to be registered call instead/in addition:
      `static auto s_registry = GrammarRegistry::pre_note<my-class>();`
     - All other macros are gone.

* 2020-02-21 Andre Sailer ([PR#631](https://github.com/aidasoft/dd4hep/pull/631))
  - Only build DDG4 Python dependent libraries and pcms if Python and PyROOT are found

* 2020-02-21 Andre Sailer ([PR#627](https://github.com/aidasoft/dd4hep/pull/627))
  - MakeGaudiMap: better inference of listcomponents and library location by using target properties
  - MakeGauiMap: Detailed information about the command is not only printed when VERBOSE=1

* 2020-02-19 Marko Petric ([PR#626](https://github.com/aidasoft/dd4hep/pull/626))
  - Check in python modules that load `libglapi` is only called if the library is not already loaded.

* 2020-02-19 Sebastien Ponce ([PR#624](https://github.com/aidasoft/dd4hep/pull/624))
  - Fixed bug in the `dd4hep_add_test_reg` function which was only taking into account the last dependency of a test when several were present.

* 2020-02-19 Marko Petric ([PR#622](https://github.com/aidasoft/dd4hep/pull/622))
  - Add missing conversion from `unicode` to `str` for python 2 case in `DDrec.py`
  - Add test to check dd4hep python module imports
  - Fixes for tests from examples:
     - typo in tessellated example
     - make gdml read test dependent on the gdml write test
  - Add check to test if the version of python used to build ROOT is the same to the version detected by CMake to build DD4hep (works only from ROOT 6.20)

* 2020-02-17 Andre Sailer ([PR#621](https://github.com/aidasoft/dd4hep/pull/621))
  - Cmake: dd4hep_add_dictionary: directly use command, no longer created bash script to be called. See details of call with `make VERBOSE=1`
  - PluginServiceV2: use `boost::split` instead of walking of char arrays, fix bug when two colons are in the environment variable. Fixes #600

* 2020-02-13 Sebastien Ponce ([PR#620](https://github.com/aidasoft/dd4hep/pull/620))
  - added missing dependency of Persist_CLICSiD_Geant4 test on Persist_CLICSiD_Save_LONGTEST

* 2020-02-11 Marko Petric ([PR#618](https://github.com/aidasoft/dd4hep/pull/618))
  - Fix from which version of ROOT `TGeoTessellated` is supported (starting only in 6.22)
  - Fix location of test file in tessellated example

* 2020-02-06 Hadrien Grasland ([PR#613](https://github.com/aidasoft/dd4hep/pull/613))
  - Use the official CMake configuration mechanism provided by TBB >= 2017 U7.
  - Do not delete move constructors in DigiKernel as this breaks current versions of TBB.

* 2020-02-05 Markus Frank ([PR#610](https://github.com/aidasoft/dd4hep/pull/610))
  - Thanks to @agheata, ROOT now supports tessellated shapes. These new shapes are now supported by DD4hep including the automatic translation to Geant4. The tessellated shapes feature is only supported when compiling against ROOT 6.20.0 or higher.
  - Added a basic shape test for the tessellated shape.

* 2020-02-03 Markus Frank ([PR#608](https://github.com/aidasoft/dd4hep/pull/608))
  - SInce we do not use DTD checking all DTD links from lcdd xml files have been removed in examples. 
  - Add 2 examples to check the usage of NTP and STP temperature/pressure conditions

* 2020-01-09 Markus Frank ([PR#606](https://github.com/aidasoft/dd4hep/pull/606))
  - Resolves #605 
  - There was an apparent bug when cloning `DetElement` trees. Not only for the top element, which should receive a new ID, this top ID was propagated to all children. This was clearly wrong. The correct solution is: 
      - Top element gets a new ID
      - Children keep their ID
  - If a user wants to preserve the old functionality the flag `DetElement::PROPAGATE_PARENT_ID` must be set in the clone statement.

* 2019-12-18 Markus Frank ([PR#604](https://github.com/aidasoft/dd4hep/pull/604))
  - Allow to set ambient temperature and pressure for all materials. 
      - predefined settings: STP, NTP
      - Example:
         - `examples/ClientTests/compact/Check_Air.xml`
       - Apply global temperature and pressure for all materials and especially for one single material.
       - Verification for Geant4: `examples/ClientTests/scripts/Check_Air.py`
  - DDG4: allow Geant4 messengers to pass parameter string.

* 2019-12-16 Markus Frank ([PR#603](https://github.com/aidasoft/dd4hep/pull/603))
  - Resolves #599
  - Resolves #601 
  - Work on #595

* 2019-11-27 Markus Frank ([PR#597](https://github.com/aidasoft/dd4hep/pull/597))
  - Intermediate release to store improvements in the development of DDDigi:
    Start implementing noise chains
  - Fix cmake build if CLHEP is included in Geant4 and not external.

* 2019-11-16 Markus Frank ([PR#596](https://github.com/aidasoft/dd4hep/pull/596))
  - Requre that all materials must be entered explicitly.
    - The default ROOT element table is disabled. 
  - Fix examples, which relied on the default ROOT elements being present
  - Fix #595 for DDCMS:
    - The density was not re-normalized to the units TGeo was expecting. Same for atomic weights.
    - Fill missing elements to `DDCMS/data/materials.xml` (Values to be cross-checked by CMS)

* 2019-11-09 Markus Frank ([PR#593](https://github.com/aidasoft/dd4hep/pull/593))
  -  Fix Geant4 conversion for Polyhedra and Polycone if start_phi != 0 (See issue https://github.com/AIDASoft/DD4hep/issues/578)
  - Preparatory work to have placeholder volumes for twisted tubes (See issue https://github.com/AIDASoft/DD4hep/issues/588)

* 2019-10-29 Andre Sailer ([PR#590](https://github.com/aidasoft/dd4hep/pull/590))
  - DumpBField: correct the column unit printout and prepare for eventual change of default length unit
  - MagneticFields example: correct the unit for the Z parameter of the MultiPole to tesla

# v01-11

* 2019-10-23 MarkusFrankATcernch ([PR#587](https://github.com/AidaSoft/DD4hep/pull/587))
  -  Have separate compilation unit for shape utilities like `set_dimension(...)`, `dimension()`, `isA()`, `instanceOf()`... 
  - Improvements to basic shape test

* 2019-10-23 Andre Sailer ([PR#579](https://github.com/AidaSoft/DD4hep/pull/579))
  - CMake: Add possibility to build only shared libraries, fixes #493: 
    - Usage `cmake ... -D BUILD_SHARED_LIBS=OFF ...`

* 2019-10-23 Andre Sailer ([PR#575](https://github.com/AidaSoft/DD4hep/pull/575))
  * CMake: Add `DD4HEP_USE_EXISTING_DD4HEP` option which together with `DD4HEP_BUILD_PACKAGES` can be used to rebuild, for example only DDG4.
     * This creates a new Package called "DD4hepSelected" which can then be used alongside the full DD4hep Package in a third project.

* 2019-10-22 Marko Petric ([PR#586](https://github.com/AidaSoft/DD4hep/pull/586))
  - Remove deprecated rootcling flags (`-cint`, `-c`, `-p`, `-std=c++`) from dictionary creation script

* 2019-10-22 Marko Petric ([PR#585](https://github.com/AidaSoft/DD4hep/pull/585))
  - Fix bug that the c++ filesystem check is called from `${DD4hep_ROOT}`
  - Install `DD4hepConfig.cmake` only in `__prefix__/cmake` to avoid path detection confusion
  - Enable choosing examples in the `examples/CMakeLists.cmake` via cmake flag `-DDD4HEP_BUILD_EXAMPLES=OpticalSurfaces` (recommended method)
  - Make each example folder to compile standalone (not recommended method)
  - Update cmake of Segmentation example to more current state and fix resulting errors
    - include segmentation example as test
  - Resolves #582 and resolves #583

* 2019-10-21 MarkusFrankATcernch ([PR#584](https://github.com/AidaSoft/DD4hep/pull/584))
  - Add function `bool isInstance(const Handle<TGeoShape>& solid)`
     - compares types of shapes and behaves like `dynamic_cast`, similar to python's `isinstance(obj,type)`
     - remove deprecated function `instanceOf` in favour of `isInstance`. Same behavior.
   - Add function `bool isA(const Handle<TGeoShape>& solid)`
     - compares types of shapes and requires exact match, no polymorphism allowed.
  - Add Geant4 conversion for shape `TGeoCtub` -> `G4CutTube`

* 2019-10-14 Marko Petric ([PR#572](https://github.com/AidaSoft/DD4hep/pull/572))
  - Install python files in `lib/pythonX.Y/site-packages` resolves #562 
    - adapt `thisdd4hep.sh` scripts
  - Add missing RPATH to examples (basically bug fix for mac)
  - Add DDG4 tools to bin to make them more accessible to users:
    - `g4MaterialScan`, `checkGeometry`, `checkOverlaps`

* 2019-10-03 MarkusFrankATcernch ([PR#577](https://github.com/AidaSoft/DD4hep/pull/577))
  - Inhibit not allowed use of `DetectorImp.h`

* 2019-10-03 Andre Sailer ([PR#574](https://github.com/AidaSoft/DD4hep/pull/574))
  - CMake: When needing `boost::filesystem` (c++14, gcc  < 8) require at least Boost 1.56, see #567

* 2019-10-03 Markus Frank ([PR#573](https://github.com/AidaSoft/DD4hep/pull/573))
  - Fix memory leak introduced when generalizing placements to include left-handed coordinate systems.

* 2019-10-03 Markus Frank ([PR#571](https://github.com/AidaSoft/DD4hep/pull/571))
  - Add example to the volume reflection mechanism
  - Fix bug in volume reflection

* 2019-10-02 Markus Frank ([PR#569](https://github.com/AidaSoft/DD4hep/pull/569))
  - First implementation to support reflection with left-handed volumes/solids
    - Changes for volumes and solids.
    - Conversion handling of `TGeoScaledShape` in DDG4

* 2019-09-30 Andre Sailer ([PR#566](https://github.com/AidaSoft/DD4hep/pull/566))
  - PythonBindings: fix issue when source files were not available, fixes #565 
  - CMake: drop DDCores dependency on DD4hepGaudiPluginMgr
  - CMake: DD4hepConfig: use find_dependency instead of find_package

* 2019-09-24 Marko Petric ([PR#564](https://github.com/AidaSoft/DD4hep/pull/564))
  - Added test for Python3 compliance of code
  - Added test to require flake8 python code formatting

* 2019-09-13 Marko Petric ([PR#540](https://github.com/AidaSoft/DD4hep/pull/540))
  - Make python code compatible to python 2 and 3
    - add `absolute_import` and `unicode_literals` to all files
      - fix API calls and cast `unicode` to `string` when needed
    - replace print statement with logging
    - remove old octal literal
    - use future division
    - use `six`:
       - replace `dict.iteritems` with `six.iteritems`
       - replace `xrange` with `range` from `six.move`
       - replace `basestring` with `six.string_types`
       - replace `raw_input` with `input` from `six.moves`
       - added a copy of six.py named ddsix.py to DDCore
    - Replace deprecated `execfile` with call to `open`, `compile` and `exec`
    - Remove usage of `apply`
    - use `io.open` instead of standard `open`
    - convert `except a,b` to `except a as b`
    - change `dict.has_key` to `key in dict`
  - Require DD4hep Python3 CI tests to pass
  - Remove obsolete `lcdd.py`
  - Remove deprecated `SystemOfUnits.py` and replace everywhere with `g4units.py`
  - Flake8 all files

* 2019-09-02 Andre Sailer ([PR#561](https://github.com/AidaSoft/DD4hep/pull/561))
  - CMake: add option `DD4HEP_BUILD_PACKAGES` so that only individual packages can be compiled. If an incorrect selection is given cmake should fail due to missing alias libraries. The option requires a whitespace or semicolon separated list.
  - CMake add option `DD4HEP_BUILD_EXAMPLES` to enable compilation of examples together with the main DD4hep packages. Default OFF
  - CMake: add `DD4hep::` aliases for all libraries and some executables

* 2019-08-26 Andre Sailer ([PR#559](https://github.com/AidaSoft/DD4hep/pull/559))
  - DD4hepConfig: make all DD4HEP_USE variables behave as booleans

* 2019-08-22 Markus Frank ([PR#554](https://github.com/AidaSoft/DD4hep/pull/554))
  - Fix property table translation to Geant4 according to suggestions from Dong Liu
    (see issue https://github.com/AIDASoft/DD4hep/issues/440 )

* 2019-08-22 Andre Sailer ([PR#552](https://github.com/AidaSoft/DD4hep/pull/552))
  * DD4hepConfig: `DD4hepConfig.CMake` now exports `DD4hep::DDCore` `DD4hep::<Component>` targets to be consumed by users of the DD4hep package, the CMake variables `DD4hep_LIBRARIES` etc. are still being filled for backward compatibility
  
  * DD4hep CMake: Only the `dd4hep_add_plugin` and `dd4hep_add_dictionary` CMake functions are to create targets still exist, `dd4hep_add_package`/`library`/`executable` were removed and instead the cmake `default add_library`/`executable` have to be used.
  
  * PluginManager: only link against boost filesystem if the compiler and standard library do not support the filesystem library
  
  * DD4hep Requirements: Now require cmake version 3.12
  
  * DD4hep Requirements: Now require c++ standard 14

* 2019-08-21 Markus Frank ([PR#553](https://github.com/AidaSoft/DD4hep/pull/553))
  - Fix unit conversion for optical surface properties. The units of the property tables were not converted from TGeo to Geant4. See dicussion in issue https://github.com/AIDASoft/DD4hep/issues/440
  - If an external world volume is supplied, the material `Air` is deduced from this solid (only used by CMS).

* 2019-08-15 Frank Gaede ([PR#550](https://github.com/AidaSoft/DD4hep/pull/550))
  - make compatible with MacOS (10.14.6)
       - address latest developments w/ new Gaudi Plugin Manager

* 2019-08-14 Markus Frank ([PR#551](https://github.com/AidaSoft/DD4hep/pull/551))
  - Moved `setDimensions` call out of the individual dd4hep shapes into the base Solid.
  - Add `Solid::dimensions()`, `Solid::setDimension()` implementation for `PseudoTrap` and  `TruncatedTube`. The solution is not optimal, because a analytical solution tends to be ambiguous due to solutions of polynomials of degree 2 and the initial parameters had to be stored as a string.
  - Upgraded shape tests to also check the shapes (using mesh vertices) after a re-dimension using the same parameters.
  - Geant4FieldTrackingSetup: Any failure in the creation of the `G4EquationOfMotion` or the `G4MagIntegratorStepper` is now FATAL and causes an exception.

* 2019-08-12 Markus Frank ([PR#549](https://github.com/AidaSoft/DD4hep/pull/549))
  - Adopted new Gaudi plugin manager V2. V1 can be enabled using compile switch in `DD4hep/config.h`. Removed the traces from the ROOT5 Reflex based plugin service. The new plugin service depends on `Boost::file_system` and `Boost::system`. 
  - Improve GDML saving from ROOT. (requires ROOT >= 6.20)
  - Fix ROOT persistency for the volume manager.
  - Fix Geant4FieldTrackingSetup: Issue warning if the `G4MagIntegratorStepper` cannot be created.
  - Examples: based the CLICSiD example on the XML sources of DDDetectors. This ensures XML sources match C++ sources.

* 2019-08-12 Andre Sailer ([PR#548](https://github.com/AidaSoft/DD4hep/pull/548))
  - Shapes: fix conversion of `startTheta` for Sphere::setDimensions
  - Shapes::get_shape_dimension: add return value conversion for angles to internal unit (radians)

* 2019-08-12 MarkusFrankATcernch ([PR#547](https://github.com/AidaSoft/DD4hep/pull/547))
  - make compatible w/ macos (c++14)
           - replace `std::make_any` w/ make_any (defined in Any.h)
           - use `std::lock_guard<std::mutex>`

* 2019-08-07 MarkusFrankATcernch ([PR#545](https://github.com/AidaSoft/DD4hep/pull/545))
  - Fix bug in Polyhedra shape (See #544).
  - Update optical surface example (resolves #440) .

* 2019-07-16 Markus Frank ([PR#539](https://github.com/AidaSoft/DD4hep/pull/539))
  - Remove clang warnings.

* 2019-07-16 MarkusFrankATcernch ([PR#538](https://github.com/AidaSoft/DD4hep/pull/538))
  - Fix coverity errors 
  - Fix Trap shape conversion to Geant4. The theta/phi angle was not converted from degree to radians (Resolves #536).

* 2019-07-15 Marko Petric ([PR#537](https://github.com/AidaSoft/DD4hep/pull/537))
  - Add a Python 3 pipeline to the CI (currently set to `allow_failure`)

* 2019-07-13 Marko Petric ([PR#535](https://github.com/AidaSoft/DD4hep/pull/535))
  - Update CI to be based on LCG 96 (ROOT 6.18, Geant 10.5, C++17)
  - Remove `FindXercesC.cmake` since it is in CMake

* 2019-07-10 Markus Frank ([PR#533](https://github.com/AidaSoft/DD4hep/pull/533))
  - Optimize STL containers: replace insert/push with emplace. 2nd. episode.

* 2019-07-10 Marko Petric ([PR#532](https://github.com/AidaSoft/DD4hep/pull/532))
  - Remove `dd_sim` (resolves #435)

* 2019-07-10 Marko Petric ([PR#531](https://github.com/AidaSoft/DD4hep/pull/531))
  - Remove shadow warnings related to code interfacing only ROOT 6.18
  - Add `DBoost_NO_BOOST_CMAKE=ON` to examples cmake call as it is necessary now
  - Set CMP0074 policy to NEW if can be set

* 2019-07-10 Markus Frank ([PR#530](https://github.com/AidaSoft/DD4hep/pull/530))
  - Optimize STL containers: replace insert/push with emplace

* 2019-07-09 Markus Frank ([PR#528](https://github.com/AidaSoft/DD4hep/pull/528))
  * Allow for various material scan types from the root interactove prompt
  
         Examples: from DDDetectors/compact/SiD.xml
         $> materialScan file:checkout/DDDetectors/compact/SiD.xml -interactive
       
         1) Simple scan:
            root [0] gMaterialScan->print(5,5,0,5,5,400)
         2) Scan a given subdetector:
            root [0] de=gDD4hepUI->instance()->detector("LumiCal");
            root [1] gMaterialScan->setDetector(de);
            root [2] gMaterialScan->print(5,5,0,5,5,400)
         3) Scan by material:
            root [0] gMaterialScan->setMaterial("Silicon");
            root [1] gMaterialScan->print(5,5,0,5,5,400)
         4) Scan by region:
            root [0] gMaterialScan->setRegion("SiTrackerBarrelRegion");
            root [1] gMaterialScan->print(0,0,0,100,100,0)
  
  * Added copyright notices to the DDRec files.

* 2019-07-09 Marko Petric ([PR#527](https://github.com/AidaSoft/DD4hep/pull/527))
  - Update CI to macOS Mojave 10.14
  - Make `PluginService.cpp` C++17 compliant (addresses partially #525)
    - replace `ptr_fun` with `lambda`
  - Remove deprecated code that uses `auto_prt`
  - Remove deprecated `set_unexpected` from  `DetectorImp.cpp`
  - Remove code associated to `DD4HEP_DD4HEP_PTR_AUTO`

* 2019-07-08 Markus Frank ([PR#524](https://github.com/AidaSoft/DD4hep/pull/524))
  - Get the new package formally into the same shape as the other packages together with an example section.
  - Add small example to test the basic development framework

* 2019-07-04 Markus Frank ([PR#520](https://github.com/AidaSoft/DD4hep/pull/520))
  - From ROOT 6.20 onwards dd4hep shall use the Geant4 unit system (mm, nsec, MeV) instead of the TGeo units (cm, sec, keV). This commit prepares for the necessary changes.
  - A new package was created, which shall host the dd4hep digitization components.
    A small tbb based multi threaded framework was put in place. Now the real work can start.
  - The material scanner has now a switch to run in interactive mode from the ROOT prompt.
    To invoke: materialScan compact.xml  x0 y0 z0 x1 y1 z1 -interactive
    If the interactive switch is missing, the old behavior is preserved.

* 2019-06-26 Markus Frank ([PR#517](https://github.com/AidaSoft/DD4hep/pull/517))
  - Material properties use now default dd4hep units
  - Translated and updated surface example from geant4 to dd4hep
  - Added shape identification using `instanceOf operator (function)`
  - Improved handling of xml files if improperly terminated

* 2019-06-26 MarkusFrankATcernch ([PR#516](https://github.com/AidaSoft/DD4hep/pull/516))
  - CMakeLists: Changed the order in which include directories are listed when compiling, move DD4hep source paths to the front. This fixes a problem if older DD4hep installations are inadvertently in one of the include paths passed to compilers or rootcling (e.g., in LCG Views), fixes #515

* 2019-06-06 Andre Sailer ([PR#514](https://github.com/AidaSoft/DD4hep/pull/514))
  - Gean4ExtraParticles: no longer add decay process to extra particles, this is done by Geant4  resolves #513 
  - ddsim: disable physics.decays by default. This should only be enabled if completely new physics lists are created resolves #513

* 2019-05-09 Markus Frank ([PR#510](https://github.com/AidaSoft/DD4hep/pull/510))
  - Add debug printout of MEE in Geant4 material conversion

* 2019-04-29 MarkusFrankATcernch ([PR#508](https://github.com/AidaSoft/DD4hep/pull/508))
  * Geant4GDMLWriteAction:
    * Add properties to Geant4GDMLWriteAction to steer writing of regions, cuts and sensitive detectors. See github issue #507
      * Property: Export region information to the GDML: ExportRegions, default: True
      * Property: Export energy cut information to the GDML: ExportEnergyCuts, default: True
      * Property: Export sensitive detector information to the GDML: ExportSensitiveDetectors, default: True
    * **Note: The Geant4 physics list must be initialized BEFORE invoking the writer with options. Otherwise the particle definitions are missing! If you ONLY want to dump the geometry to GDML you must call**
      ```
         /run/beamOn 0
      ```
      before writing the GDML file!
      You also need to setup a minimal generation action like:
       ```py
       sid.geant4.setupGun('Gun','pi-',10*GeV,Standalone=True)
       ```

* 2019-04-29 Frank Gaede ([PR#506](https://github.com/AidaSoft/DD4hep/pull/506))
  - add utility materialBudget.cpp 
       - create plots w/ integrated radiation and interaction lengths
  - bug fix in materialScan.cpp
       - print correct endpoint

* 2019-04-17 Marko Petric ([PR#503](https://github.com/AidaSoft/DD4hep/pull/503))
  - DDG4: DDSim add option Physics.zeroTimePDG to configure ignoring particles of given PDG when their properTime is ZERO, e.g. charged leptons undergoing FSR, fixes #390 
  - DDG4: DDSim: fix parsing of rejectPDGs values from the command line

* 2019-04-12 Marko Petric ([PR#502](https://github.com/AidaSoft/DD4hep/pull/502))
  - DDSim: Add Higgs PDG code 25 to rejected codes for reading events

* 2019-04-10 MarkusFrankATcernch ([PR#501](https://github.com/AidaSoft/DD4hep/pull/501))
  - Add access to all Geant4Action derivatives to the Geant4 top level physical volume (world).
  - Add Geant4 GDML writer action accessible and configurable from the Geant4 prompt

* 2019-04-09 Mircho Rodozov ([PR#496](https://github.com/AidaSoft/DD4hep/pull/496))
  - Added powerpc macros check to include header `cxxabi.h`

* 2019-04-01 Markus Frank ([PR#494](https://github.com/AidaSoft/DD4hep/pull/494))
  -  Use shared_ptr instead of home made ref counting for ConditionUpdateCalls
  -  Implement construction parameter access for solids.  This one is a bit tricky: Some shapes (ShapeAssembly, Boolean shapes) had no such parameters. Added them as the sequence of the basic shape parameters + the corresponding matrices.
  -  Add move constructors to handles 
  -  Improve const-ness of detector object in DDG4

* 2019-03-11 Markus Frank ([PR#491](https://github.com/AidaSoft/DD4hep/pull/491))
  - Implemented basic handles to support surface objects
  - Implemented import of surface optical objects in compact to create TGeo surface objects and tabulated properties.
  - Implemented the translation from TGeo to Geant4
  - Added physics components for DDG4 handling Cerekov, Scintillation and generic optical photon physics
  - Added examples
  
  Please Note: 
  1) This is only enabled for a ROOT version > 6.17 (which is supposed to come)
  2) There are still changes in ROOT in the pipeline. The code shall have to be adapted accordingly once these changes are activated.

* 2019-03-06 ebrianne ([PR#489](https://github.com/AidaSoft/DD4hep/pull/489))
  - Added Initialization of G4EmSaturation to initialize birks coefficients - for g4 version > 10.03

* 2019-02-19 Andre Sailer ([PR#486](https://github.com/AidaSoft/DD4hep/pull/486))
  - DDRec: Surface: add accessor to DetElement member
  - DDRec: DetectorData: add Extension holding a map of String to Doubles

* 2019-02-14 Paul Gessinger ([PR#485](https://github.com/AidaSoft/DD4hep/pull/485))
  * In `DD4hepConfig.cmake`, figure out if build has compatible standard set and print error if not

* 2019-02-13 Frank Gaede ([PR#483](https://github.com/AidaSoft/DD4hep/pull/483))
  - fix drawing of surfaces for z-disks and cylinders (resolves #482)

# v01-10

* 2019-01-31 Markus Frank ([PR#480](https://github.com/aidasoft/dd4hep/pull/480))
  - Fix bug in `geoDisplay` to allow passing the volume display depth as an argument
  - Added a static creator to the Detector class to create non-traced instances: `std::unique_ptr<Detector> Detector::make_unique(const std::string& name);` It is the users responsibility to release the allocated resources and to avoid clashed with existing `TGeoManager` instances.
  -  Allow direct access to the solid instance of the DetElements's placement.

* 2019-01-15 Markus Frank ([PR#478](https://github.com/aidasoft/dd4hep/pull/478))
  - Fix bug in `geoDisplay` see #477

* 2019-01-10 Markus Frank ([PR#476](https://github.com/aidasoft/dd4hep/pull/476))
  - Fix bug in ConditionsUserPool whan scanning DetElement conditions
  - Improve conditions handling: Allow to bind sub-class entities of ConditionObject to thew opaque data block.

* 2018-12-14 Markus Frank ([PR#475](https://github.com/aidasoft/dd4hep/pull/475))
  - Add named shape constructors (see #469)

* 2018-12-13 Markus Frank ([PR#474](https://github.com/aidasoft/dd4hep/pull/474))
  - Fix possible access violation

* 2018-12-13 MarkusFrankATcernch ([PR#473](https://github.com/aidasoft/dd4hep/pull/473))
  - Improve handling of condition dependencies ( main work item)
    - Improve logic flow in the DDCond/ConditionDependencyHandler
    - Improve the functionality of the conditions resolver accessible from the update context. Allow for the creation (and registration) of multiple conditions in one single callback.
  - Add a shape check for eight-point solids.

* 2018-12-07 Frank Gaede ([PR#470](https://github.com/aidasoft/dd4hep/pull/470))
  - add `#include <memory>` to `run_plugin.h` (needed for `std::unique_ptr` and gcc 4.9)

* 2018-12-06 Markus Frank ([PR#468](https://github.com/aidasoft/dd4hep/pull/468))
  - To fix issue #466  we had to go back to the original implementation which was actually correct (see https://github.com/AIDASoft/DD4hep/commit/36d4b01e0688f690ac2e506a62e00627bb6b798c#diff-7219d47bc4ab7516e0ca6c4f35f2602f).
  - Added an example to show how to perform scans of the volume hierarchy with user defined callback functors. See for details `examples/ClientTests/src/PlacedVolumeScannerTest.cpp`.
  
  - Added conversion between `TGeoArb8` and `G4GenericTrap`, fixes #465

* 2018-12-05 Markus Frank ([PR#467](https://github.com/aidasoft/dd4hep/pull/467))
  - Harmonize argument names in `Shapes.h` with their actual functionality. For many shapes in `DD4hep/Shapes.h` the argument names were misleading: very often deltaPhi was mentioned, whereas the code actually used instead of (phi, deltaphi) the input arguments to the ROOT constructors (startPhi,endPhi) or (phi1, phi2).  Wherever ROOT uses (startPhi,endPhi) the argument names were changed accordingly. Please note a bug was found in the legacy constructor:
    ```cpp
      /// Legacy: Constructor to create a new identifiable tube object with attribute initialization
      Tube(const std::string& nam, double rmin, double rmax, double dz, double startPhi, double endPhi)
    ```
    Here opposite to all other constructors delta_phi was used as such - in contradiction to other constructors of the same class. This was rectified.

* 2018-11-30 Andre Sailer ([PR#462](https://github.com/aidasoft/dd4hep/pull/462))
  - Introduce compile flag to minimize conditions footprint

* 2018-11-27 Markus Frank ([PR#461](https://github.com/aidasoft/dd4hep/pull/461))
  - Add shape constructor for regular trapezoids (TGeoTrd1) ( see #460). Trd2 cannot be divided the same way as Trd1 shapes. Hence the addition became necessary. Due to the imprecise name of Trapezoid the names Trd1 and Trd2 (aka old Trapezoid) are favored. The usage of Trapezoid is supported for backwards compatibility using a typedef.

* 2018-11-22 Markus Frank ([PR#459](https://github.com/aidasoft/dd4hep/pull/459))
  - Fix bug in DDCodex geometry, add debugging to VolumeBuilder

* 2018-11-14 Markus Frank ([PR#458](https://github.com/aidasoft/dd4hep/pull/458))
  - Improve ROOT persistency: flag user extensions as persistent only while saving/loading
  - Need to rename DDEve library: nameclash with ddeve executable on Apple: cmake fails to build ddeve as exe and DDEve as library. libDDEve.so is now called libDDEvePlugins.so.

* 2018-11-13 Markus Frank ([PR#457](https://github.com/aidasoft/dd4hep/pull/457))
  ## Provide support for Volume divisions.
  Since DD4hep requires Volumes (aka `TGeoVolume`) and PlacedVolumes (aka `TGeoNode`) to be enhanced with the user extension mechanism, therefore shape divisions **must** be done using the division mechanism of the DD4hep shape or the volume wrapper. Otherwise the enhancements are not added and you will get an exception when DD4hep is closing the geometry or whenever you do something with the volume, which is served by the user extension. The same argument holds when a division is made from a `Volume`. Unfortunately there is no reasonable way to intercept this call to the `TGeo` objects - except to the sub-class each of them, which is not really acceptable either.
  
  Hence: **If you use DD4hep: Never call the raw TGeo routines.**
     
  For any further documentation please see the following ROOT documentation on [TGeo](http://root.cern.ch/root/html/TGeoVolume.html)
  
  For an example see `examples/ClientTests/src/VolumeDivisionTest.cpp`
  and `examples/ClientTests/compact/VolumeDivisionTest.xml`
  
  To execute: 
  ```
  geoDisplay -input file:<path>/examples/ClientTests/compact/VolumeDivisionTest.xml
  ```

* 2018-11-13 Hadrien Grasland ([PR#438](https://github.com/aidasoft/dd4hep/pull/438))
  - Make FindPackage(DD4hep) work even if thisdd4hep.sh was not sourced

* 2018-11-09 Frank Gaede ([PR#456](https://github.com/aidasoft/dd4hep/pull/456))
  - add `#include <memory>` in `VolumeAssembly_geo.cpp` gcc 4.9 compatibility

* 2018-11-07 Markus Frank ([PR#455](https://github.com/aidasoft/dd4hep/pull/455))
  - Improve VolumeBuilder pattern matcher
  - Allow XML based volume creation based on factories.

* 2018-11-02 Markus Frank ([PR#454](https://github.com/aidasoft/dd4hep/pull/454))
  - Improve generic Volume assembly and XML volume builder

* 2018-11-01 Markus Frank ([PR#452](https://github.com/aidasoft/dd4hep/pull/452))
  - Fixed the DetElement cloning mechanism it to properly replicate DetElement trees.
    - Tested with one LHCb upgrade detector.
  - Added numeric epsilon to the default math dictionary of the expression evaluator:
    ```cpp
    int:epsilon  --> std::numeric_limits<int>::epsilon()
    long:epsilon  --> std::numeric_limits<long>::epsilon()
    float:epsilon  --> std::numeric_limits<float>::epsilon()
    double:epsilon  --> std::numeric_limits<double>::epsilon()
    ```

* 2018-10-30 Markus Frank ([PR#451](https://github.com/aidasoft/dd4hep/pull/451))
  - Add copyright notices
  - Make ddeve a program not only a ROOT script

* 2018-10-30 Markus Frank ([PR#450](https://github.com/aidasoft/dd4hep/pull/450))
  - The DDUpgrade example was removed from the main repository. Since it is only of interest for LHCb, it moved to a separate repository at CERN/gitlab.
  - Issue #449 should be fixed now.
  - The XML volume builder utility was improved.

* 2018-10-18 Markus Frank ([PR#447](https://github.com/aidasoft/dd4hep/pull/447))
  -  Fix plugin manager cmake file by removing explicit dependency on c++ standard.

# v01-09

* 2018-10-15 Markus Frank ([PR#442](https://github.com/aidasoft/DD4hep/pull/442))
  - DDCMS: Update to support namespaces
  - DDUpgrade test example for the LHCb upgrade
  - Fix nested detectors (in fact worked only for first level parents)
  - Add VolumeBuilder XML utility to work on XML-tree patterns

* 2018-09-12 Hadrien Grasland ([PR#437](https://github.com/aidasoft/DD4hep/pull/437))
  - Remove if string equals ON/OFF in cmake IF statements and check default CMake truth values

* 2018-08-20 Oleksandr Viazlo ([PR#434](https://github.com/aidasoft/DD4hep/pull/434))
  - DD4hep_Mask_o1_v01_geo
    - allow rotation around x-axis (instead of y)
    - phi1 and phi2 cone angles configurable from the xml-file

* 2018-08-10 Markus Frank ([PR#433](https://github.com/aidasoft/DD4hep/pull/433))
  - Fix DDCMS example to use true namespace names rather than using "_"
    This should resolve issue https://github.com/AIDASoft/DD4hep/issues/421

* 2018-08-09 Markus Frank ([PR#432](https://github.com/aidasoft/DD4hep/pull/432))
  - Make the expression evaluator understand variable names with namespaces
      - Variable names containing a `:` or `::` are now accepted by the expression evaluator. This is first step towars resolving #421 
     - It has now to be seen what has to be done further. The DCMS example was not yet updated to use this feature.
  - Add new example in `examples/ClientTests` to test this functionality.

* 2018-08-09 Andre Sailer ([PR#429](https://github.com/aidasoft/DD4hep/pull/429))
  - DDG4: add possibility to simulate all events in a file by passing NumberOfEvents < 0. Fixes #237 
     * The de-facto limit is ~2 billion, which should be fine for input files.

* 2018-08-08 Markus Frank ([PR#430](https://github.com/aidasoft/DD4hep/pull/430))
  - Fix HEPMC reader for unknown generator status codes 
  - Update DDCodex example, feature imports from plain ROOT file 
  - Allow debugging Geant4VolumeManager

* 2018-08-07 Mircho Rodozov ([PR#428](https://github.com/aidasoft/DD4hep/pull/428))
  - Adapt to root interfaces changes for `TGeoMatrix::Inverse` (https://github.com/root-project/root/pull/2365), fixes #426

* 2018-08-06 Andre Sailer ([PR#424](https://github.com/aidasoft/DD4hep/pull/424))
  - CMake: Ensure proper tls flag (global-dynamic) for Geant4 build, added option DD4HEP_IGNORE_GEANT4_TLS  to override the check. Closes #419

* 2018-07-31 Andre Sailer ([PR#420](https://github.com/aidasoft/DD4hep/pull/420))
  - DDG4: Import the ddsim python program from https://github.com/iLCSoft/lcgeo
    For example:
     - `ddsim --help`
     - `ddsim --dumpSteeringFile > mySteer.py`
     - `ddsim --steeringFile=mySteer.py --compactFile myDetector.xml`

# v01-08

* 2018-07-02 Markus Frank ([PR#418](https://github.com/aidasoft/dd4hep/pull/418))
  - Add DDCodexB in standalone mode with simulation script and basic skeleton for DDEve

* 2018-06-28 Markus Frank ([PR#417](https://github.com/aidasoft/dd4hep/pull/417))
  - Steer debug printouts in CondDB2DDDB and DDDB2Objects by parsing xml files.

* 2018-06-26 Markus Frank ([PR#416](https://github.com/aidasoft/dd4hep/pull/416))
  - Separate the hit class and add dictionary. No base class - entirely independent.
  - Allow to save the hit class to ROOT (but without MC truth)
    See MyTrackerHit.h for details.

* 2018-06-26 Markus Frank ([PR#415](https://github.com/aidasoft/dd4hep/pull/415))
  - Add small example how to specialize a new sensitive action and attach it to a detector in DDG4.

* 2018-06-26 Frank Gaede ([PR#414](https://github.com/aidasoft/dd4hep/pull/414))
  - bug fix in Geant4EventReaderGuineaPig
       - fix ignoring input lines with 'nan'

* 2018-06-26 Shaojun Lu ([PR#412](https://github.com/aidasoft/dd4hep/pull/412))
  - Added one more if statement: If the track went into new Volume, 
    - then extracted the hit in previous Volume,
    - and start a new hit in this current Volume,
      - in this current process, also allow the same following treatments for the new hit.

* 2018-06-21 Markus Frank ([PR#409](https://github.com/aidasoft/dd4hep/pull/409))
  - Support out-of-source builds of DD4hep examples.
    Comes with an expense: A new environment DD4hepExamplesINSTALL.
    has to be defined to support internal file accesses and loads.
    The builds were also checked with read-only installation directories.
    Solves issue https://github.com/AIDASoft/DD4hep/issues/382
  - Smallish improvement to the ConditionsManager.

* 2018-06-21 Andre Sailer ([PR#408](https://github.com/aidasoft/dd4hep/pull/408))
  - Cmake: fix for configuring with Geant4 with internal CLHEP, fixes #406 
  - Cmake: fix for configuring with BUILD_TESTING=OFF, fixes #407

* 2018-06-07 Markus Frank ([PR#404](https://github.com/aidasoft/dd4hep/pull/404))
  - Fix basic shape tests for PseudoTrap

* 2018-06-07 Markus Frank ([PR#403](https://github.com/aidasoft/dd4hep/pull/403))
  - patch for truncated tubes shapes.

* 2018-06-04 Markus Frank ([PR#402](https://github.com/aidasoft/dd4hep/pull/402))
  - Fix truncated tube shape

* 2018-06-04 Markus Frank ([PR#400](https://github.com/aidasoft/dd4hep/pull/400))
  - Allow for world volumes other than boxes. See `examples/ClientTests/compact/WorldVolume.xml` how to set it up. The effective thing is that the top level volume must be set to the TGeoManager before `Detector::init()`. If a top level volume is set, it is implicitly assumed to be the world volume. Otherwise the already existing mechanism (box volume) is activated.
  - Add new basic shape tests.

* 2018-06-01 Markus Frank ([PR#399](https://github.com/aidasoft/dd4hep/pull/399))
  - As discussed in issue #398 The use of TGeoUnits is inconvenient. The dd4hep units are now exposed in the python modules.
  - Basic shapes are now tested in the regular ctest executions. The mesh vertices of the shapes were 
  saved to a reference file and are compared to in subsequent runs. The reference files reside in 
  `examples/ClienTests/ref`. See `examples/ClientTests/compact/Check_Shape_*.xml` for details.
  - The ROOT UI  and some dump plugins were enhanced to expose more information.

* 2018-05-30 Markus Frank ([PR#397](https://github.com/aidasoft/dd4hep/pull/397))
  - Enable to start DDG4 using a saved detector description in a ROOT file.
     -  Added corresponding test: `Persist_CLICSiD_Geant4_LONGTEST`
  - Fix shape constructors for Trap and PseudoTrap
  - The python module `DD4hep.py` is gone as discussed in the developers meeting use `dd4hep.py` instead.
      - on masOS : your "git pull" possibly deletes both files. you may have to checkout `dd4hep.py` again, due to fact that the filesystem is case-insensitive. 
  - Add example for LHCb CODEX-b.

* 2018-05-29 Markus Frank ([PR#394](https://github.com/aidasoft/dd4hep/pull/394))
  - Consistently handle cmake command line options in case no Geant4 or no documentation should be built.

* 2018-05-28 Markus Frank ([PR#393](https://github.com/aidasoft/dd4hep/pull/393))
  - Improvements to `geoPluginRun`.
    `$> geoPluginRun -ui -interactive`
    results in DD4hep enabled ROOT prompt.
  - DD4hepUI: improvements to interact with DD4hep instance from ROOT prompt
  - DDDB improve configuration for printing and debugging
  - DDDB: allow to block certain XML branches

* 2018-05-22 Frank Gaede ([PR#389](https://github.com/aidasoft/dd4hep/pull/389))
  - fix bug in input handling, for details see discussion #387 
     - exclude leptons with zero lifetime from Geant4

* 2018-05-22 Markus Frank ([PR#388](https://github.com/aidasoft/dd4hep/pull/388))
  - Update doxygen information for some undocumented classes. 
   - Add licence header to files where not present.

* 2018-05-22 Marko Petric ([PR#380](https://github.com/aidasoft/dd4hep/pull/380))
  - Update LICENSE to LGPLv3
    - The name of the file containing the LICENSE has ben changed from LICENCE->LICENSE as all source files reference `For the licensing terms see $DD4hepINSTALL/LICENSE.`

* 2018-05-16 Markus Frank ([PR#386](https://github.com/aidasoft/dd4hep/pull/386))
  - Fix bug in variable order of `ExtrudedPolygon` (x<->y)

* 2018-05-15 Markus Frank ([PR#384](https://github.com/aidasoft/dd4hep/pull/384))
  # Implementation of non-cylindrical tracking region (resolves #371)
    -  It is possible to define volumes in a parallel world such as e.g. a tracking region. In principle any volume hierarchy may be attached to the parallel world. None of these volumes participate in the tracking as long as the "connected" attribute is set to false. The hierarchy of parallel world volumes can be accessed from the main detector object using
       ```cpp
       dd4hep::Volume parallel = dd4hep::Description::parallelWorldVolume()
       ```
    This parallel world volume is created when the geometry is opened together (and with the same dimensions) as the world volume itself.
    -  IF the NAME of the volumes is "tracking_volume" within the compact notation it is declared as the Detector's trackingVolume entity and is accessible as well:
       ```cpp
       dd4hep::Volume trackers = dd4hep::Description::trackingVolume()
       ```
    -  Although the concept is available in the DD4hep core, it's configuration from XML is only implemented for the compact notation. For details see the example `examples/ClientTests/compact/TrackingRegion.xml`.
     -  If the volume should be connected to the world:   connected="true". This is useful for debugging because  the volume can be visualized else if the volume is part of the parallelworld: connected="false". The volume is always connected to the top level. The anchor detector element defines the base transformation to place the volume within the (parallel) world.
  
  ```xml
       <parallelworld_volume name="tracking_volume" anchor="/world" material="Air" connected="true" vis="VisibleBlue">
         <shape type="BooleanShape" operation="Subtraction">
           <shape type="BooleanShape" operation="Subtraction">
             <shape type="BooleanShape" operation="Subtraction"  >
               <shape type="Tube" rmin="0*cm" rmax="100*cm" dz="100*cm"/>
               <shape type="Cone" rmin2="0*cm" rmax2="60*cm" rmin1="0*cm" rmax1="30*cm" z="40*cm"/>
               <position x="0*cm" y="0*cm" z="65*cm"/>
             </shape>
             <shape type="Cone" rmin1="0*cm" rmax1="60*cm" rmin2="0*cm" rmax2="30*cm" z="40*cm"/>
             <position x="0" y="0" z="-65*cm"/>
           </shape>
           <shape type="Cone" rmin2="0*cm" rmax2="55*cm" rmin1="0*cm" rmax1="55*cm" z="30*cm"/>
           <position x="0" y="0" z="0*cm"/>
         </shape>
         <position x="0*cm"   y="50*cm" z="0*cm"/>
         <rotation x="pi/2.0" y="0"     z="0"/>
       </parallelworld_volume>
  ```
  
  
  # Enhancement of assemblies, regions and production cuts (resolves #373)
  On request from FCC particle specific production cuts may be specified in the compact notation. These production cuts (Geant4 currently supports these for e+, e-, gammas and protons) are specified as "cut" entities in the limitset. (See the example `examples/ClientTests/compact/Assemblies.xml`).
  
    - The hierarchy of cuts being applied is:
        - If present particle specific production cuts for a region are applied.
        - else the "cut" attribute of the compact region specification is used
        - else the global Geant4 cut is automaticallly applied by Geant4.
  
  ```xml
    <limits>
      <limitset name="VXD_RegionLimitSet">
        <!--
             These are particle specific limits applied to the region
             ending in Geant4 in a G4UserLimits instance
        -->
        <limit name="step_length_max"  particles="*" value="5.0" unit="mm" />
        <limit name="track_length_max" particles="*" value="5.0" unit="mm" />
        <limit name="time_max"         particles="*" value="5.0" unit="ns" />
        <limit name="ekin_min"         particles="*" value="0.01" unit="MeV" />
        <limit name="range_min"        particles="*" value="5.0" unit="mm" />
  
        <!--
             These are particle specific production cuts applied to the region
             ending in Geant4 in a G4ProductionCuts instance
        -->
        <cut   particles="e+"          value="2.0"   unit="mm" />
        <cut   particles="e-"          value="2.0"   unit="mm" />
        <cut   particles="gamma"       value="5.0"   unit="mm" />
      </limitset>
    </limits>
  ```
  
  # SensitiveDetector types not changed by Geant4SensDetActionSequence (resolves #378)
  The sensitive detector type defined in the detector constructors is no longer changed intransparently in the back of the users. This may have side-effects for creative detector constructor writers, who invent sd types out of the sky. These obviously will not work with Geant4, because in Geant4 a mapping of these types must be applied to supported sensitive detectors. Now the mapping of a sd type (e.g. "tracker") is strict in the python setup. The default factory to create any sensitive detector instance in Geant4 (ie. an object of type G4VSensitiveDetector, G4VSDFilter, Geant4ActionSD) is a property of the Geant4Kernel  instance and defaults to:
    ```cpp
     declareProperty("DefaultSensitiveType", m_dfltSensitiveDetectorType = "Geant4SensDet");
    ```
     - Since the actual behavior is defined in the sequencer instanciated therein this default should be sufficient for 99.99 % of all cases. Otherwise the factory named "Geant4SensDet" may be overloaded.

* 2018-05-15 David Blyth ([PR#379](https://github.com/aidasoft/dd4hep/pull/379))
  - Geant4FieldTrackingConstruction now properly overrides `constructField()`

* 2018-05-03 David Blyth ([PR#377](https://github.com/aidasoft/dd4hep/pull/377))
  - Geant4Handle unhandled reference to shared actions.  This affected the destruction of shared actions.

* 2018-05-03 Markus Frank ([PR#375](https://github.com/aidasoft/dd4hep/pull/375))
  - Development of a small user example on how to do analysis in `DDG4`.
     - See `examples/DDG4/src/HitTupleAction.cpp`
     - Simply collect the energy deposits of hits and write an N-tuple with them.
     - The example shows how to access the hit collections and to extract the data in order to write other more sophisticated analyses.
  
   - This `DDG4` action is used in one of the Minitel examples: `examples/ClientTests/srcipts/MiniTelEnergyDeposits.py`

* 2018-05-02 Markus Frank ([PR#374](https://github.com/aidasoft/dd4hep/pull/374))
  - recommission the multithreaded SiD example 
     - `DDG4/examples/SiDSim_MT.py` vs. `DDG4/examples/SiDSim.py`

* 2018-04-19 Markus Frank ([PR#370](https://github.com/aidasoft/dd4hep/pull/370))
  * Allow to disable building the documentation cmake option BUILD_DOCS. By default ON and backwards compatible. If set to OFF no doc shall be built. (not everybody has biber installed)
  * Move from `DD4hep.py` to `dd4hep.py`, since `DD4hep.py` has to disappear due to conflicts on MAC.

* 2018-04-13 Markus Frank ([PR#367](https://github.com/aidasoft/dd4hep/pull/367))
  - resolves #361
    The Detector object has a state `Detector::state()` with three values:
    ```cpp
      /// The detector description states
      enum State   {
        /// The detector description object is freshly created. No geometry nothing.
        NOT_READY = 1<<0,
        /// The geometry is being created and loaded. (parsing ongoing)
        LOADING   = 1<<1,
        /// The geometry is loaded.
        READY     = 1<<2
      };
    ```
      It starts with `NOT_READY`, moves to `LOADING` once the geometry is opened and goes to `READY` once the geometry is closed. As suggested in the developers meeting: the initial field object is invalid and gets created only once the geometry is opened. As a corollary, the field may not be accessed before. Geometry parsers must take this behavior into account! 
  
  - Address some compiler warnings.
    - Mainly add override/final statements in header files.
  - Implement a module to invoke python as a DD4hep plugin:
    invoked e.g. by: `geoPluginRun -plugin DD4hep_Python -dd4hep -prompt`
    ```
    geoPluginRun -plugin DD4hep_Python -help
    Usage: -plugin <name> -arg [-arg]                                                  
       name:   factory name     DD4hep_Python                                        
       -import  <string>        import a python module, making its classes available.
       -macro   <string>        load a python script as if it were a macro.          
       -exec    <string>        execute a python statement (e.g. import ROOT.    
       -eval    <string>        evaluate a python expression (e.g. 1+1)          
       -prompt                  enter an interactive python session (exit with ^D)   
       -dd4hep                  Equivalent to -exec "import dd4hep"                
       -help                    Show this online help message.                       
  
       Note: entries can be given multiple times and are executed in exactly the     
             order specified at the command line!                      
    ```
    Implementation wise the plugin is a simple CLI wrapper for TPython.

* 2018-04-12 Marko Petric ([PR#362](https://github.com/aidasoft/dd4hep/pull/362))
  - Update DD4hepManual

* 2018-04-12 Markus Frank ([PR#360](https://github.com/aidasoft/dd4hep/pull/360))
  - Examples: only build some examples depending on the availability of dependencies.
  - DDCore: Add interface to allow URI blocking during file parsing. Default is as now.
  - DDCMS: Add conversion of new shapes.

* 2018-04-10 Markus Frank ([PR#359](https://github.com/aidasoft/dd4hep/pull/359))
  - Bunch of fixes. Mostly in `examples/DDDB`
  - Only build `examples/DDDB` if XercesC is present.
  - Only build `examples/DDCMS` if CLHEP is present

* 2018-04-09 Markus Frank ([PR#357](https://github.com/aidasoft/dd4hep/pull/357))
  - Add configuration options for loading DDDB

* 2018-04-05 Markus Frank ([PR#351](https://github.com/aidasoft/dd4hep/pull/351))
  - To avoid unwanted disappearing conditions sub pools, a conditions slice may be instructed to collect shared references to the used pools in the slice.
  - For python:
    - Move DDG4/SystemOfUnits.py to DDG4/g4units.py
      Keep SystemOfUnits.py with deprecation warning
    - move DD4hep.py to dd4hep.py. 
      import dd4hep also imports all TGeoUnit units into its namespace.
      Hence: import dd4hep; print dd4hep.m  gives: "100.0"
    - DD4hep.py is kept for backwards compatibility issuing a deprecation warning
    - DDG4.py: imports g4units    as G4Units:         DDG4.G4Units.m etc.
    - DDG4.py: imports TGeoUnit as TGeo4Units:  DDG4.TGeoUnits.m etc.
    - Some problem with replacing DD4hepUnits.h with TGeoSystemOfUnits.h
      Surface test complains. To be investigated. Keep old DD4hepUnits for the time being.

* 2018-04-05 Markus Frank ([PR#350](https://github.com/aidasoft/dd4hep/pull/350))
  - Merge `DDCMS` and `DDCMSTests` to `DDCMS`
  - Move `DDDB` to the `examples/`
  - Add tests `DDDB_DeVelo` and `DDDB_DeVelo_Gaudi` missing from #349

* 2018-04-05 Markus Frank ([PR#349](https://github.com/aidasoft/dd4hep/pull/349))
  - Resolves #339 
  - DDDB conditions had a bug when loading from file base. The IOV was not handled properly. Now the resulting IOV is configurable using properties.
  - Added Gaudi like example use case for options handling with the DeVelo detector elements.
  - Configuration improvement in DDG4 action Output2ROOT:
    - New property "DisableParticles" allows to suppress the MCParticle record from being written to the ROOT file.
    - dto. the option "DisabledCollections" allows to NOT write any hit collection.
    - Unit tests for these options are not (yet) present.
  - DDCond: allow for user defined conditions cleanup policies. Base class `dd4hep::cond::ConditionsCleanup`. Callbacks are issued to the class for IOV type pools and IOV dependent pools asking if the pools should be processed.

* 2018-04-05 Marko Petric ([PR#344](https://github.com/aidasoft/dd4hep/pull/344))
  - Move `DDCMS` into examples as it is not core functionality

# v01-07-02

* 2018-06-26 Frank Gaede [PR#413](https://github.com/AIDASoft/DD4hep/pull/413)
  - bug fix in Geant4EventReaderGuineaPig
    - fix ignoring input lines with 'nan'
    - did not work on SL6 w/ gcc


# v01-07-01

* 2018-05-17 Frank Gaede
   - fix bug in input handling, for details see discussion [#387](https://github.com/AIDASoft/DD4hep/issues/387)
     - exclude leptons with zero lifetime from Geant4

# v01-07

* 2018-03-26 Javier Cervantes Villanueva ([PR#343](https://github.com/AIDASoft/DD4hep/pull/343))
  - Fix bug in calculating eta, introduced in #138 
    - use `magFromXYZ` instead of `radiusFromXYZ` to calculate pseudorapidity

* 2018-03-19 Frank Gaede ([PR#338](https://github.com/AIDASoft/DD4hep/pull/338))
  - Include fixes from Chris Burr for the alignments calculator.
   - Add a small study for the LHCb upgrade defining reasonable detector element conditions for the Velo pixel detector using the DDCond derived condition mechanism.
   - To be done: somehow get an example for this mechanism, which works outside Gaudi.

* 2018-03-23 Markus Frank ([PR#340](https://github.com/AIDASoft/DD4hep/pull/340))
  - Improvement for DDDB - case study to implement real-world detector elements.

* 2018-03-28 Frank Gaede ([PR#345](https://github.com/AIDASoft/DD4hep/pull/345))
  - Remove `DDSurfaces` folder as it was merged in `DDRec`

* 2018-03-28 Frank Gaede ([PR#341](https://github.com/AIDASoft/DD4hep/pull/341))
  - Remove top level `DDSegmentation` folder as it is not needed anymore

# v01-06

* 2018-01-31 Ete Remi ([PR#297](https://github.com/aidasoft/dd4hep/pull/297))
  - Geant4Output2LCIO
     - Switch run header writing order at beginning of run instead of end of run

* 2018-03-15 Marko Petric ([PR#335](https://github.com/aidasoft/dd4hep/pull/335))
  - Deduce CLHEP location from `CLHEP_INCLUDE_DIR` ( Fixes #314 )
  - Add `gSystem.Load('libglapi')` to `testDDPython.py` which failed due to TLS issues on local machine
  - Add Geant4 10.4 to test suite

* 2018-03-14 Markus Frank ([PR#336](https://github.com/aidasoft/dd4hep/pull/336))
  - New shape definitions for CMS
    Generic Polyhedra, ExtrudedPolygon, CutTube, TruncatedTube, PseudoTrap.
  - DDDB: allow to configure the match for the entity resolver.

* 2018-03-13 Daniel Jeans ([PR#333](https://github.com/aidasoft/dd4hep/pull/333))
  - changed utility name from graphicalMaterialScan -> graphicalScan
  - add visualisation of electric and magnetic fields (switched by parameter)
  - change order of x/y/z range parameters for ease of use
  - added parameter for name of output root file

* 2018-03-12 Markus Frank ([PR#334](https://github.com/aidasoft/dd4hep/pull/334))
  - `step_length_max` not propagated to Geant4 for `G4UserLimit` instances.
   - All limits can now be set separately for each particle type (or for all as a catchall situation if the particle type is '*'.
   - Fixes #327

* 2017-11-29 Markus Frank ([PR#271](https://github.com/aidasoft/dd4hep/pull/271))
  - Add `LHeD` example (contribution from Peter Kostka)
  - Fix to support `python3` in `DDG4`
  - Fix issue in Volumes.h for backwards compatibility with gcc version < 5
  - Fix type definition of `XmlChar` to also support Xerces-C 3.2.0
  - Fix `AClick` initialization files in `DDG4` (remove dependency on `libDDSegmentation`)

* 2017-11-29 Marko Petric ([PR#270](https://github.com/aidasoft/dd4hep/pull/270))
  - Fields: Remove pessimizing move and shadow warnings

* 2018-02-06 Ete Remi ([PR#301](https://github.com/aidasoft/dd4hep/pull/301))
  - Geant4Output2LCIO plugin
     - Added run number and event number offset properties. Enable steering of run and event counters from outside
     - Added 3 event parameters properties for int, float and string event parameters

* 2018-02-05 Markus Frank ([PR#304](https://github.com/aidasoft/dd4hep/pull/304))
  - Restructure the files a bit to simplify the use of the `XML` and `JSON` parsers and the spirit utilities in other projects.

* 2018-02-28 Frank Gaede ([PR#323](https://github.com/aidasoft/dd4hep/pull/323))
  - make Geant4InputHandling.cpp compatible w/ gcc49 
       - avoid stringstream move operation
       - fixes #320

* 2018-01-24 Markus Frank ([PR#292](https://github.com/aidasoft/dd4hep/pull/292))
  -  Fix bug in Alignment test:
     - A long standing bug was fixed for the test `t_AlignDet_Telescope_align_new`.
     - It was assumed that the `DetElements` were ordered in memory, which is **not** true (probably due to memory randomization).
     - Moved to path dependent ordering. Test now OK.
     - Closes #282 
  
  -  Removed a Bunch of `rootcling` warnings
     - Apparently `rootcling` now expects a class with the name of the dictionary header file.
     - To satify this some dummy anonymous classes were added.
     - Closes #290

* 2018-01-26 Marko Petric ([PR#296](https://github.com/aidasoft/dd4hep/pull/296))
  -  Check dynamic cast if it was successful

* 2018-01-22 Markus Frank ([PR#289](https://github.com/aidasoft/dd4hep/pull/289))
  - Add possibility to parse `XML` from string
    - Please see  #288 for details.
    - The main interface was not changed. Rather a view to the Detector class supports this functionality.
     - The existing view `dd4hep::DetectorLoad` was enhanced to allow the required functionality:
     ```cpp
      Detector detector = ....;
      /// So here is the actual test: We parse the raw XML string
      DetectorLoad loader(detector);
      loader.processXMLString(buffer,0);
     ```
     - An example was added (`ClientTests/src/XML_InMemory.cpp`, which illustrates the usage).
  
  - Improve DDCond manual
    - Improve that the DDCond manual reflects the source code.
    - The possible plugins provided are not all described. This shall be future work.
  
  - Enable the GDML reader plugin
    - Closes #275

* 2018-03-01 Markus Frank ([PR#325](https://github.com/aidasoft/dd4hep/pull/325))
  - Add dictionaries to `DDRec`. 
    - Consequently add the python interface to access the exported classes.

* 2018-03-01 Ete Remi ([PR#321](https://github.com/aidasoft/dd4hep/pull/321))
  - Geant4InputAction and Geant4EventReader : 
      - Register input action pointer to event reader, enabling the event reader to access the Geant4 context
  - Added new class LCIOEventParameters to handle LCIO input event parameters
  - LCIOFileReader : 
      - Add extension with LCIO input event parameters to Geant4event
  - Geant4Output2LCIO : 
      - Get LCIO event parameters from event extension (if any) and write them to LCIO output event

* 2018-03-01 David Blyth ([PR#317](https://github.com/aidasoft/dd4hep/pull/317))
  - DDCore: added CartesianStrip segmentations

* 2018-03-02 Frank Gaede ([PR#328](https://github.com/aidasoft/dd4hep/pull/328))
  - finalize the python bindings for DDRec:
      - implement StructExtension(DetElement&) and toString() 
      - add DetectorData classes to python 
      - add example `dumpDetectorData.py`
      - closes #306 
  - remove deprecated methods and classes in DDRec
      - resolves #326

* 2018-03-06 Andre Sailer ([PR#331](https://github.com/aidasoft/dd4hep/pull/331))
  - Geant4InputHandling: reject stable particles without lifetime (e.g., initial state electrons), fixes #330

* 2018-03-07 Markus Frank ([PR#329](https://github.com/aidasoft/dd4hep/pull/329))
  - Fixes #324
  - Call `TGeoMixture::ComputeDerivedQuantities()` for ROOT >= 6.12.0 (Resolves #281)
  - Start to address issue: https://github.com/AIDASoft/DD4hep/issues/327

* 2018-03-07 Daniel Jeans ([PR#322](https://github.com/aidasoft/dd4hep/pull/322))
  - correct coded expressions for quadrupole, sextupole and octopole fields.
  - correct expression for octopole field in documentation

* 2018-03-09 Ercan Pilicer ([PR#319](https://github.com/aidasoft/dd4hep/pull/319))
  - Remove unnecessary `import exceptions` from python

* 2017-11-30 Markus Frank ([PR#272](https://github.com/aidasoft/dd4hep/pull/272))
  ### DDG4 enhancements
  - Enable framework support in DDG4. The `Geant4Context` is the main thread specific accessor to the `dd4hep`, `DDG4` and the user framework.
      - The access to the `dd4hep` objects is via the `Geant4Context::detectorDescription()` call,
      - the access to `DDG4` as a whole is supported via `Geant4Context::kernel()` and
      - the access to the user gframework using a specialized implementation of:
      `template <typename T> T& userFramework()  const;`
   
      A user defined implementations must be specialized somewhere in a compilation unit of the user framework, not in a header file. The framework object could host e.g. references for histogramming, logging, data access etc.
    
      This way any experiment/user related data processing framework can exhibit it's essential tools to `DDG4` actions.
    
      A possible specialized implementations would look like the following:
    ```cpp
      struct Gaudi  {
        IMessageSvc*   msg;
        IHistogramSvc* histos;
        ....
      };
      template<> Gaudi& Geant4Context::userFramework<Gaudi>()  const  {
        UserFramework& fw = m_kernel->userFramework();
        if ( fw.first && &typeid(T) == fw.second ) return *(T*)fw.first;
        throw std::runtime_error("No user specified framework context present!");
      }
    ```  
      To access the user framework then use the following call:
      ```cpp
      Gaudi* fw = context->userFramework<Gaudi>();
      ```
      of course after having initialized it:
    ```cpp
      Gaudi * fw = ...;
      GaudiKernel& kernel = ...;
      kernel.setUserFramework(fw);
    ```
  
  - `G4RunManager` is now a plugin. To enable user defined run managers in `DDG4`, the run manager is encapsulated in a `Geant4Action`. See `DDG4/plugins/Geant4RunManagers.cpp` how to implement such plugins. Currently there are to factories implemented: 
     -  `G4RunManager` -> factory invokes the single threaded `G4RunManager`
     -  `G4MTRunManager `-> factory invokes the multi threaded `G4MTRunManager`
  
    The factory names here are identical to the names of the native G4 classes.

* 2017-11-30 Marko Petric ([PR#268](https://github.com/aidasoft/dd4hep/pull/268))
  - Introduce new DD4hep web-site: http://dd4hep.cern.ch/
    - Page is automatically build from [this repo](https://gitlab.cern.ch/CLICdp/DetectorSoftware/DD4hep-website)
  - Restructured `doc` folder and removed not doc related stuff to `etc` 
  - Fixes to Doxygen configuration
    - create reference manual `make reference`
  - Better integration of DD4hep tex documentation and cmake
    - create manuals in PDF format `make pdf` create manuals in HTML format `make html`
    - allow creation of documentation if dependencies are not present
  - extended gitlab runners to automatically build documentation PDF and HTML and also reference manual and publish to web-site, thus ensuring always up to date information.

* 2018-02-14 Markus Frank ([PR#310](https://github.com/aidasoft/dd4hep/pull/310))
  - Fix MC truth problem in DDG4
  - Update of the `XML` reader to simplify externalization

* 2018-02-15 David Blyth ([PR#302](https://github.com/aidasoft/dd4hep/pull/302))
  - Added "NOINSTALL" option to cmake build function dd4hep_add_library

* 2018-02-16 Andre Sailer ([PR#315](https://github.com/aidasoft/dd4hep/pull/315))
  - DDG4::ExtraParticles: particles with too small life time were tagged as stable, now correctly marked unstable
  - DDG4::Geant4InputHandling: pass all particles except strings, quarks and gluons to Geant4 to get completely consistent MCParticle History, fixes #307 
  - DDG4::Geant4PrimaryHandler: add property RejectPDGs
  - DDG4::Geant4InputHandling: print geant4 primary particle chains if VERBOSE

* 2017-11-16 David Blyth ([PR#258](https://github.com/aidasoft/dd4hep/pull/258))
  - Corrected `EcalBarrel` constructor after finding significant layer overlaps
  - Corrected `CylindricalEndcapCalorimeter` factors of 2 in layer/slice thickness
  - Corrected `ForwardDetector` factors of 2 in layer/slice thickness

* 2017-12-05 Marko Petric ([PR#278](https://github.com/aidasoft/dd4hep/pull/278))
  - Remove `DD4HEP_USE_CXX11` and `DD4HEP_USE_CXX14` and use [CMake default way](https://cmake.org/cmake/help/v3.1/variable/CMAKE_CXX_STANDARD.html)`CMAKE_CXX_STANDARD` to define C++ standard

* 2017-12-05 Marko Petric ([PR#277](https://github.com/aidasoft/dd4hep/pull/277))
  - Fix logic for ignoring warnings in dictionary creation
  - Add `-Wno-overlength-strings` to all generated targets

* 2017-12-05 peterkostka ([PR#276](https://github.com/aidasoft/dd4hep/pull/276))
  - Update of LHeD example - overlaps (problem with SubdetectorAssembly)

* 2017-12-01 Markus Frank ([PR#275](https://github.com/aidasoft/dd4hep/pull/275))
  - Add Support to Export and Import GDML Data
    - Allow extorts and imports of partial geometry trees with GDML. **This requires however a future version of ROOT**, since some changes were necessary in the ROOT GDML handlers. The code is commented out for the time being. The plugin implementation is in `DDCore/src/gdml/GdmlPlugin.cpp`
  - Remove print statements from DDG4 scripts
     - Replace prints with calls to the python logging utility. Hopefully this shall give at some point python3 compatibility.
  - Update `G4Stepper` factories
    - Add some missing steppers

* 2017-12-01 Marko Petric ([PR#274](https://github.com/aidasoft/dd4hep/pull/274))
  - Revert `DDG4.py` Geant4 initialization parameters back to values specified in Geant4 (see discussion #266)

# v01-05

* 2017-11-10 Dan Protopopescu ([PR#262](https://github.com/aidaSoft/DD4hep/pull/262))
  - Added createGearForSiD minimal plugin solely for use with LCCalibration

* 2017-11-10 Frank Gaede ([PR#261](https://github.com/aidaSoft/DD4hep/pull/261))
  - add `Geant4::HitData::MonteCarloContrib::length`  (step length) 
  - set in all CalorimeterSDActions
  - write out it LCIO if `Geant4Sensitive::DETAILED_MODE` and LCIO_VERS>v02-10

* 2017-11-10 Whitney Armstrong ([PR#260](https://github.com/aidaSoft/DD4hep/pull/260))
  - added electric and magnetic field functions that return the field value (taking the position as the only argument) directly in `dd4hep::OverlayedField`

# v01-04

* 2017-10-17 Markus Frank ([PR#248](https://github.com/aidasoft/DD4hep/pull/248))
  ### VolumeManager Implementation
  A possibly important bug was fixed for the lookup of top level subdetectors in the `VolumeManager` by volume identifers of (sensitive) volumes. Due to a bug in the de-masking possible wrong top level subdetectors were returned. The default use cases typically do not use this call and hence should not be affected.

* 2017-10-17 Shaojun Lu ([PR#247](https://github.com/aidasoft/DD4hep/pull/247))
  - Fix C++11 pointer error by adding include <memory> for 'unique_ptr' (GCC 4.9).

* 2017-10-13 Marko Petric ([PR#246](https://github.com/aidasoft/DD4hep/pull/246))
  ### DDCMS:
  - Improve the CMS excercise. New examples etc.
  - Support for simulation using DDG4 (at least partially - since not all subdetector volumes are accepted by Geant4).
  
  ### DDG4:
  - Event reader returns `EVENT_READER_EOF` if `EOF` is detected rather than a generic IO error.
  - Add generator status word to the `Geant4Particle` object. Remove the extension mechanism, which is very heavy to just add one integer.
  
  ### General:
   - We need to distinguish the plugins using some namespace mechanism. I started to introduce the namespace separator `"_".` Hence all DD4hep plugins start with `DD4hep_<plugin>`. I hope this does not break everything. If it does, please notify me and we can undo.

* 2017-10-13 Whitney Armstrong ([PR#243](https://github.com/aidasoft/DD4hep/pull/243))
  - Added helper function `getAttrOrDefault` (defined in  `DDCore/include/XML/Helper.h`) 
   This  function `getAttrOrDefault(xml::Element e, xml::XmlChar attr_name, T default_value)` will return the attribute  name,  converted to to type `T` but if it is not found it will return `default_value`. When building new detectors supplying this is useful for supplying default attribute values.

* 2017-10-19 Markus Frank ([PR#249](https://github.com/aidasoft/DD4hep/pull/249))
  * Improve the CMS tracker visualisation
  * Add DDG4 simulation example to DDCMS
  * Add some plugins to add visualisation attributes if required (not for the compact description)

* 2017-11-01 David Blyth ([PR#254](https://github.com/aidasoft/DD4hep/pull/254))
  - DDG4/python/DDG4.py: loadDDG4() changed to not raise exception if libraries are already loaded

* 2017-11-01 David Blyth ([PR#252](https://github.com/aidasoft/DD4hep/pull/252))
  - Added requirement of Python 2 in cmake/FindPYTHON.cmake.  This makes clear the requirement of Python 2, and resolves the issue where CMake tries to build with Python 3 in a system where both exist.

* 2017-11-07 Frank Gaede ([PR#256](https://github.com/aidasoft/DD4hep/pull/256))
  - bug fix in `BitField64::operator[std::string]() `
  - make uses of TString in DocumentHandler.cpp compatible with clang9 (on Mac)

# v01-03

* 2017-10-12 Frank Gaede ([PR#244](https://github.com/AIDASoft/DD4hep/pull/244))
  - allow for unbounded surfaces in DDRec
       - add new property `SurfaceType::Unbounded`
       - if a surface is marked unbounded `Surface::insideBounds()` ignores the volume boundaries (and only checks the distance to the surface)

* 2017-09-19 Whitney Armstrong ([PR#233](https://github.com/AIDASoft/DD4hep/pull/233))
  - Added helper  `CellIDPositionConverter::cellDimensions(const CellID& cell)`

* 2017-10-09 Frank Gaede ([PR#242](https://github.com/AIDASoft/DD4hep/pull/242))
  - improve `BitFieldCoder` class
      - remove heap allocation of BitFieldElements
      - add move constructors for efficient filling of vector

* 2017-09-29 Frank Gaede ([PR#238](https://github.com/AIDASoft/DD4hep/pull/238))
  - add new threadsafe class `BitFieldCoder` as replacement for `BitField64`
  - use as `const` everywhere
  - re-implement `BitField64` using `BitFieldCoder`
    - is thread safe if used locally 
    - can be instantiated from `const BitFieldCoder*`

* 2017-09-18 Markus Frank ([PR#234](https://github.com/AIDASoft/DD4hep/pull/234))
  - Created a new example showing the CMS tracking detector
    - Get CMS going with their evaluation. Added a package DDCMS with the conversion plugins for the silicon trackers and the corresponding conversion mechanism for their `xml` structure.

* 2017-09-18 Frank Gaede ([PR#232](https://github.com/AIDASoft/DD4hep/pull/232))
  - fix reading of stdhep/lcio generator files with generator statuses not in [0,3]
  - add `G4PARTICLE_GEN_BEAM` and `G4PARTICLE_GEN_OTHER` to DDG4
    -  `G4PARTICLE_GEN_BEAM`  is generally agreed to be used for beam particles (HepMC, LCIO)
    -  all other status codes vary from generator to generator and we use OTHER
  - for stdhep or lcio input the true generator status is preserved in the lcio output, regardless of its value
  - create a vertex for every parent-less particle in LCIOEventReader
    - this allows for example to read GuineaPig files ( non-prompt pair particles) or special user created files with non-prompt particles
   - Resolves #101

* 2017-09-20 Markus Frank ([PR#235](https://github.com/AIDASoft/DD4hep/pull/235))
  - A more complete version of the CMS tracker
     - Enhanced the CMS tracker example to be more complete.
     - Stopped at some point to convert all CMS algorithms. Hence, the tracker is not complete, but the remaining work looks to be purely mechanical.

* 2017-10-02 Frank Gaede ([PR#239](https://github.com/AIDASoft/DD4hep/pull/239))
  - add cell sizes to printout of `LayeredCalorimeterData::layer`
      - used in `dumpdetector -d`

* 2017-09-14 Frank Gaede ([PR#231](https://github.com/AIDASoft/DD4hep/pull/231))
  - adapt LCIOEventReader for Pythia8 and Whizard2
    - add all parent-less particles to outgoing vertex
    - fixes #226 and closes #229 
    - also used for stdhep files

* 2017-09-07 Daniel Jeans ([PR#227](https://github.com/AIDASoft/DD4hep/pull/227))
  - Fix calculation of cell position in `MegatileLayerGridXY`
  - previously, returned position was the lower corner of the cell
  - after this bug fix, it's the cell centre

* 2017-10-05 Frank Gaede ([PR#241](https://github.com/AIDASoft/DD4hep/pull/241))
  - remove deprecated and unused classes from DDRec

* 2017-10-05 Frank Gaede ([PR#240](https://github.com/AIDASoft/DD4hep/pull/240))
  - add `dd4hep::rec::FixedPadSizeTPCData.zMinReadout`
       - needed to describe the cathode thickness

* 2017-08-21 Markus Frank ([PR#221](https://github.com/AIDASoft/DD4hep/pull/221))
  - Document several classes in doxygen notation.
     - Aim is that there are (at least) no class headers without docs.
     - See [documentation](http://test-dd4hep.web.cern.ch/test-dd4hep/doxygen/html/annotated.html)

# v01-02                                                                                                                                                                        

* 2017-07-14 Daniel Jeans ([PR#204](https://github.com/AIDAsoft/DD4hep/pull/204))
  - add ncellsX/Y as a "parameter", allowing it to be set in compact description. This change is for easier use in the case of a MultiSegmentation. (Only a uniform segmentation can be defined in this way: for more complex cases, must set by driver.)                                                                                                                                                                                                                                 
  - change from array to std::vector to store ncells information                                                                                                                                                                             

* 2017-07-17 Markus Frank ([PR#206](https://github.com/AIDAsoft/DD4hep/pull/206))
  ## Implement ROOT persistency mechanism for detector descriptions (continuation of AIDASoft/DD4hep#202).
  Object extensions are now persistent.                                                                   
                                                                                                          
  This is however not for free: **it requires a dictionary for the extension itself and it requires a dictionary for the class holding the extension**. These are:
   - `dd4hep::DetElement::DetElementExtension<IFACE,CONCRETE>` for `DetElement` extensions.                                                                       
   - `dd4hep::SimpleExtension<IFACE,CONCRETE>` for simple extension managed by the user framework (user calls explicitly destructor).                             
   - `dd4hep::DeleteExtension<IFACE,CONCRETE>` for simple extension managed by dd4hep (dd4hep calls automatically destructor on hosting object destruction).      
   - `dd4hep::CopyDeleteExtension<IFACE,CONCRETE>`. As above, but these extensions support calling the copy constructor of the embedded object and hence allow to copy also the hosting objects.
                                                                                                                                                                                                
  Please note: to persistify these objects it was necessary to no longer use the type-info of the objects as an identifier, but rather a 64-bit-hash of the raw type-info-name. This relies that this name is identical across platforms. This typically is true for linux, but not enforced by any standard.                                                                                                                                                                             

* 2017-08-11 Andre Sailer ([PR#213](https://github.com/AIDAsoft/DD4hep/pull/213))
  - CMake:: dd4hep_generate_rootmap: use CMAKE_INSTALL_LIBDIR if it is set. If the macro is called from other libraries this variable might be set and should be used for consistency. Fixes #212

* 2017-07-13 Markus Frank ([PR#202](https://github.com/AIDAsoft/DD4hep/pull/202))
  ## Implement ROOT persistency mechanism for detector descriptions              
                                                                                 
  The following commits allow to save and restore Detector objects (ie. the full detector description) to/from ROOT. Most changes affected the usage of void pointers. 
                                                                                                                                                                       
  It is **NOT** possible to:                                                                                                                                           
   * save object extensions. Another usage of void pointers and type-info objects, which both cannot be saved.                                                         
   * save callback objects. Callbacks store in memory pointers to member functions. These depend on the loading of libraries at run-time and hence may differ from application to application.
   * save conditions slices. This shall be a futute extension. There should be no fundamental problem doing so - it was simply not yet done.                                                  
                                                                                                                                                                                              
  In the example area a new slot call "Persistency" was created with various tests:                                                                                                           
   * Save and restore simple conditions                                                                                                                                                       
   * Save and restore the MiniTel detector (`examples/ClientTests/compact/MiniTel.xml`)                                                                                                       
   * Save and restore the CLICSiD detector (`examples/CLICSiD/compact/compact.xml`)                                                                                                           
                                                                                                                                                                                              
  The detector examples also have a set of tests associated to check the validity of the restored information.                                                                                
                                                                                                                                                                                              
  ## Integrate `DDSegmentatation` into `DDCore`                                                                                                                                               
                                                                                                                                                                                              
  As a first step to start the cleanup of `DDSegmentation` the code was integrated into `DDCore`. For backwards compatibility a dummy library `libDDSegmentation.so` is kept, so that client cmake files directly referring `DDSegmentation` still work. **This tough is only a temporary measure and users should remove references to the DDSegmentation library.** Include files can be accessed as before. These are part of the `DDCore` include directory.                          

* 2017-08-15 Yorgos Voutsinas ([PR#216](https://github.com/AIDAsoft/DD4hep/pull/216))
  * Adding variable "nocore" for beam pipe (with default value = false) In case the variable appears in the BP xml file as "true", the BP sections will have no core of beam material, in order that someone might add various BP walls made of different materials while avoiding G4 overlaps. Example of use:                                                                                                                                                                           
  ```xml                                                                                                                                                                                                                                     
  <detector name="BeBeampipe" type="DD4hep_Beampipe_o1_v01" insideTrackingVolume="true" nocore="true" vis="BeamPipeVis">                                                                                                                     
  ```                                                                                                                                                                                                                                        

* 2017-08-15 Jan Strube ([PR#215](https://github.com/AIDAsoft/DD4hep/pull/215))
  - LCIOEventReader: adding parameter for the name of the MCParticle collection: `MCParticleCollectionName`

* 2017-08-14 Markus Frank ([PR#217](https://github.com/AIDAsoft/DD4hep/pull/217))
  ## Enhance ROOT Detector Description Persistency                               
  Conditions slices from the DD4hep conditions store can now be saved to ROOT files ans named objects. Conditions persistency examples are added to the `examples/Conditions` section.
  The examples are derived from the Telescope and CLICSiD example:                                                                                                                    
   - `Conditions_Telescope_root_save`                                                                                                                                                 
   - `Conditions_Telescope_root_load_iov`                                                                                                                                             
   - `Conditions_Telescope_root_load_pool`                                                                                                                                            
   - `Conditions_CLICSiD_root_save_LONGTEST`                                                                                                                                          
   - `Conditions_CLICSiD_root_load_iov_LONGTEST`                                                                                                                                      
   - `Conditions_CLICSiD_root_load_usr_LONGTEST`                                                                                                                                      
   - `Conditions_CLICSiD_root_load_cond_LONGTEST`                                                                                                                                     
                                                                                                                                                                                      
  ## Fix Handle Problem when Accessing Materials from Volumes                                                                                                                         
  A cast problem was not spotted in the last commit, which manifested itself in accessing invalid materials from volumes. This commit resolves #211 , reported by @jhrdinka.          
                                                                                                                                                                                      
  A set of tests checks this behaviour in the ClientTests: `ClientTests_volume_materials_<text>`, where the volume tree is scanned and for all sensitive volumes and access the corresponding `materials/TGeoMedium` entities from the volume object.                                                                                                                                                                                                                                     

* 2017-07-18 Shaojun Lu ([PR#208](https://github.com/AIDAsoft/DD4hep/pull/208))
  - added "#include <memory>" for the smart pointers to DD4hepRootPersistency.cpp

* 2017-07-19 Markus Frank ([PR#209](https://github.com/AIDAsoft/DD4hep/pull/209))
  - Implement `dd4hep::Tube` using `TGeoTubeSeg` (See also Issue AIDASoft/DD4hep#203 for details)

* 2017-07-21 Markus Frank ([PR#210](https://github.com/AIDAsoft/DD4hep/pull/210))
  ## Implement ROOT persistency mechanism for the conditions                     
                                                                                 
  Conditions pools can now be made persistent provided all the dictionaries for the payload objects are provided. A new class `ConditionsRootPersistency` allows to save and re-load conditions pools to/from a ROOT file. Such pools can either be:                                                                                                                                                                                                                                      
   - Simple `ConditionsPool` objects                                                                                                                                                                                                         
   - The entire `IOV` indexed pool set (class `ConditionsIOVPool`) or                                                                                                                                                                        
   - A the pool used by a `ConditionsSlice` (class `UserPool`).
   - A std::vector<Condition> which belong all to the same IOV

  In any case the restoration of the saved conditions is performed through the `ConditionsManager` interface in order to ensure proper management of the added condition objects.

  Some example plugin tasks were added in examples/Conditions:
   - `DD4hep_ConditionExample_save` to save conditions to a ROOT file.
   - `DD4hep_ConditionExample_load` to restore conditions from file.

  Others to come.

  ## Split of dictionary files
  The ROOT dictionary creation in `DDCore` was getting increasingly large. Now the ROOT dictionaries are created in several files, what firstly allows them to be produced in parallel and secondly eases the compilation due to smaller generated file sizes.


# v01-01

* 2017-07-06 Markus Frank ([PR#201](https://github.com/AIDASoft/DD4hep/pull/201))
  ## DDCore: Changes to the VolumeManager interface
  
    Recent descrepancies showed that the call to lookup a placement
    from the volume manager may have an ambiguous meaning:
    It may (as used until now) be the placement of thge closest
    detector element - a functionality used by various tests
    or be the placement of the sensitive volume itself.
    So far, since each sensitive volume in the DD4hep tests
    is represented by a DetElement structure, both
    approaches returned the same placed volume.
  
    Since there is the possibility to have sensitive volumes, which are not
    directly connected to a single DetElement structure, this call was
    split to resolve this ambiguity:
  
    /// Lookup a physical (placed) volume identified by its 64 bit hit ID
    PlacedVolume lookupVolumePlacement(VolumeID volume_id) const;
    /// Lookup a physical (placed) volume of the detector element
    /// containing a volume identified by its 64 bit hit ID
    PlacedVolume lookupDetElementPlacement(VolumeID volume_id) const;

* 2017-07-06 Andre Sailer ([PR#200](https://github.com/AIDASoft/DD4hep/pull/200))
  - Now will give a warning if multiple entities (e.g., constants) of the same name are defined in the XML

# v01-00-01

* 2017-07-04 Frank Gaede ([PR#199](https://github.com/AIDASoft/DD4hep/pull/199))
  - bug fix for VolumeManagerContext::toElement() and VolumeManagerContext::placement() 
        - set flag=true in VolumeManager_Populator::add_entry when 
           a ContextExtension is needed, i.e. sensitive volume is not DetElement's volume
  - fixes problems in CellIDPositionConverter

# v01-00

* 2017-06-22 Marko Petric ([PR#192](https://github.com/AIDASoft/DD4hep/pull/192))
  - Move `AlignDet_Telescope_readback_xml` to later in the pipeline since it depends on the output of `AlignDet_Telescope_write_xml`

* 2017-06-22 Andre Sailer ([PR#191](https://github.com/AIDASoft/DD4hep/pull/191))
  - Surface: fix memory leak of transformation matrix
  - XML::Layering: fix memory leak of contained layers in the object

* 2017-06-23 Andre Sailer ([PR#197](https://github.com/AIDASoft/DD4hep/pull/197))
  - Fix memory leaks for Tube, EllipticalTube and Polyhedron

* 2017-06-23 Andre Sailer ([PR#196](https://github.com/AIDASoft/DD4hep/pull/196))
  - CMake: add `Project( DD4hep )`, needed to get the correct CMAKE_CXX_COMPILER_ID on macs due to CMP0025 (cmake policy)
  - CMake: fix treatment of linker flags, they are now properly set for Linux and Macs to error when undefined functions are encountered at link time
  - CMake: fix elif --> elseif when checking threading libraries

* 2017-06-23 Frank Gaede ([PR#195](https://github.com/AIDASoft/DD4hep/pull/195))
  - fix crash in `dd4hep::rec::Surface` after changes in Handle assignment (PR #193)
  - fix use of deprecated `dd4hep::rec::MaterialManager` c'tor in Surface

* 2017-06-20 Frank Gaede ([PR#185](https://github.com/AIDASoft/DD4hep/pull/185))
  - bug fix in material utilities
       - call `MaterialManager( Volume v)` with `Detector.world().volume()`

* 2017-06-20 Marko Petric ([PR#184](https://github.com/AIDASoft/DD4hep/pull/184))
  - Reinstate the full test-suite on Travis

* 2017-06-20 Markus Frank ([PR#183](https://github.com/AIDASoft/DD4hep/pull/183))
  - Unify header guards in DDCore
  - Add header to steer ignoring warnings of rootcling generated dictionaries.

* 2017-06-20 Frank Gaede ([PR#182](https://github.com/AIDASoft/DD4hep/pull/182))
  - cleanup of namespace `dd4hep::rec`
    - remove obsolete bwd compatibility for `DD4hep::DDRec`
    - re-introduce `[deprecated]` warnings for unmaintained classes in DDRec/API 
    - re-fix deprecated c'tor for `MaterialManager` in material utilities

* 2017-06-20 Markus Frank ([PR#181](https://github.com/AIDASoft/DD4hep/pull/181))
  - Attack many warnings from:
    - `-Wshadow`
    - `-Winclude-hygiene`
    - `-Woverlength-strings` (int cling dictionaries)

* 2017-06-20 Markus Frank ([PR#179](https://github.com/AIDASoft/DD4hep/pull/179))
  - Remove a bunch of shadow warnings and include-hygiene warnings.

* 2017-06-21 Marko Petric ([PR#169](https://github.com/AIDASoft/DD4hep/pull/169))
  - Make boost explicit requirement for DD4hep and drop DD4HEP_USE_BOOST

* 2017-06-21 David Blyth ([PR#168](https://github.com/AIDASoft/DD4hep/pull/168))
  - Added environment helper scripts `thisdd4hep_only.(c)sh` that only set up variables for DD4hep and not for dependencies.

* 2017-06-19 Markus Frank ([PR#178](https://github.com/AIDASoft/DD4hep/pull/178))
  - Update documentation after reorganization of namespaces (put back previous docs).

* 2017-06-19 Markus Frank ([PR#175](https://github.com/AIDASoft/DD4hep/pull/175))
  ## DD4hep namespace reorganization
  
  Re-organize namespaces according to the decisions of the DD4hep developers meeting from 16th June we have decided:
  
  1. all namespaces will be lower case and shorter
      * rename namespace `DD4hep` -> `dd4hep`
      * rename namespace `DD4hep::DDRec` -> `dd4hep::rec`
      * rename namespace `DD4hep::Simulation` -> `dd4hep::sim`
      * rename namespace `XML` -> `xml` and `JSON` -> `json`
      * rename all other namespaces according to this pattern
  2. The namespace `DD4hep::Geometry::` will be incorporated into `dd4hep::`
  3. All utilities will be moved `dd4hep::detail`
  4. `LCDD` will be renamed to `Detector` and current `Detector.h` will be renamed to `DetElement.h`
  8. Examine if `DDSegmentation` can be incorporated into `DDCore` and make it volume aware
      * If this this cannot be achieved in whole place `DDSegmentation` into the right namespace
  
    ## DDParsers
  
    DDParsers are now a separate package. This does not make it yet standalone,
    but it is at the same level as e.g. DDSeqmentation. Any librarian is
    encouraged to externialize it fully.

# v00-24

* 2017-06-08 Markus Frank ([PR#160](https://github.com/AIDASoft/DD4hep/pull/160))
  * Add a new class `AlignmentsNominalMap`, which behaves like a `ConditionsMap` and handles alignment entries. 
  * The `AlignmentsNominalMap` is not a conditions cache per-se. This implementation  behaves like a `conditionsmap`, but it shall not return real conditions to the user, but rather return the default alignment objects (which at the basis are conditions as well) to the user. These alignments are taken from the `DetElement` in question `Alignment DetElement::nominal()`.
  
  * The basic idea is to enable users to write code "as if" there would be conditions present. This is important to ease in the lifetime of the experiment the step from the design phase (where obviously no conditions are taken into account) to a more mature phase, where alignment studies etc. actually are part of the "bread and butter work".
  
  * Added a corresponding example in examples/AlignDet:
  ```
  $>   geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_nominal \
           -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml
  ```
     * Access the DetElement nominal conditions using the `AlignmentNominalMap`.
     Any use of DDCond is inhibited.
       1) We use the generic printer, which during the detector element scan accesses the conditions map.
       2) We use a delta scanner to extract the nominal deltas from the `DetElement`'s nominal alignments
       3) We use a `ConditionsTreeMap` to perform the alignments re-computation.

* 2017-06-08 Markus Frank ([PR#159](https://github.com/AIDASoft/DD4hep/pull/159))
  # Implementation of the decisions made at the Conditions mini-workshop
  
  ## Access mechanisms of DD4hep conditions for utilities
  
  Access to conditions is solely supported using the interface class DDCore/ConditionsMap.
  * All utilities must use this interface.
  * Any concrete implementation using conditions/alignment utilities must implement this interface
  * Basic implementation using STL `map`, `multimap` and `unordered_map` are provided.
  * A special no-op implementation of this interface shall be provided to access "default" alignment conditions. This implementation shall fall-back internally to the `DetElement::nominal()` alignment. 
  Known clients: `VolumeManager` (hence: DDG4, DDRec, etc.)
  
  Though this sounds like a trivial change, the consequences concern the entire conditions
  and alignment handling. This interface decouples entirely the core part of DD4hep
  from the conditions cache handling and the alignment handling.
  
  Based on this interface most utilities used to handle conditions, detectors scans
  to visit `DetElement` related condition sets, alignment and conditions printers etc.
  
  For details, please see:
  ```
  DDCore/include/DD4hep/AlignmentsPrinter.h
  DDCore/include/DD4hep/AlignmentsProcessor.h
  DDCore/include/DD4hep/ConditionsPrinter.h
  DDCore/include/DD4hep/ConditionsProcessor.h
  DDCore/include/DD4hep/DetectorProcessor.h
  ```
  
  ## Naming conventions for detector conditions
  
  * Condition are logically attached to DetElements
     * Condition names are: `DetElement.path()+"#"+condition-name`
         Example: `/world/LHCb/DownstreamRegion/Muon/M5/M5ASide/R3ASide/Cham046#alignment`
  
  * Condition keys are a `int64` compound of two `int32`:
  ```cpp
     union {
       int64 key;
       struct {
         int32 item_key;
         int32 det_key;     // Needs to be the high word to have a properly ordered map
       } values;
     };
     det_key  = hash32(DetElement.path())
     item_key = hash32(condition-name)
  ```
     **Condition keys must be unique throughout the detector description.**
  
  * Alignment conditions naming conventions:
    * Alignment-delta conditions are called `alignment_delta`.
    * Fully qualified alignment conditions are called `alignment`.
     DD4hep provided alignment utilities rely on this convention.
  
  * Other conditions can be named freely.
  
  ## Important Notice
  The **Alignment conditions naming conventions** are already used by several utilities involving alignments. If you plan to use these, do not freely ignore these recommendations. When the naming conventions are ignored, these utilities shall not work.
  
  ## Updates to DDCond
  DDCond implements a working conditions cache following the design criteria sketched above. The `conditionsSlice` object implements (though by forwarding to the `ConditionsUserPool`) a `ConditionsMap` interface.
  
  The `DD4hep_ConditionsMapUserPool` plugin implements in a very efficient way this interface using an ordered map. Using the above described key definition, this implementation allows very efficient scans of conditions/alignments etc. of individual detector elements, since conditions which belong to the same detector element are contiguous.
  
  ## Alignment handling/computations
  Using the conditions maps, the computation of (mis-)alignment data from deltas
  is no longer bound to the conditions mechanisms.
  
  A special utility called `AlignmentsCalculator` is put in place (see `DDCore/include/DD4hep/AlignmentsCalculator.h`) to facilitate the computation of a coherent set of alignments given a set of delta-parameters. This mechanism is much simpler, easier to understand and far less code intensive than the previously designed callback mechanism where alignments are obtained using conditions derivation.
  
  ## Update of the existing examples
  
  The example sets in DDDB, `examples/Conditions, examples/AlignDet`, `examples/DDDB` were updated according to the changed mechanism of accessing conditions. Here we can see the real benefits of the new approach: keeping same functionality, the examples became way off simpler. Simply count the number of lines of code.

* 2017-06-17 Marko Petric ([PR#170](https://github.com/AIDASoft/DD4hep/pull/170))
  - Add clang flag to warn about using namespace directive in global context in header

* 2017-06-17 Frank Gaede ([PR#167](https://github.com/AIDASoft/DD4hep/pull/167))
  - renamed the namespace DD4hep::DDRec to dd4hep::rec (see #166)
         - provide backward compatibility to outside world for now
  - moved the interfaces in namespace DDSurfaces to dd4hep::rec
         - provide backward compatibility to outside world for now

* 2017-06-15 Frank Gaede ([PR#165](https://github.com/AIDASoft/DD4hep/pull/165))
  - started to cleanup DDRec
        - don't use  LCDD::getInstance() in SurfaceManager and SurfaceHelper
        -  deprecate unused(?) classes in DDRec/API and DDRec/Extensions
        -  deprecate MaterialManager() using LCDD::getInstance()

# v00-23

* 2017-05-12 Marko Petric ([PR#152](https://github.com/aidasoft/DD4hep/pull/152))
  - Update CI to GCC 7.1 and LLVM 4.0 and include Geant4 10.3

* 2017-05-22 Frank Gaede ([PR#154](https://github.com/aidasoft/DD4hep/pull/154))
  - protect against NANs in Guineapig pairs files in Geant4EventReaderGuineaPig
  - make INFO printout more consistent  with dd4hep style

* 2017-06-07 Frank Gaede ([PR#157](https://github.com/aidasoft/DD4hep/pull/157))
  -  bug fix in test_cellid_position_converter
       - with this no tests for position from cellID lookup should fail
  - re-implement ```CellIDPositionConverter::cellID(pos)```

* 2017-06-08 Marko Petric ([PR#156](https://github.com/aidasoft/DD4hep/pull/156))
  - Mark all fallthroughs in case statements with attributes to suppress warning

* 2017-06-01 Frank Gaede ([PR#155](https://github.com/aidasoft/DD4hep/pull/155))
  - add new class rec::CellIDPositionConverter
        - replaces rec::IDDecoder
        - implement positionNominal(CellID id) and cellID(position)
       - prepare for using alignment map by separating transforms to DetElement and daughter volume
       - do not use deprecated methods/members in VolumeManager
  - add test_cellid_position_converter.cpp
  - add VolumeManagerContext::toElement
       - transform from sensitive volume to next DetElement

# v00-22

* 2017-04-28 Markus Frank ([PR#148](https://github.com/aidasoft/DD4hep/pull/148))
  Improvements to the compact xml processing
  ===========================================
  
  Elements may now be specified within compact in 2 ways:
  1) old way: create an effective element:
  `    <element Z="4" formula="Be" name="Be" >`
  `      <atom type="A" unit="g/mol" value="9.01218" />`
  `    </element>`
  
  2) create element by defining an isotope mixture:
  `    <isotope name="C12" Z="6" N="12"/>`
  `      <atom unit="g/mole" value="xxxx"/>`
  `    </isotope>`
  `.....`
  `   <element name="C">`
  `     <fraction n="0.9893" ref="C12"/>`
  `     <fraction n="0.0107" ref="C13"/>`
  `   </element>`
  
  Improved debugging of compact xml conversions
  The following tags allow to enable increased prinout depending on additional tags in the compact file:
  `    <debug>`
  `      <type name="isotopes"     value="1"/>`
  `      <type name="elements"     value="1"/>`
  `      <type name="materials"    value="0"/>`
  `      <type name="visattr"      value="0"/>`
  `      <type name="regions"      value="0"/>`
  `      <type name="readout"      value="0"/>`
  `      <type name="limits"       value="0"/>`
  `      <type name="segmentation" value="0"/>`
  `    </debug>`
  
  Disable the ROOT TGeo element table from the compact xml. 
  Note: ALL elements must then be specified in the XML material database.
  `    <geometry>`
  `      <clear name="elements"/>`
  `    </geometry>`
  
  By default compact accepts exactly one input file (others may be included therein).
  In order to process iteratively multiple input files, the opening and the closing of the
  geometry steering tags may be added to the compact description. This example 
  gives the default behaviour:
  `    <geometry open="true" close="true"/>`
  
  Please note: per compact file exactly ONE geometry tag is allowed.
  
  
  Debugging the DDG4 geometry conversion mechanism
  =============================================
  New boolean properties of the Geant4DetectorGeometryConstruction object,
  which result in debugging printouts (defaults are OFF):
    DebugMaterials
    DebugElements
    DebugShapes
    DebugVolumes
    DebugPlacements
    DebugRegions
    PrintPlacements
    PrintSensitives

* 2017-04-28 Ben Couturier ([PR#146](https://github.com/aidasoft/DD4hep/pull/146))
  * Trivial fix for the DDDB converter to create paramphysvol3D volumes, which are otherwise ignored.

* 2017-04-20 Andre Sailer ([PR#145](https://github.com/aidasoft/DD4hep/pull/145))
  - LCIOOutput: Add setting of ProducedBySecondary bit for SimTrackerHits if the hit is produced by a particle that is not stored in the MCParticle collection, needs lcio 2.8

* 2017-05-05 Andre Sailer ([PR#150](https://github.com/aidasoft/DD4hep/pull/150))
  - Always create dd4hepConfigVersion.cmake in CMAKE_INSTALL_PREFIX and cmake folder
  - Create DD4hepConfig.cmake also in cmake folder
  - renamed Cmake Macro GENERATE_PACKAGE_CONFIGURATION_FILES to DD4HEP_GENERATE_PACKAGE_CONFIGURATION_FILES so it does not clash with the macro of the same name in ilcutil/cmakemodules

* 2017-05-07 Andre Sailer ([PR#151](https://github.com/aidasoft/DD4hep/pull/151))
  - Use cmake to create Version.h file to contain dd4hep version information and macros
  - Change the way dd4hep package version is defined and set standard cmake variables for this purpose

# v00-21

* 2017-04-03 Marko Petric ([PR#142](https://github.com/AIDASoft/DD4hep/pull/142))
  - Update to the CI system:
    - Install directly cvmfs on base system, which removes the need for the parrot connector 
    - Replace CernVM docker with plain docker
    - This reduces the build run time from 50 min to 25 min

* 2017-03-27 Shaojun Lu ([PR#134](https://github.com/AIDASoft/DD4hep/pull/134))
  - Set verbose true for G4EmSaturation to printout Birks coefficient.

* 2017-03-29 Frank Gaede ([PR#139](https://github.com/AIDASoft/DD4hep/pull/139))
  - add a utility to dump the B-field for a given Volume
          - usage: dumpBfield compact.xml x y z dx dy dz [in cm]  
          - will dump the B-field in volume [-x:x,-y:y,-z,z] with steps [dx,dy,dz]

* 2017-03-29 Joschka Lingemann ([PR#138](https://github.com/AIDASoft/DD4hep/pull/138))
  - Direct implementation that calculates eta from cartesian coordinates
  - Fix: Add registration of Phi-Eta segmentation

* 2017-03-29 Joschka Lingemann ([PR#137](https://github.com/AIDASoft/DD4hep/pull/137))
  - Adding GridRPhiEta a segmentation of equidistant size in R, Phi and Pseudorapidity
  - Adding GridPhiEta a segmentation of equidistant size in Phi and Pseudorapidity

* 2017-03-28 Markus Frank ([PR#135](https://github.com/AIDASoft/DD4hep/pull/135))
  - Accidentally the Segmentations of Joschka were added in the wrong place of the hierarchy.
     I removed them. He will later add them to the proper location.
  - Some C++ warnings concerning the C++11 standard were also fixed.

* 2017-03-24 Yorgos Voutsinas ([PR#132](https://github.com/AIDASoft/DD4hep/pull/132))
  - modifying the LayeredCalorimeterData struct in order to cope with conical shaped calorimeters

* 2017-03-31 Markus Frank ([PR#143](https://github.com/AIDASoft/DD4hep/pull/143))
  - Add new test for multi segment multi collections segmentations using a calorimeter endcap.
     See also: https://github.com/AIDASoft/DD4hep/issues/141, which is still unresolved,
     but seems not to be directly related to the Monte-Carlo truth handling.
  - Side effect: add Geant4EventActions to dump hits and particles
  - Fix a linker problem for unicode tags.

* 2017-03-31 Frank Gaede ([PR#140](https://github.com/AIDASoft/DD4hep/pull/140))
  - clarify documentation for CartesianField and implementations
  - make clear that  void fieldComponents() has to add
     the new field to the given field vector

* Marko Petric 2017-03-23 
  - Convert Release notes to markdown
  - Add some text about WIP and issue tracking
  - Add CONTRIBUTING.md and PULL_REQUEST_TEMPLATE

* Markus Frank 2017-03-17
 
 - Add a new input type by parsing JSON files. The basic json interpreter
    is present and offers essentially the same interface as the XML persers.
    The integration to the compact dialect however is not yet done.
    It should become however a valid alternative to parsing XML files
    with a small maintenance overhead, since the enhancement implementation
    present in XMLDimesion.h, XMLDetector.h and XMLChildValue.h is shared
    and instantiated for both.

* Markus Frank 2017-03-10
 
 - New round to kill coverity deficiencies.
 - Add exception try-catch clauses around various main programs in UtilityApps
    See UtilityApps/src/main.h for details.

* Markus Frank 2017-03-09
 
 - Fix issue with long volume ids exceeding 32 bits.
    Test added in examples/ClientTests
 - Fix volume manager id identical placed volumes are used in different places of the hierarchy.
    Test added in examples/ClientTests
 - Remove various svn left-overs (ID$ strings etc.)


* Frank Gaede 2017-02-10
 
  - allow event readers to create more than one vertex per event
     this should be possible as most generator formats allow to specify
     more than one event vertex 
  - changed signature of Geant4EventReader::readParticles(int,Vertex*, Particles&)
        to Geant4EventReader::readParticles(int,Vertices&, Particles& )
  - implement in LCIOEventReader, Geant4EventReaderHepEvt and Geant4EventReaderHepMC
  - for now still one vertex only is created using the first final state particle
        for HepEvt and LCIO

# v00-20

* Frank Gaede 2017-01-13 
- fix computation of radiation and interaction lengths
- require the density to be specified in the materials xml file
 
* Frank Gaede 2017-01-04 
- adopt numbers in test_surface.cc to elements.xml
- use elements.xml generated with plugin in DDTest
-  add ROOT version to dumped element table

* Frank Gaede 2016-12-22 
- fix test_surfaces by adapting to elements.xml
- fix test_units by including elements.xml
 
* Markus Frank 2016-12-21 
- Add plugin to dump the default ROOT element table
 
* Markus Frank 2016-12-20 
- Remove obsolete build flags
 
* Markus Frank 2016-12-19 
- Add Multi-threading conditions example
 
* Andre Sailer 2016-12-16 
- Add drivers for Beampipe, Mask and Solenoid from lcgeo, changed name to dd4hep_*
 
* Rosa Simonielo, Frank Gaede 2016-12-15 
- add new struct rec::NeighbourSurfacesStruct defined for neighbouring surfaces
 
* Frank Gaede 2016-12-14 
- fix library pathes in env scripts for macos
- use DD4HEP_LIBRARY_PATH and full lib path on mac
- apply rpath compiler settings to GaudiPluginService
- make compatible w/ Geant4 10.3
 
* Marko Petric 2016-12-13 
- Fix compiler flag handling
 
* Daniel Jeans 2016-12-08 
- add utility graphicalMaterialScan
 
* Markus Frank 2016-12-07 
- Fix compiler error on MacOSX gcc 4.9
 
* Marko Petric 2016-12-07 
- Remove few tests from Travis
- move flag to CMAKE_SHARED_LINKER_FLAGS
- Remove the dynamic lookup on runtime for libs on mac
 
* Markus Frank 2016-12-05 
- First version of conditions and alignments
 
* Andre Sailer 2016-12-06 
- DDTest: fix location to install DDtest header files
- Remove minimum required cmake version from DD4hepBuild, this interferes with other packages depending on dd4hep
 
* Marko Petric 2016-12-02 
- Fix missing CLHEP in thisdd4hep.sh
- Clean up FindPYTHON.cmake file
- Change gaudi auto_ptr to unique_ptr since the auto is deprecated
- Fix initAClick
 
* Marko Petric 2016-11-29 
- Fix rpath issues on mac with python
- Add custom DynamicPath on mac when loading python libs
- Add function to set the DD4HEP_LIBRARY_PATH
 
* Marko Petric 2016-11-28 
- Update all minimum CMake versions
- Fix gitlab builds
- Rename MakeRootMap to MakeGaudiMap for consistency
- Remove ROOT5 things and make approprite fixed to the usage of ROOTConfig.cmake
 Remove FindROOT.cmake and use from now on ROOTConfig.cmake
 
 
* Markus Frank 2016-11-30 
- New version of conditions handling
 
* Andre Sailer 2016-11-25 
- Fix ProductionCut conversion in Geant4Converter.cpp: cut is a range
 
* Markus Frank 2016-11-24 
- Fix linker errors on MAC
- Remove compiler warnings on MACOSX
- First fixes to version of alignment constant processing also remove where visited the $ statements in the files.
They are useless, since git does not support them. Fixed some tests, which no longer properly worked in
the conditions area.
- First usable version of alignment constant processing
 
 
* Markus Frank 2016-11-16 
- Try to improve efficiency using C++11 default operations
 
* Markus Frank 2016-11-10 
- Simplify opaque data mappings for conditions
 
# v00-19
* 2016-11-17 D.Jeans
- rewritten MegatileLayerGrid class 
  - not backward compatible 

* M.Petric:
- improved Travis CI configuration 
- remove some tests from CI

# v00-18

* 2016-11-09 F.Gaede
- updated rec::LayeredCalorimeterData::Layer:
  - remove deprecated thickness 
  - add phi0  
- add copy assignement to rec::MaterialData

* 2016-11-08 M.Frank
- Improve conditions handling. Started to implement using simple telescope
  detector from C.Burr.
- Add variable DD4HEP_LIBRARY_PATH to be used on MacOSX El Capitane
  to overcome DYLD_LIBRARY_PATH protection issues.
- Fix to copy with negative(rebased) IDDDescriptor fields
  (affects IDDDescriptor and VolumeManager).
  2 New tests:
  - ClientTests/Bitfield_SidesTest.xml to test readout string:
    system:8,barrel:-2
  - ClientTests/Bitfield_SidesTest2.xml to test readout string:
    system:16,barrel:16:-5
    
* 2016-10-18 M.Frank
-  Due to pressure of the FCC folks, I tried to implement a more dd4hep like implementation of the
  the segmentation objects. For testing only CartesianGridXY. If this mechanism works,
  it could be a starting recipe for the rest of the segmentations. The draw-back of this approach is,
  that assignments are not reversible:
  dd4hep::CartesianGridXY xy = readout.segmentation();  // Works
  dd4hep::Segmentation seg = xy;                    // Should not work

-  Reason: the managed objects are different....at some point in time I will have to find a
  clean solution for this, but the required changes for such a solution shall be manageable.

# v00-17

* 2016-09-29 V.Volkl
  - Small fixes and extensions of DDEve. Show "Views" menu even if specifying xml on the command line.
    Add possibility to specify event data in EveDisplay method, rather than having to select it in the gui dialogue.
    Selects FCC EventHandler if specified in the config xml (as an "eventHandler"-attribute to the "display" node),
    but keeps default behavior if not.


* 2016-08-24 M.Frank
      - Adding first somehow useful implementation to use conditions and the consequent loading thereof.
	Used by the DDDB implementation/example. DDDB is an alternative way to populate the dd4hep
	detector description using LHCb's detector description database.
	The reason is, that only a running experiment has a reasonable base to conditions data
	to excercise the dd4hep conditions.
	If interested, please have a look in the DDDB examples.

  - Still TODO:
    - A formal way to bootstrap the conditions loading still has to be found.
    - Conditions loading from XML files and a small comprehensive example.

# v00-16
* 2016-07-26 Shaojun Lu  
- Added new MegatileLayerGridXY segmentation for Scintillator strip Ecal, which is implemented by K.Kotera, 
 and used by SEcal04Hybrid geometry driver.

 - many ( minor fixes ) by F. Gaede, M. Petric, A Sailer, ...

* 2016-06-24 M.Frank
- Implement multiple segmentations.
  Though one readout objects (associated to one DetElement) may only have on segmentation,
  The MultiSegmentation type allows to have several sub-segmentations, which can be chosen
  from.
  Please see examples/ClientTests/*/MultiSegmentations

- At the same time allow the readout object to defined multiple collections through
  the IDDescriptor.
  Please see examples/ClientTests/*/MultiCollections

- The combined example can be found in
  Please see examples/ClientTests/*/NestedSegmentations



* 2016-05-03 M.Frank
- Green light is ON. You may use revision 2237 and above.

* 2016-05-03 M.Frank
- **Mega commit. Do not use until green signal arrives with seperate release notice.**
- Package DDDB: Prove of concept, that we can populate the geometry also from other
  sources than xml files obeying the lccdd notation.
- DDCore/XML: Implement the possibility to use external entity resolution for Xerces.
  There is not equivalent in TinyXML (AFAIK).
- Remove some of the Spirit parsers, since the libraries are getting way too big.
  This implicitly assumes, that object properties of 'exotic' types such as
  deque<long double> etc. are not really used. If you need properties stay to the obvious:
  string, int float, double and vectors, maps and sets thereof!
- DDG4 needed to public the DDG4Python include directory (Now in DDG4/Python).
  Reason: try to create ROOT 6 dictionaries, which can be relocated for release purposes.
- ROOT 6 dictionary generation: Create dictionaries with the additional flag:
  "-inlineInputHeader" which physically includes the header files used to generate the 
  dictionaries --> relocatability.

* 2016-03-21 M.Frank
- Add additional vertex argument to EventReader::readParticles, which supposedly should

* 2016-03-03 M.Frank
- Improve/complete job options type palette. 
- Allow modify the Geant4IsotropGenerator to shoot particle distributions according
  to several distributions:
  flat in : uniform, cos(theta), eta/pseudorapidity, ffbar (1+cos^2 theta)
  Option: generator.Distribution = 'cos(theta)'
  Default is uniform.

# v00-15     
* 2016-02-15 A.Sailer
-  LCIOStdHepReader.cpp
   - fixed memory leak, compiled and valgrinded

* 2016-02-10 F.Gaede
- added utility  dd4hep::xml::setDetectorTypeFlag()
  to set the TypeFlag from xml element <type_flag type="0x42"/>
- select detectors with 
  dd4hep::DetectorSelector(description).detectors(  ( dd4hep::DetType::TRACKER | dd4hep::DetType::ENDCAP )) ;
- used in UtilityApps/dumpdetector.cc
- added detector_types.xml with int constants defined in DetType.h, include with :
  `<include ref="${DD4hepINSTALL}/DDDetectors/compact/detector_types.xml"/>`
  - add elements to xml, e.g.
    <type_flags type="DetType_TRACKER +  DetType_ENDCAP  + DetType_PIXEL + DetType_STRIP "/>

* 2016-02-10 M.Frank
- DDCond:
    First implementation of conditions access. Required some movements of files
    from DDG4 (Properties) and some changes in the core conditions implementation.
    The basic infrastructure is now present. Don't know yet however how buggy it is...

* 2016-02-05 F.Gaede
-  added DetElement::typeFlag() and DetElement::setTypeFlag(unsigned int types)
- added utility class DetType for setting detector type properties in type flag

* 2016-02-03 N.Nikiforou
- DDDetectors
  - Added plugin dd4hep_GenericSurfaceInstallerPlugin, copied from lcgeo
    to allow installation of surfaces to any sliced detector
  cmake/Doxyfile.in
  - Added DDDetectors to the sources directory so Doxygen picks up the
    plugin documentation

* 2016-02-02 F.Gaede
- set momentum at endpoint (if lcio version > 2.6 ) for MCParticles in Geant4Output2LCIO.cpp 
- fixed printlevel in Geant4Action::print()
- removed random lines drawn in teveDisplay
- dont't print DetectorData by default in dumpdetector

* 2016-02-02 A.Sailer
- DDG4
  - Implemented SimulatorFlags (DDFORHEP-17)
    * In ParticleHandler: vertexIsNotEndpointOfParent, Stopped, leftDetector
    * Steerable distance for vertexIsNotEndpointOfParent
    * TCUserParticleHanlder Simulator flags: DecayedInTracker, DecayedInCalorimeter

* 2016-01-21 A.Sailer/M.Frank
- DDG4
  - Improved user defined physics constructors
    * Allows user defined global rangecut
    * Allows extending particle table (e.g. B-Hadrons unknown to geant4)

* 2016-01-18 F.Gaede
- fixed default c'tors in DDRec/DetectorData.h  to zero initialize sub-structures (LayerLayout etc)

* 2016-01-13 F.Gaede
- allow the default sensitive action to be a tuple with parameter dictionary in DDG4.py/ddsim.steer e.g.:
  SIM.action.tracker = ("Geant4TrackerWeightedAction", {"HitPositionCombination" : 1 , "CollectSingleDeposits" :  False } )
- fixed endpoint in MC-truth particles
 
* 2016-01-11 F.Gaede
- fix hit momentum in Geant4TrackerAction (Geant4SDActions.cpp)

* 2016-01-11 A.Sailer
- DDG4
  - Add LargestAcceptableStep to field setup

* 2016-01-07 A.Sailer
- DDG4
  - Fix the logic for skipping of events at the start of input files: LCIO, HepMC, stdhep and HepEvt
  - Event number in lcio outputfile takes skipped events into account now

* 2016-01-06 F.Gaede
- bug fix in DDSurfaces::ISurface for surface properties (orthogonal/parallel to z)
  (affected ILD VXD)

* 2016-01-05 F.Gaede
- added print functions for rec::DetectorData objects
- used in dumpdetector:
  dumpdetector compact.xml -d 


* 2015-12-09 N.Nikiforou
- UtilityApps/teveDisplay
  - Modified teveDisplay to have three separate collections of surface vectors (u,v,n) 
    instead of one collection. Now the vectors are colored differently.
  - Changed signature of getSurfaceVectors() to allow selection of o,u,v,n vectors to 
    add to the collection, added option for name and color of collection.
  - Renamed surface list collections to "HelperSurfaces" and "SensitiveSurfaces".
  - Changed signature of getSurfaces() to allow setting of name and color of collection.
  

* 2015-12-01 M.Frank
- DDG4
  - Remove some of the compilation warnings
  - Remove energy cut in sensitive detectors
    If you though need, use a Geant4Filter to mask the action processing
  examples/CLICSiD
  - Add a few tests for materialscan and simulation setups using XML or CINT.
  examples/ClientTests
  - Add a few tests for materialscan and simulation

* 2015-11-24 M.Frank
- DDG4
  - Implementation of multi-threading mode.
    To use and understand, please consult the DDG4 manual from the doc area.
  - "Old" Single threaded mode and functionality is preserved.
  - Numerous new classes supporting thread and master specific setup
    functionality.
  - Support for python configuration (see manual for details)
  - Support for global Geant4Actions executing in thread reentrant shared mode.
    (see manual for details)
  - DDG4 examples: DDG4/examples/CLICSidSimu.py deleted and replaced with:
    1) DDG4/examples/SiDSim.py  (single threaded, old example)
    2) DDG4/examples/SiDSim_MT.py (multi threaded version of SiDSim.py)

 -DDCore:
  - Support user formats for the default printout statements for nicer printouts.
  - Removal of compiler warnings

* 2015-11-23 F.Gaede
- added glbal method dd4hep::versionString() 

* 2015-11-13 S.Lu
-  Added a new AHcal Barrel segementation: TiledLayerGridXY, to be used e.g.  
   for the ILD AHcal Barrel Sensitive layer segmentation design.

* 2015-10-30 F.Gaede
- fixed LCIOFileReader to use sequential reading mode
  as using direct access always required runNum==0 and sequential evt nums ...
   

* 2015-10-15 F.Gaede
- DDSurfaces::ISurface/Surface
  - added implemation of conical surfaces 
    - ICone, VolConeImpl, VolCone, ConeSurface

* 2015-10-13 M.Frank
- DDG4
  - Remove explicit constructors for modular physics lists.
    Use the native Geant4 provided G4PhysListFactory instead.
    The physics list is instantiated as before with the "Extends" property.
  - Implement angular ranges in the Geant4IsotropeGenerator for phi [0,2pi] and theta[0,pi]
  - New sensitive detector Geant4TrackerWeightedAction
    Attempt to properly process combined deposits in tracking detectors.
  - Debug component Geant4SurfaceTest similar to the standalone program
    test_surfaces, but to be appended as a DDG4 event action for event by event
    tests of hits.

* 2015-10-09 M.Frank
- DDG4
  - Extend the functionality of the DDG4 plugins
    Geant4InteractionVertexBoost and Geant4InteractionVertexSmear.
    If the Mask property is set to -1, all interactions present
    at call time are smeared/boosted according to the parameters.
    Note, that the position of the plugins within the generation 
    sequence is important.
  - Add a new generator Geant4GeneratorWrapper to allow the use
    of native G4 generator classes such as the G4ParticleGun or
    the G4GeneralParticleSource. Still to be tested.
  - Extend the Geant4Random functionality: May now be instantiated
    like any other Geant4Action. Users may set the seed or the 
    luxury level.
    All calls to HepRandom (CLHEP) or gRandom (ROOT) will re-use 
    this instance. See DDG4/examples/CLICRandom.py for illustration.
  - Updated DDG4 manual.

* 2015-09-30 F.Gaede
- updated createGearForILD.cpp to use new parameters in LayeredCalorimeterData
  ( innerThickness, innerRadLength, etc)


* 2015-09-15 F.Gaede
- refactoring of Surface classes:
    - made VolSurface a reference counting handle to 
      a polymorphic pointer of type VolSurfaceBase
      - this should facilitate the creation
        of dedicated surface classes for special cases
        such as inside the beampipe or the face of the calorimeter

* 2015-08-27 F.Gaede
- added macros DD4HEP_VERSION_GE(MAJV,MINV) and DD4HEP_VERSION_GT(MAJV,MINV)
  to Detector.h

- increased version number to v00-15


* 2015-08-20 M.Frank
- DDCore: Modularize/abstract plugin manager 
- VolumeManager: Fix bug propagating sensitive detectors
  Showed up in the examples of nested detectors


* 2015-08-12 N.Nikiforou
- DDCore/XML: Added new helper functions to Layering engine:
   - double absorberThicknessInLayer(xml::Element e) : 
     returns total absorber thickness in given layer
   - void sensitivePositionsInLayer(xml::Element e, std::vector<double>& sens_pos) :
     provides positions of sensitive slices within a layer with respect to the 
     center of the layer


* 2015-07-25 M.Frank
- DDSegmentation: Remove several 'shadow' warnings.

- New build system for the dd4hep core, which greatly simplyfies the cmake files
  for the various sub-packages. 
  Tested with Andre's build script doc/CompileAllOptionPermutations.sh.
  (GEAR usage not tested though)

# v00-14     
* F.Gaede
- fixed writing of generator status for lcio::MCParticle
  in Geant4Output2LCIO.cpp

* A.Sailer 
- Add missing scope to /DD4hep/Factories.h, to not force everyone to use using namespace

* F.Gaede: 2015-07-15
- changed type in SurfaceList and SurfaceMap from Surface to ISurface
- added methods length_along_u/v() to ISurface
- moved setting of CMAKE_CXX_FLAGS (if DD4HEP_USE_CXX11) from DD4hep.cmake to CMakeLists.txt

* 2015-07-11 M.Frank
-  added starter docs dd4hepStartersGuide.pdf

*** **Important** ***
  **Before updating, backup your existing and working checkout. Though I was running 
  the basic tests, there may be unchecked corners of the software.**

Notes:
- Backwards compatibility mode for the usage of ROOT 5 and ROOT 6 alternatively.
- For ROOT 6 the Gaudi PluginService is used and added to the distribution.
- dd4hep is distributed with a licence. See $DD4hepINSTALL/LICENSE for details.
- In the doc area the $DD4hepINSTALL/doc/CREDITS file everybode should add her/his 
  name, if contributed significantly.
- Unfortunately this meant to add/change the headers of all files and give a 
  pointer to the licence file. If I accidentally changed the author name, please
  chagne it back.
- The plugin factory declaration statements were changed to accomodate both
  ROOT 5 and ROOT 6 and to keep the number of 'ifdef' statements at a minimum.
- TODO: Properly change the cmake scripts to accomodate for ROOT 6 and the 
  automatic switch when building dd4hep. 
- For reference reasons: this commits is revision 1812 + 1813 (DDDetectors)

# v00-13  
* 2015-07-03 F.Gaede
- updated doxygen for detector rec::DetectorData structs and usage in convertToGear

* 2015/07/02 Nikiforos Nikiforou
- Added isRadiator() helper function in DDCore/XML/XMLDetector.h/cpp 
  Mirroring functionality of isSensitive() and used in geometry drivers
  e.g. to calculate total absorber thickness
  
- Extended LayeredCalorimeterStruct in DDRec/DetectorData.h to include 
  additional parameters required by Pandora:
  - inner_phi0 (e.g. alignment for inner symmetry)
  - outer_phi0 (e.g  aligment for outer symmetry)
  - NOTE: phi0 still remains but is deprecated
  - gap0 (e.g stave gap along r-phi)  
  - gap1 (e.g middle stave gap along z)
  - gap2 (reserved for future use)

*  2015-06-29 A.Sailer
 - always store lcio collections, even if they are empty in Geant4Output2LCIO (DDG4/ddsim)


* 2015-06-29 F.Gaede
- changed env scripts to prepend to library pathes (dd4hep and dependant packages)
   - using a newer lcgeo version than the one provided in ilcsoft can simply
        be done w/ source ./bin/thislcgeo.sh 
	      
# v00-12  
* 2015/06/17 Markus Frank
- Add possibility to block access to constants by name from Detector.
  Functionality enabled by a constant named "Detector_InhibitConstants" with value "1", "True", "Yes".
  Test: geoDisplay -compact file:../DD4hep.trunk/examples/ClientTests/compact/InhibitConstants.xml 

- Add DetectorHelper handle object to easily access the sensitive detector object of a detector
  using either the subdetector name or the detector element (or one of its children).
  See: dd4hep/DetectorHelper.h
  Test: ``` 
  geoPluginRun -input file:../DD4hep.trunk/examples/CLICSiD/compact/compact.xml \
                     -plugin CLICSiD_DetectorHelperTest  \
         optional:  -<detector-name (default:SiVertexEndcap)>  [Note the '-'!!!] 
         ```

- Check when adding new sub-detector detector elements if a given one already exists.
  if yes: exception.
  Test: `geoDisplay -compact file:../DD4hep.trunk/examples/ClientTests/compact/DuplicateSysID.xml`


* A. Sailer
- DDSegmentation: Added function cellDimension which returns the dimension of any given cell

* S.Lu:
- Added a switch for BirksLaw to Geant4StepHandler. 
  For backward compatable, by default, do not apply BirksLaw, and apply BirksLaw with call 'doApplyBirksLaw()'.
  added optional Geant4ScintillatorCalorimeterAction
- Added one 'Geant4ScintillatorCalorimeterAction' into DDG4 plugins Geant4SDActions for  scintillator to take 
  Geant4 BirksLaw effect as sensitive detector active layer.

* F.Gaede
- added SurfaceManager class that holds maps of surfaces for 
  "world","tracker","calorimeter","passive" and all individual subdetectors
    - to be used in reconstruction code where a lookup from cellID
      to corresponding surface is needed
    - needs plugin "InstallSurfaceManager" to be in the compact file
    - access via:  
      SurfaceManager surfMan = *description.extension< SurfaceManager >() ;
      const SurfaceMap& surfMap = *surfMan.map( "world" ) ;
- renamed old SurfaceManager to SurfaceHelper

- added method: Volume createPlacedEnvelope()
  - to be used in all LC detector drivers to create a placed envelope volume
    rest of the detector is then instantiate in this volume

- added BooleanShape plugin for creating generic boolean
  shapes (subtraction, union or intersection )
   - to be used for envelope definition in xml

   - added new build_type: BUILD_ENVELOPE 
      - can be used to visulize envelopes only (geoDisplay, teveDisplay,...)

* 2015/05/22 Nikiforos Nikiforou
- Fixed bug in DDG4/lcio/LCIOConversions.cpp, which stored only the PDG of the primary particle 
  in multiple particle contributions in SimCalorimeterHits instead of both the primary and secondary PDGs.


* 2015/05/13 Markus Frank
- Fix bug in DDG4, which forced DDG4 to exit if a property from the Geant4 prompt could
  not be set. No an error message is printed and the prompt is returned to the user.
  (Bug submitted by A. Sailer)

* 2015/05/11 Andre Sailer
- CMake updates:
  - Split libraries into components: use find_package(dd4hep COMPONENTS <component> [...] )
    to find the components you need.
  - At the moment there are these components are sensible to link against: DDRec, DDG4, DDEve, DDSegmentation
  - To link against the librarie use either dd4hep_COMPONENT_LIBRARIES or dd4hep_<COMPONENT>_LIBRARY,
    where <COMPONENT> needs to be replaced by the UPPER case name of the component

* 2015/05/09 Markus Frank
- Allow to access detectors by type from description.
  - The sensitive type of a detector is set in the 'detector constructor'.
  - Not sensitive detector structures have the name 'passive'
  - Compounds (ie. nested detectors) are of type 'compound'
- Dump detector types using plugin:

         geoPluginRun -plugin DD4hepDetectorTypes -input <compact-file>

* 2015/03/12 Markus Frank
- Add support for ellipsoids in gdml/description and geant4 conversion.
- Allow to include files with environment variables from compact notation.

* 2015/03/09 Markus Frank
- Finalize move to geant4 version 10, started by A.Sailer
- Add non-isotrop particle generator in DDG4 (request P.Kostka)
- Isolate ROOT plugin manager as start to move to ROOT 6

# v00-11
* F.Gaede
- DDDetectors
  - new Package containing generic dd4hep detector palette (MF)
  - added 'dimension' argument to dd4hep_SiTrackerBarrelSurfacePlugin (FG)
    to handle pixel and strip detectors 

- DDRec (Frank Gaede)
  - added simple data structs in DDRec/DetectorData.h
    providing interface needed for GEAR:
  - added plugin for creating GEAR file for ILD
  - Extended IDDecoder to give access to layer, system and barrel/endcap information. (CG)
    IDDecoder is now independent of Readout object, instead it finds the correct
    readout based on the cellID / position
  - fixed surface implementations: removed inline virtual functions
  - added methods:
    VolSurface::length_along_u()
    VolSurface::length_along_v()
    Surface::volumeOrigin()
    CylinderSurface::center()
    ICylinder::center()  

- DDG4 (Markus Frank)
  - Fix Geant4 Primary input conversion
  - Fix LCCollectionVec for LCIO output
  - fixed units for MCParticle output (FG)
  - fixed generator status: set to 0 if (FG)
    particle created in simulation
  - Add cylindrical tracking volume to particle handler
  - More modular user particle handling with example Geant4TCUserParticleHandler
  - fixed charge (don't devide by three) (FG)
  - fixed logic for writing out MCParticles: (FG)
    - don't write particles created in calo
      unless scattered back into tracker)
    - don't write particles created in tracker
      if below MinimalKineticEnergy ( Geant4ParticleHandler)
  - Added HepMC ascii reader
  - added simply main program 'dd_sim' to run  (FG)
    a DDG4 based simulation with xml configuration files
  - change TrackerCombine SD to use energy weighted positions
  - added utility to dump generated files: scripts/dumpDDG4.C
  - fixed dictionaries for map iteration on MacOS in python (FG)
    -> added operators ==/!= for map<>::iterator
  - added component to allow the setup of the magnetic field tracking in Geant4 from python

- DDCore (Markus Frank)
  - fixed Detector el-mag. field converter 
  - add generic surface installers 
  - allow for string constants in description define section
  - added arguments for surface plugins

- DDSegmentation (FG) :
  - fixed compiler warning w/ left shift count >= width of type 
  - fixed wrong type string "CartesianGridXY"
  - added CartesianGridYZ
  - Implementation of a Polar R-phi segmentation  (MP)
  
 - examples 
  - add new test MiniTel(escope) to ClientTests (MF)
  - moved old obsolete examples to attic
  - add new example: LHeD_tracker (MF) 
  - add new example: SimpleDetector (FG)
    (ILD like VXD and SIT )
    
- CMake:
  - made DD4HEP_USE_BOOST an option flag (FG)
  - added dd4hepG4 library dependence to DDEve (MF)
  - disable dot -> have simpler (and faster) inheritance graphs w/ Doxygen
  - fixed missing search field in Doxygen doc (FG)
  - include boost automatically if build with Geant4

 
- Documentation
  - improved Doxygen documentation (MF)
  - added manual for DDRec (FG)
  - new version of the dd4hep manual (MF)
  - fixed code formating with emacs style:
    ./doc/format_code.sh

- removed subpackages ( moved to https://svnsrv.desy.de/public/aidasoft/DD4hep/attic ):
      DDExamples_obs/
      DDSense/
      examples/ILDExDet/
      examples/ILDExSimu/
      examples/firstExample/
      examples/noviceN04/

- enabled testing in nightly builds with cmake, ctest and cdash (FG)

- plus many small bug fixes  (use svn log for details )

* 2014/09/26 Markus Frank
- Quite some effort to improve the doxygen documentation.
  The current revision is can be found at:
  https://cern.ch/frankm/DD4hep/html/index.html

* 2014/02/06 Markus Frank
- DDDetectors: Add example surface plugin with arguments.
    - example code: DDDetectors/src/SurfaceExamplePlugin.cpp
    - compact:
```   
       <plugin name="DD4hep_SurfaceExamplePlugin">
          <argument value="SiTrackerBarrel"/>
          <argument value="aa=1*mm"/>
          <argument value="bb=2*cm"/>
          <argument value="cc=3*m"/>
       </plugin>
```

    - output:
    
```cpp
    SurfaceExamplePlugin: argument[0] = SiTrackerBarrel value = -1
    SurfaceExamplePlugin: argument[1] = aa=1*mm value = 0.1
    SurfaceExamplePlugin: argument[2] = bb=2*cm value = 2
    SurfaceExamplePlugin: argument[3] = cc=3*m value = 300
```

* 2014/08/15 Markus Frank
- DDG4: Impreoved LCIO handling for DDG4
- DDG4: Input handling:
  - LCIO input file reading with multiple *independent* inputs
    to support overlay, multiple interactions, etc.
  - Primary verex smearing independent for each input
  - Primary vertex boosts independent for each input
- DDG4: Output handling
  - MC Particle handling
  - Improved MC truth handling for produced Hits.
    Still needs revisiting.

# v00-10      
*  Frank Gaede
- fixed version numbers and release notes for release v00-08 and v00-09
  ( both have the wrong version number for the libraries 0.7 !)
- added ./doc/prepare_a_release.howto
- added new method to BitField64: setValue(unsigned lowWord, unsigned highWord )
 - added some simple unit tests for BitField64


* 2014/08/15 Markus Frank
- DDG4 Add possibility for user hook to the particle handler +
  Data member to store additionbal particle data for the
  output stage.
- Allow to display MC particles in DDEve
  Add small help file describing how to start DDEve.
  doc/DDEveManual.pdf
- Update CLICSiD Example.

# v00-09      
* 2014/08/07 Markus Frank
- DDG4: First version to support MC truth in DDG4 including 
  particle filtering to optimize the size of the MC record.
- DDG4 fix SimpleCalorimter sensitive action and properly support
  hit aggregations.
- DDEve smaller modifications to support DDG4IO if dd4hep was 
  built with the Geant4 option ON.

# v00-08
- Some minor fixes:
  - made compatible with older geant4 versions (9.5)
  - add Bitflag to store CellID1 in SimCalorimeterHit collections
  - fix position conversion from Geant4 to ROOT
  - add cellID determination to SensitiveAction
  - ...


* Andre Sailer, 2014-07-17
- Unify cmake option variables, small cmake corrections
- Change options `_WITH_` to `_USE_`
- Print Warning that variables with `_WITH_` are deprecated
- Change Defintions to `_USE_` as well
- Add REQUIRED to find_package geant4 and xercesc if they are turned on
- Updated documentation
- Updated ILDExDet example

* Christian.Grefe, 2014-07-15
- Made DDSegmentation optionally a stand-alone package
  create DDSgementationConfig.cmake when build as part of dd4hep

* Markus Frank, 2014-07-02
- add LCIO conversions from DDSim - sensitive detectors 
  can now simply instantiate LCIO Sim hits
  
* 2014/06/30 Markus Frank
- DDG4: Support user (context-) extensions for Geant4.
  User contexts are availible for events and runs. The contexts
  are accessible from the Geant4Context meber of each Geant4Action,
  the base class for all processing entities.
   - Adding a context:
```
      context()->event().addExtension(pointer_to_object, typeid(TYPE), 0);
      or 
      context()->event().addExtension<TYPE>(pointer_to_object);
      In the first case the object is NOT DELETED at the end of the event.
      In the second case the object is deleted.
```      
    - Accessing the context:
```    
          TYPE* ctx = context()->event().extension<TYPE>(false);
      The boolean argument indicates if an exception should be thrown
      if the extension is not present ie. here no exception.
```      
- DDCore: adjust a few print statements.

* 2014/06/27 Markus Frank
- Separate XML loading from Detector implementation.
- New package: DDEve: a bit more sophisticated TEve specialization
- To start use examples/CLICSiD/compact/DDEve.xml
- DDEve can interface currently to ROOT files created by DDG4.
- An example data file is provided as well in:
- examples/CLICSiD/compact/CLICSiD_events.root
- Documentation is lacking a bit....
- start the display with: `$> root.exe ${DD4hepINSTALL}/examples/DDEve/DDEve.C`


* 2014/06/19 Frank Gaede
- replaced TGeoUnits.h by  DD4hepUnits.h
- renamed namespace tgeo to dd4hep -> users need to replace tgeo::mm by dd4hep::mm


* 2014/06/12 Frank Gaede
- changed default unit for angles to be radians   (tgeo::rad = 1.)
- enforce usage of units from TGeoUnits.h in xml expression evaluators
- convert to degrees for angles in TGeoShapes constructors
- NB: there is one inconsistency left here:  angles returned
  from TGeoShapes are already in degrees, this is the one
  case where a quantity returned from dd4hep/TGeo does not have the default
  units - thus one should not write
  
 ` double phi = coneSeg.Phi1() / tgeo::rad ;`
 
  but rather
  
  `double phi = coneSeg.Phi1() * tgeo::deg ;`


* 2014/06/03 Markus Frank
- Provision for ROOT persistency for dd4hep detector descriptions:
  Create Cint dictionary for dd4hepCore by default when building the library.
  For the time being the area of DDSegmentation is left out, since these
  objects require changes due to the handling of references and template
  specializations. Besides these, dd4hep detector descriptions can be
  saved and read-back directly to/from ROOT files - which may improve
  a lot the startup time of processes.

- The DDG4 dictionary is created by default when building the library.
  When importing DDG4 from python only the dd4hepCore dd4hepG4 libraries
  must be present. It should no longer be necessary to compile the
  necessary AClick on the fly.

- Investigations on wirframe drawing:
  Wireframes are a property of the scene graph. You can toggle to wireframe
  using the 'w' key on the keyboard. Other representations are 
  'r': Solid view
  't': Solid + wireframe view.
  This information is accessible from the help menu of the ROOT OGL widget.

# v00-07
* 2014/05/28 Markus Frank

- Several fixes and improvements in DDG4
  - Cache output levels in Geant4Kernel object
  - Add shell handler to measure energy escape e.g. in calorimeters
- Fix bug in Polycone shapes concerning angles.
      **IMPORTANT: If you forked the repository examples/CLICSiD
      your PolyconeSupport does not work anymore, because the angles
      were given in degrees. They should be given in radians!**

* 2014/05/21 Markus Frank
- Fix material creation from XML
    JIRA bug: dd4hep -  DDFORHEP-4
    https://sft.its.cern.ch/jira/browse/DDFORHEP-4
    using <composite> in material xml files results in wrong material properties

- Fix JIRA bug: dd4hep -   DDFORHEP-3
    https://sft.its.cern.ch/jira/browse/DDFORHEP-3
   Recursive assemblies result in error in TGeo geometry
    Bug results in errors when closing the geometry like:
    "Error in <TGeoVoxelFinder::SortAll>: Wrong bounding box for volume SIT_assembly"

- Provision to store survey constants to the DetElement as an extension
    for Alignment calculations (priv. comm. C.Parkes)

- Fix bug in expression evaluation of angles.
    All detector constructors receive angles in "rad". No correction 
    from degree to rad should be necessary.

* 2014/05/21 Frank Gaede
- add MaterialManager class providing
  - access to materials at any point or on straight
    line between two points
  - creation of material with averaged properties (A,Z,rho,x0,Lambda)
 
- added utility print_materials to print material properties along  
  a straight line between two points including integrated radiation and 
  interaction lengths (useful for debugging geometries and materials)
 
- use avaeraged material for Surfaces where the thickness extends
  beyond the volume boundaries
   - introduced new simple data class MaterialData for this       
    

- known issues
   - materials don't work for detectors with assemblies in assemblies
        as the TGeo navigation dose not seem to work:
	Error in <TGeoVoxelFinder::SortAll>: Wrong bounding box for volume SIT_assembly
        -> ROOT bug or feature ?

    - using <composite/> in compound materials results in incorrect material properties
       see ILDExDet/compact/materials Polysterene as example


* 2014/05/06 Frank Gaede
- DDSurfaces/DDRec:
  - added ICylinder and CylinderSurface to provide
    access to cylinder radius for the tracking
- teveDisplay: 
  - draw sensitive surfaces in red and helper surfaces in gray
  - protect against missing lcio input file
  - reduced printout from VolumeManager  (changed INFO to DEBUG)

* 2014/04/05-2014/04/29 Frank Gaede
- Implementation of Surface classes for track reconstruction
  - abstract interfaces in DDSurface:  ISurface and IMaterial  
  - implementation in DDRec:  Surface, SurfaceManager, DetectorSurfaces
  - basic functionality exists:
  - meassurement surface vectors u,v, normal and origin
  - check if point is inside the bounds of the corresponding volume
  - currently the material is taken to be the one from the volume 
      -> to do: average material if surface thicknesses extend beyond the
                volume for comound materials
  - added drawing of surfaces and (Sim)TrackerHits and (Sim)CalorimeterHits to teveDisplay
     ( link lcio file to teve_infile.slcio )
  - added some surfaces to ILDExDet: VXD, SIT, TPC
  - added test_surfaces to ILDExSimu
     (fails currently for some hits that are not created on the surface ...)
  - added some assemblies to ILDExDet to better structure the geometry hierarchy   

* 2014/04/28  Markus Frank
- First attempt to understand fully the handling of recursive assemblies
     from the 2014/04/25 was apparently with limited success. Now they should
     work!

* 2014/04/25  Markus Frank
- Moved boost::spirit parsers to DDCore, since these parsers are also used
     in other packages such as DDCond and DDAlign - whenever strings have to be
     converted to objects.
     Consequently, the boost spirit code fragments were removed from DDG4.
- Improved and more correct type checking when assigning handles
- Explicit implementation of the object extension mechanism to be used 
     through inheritance or aggregation rather than having seperate and nearly
     identical implementations for various objects.
- Improved defintition of basic objects describing Conditions and Alignment.
- Improved implementation of DDAlign, the alignment support.
     Not to be used currently - experimental package.
- Initial implementation of DDCond the package supporting the handling of 
     conditions.
     Not to be used currently - experimental package.
- Geant4Converter: I convinced myself, that the bug in handling assemblies
     of assemblies is resolved. To be tested though.

* 2014/02/07  Frank Gaede
- added DDGear, support for interfacing to gear for backward compatibility
  - lives currently in ILDExDet ( should become (optional) part of core dd4hep)
  - users need to GearParameter objects as extensions to the DetElement
       -> see ILDExTPC_geo.cpp, ILDExVXD_geo.cpp, ILDExTPCSIT_geo.cpp as examples
  - program convertToGear creates gear xml file from compact file 
       ( only if  GearParameter objects are avialble as extensions)
  - added test_convert_ild_gear 
- fixed  geometry in ILDExVXD_geo.cpp, ILDExTPCSIT_geo.cpp
     wrt. to position of support ans sensitive ladders

# v00-06   
* 2014/02/04  Frank Gaede
- fixed environment scripts thisdd4hep.sh and this${PackageName}.sh
    to also export library pathes for external libraries (CLHEP, LCIO, gear)

- added some simple tests to ILDExDet, ILDExSimu and noviceN04
  - provide cmake script run_test_package.sh ( configured as run_test_${PackageName} ) 
      to call test binaries after initializing the environment
      (needed for automated/nightly tests)

- updated Readme.txt wrt build procedure and running the tests


* 2014/01/17    Markus Frank
- Prepare implementation of Alignment using Release and Grap features of TGeoVolume
    and TGeoNode. In principle this was done in a backwards compatible way, but 
    I cannot really check 100% the #ifdef's.
- Fix handling of XML collections using XercesC.
    (Bug reported by P.Kostka).
- Fix several compiler warnings issued by Coverity.

* 2013/12/17    Markus Frank
- Whole load of fixes to keep coverity quiet....
- Fix bug in DDG4 field interface component
- Allow the usage of units, positions and lorentzvectors and in DDG4 properties.
    Example using a position property (ROOT::Math::XYZVector):
    Idle> /ddg4/Gun/position (1*mm,1*cm,0.01*m) 
    Geant4UIMessenger: +++ Gun> Setting property value position = (1*mm,1*cm,0.01*m)  
                       native:( 1 , 10 , 10 ).

- Support for std::vector<T>/std::list<T>/std::set<T> properties.

  - Support for include directives of the format
```
        <include ref="file-name"/>
    for the top level <lccdd> tags
       Parent tag    Included root tag  Data content
    -- <lccdd>       <define>           See below.
                     <detectors>
		     <detector>
		     <display>
		     <lccdd>            Nested includes (with DTD validation for Xerces-C)
    -- <define>      <define>           Set of constants
    -- <detectors>   <detector>         Single XML subdetector description
    -- <detectors>   <detectors>        Multiple XML subdetector descriptions
    -- <display>     <display>          Set ov visualization attributes

    DTD validation is only performed using Xerces-C for root tags <lccdd>.
```
- Note: 
    `<lccdd>` include directives are executed BEFORE any other directive!
    Other include directives are executed BEFORE the corresponding named directive!

* 2013/11/14    Markus Frank
- Reformat files in DDCode and DDG4 to remove the <TAB>s from the source files.
    Hopefully the content is now more readable.
- DDCore: Remove empty and named constructors for shapes. Empty constructors
    were simply a dangerous hook to memory leaks, named constructors are 
    deprecated since along time.
- DDG4: add support for messengers to access and modify properties from
    the Geant4 prompt. For the test hit classes and the simple sensitive 
    detector a ROOT I/O plugin was provided.
- DDG4: Split into several libraries:
  - libDDG4.so pure    link library without any plugins
  - libDDG4Legacy.so   'Old' plugins. The old stuff at some point
                               will have to disappear.
  - libDDG4Plugins.so  New framework plugins
  - libDDG4LCIO.so     LCIO output plugin for new framework

* 2013/11/03    Markus Frank
- doc: Add CHEP2013 paper about dd4hep as a start of the documentation 
    section. More to come hopefully.
- DDCore: several small improvements:
  - Segmentations are no longer Ref_t's.
  - Base internal implementing object directly on the segmentations 
      classes from Christian.
  - Rearrangement of some code from Handle.h to Primitives.h
  - Allow to attach extensions to Detector
- DDG4: Fix Frank's simulation problem, which he caused himself
    introducing his famous factories....
- DDG4: First attempt to fix simulation problem with mignetic field.
    Ready for testing.
- DDG4: First commit of a independent simulation aimed for LHCb and LHeC
    + other newcomers. Documentation needs to be written.
    Features:
  - Modular setup of all Geant4 action routines as plugins, including the
      Run-, event-, tracking-, stacking-, stepping- and generator-actions 
      as well as the physics lists with particle and physics constructors.
  - Setup is possible in XML or with C++ either as an AClick in root or
      standalone. Python based starter to come.
  - Missing is a modular description of the G4 UI and VIS manager(s).
  - Documentation is to come. Before documentation is not present
      the usage is discouraged.
    Note:
  - This version of DDG4 uses boost::spirit. It will compile without
      the BOOST include files and the old functionality is preserved, 
      but it will not work.
      To use BOOST add to the cmake statement the following tags:
      cmake <regular tags> \
      -DDD4HEP_USE_BOOST=ON \
      -DBOOST_INCLUDE_DIR=${BOOST_INCLUDE_DIR}  ....
      Where BOOST_INCLUDE_DIR should point to the boost header directory.
      BOOST libraries are not used.

* 2013/10/18    Markus Frank
- Throw exception if the expression evaluator sees a problem.
    The message printed before was often not seen.
- Improve printout on exceptions and plugin abortion
- Remove the typedef RotationZYX -> Rotation and
    import the type RotationZYX from ROOT::Math.

- Add more explicit error messages in case a plugin is not found.
- Add file TGeoUnits.h, which allows to use the "usual SystemOfUnits"
    from CLHEP using TGeo with the namespace tgeo::xxx
```
    Example: tgeo::mm -> 0.1 (Geant3: cm, sec,  GeV, degree)
                 ::mm -> 1.0 (Geant4: mm, nsec, MeV, radian)
    I hope I got all translations right.
    Geant4                                   Geant3
    ----------------------------------------------------------------
    millimeter         mm=1,cm=10            centimeter  mm=0.1,cm=1
    nanosecond         s=1e9                 second      ns=1e-9,s=1
    Mega electron Volt GeV=1e3               GeV         GeV=1
    radian             rad=1                 degree      rad=180/pi

    Unclear to me was if in Geant3 steradian is also 2*2*pi or 2*2*180 degree.
```


* F. Gaede
- changes to build system:
         (see ReadMe.txt for details)

- moved examples to ./example directory   
- they are not built as part of dd4hep anymore

- install thisdd4hep.sh in ./bin
           (modified to have the correct pathes) 

- added -DINSTALL_DOC=on/off option
           to build doxygen documentation (in ./doc/html/index.html)

- create DD4hepConfig.cmake for easy building against dd4hep
           ( see examples CMakeLists.txt)

- fixed doxygen API documentation (C.Rosemann)
          (greatly improved with many graphs)

# v00-04
* F. Gaede
- placement options are now compatible with geant4 
- fixed Geant4SensitiveDetector::getVolumeID()
- modified ILDEx model to not use assemblies (for now !?)
- removed obsolete geant4 example ILDEx
- Fix population procedure of volume manager
- Fix uniqueness of volume identifiers.
- Remove call with lookups by placed volume to the volume manager
- improved ILDEx toy example:	   		
  - fixed creation of cellIDS for simhits
  - creates SimTrackerHits for VXD, SIT and TPC
    - creates SimCalorimeterHits for AHcal barrele and endcap
    - works now wigth ILDExSimu _and_ SLIC if no assemblies are used
    - assemblies work with the dd4hep Geant4Converter and VolumeManager
  - added a prototype example for a ROOT independent 
              plugin mechanism for SensitiveDetectors
  - added example implemetation for Calice test beam
          calorimeters: CaliceTbeam  (Shaojun Lu, DESY)


* 2013/21/06    Markus Frank
- Since the placement model for combined translations and rotations
      caused more confusion than good, it was agreed to move to a more
      Geant4 like placement model using Vectors, Rotations and Transformations
      from ROOT::Math. To place a physical volume only one of these
      is allowed as an argument: either a rotation or a translation
      for simple placements. Combinations may be constructed with Transform3D
      objects allowing more complicated placements.
      The same mechanism was applied for the operations to construct
      boolean solids.
- The CLICSid Examples were updated according to the changes necessary
      from 1)
- The Tesla drivers from the ILDExDet example were removed. They are
      not compliant to 1).
- The G4 package allows now to translate TGeo geometries to Geant4.
      Visual inspection has shown an agreement between the two geometries.
- The TGeo to Detector/GDML conversion is still buggy. Hence, simulations
      using slic as an engine do not yet work. This is being looked at.

   IMPORTANT NOTICE:
   The changes described in above may affect existing detector constructors.
   Unless all problems are solved you should be careful to move to the svn
   head. In any case save your checkout or use the tag v00-03.

 

# v00-03
- broken geometry (for some rotations/translations)
- 'backup release' before placement options 
   will be reduced

# v00-02
* 2013/02/06    Markus Frank
- Simplyfy inheritance for common objects from TNamed.
     Use direct inheritance of Object from TNamed rather than hidden 
     using the Value<a,b> construct.
- Added physical volume manager to simplyfy the detector element 
     and sensitive detector lookup from a given physical volume.
     This implies: If the volume manager is instantiated, the readout specifiers
     MUST be correct. This is in most xml files NOT the case.
     The "system" field holding the sibdetector-id is MANDATORY!
     Otherwise you will receive messages like this:

     Exception:SITCollection: This ID descriptor has no field with the name:CellID0
               while programming VolumeManager. Are your volIDs correct?

- To check the volume identification, add argument -volmgr to geoConverter,
     geoDisplay, etc.
- To trace possible memory leaks:
     export DD4HEP_TRACE=Yes
     then run you converter etc. At the end a table is displayed with a "leakage"
     column, showing how many objects were not deleted.
```
     +----------------------------------------------------------------+
     |   I n s t a n c e   c o u n t e r s   b y    T Y P E I N F O   |
     +----------+---------+-------------------------------------------+
     |   Total  | Leaking |      Type identifier                      |
     +----------+---------+-------------------------------------------+
     |        13|        0|dd4hep::DetElement::Object
     |         3|        0|dd4hep::SensitiveDetector::Object
     |         3|        0|dd4hep::Readout::Object
     |         1|        0|dd4hep::OverlayedField::Object
     |         1|        0|dd4hep::CartesianField::Object
     ....
     Ideally the second column only has "0"s. Instances of 1 may be OK (singletons).
```
- Prenotice:
     As soon as ROOT v 6.00 is out, we will have to use it!
     Andrei kindly agreed to implement a few changes to TGeo, which will make 
     life much easier and the implementation cleaner and less cumbersome.


# v00-01
* 2013/20/03    Markus Frank
- Finished the compact->description converter
     Extract description information
```
      $ > geoConverter -compact2description -input file:<compact-input-xml-file> -output <detector>.description
      $ > <SimDist>/scripts/slic.sh -o output.slcio -g SiD.description -m <geant4-macro>.mac -r 100 
```

- Finished the compact->gdml converter
  Full GDML cycle can be tested (deplace <detector> with some meaningful identifier):
  - Extract gdml information
      `$ > geoConverter -compact2gdml -input file:<compact-input-xml-file> -output <detector>.gdml`
  - Extract visualisation hints to csv file
      `$ > geoConverter -compact2vis -input file:<compact-input-xml-file> -output <detector>.vis.csv -ascii`
  - Load GDML into ROOT:
      `$ > root.exe ../doc/gdml_root.C+\(\"<detector>\"\)`
      The macro expects *at least* the <detector>.gdml file. If no visualisation
      information is found, the resulting graphics however is not very nice!
 

* 2013/22/02    Markus Frank
- Today a decision was taken to remove all _X() and _A() macros 
  accessing static unicode strings. A common macro will be used to
  address both XML elements and XML attributes using the macro _U(...).
  The original usage only created confusion.
  All _X and _A macros were replaced by _U.

- Improve the executables running the plugin mechanism in UtitlityApps.
  They now complain if the plugin was not found.


* 2013/21/02    Markus Frank
- Require strict checks for the existence of xml attributes.
  e.g. default=false for Element::attr<bool>() is no longer an option.
  if the attribute is not present in the element, an exception
  is thrown.

- Slightly change the factory method for creating detector elements.
  Use a more generic pattern do support also more generic XML 
  processing.

* 2013/20/02    Markus Frank
- Install rootmap mechanism for the usage of plugins.
  It is no longer necessary to have separate programs
  for the various detector models (ILD/SiD).
  One generic display program servest them all.
  ==> Creation of package DDExamples/UtilityApps
  example: $> bin/displayGeo -compact <compact-xml-file>

- This move has another consequence:
  All libraries are build into a common install area:
  the <build directory>/lib.
  All executable binaries are build into a common install area:
  the <build directory>/bin.

- The LD_LIBRARY_PATH and PATH variables as defined in 
  'thisdd4hep.(c)sh' do include these directories

* 2013/20/02    Markus Frank
- dd4hep release notes. Better start them late than never.
  If you perform significant changes to the dd4hep core,
  plase leave a small notice here.
