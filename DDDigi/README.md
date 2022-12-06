<style>
large { color: blue;font-weight: 700;font-size: 20px }
</style>


![DDDigi](../doc/logo_small.png)


DDDigi: The digitization tools of DD4hep
========================================

DDDigi is supposed to interprete the results of the detector simulation phase
e.g. using the simulation toolket [DDG4](../DDG4) with its command line interface
[ddsim](../DDG4/python/DDSim).
This is the final phase of data processing to obtain data from the simulation 
branch which are as close as possible to the response of the front-end electronics
of a real experimental setup.

The simulation phase ejects for all subdetectors and the individual 
sensitive elements contained the energy deposits created by particles from a 
primary interaction passing the sensitive volume.
These data have to processed for taking into accound imperfections of the 
apparatus such as 

- uncertainties in cable lengths resulting in shifts of time of signal arriving.
- uncertainties in the position of sensitive volumes.
- dead or hot readout channels.
- conversion of the final signal data into the equivalent of ADC counts.
- ...

In addition these effects should be computed in the presence of

- multiple interactions in the same beam-crossing
- spillover signal from previous or following interactions due to the
  relaxation time of the apparatus.

DDDigi addresses these issues in a very modular way and implements a multi-threaded approach
to perform such data processing.
To support multi-threading and to avoid any complication for users due to data races 
the following basic assumptions were made:

- The signals from disjunct subdetectors are generally independent
  This means the detector response of any tracking subdetector is uncorrelated with
  the detector response of e.g. a calorimeter.
- Within a subdetector the response of disjunct units are uncorrelated. This means that 
  e.g. hits on one layer of a layered tracking device are uncorrelated 
  with the hits produced in another layer.
- Within one such a layered device the response of individual sensors is tyically uncorrelated.

These assumptions however are not strict, but rather require input from the designer of such
a digitization application for a given appratus. These basic assumption only show the maximal
level of parallelization possible when processing data signals.
The parallelization can be configured for each subdetector at each level according to 
boundary conditions e.g. given by cross-talk or the data volume arising from the
energy deposits of the simulation.

DDDigi Components Description
=============================

DigiDDG4ROOT
------------

- Reader for ROOT files produced with DDG4. <br/>
  Properties:                                                                                        <br/>

    |**Property**  |**Data type**    |**Description**                                                  |
    |:---          |:---             |:---                                                             |
    |`.input`      | vector<string>  | List of input files to be processed                             |
    |`.tree`       | string          | Name of the main data tree. default: `EVENT`                    |
    |`.containers` | vector<string>  | List of containers to be loaded to DDDigi.                      |
    |              |                 | If empty => all.                                                |
    |`.segment`    | string          | Name of the input segment. default: "inputs"                    |
    |`.mask`       | integer         | Mask of this input source in the store. default: NO_MASK (0x0)  |
    |`.rescan`     | boolean         | Rescan input sources for continuous execution. default: true    |
    |`.keep_raw`   | boolean         | Keep raw input as opaque objects in the DDDigi store.           |

  Functionality: self explaining

DigiEventAction
---------------

- Generic event level action which can execute workers in parallel                                   <br/>
  Properties:                                                                                        <br/>
    |**Property**     |**Data type**    |**Description**                                                  |
    |:---             |:---             |:---                                                             |
    |`.parallel`      | boolean         | Flag to indicate parallel action execution                      |

DigiContainerProcessor
----------------------

- Base class of all actors dealing with one of more containers sequentially.<br/>

DigiContainerSequence
---------------------
- Sequencer of `DigiContainerProcessor` entities with container based work load splitting.<br/>

  Properties:                                                                                        <br/>
    |**Property**     |**Data type**    |**Description**                                                  |
    |:---             |:---             |:---                                                             |
    |`.parallel`      | boolean         | Flag to indicate parallel action execution                      |

DigiContainerSequenceAction
---------------------------

- Specialization of the `DigiEventAction`.                                                           <br/>
  Process continers from the data store according to the `input_segment` and the `input_mask`.
  Output of workers is delivered to the `output_segment` with mask `output_mask`.                    <br/>

  Properties: Properties of the `DigiEventAction` apply!                                             <br/>

    |**Property**     |**Data type**    |**Description**                                                  |
    |:---             |:---             |:---                                                             |
    |`.input_segment` | string          | Name of the input data segment                                  |
    |`.input_mask`    | integer         | Mask identifier of the input containers to be processed         |
    |`.output_segment`| string          | Name of the output data segment                                 |
    |`.output_mask`   | integer         | Mask identifier of the output containers                        |

DigiMultiContainerProcessor
---------------------------

- Specialization of the `DigiEventAction`.
  Process multiple containers from the data store according to the `input_segment` 
  and the list of `input_masks`. Allows to re-use the same container action.
  Output of workers is delivered to the `output_segment` with mask `output_mask`.                    <br/>

  Properties: Properties of the `DigiEventAction` apply!                                             <br/>

    |**Property**     |**Data type**    |**Description**                                                  |
    |:---             |:---             |:---                                                             |
    |`.input_segment` | string          | Name of the input data segment                                  |
    |`.input_mask`    | integer         | Mask identifier of the input containers to be processed         |
    |`.output_segment`| string          | Name of the output data segment                                 |
    |`.output_mask`   | integer         | Mask identifier of the output containers                        |

DigiAttenuator
--------------
 
- Deposit attenuator for energy deposits according to decay time constant.                           <br/>
  Action functor of the `DigiAttenuatorSequence`                                                     <br/>
  Properties: Properties of the `DigiContainerSequenceProcessor` apply!                              <br/>
    |**Property**     |**Data type**    |**Description**                                                  |
    |:---             |:---             |:---                                                             |
    |`.factor`        | double          | Signal reduction factor applied to all entries processed        |

DigiAttenuatorSequence
----------------------
 
- Container sequencer to attenuate a list of containers.                                             <br/>
  Properties:  Properties of the `DigiContainerSequenceAction` apply!                                <br/>
    |**Property**     |**Data type**    |**Description**                                                  |
    |:---             |:---             |:---                                                             |
    |`.processor_type`| string          | Processor type used for single container attenuation.           |
    |                 |                 | default: `DigiAttenuator`                                       |
    |`.containers`    | vector<string>  | List of containers to be attenuated.                            |
    |`.signal_decay`  | string          | Decay function. default: `exponential`                          |
    |`.t0`            | double          | Time constant for exponential signal decay.                     |



![HORIZON2020](../doc/usermanuals/DD4hep/figures/AIDAinnova.png)
