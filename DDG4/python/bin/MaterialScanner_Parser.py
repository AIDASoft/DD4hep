#!/usr/bin/env python3
# ==========================================================================
#  AIDA Detector description implementation
# --------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
# ==========================================================================
"""
Parser for the output of the Geant4MaterialScanner/MaterialScan.
Used by g4GraphicalScan.py and g4PolarAngleScan.py to extract the material budget along a geantino track.
In case of any updates to the output format of the scanner, only this file needs to be adapted.
This is a pure parser, how the infromation is used differs between g4GraphicalScan and g4PolarAngleScan.
"""

import re
import math
from dataclasses import dataclass, field
from typing import Iterator, Optional


# The MaterialScanner prints one block per event. Example block (the noqa: E501 comments are to avoid line length warnings in flake8):
#   +-------------------------------------------------------------------------------------------------------------------------------------------------- # noqa: E501
#   + Material scan between: x_0 = (   0.00,   0.00,   0.00) [cm] and x_1 = (-1525.80, 265.01,2500.00) [cm]  TrackID:1: # # noqa: E501
#   +-------------------------------------------------------------------------------------------------------------------------------------------------- # noqa: E501
#   |     \   Material           Atomic                 Radiation   Interaction               Path   Integrated  Integrated    Material # noqa: E501
#   | Num. \  Name          Number/Z   Mass/A  Density    Length       Length    Thickness   Length      X0        Lambda      Endpoint # noqa: E501
#   | Layer \                        [g/mole]  [g/cm3]     [cm]        [cm]          [cm]      [cm]     [cm]        [cm]     (     cm,     cm,     cm) # noqa: E501
#   +-------------------------------------------------------------------------------------------------------------------------------------------------- # noqa: E501
#   |     1 Air                    7   14.784   0.0012  30528.8402   71282.7920     66.425    66.43    0.002176    0.000932  ( -34.46,   5.99,  56.47) # noqa: E501
#   |     2 CarbonFibStr           4    8.127   1.4500     33.2316      37.9830      0.038    66.46    0.003319    0.001932  ( -34.48,   5.99,  56.50) # noqa: E501
#   ...
#  GenerationInit   WARN  +++ Finished run 1 after ...

# Matches the "Material scan between" line, extracting x_1 endpoint
END_RE = re.compile(
    r"Material scan between:.*x_1\s*=\s*\(\s*([-\d.]+),\s*([-\d.]+),\s*([-\d.]+)\)"
)

# Matches the "Material scan between" line, extracting x_0 start point
START_RE = re.compile(
    r"Material scan between:\s*x_0\s*=\s*\(\s*([-\d.]+),\s*([-\d.]+),\s*([-\d.]+)\)"
)


@dataclass
class MaterialStep:
    """One material layer (one line) as reported by the MaterialScanner."""
    index: int
    name: str             # name of the material
    x0_cm: float          # radiation length of material [cm]
    li_cm: float          # interaction length of material [cm]
    thickness_cm: float   # traversed thickness in this layer [cm]
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
    """
    current_event: Optional[ScanEvent] = None
    in_scan = False

    for line in lines:
        if "Material scan between" in line:
            # start a new event; emit the previous one if it exists
            if current_event is not None:
                yield current_event

            # Get the geantino start and end points
            # Used to get the direction
            start_m = START_RE.search(line)
            end_m = END_RE.search(line)

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
        ):  # this line contains material information
            parts = line.split()

            try:
                index = int(parts[1])  # first token after '|' should be a step index
            except ValueError:
                continue  # skip header/separator lines

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
