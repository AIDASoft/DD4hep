# EDM4hep Arrow stream output in DD4hep/ddsim

## Status

This branch adds Arrow IPC stream output for EDM4hep events in `Geant4Output2EDM4hep`.

- `.arrow` output is accepted by DDSim and routed to the EDM4hep Arrow backend automatically.
- FIFO targets are also routed to the Arrow backend automatically.
- The default ROOT behavior remains unchanged for `.root` output.
- The FIFO path avoids `lseek()` on the pipe by writing directly to the fd, which avoids the `ESPIPE`/`lseek failed` crash.

## Current behavior

Use a normal `.arrow` filename:

```bash
ddsim --compactFile=$DETECTOR_PATH/epic_craterlake_tracking_only.xml \
      --outputFile=test_stream.arrow \
      -N10 -G
```

Or write to a FIFO:

```bash
mkfifo /tmp/test_stream.arrow
# run ddsim writing to /tmp/test_stream.arrow
```

No explicit `useArrow=True` or `OutputBackend=arrow` setting is required for either case. The output target itself determines the backend.

## Implementation notes

- `Geant4Output2EDM4hep` supports both ROOT and Arrow backends through a single plugin.
- Arrow output writes one record batch per event using the IPC stream writer.
- The pipe-safe path uses a raw fd-backed `arrow::io::OutputStream` and does not call `lseek()`.

## Default ROOT output

```python
SIM.outputConfig.output = "output.root"
```

This remains ROOT output by default.

## License

Same as DD4hep (LGPLv3)
