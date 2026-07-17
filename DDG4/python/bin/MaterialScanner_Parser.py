import re
import math
from dataclasses import dataclass, field
from typing import Iterator, Optional

# Matches the "Material scan between" line, extracting x_1 endpoint
_SCAN_RE = re.compile(
    r"Material scan between:.*x_1\s*=\s*\(\s*([-\d.]+),\s*([-\d.]+),\s*([-\d.]+)\)"
)

# Matches the "Material scan between" line, extracting x_0 start point
_START_RE = re.compile(
    r"Material scan between:\s*x_0\s*=\s*\(\s*([-\d.]+),\s*([-\d.]+),\s*([-\d.]+)\)"
)


@dataclass
class MaterialStep:
    """One material layer as reported by the MaterialScanner."""
    index: int
    name: str
    x0_cm: float          # radiation length [cm]
    li_cm: float          # interaction length [cm]
    thickness_cm: float   # layer thickness [cm]
    path_length_cm: float # integrated path length [cm]
    endpoint_cm: tuple    # (x, y, z) endpoint [cm]


@dataclass
class ScanEvent:
    """One complete MaterialScanner block (one geantino track)."""
    # start and end points in cm, as printed by the MaterialScanner
    start_cm: tuple               # (x, y, z)
    end_cm: tuple                 # (x, y, z)
    # unit direction vector, derived from start and end points
    direction: tuple              # (dx, dy, dz), normalised
    steps: list[MaterialStep] = field(default_factory=list)


def parse_materialscanner_output(lines: Iterator[str]) -> Iterator[ScanEvent]:
    """
    Parse MaterialScanner output line by line and yield one ScanEvent per track.

    Parameters
    ----------
    lines : iterable of str
        The raw output lines from ddsim (e.g. proc.stdout or result.stdout.splitlines()).

    Yields
    ------
    ScanEvent
        One event per completed material scan block.
    """
    current_event: Optional[ScanEvent] = None
    in_scan = False

    for line in lines:
        if "Material scan between" in line:
            # start a new event; emit the previous one if it exists
            if current_event is not None:
                yield current_event

            start_m = _START_RE.search(line)
            end_m = _SCAN_RE.search(line)

            if end_m:
                x1, y1, z1 = (float(v) for v in end_m.groups())
                norm = math.sqrt(x1**2 + y1**2 + z1**2)
                direction = (x1 / norm, y1 / norm, z1 / norm) if norm > 0 else (0, 0, 0)
            else:
                direction = None

            if start_m:
                start_cm = tuple(float(v) for v in start_m.groups())
            else:
                start_cm = (0.0, 0.0, 0.0)

            end_cm = (x1, y1, z1) if end_m else None

            current_event = ScanEvent(
                start_cm=start_cm,
                end_cm=end_cm,
                direction=direction,
            )
            in_scan = True

        elif (
            in_scan
            and "(" in line
            and len(line.split("(")[0].split()) == 12
            and line.split()[0] == "|"
        ):
            parts = line.split()
            try:
                index = int(parts[1])
            except ValueError:
                continue
            try:
                x0_cm    = float(parts[6])
                li_cm    = float(parts[7])
                thick_cm = float(parts[8])
                path_cm  = float(parts[9])
                endpos   = line.split("(")[1].split(")")[0].split(",")
                endpoint = tuple(float(v) for v in endpos)
            except (ValueError, IndexError):
                continue

            if x0_cm <= 0.0 or li_cm <= 0.0 or thick_cm <= 0.0:
                continue

            current_event.steps.append(MaterialStep(
                index        = index,
                name         = parts[2],
                x0_cm        = x0_cm,
                li_cm        = li_cm,
                thickness_cm = thick_cm,
                path_length_cm = path_cm,
                endpoint_cm  = endpoint,
            ))

        elif "Finished run" in line or "Initializing event" in line:
            if in_scan and current_event is not None:
                yield current_event
                current_event = None
            in_scan = False

    # emit the final event if the output ended without a closing marker
    if current_event is not None:
        yield current_event
