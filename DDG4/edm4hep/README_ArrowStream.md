# EDM4hep Arrow Stream Writer for DD4hep/ddsim

## Overview

This is a **proof-of-concept** implementation of Arrow IPC stream writing for EDM4hep events in DD4hep/ddsim. It enables streaming simulation output through named pipes or sockets for real-time processing, without requiring a downstream consumer to be implemented yet.

**Architecture**: Arrow streaming is integrated as an output backend within the existing `Geant4Output2EDM4hep` plugin, selected via the `OutputBackend` property.

## Features

- **Integrated Backend Selection**: Single `Geant4Output2EDM4hep` plugin supports both ROOT and Arrow backends
- **Arrow IPC Stream Output**: Writes EDM4hep frames as Apache Arrow IPC stream records
- **Shared Frame Population**: Both backends use the same particle and hit collection conversion logic
- Uses `podio::convertFrameToTable()` to convert EDM4hep frames to Arrow tables
- Writes RecordBatch-per-event to an Arrow IPC stream (file, named pipe, or socket)

## Dependencies

- Apache Arrow (tested with Arrow provided by podio)
- podio with Arrow support (recent master, includes `ArrowFrameConverter`)
- EDM4hep

## Building

The Arrow backend is automatically enabled in the `Geant4Output2EDM4hep` plugin if Arrow is found:

```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/arrow ..
make
```

Check the CMake output for:
```
-- EDM4hep plugin: Arrow backend support enabled
```

If Arrow is not available:
```
-- EDM4hep plugin: Arrow backend support disabled (arrow_shared not found)
```

## Usage

### Basic File Output with Arrow Backend

In your ddsim steering file:

```python
from DDSim.DD4hepSimulation import DD4hepSimulation
from g4units import *

SIM = DD4hepSimulation()

# Configure Arrow stream output
SIM.outputConfig.output = "/tmp/edm4hep_stream.arrow"

# Select Arrow backend (ROOT is default)
output = SIM.outputConfig.part
output.userParameters["OutputBackend"] = "arrow"

# Optional: specify which collections to write (Arrow backend only)
# output.userParameters["CollectionsToWrite"] = ["MCParticles", "SimTrackerHits"]

# Run simulation
SIM.run()
```

### Named Pipe (FIFO) Example

Create a named pipe for streaming:

```bash
mkfifo /tmp/edm4hep_events.fifo
```

Configure ddsim to write to the pipe:

```python
SIM.outputConfig.output = "/tmp/edm4hep_events.fifo"
SIM.outputConfig.part.userParameters["OutputBackend"] = "arrow"
```

A downstream consumer can read from the pipe in parallel (e.g., EICrecon with Arrow stream reader).

### Default ROOT Output (Backward Compatible)

If you don't specify `OutputBackend`, the plugin defaults to ROOT output:

```python
# This still works - ROOT backend is default
SIM.outputConfig.output = "output.root"
# No need to specify OutputBackend
```

## Implementation Details

### Backend Architecture

The `Geant4Output2EDM4hep` plugin now supports multiple output backends:

- **ROOT Backend** (default): Uses `podio::Writer` to write ROOT files
- **Arrow Backend** (optional): Writes Arrow IPC streams directly

Backend selection is controlled by the `OutputBackend` property (`"root"` or `"arrow"`).

### Key Methods

- **beginRun()**: Dispatches to `initializeROOTBackend()` or `initializeArrowBackend()`
- **commit()**: Dispatches to `commitROOT()` or `commitArrow()`
- **endRun()**: Closes both ROOT and Arrow resources appropriately

### Data Flow (Arrow Backend)

1. Geant4 simulation produces hits and particles
2. Plugin methods populate `m_frame` with EDM4hep collections (shared with ROOT backend)
3. `commitArrow()` converts the frame to an Arrow Table (1 row per event)
4. Extract RecordBatch from Table
5. Write RecordBatch to Arrow IPC stream
6. Clear frame for next event

### Arrow IPC Format

- Uses Arrow IPC Streaming Format (not Arrow IPC File Format)
- Each event is written as a separate RecordBatch
- Schema is established from the first event
- Stream can be read incrementally as events arrive

## Configuration Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `OutputBackend` | string | `"root"` | Output backend: `"root"` or `"arrow"` |
| `CollectionsToWrite` | vector<string> | (all) | Collections to write (Arrow backend only; ROOT writes all) |
| `RNTuple` | bool | `false` | Use RNTuple format (ROOT backend only) |

## Limitations / TODOs

- [ ] Run and metadata frames not yet streamed in Arrow backend (only event frames)
- [ ] No schema evolution handling
- [ ] Collection filtering (`CollectionsToWrite`) needs more testing
- [ ] No benchmarking: Arrow vs ROOT performance comparison
- [ ] Arrow backend not tested with multi-threading

## Testing

### Test Arrow Backend

```bash
# Run a simple simulation with Arrow output
ddsim --compactFile=my_detector.xml \
      --outputFile=/tmp/test_stream.arrow \
      --numberOfEvents=10 \
      --gun.particle="mu-" \
      --gun.energy=10*GeV

# Inspect the output with Arrow tools
python3 -c "
import pyarrow as pa
import pyarrow.ipc as ipc

with open('/tmp/test_stream.arrow', 'rb') as f:
    reader = ipc.open_stream(f)
    for i, batch in enumerate(reader):
        print(f'Event {i}: {batch.num_rows} rows, {batch.num_columns} columns')
        print(f'  Columns: {batch.schema.names}')
"
```

### Test ROOT Backend (Regression)

```bash
# Verify default ROOT output still works
ddsim --compactFile=my_detector.xml \
      --outputFile=/tmp/test_root.root \
      --numberOfEvents=10
```

## Migration from Separate Plugin

If you used the previous separate `Geant4Output2EDM4hepArrowStream` plugin:

**Before**:
```python
SIM.outputConfig.outputType = "Geant4Output2EDM4hepArrowStream"
SIM.outputConfig.output = "stream.arrow"
```

**After**:
```python
SIM.outputConfig.outputType = "Geant4Output2EDM4hep"  # default, can omit
SIM.outputConfig.output = "stream.arrow"
SIM.outputConfig.part.userParameters["OutputBackend"] = "arrow"
```

## Related Work

- podio Arrow backend: [https://github.com/AIDASoft/podio/pull/999](https://github.com/AIDASoft/podio/pull/999)
- podio `convertFrameToTable()`: [https://github.com/AIDASoft/podio/pull/980](https://github.com/AIDASoft/podio/pull/980)
- EICrecon Arrow reader: [https://github.com/eic/EICrecon/pull/2857](https://github.com/eic/EICrecon/pull/2857)

## Contributing

This is a draft proof-of-concept. Feedback and contributions are welcome to:
- Test with real detectors and multi-threaded simulations
- Add streaming of run/metadata frames to Arrow backend
- Implement benchmarks comparing Arrow and ROOT backends
- Test named pipe streaming with downstream consumers

## License

Same as DD4hep (LGPLv3)
