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
Material scan along the polar angle, creating a series 1D histograms.
The results are presented in material depth [cm], number of radiation lengths,
and number of interaction lengths.
For each of these three, the scan seperates contribution per found material,
creating a histogram for each material individually,
a stacked histogram with all contributions, and one histogram with only the
total value if only the sum of all contributions is of interest.

The behaviour of the script follows the script in k4geo/utils/material_plots.py
in terms of configurable options.
"""

import os
import sys
import re
import argparse
import subprocess
import math
import array
import bisect
import ROOT
from collections import defaultdict

parser = argparse.ArgumentParser(
    formatter_class=argparse.RawTextHelpFormatter,
    description=(
        "Material budget scan in theta/eta/cosTheta/thetaRad for a given geometry,\n"
        "using Geant4 particle gun and the MaterialScanner stepping action.\n"
        "\n"
        "The scan shoots geantinos at fixed angle values and random phi, parses\n"
        "the ddsim output, and saves THStacks of material depth, radiation length,\n"
        "and interaction length vs angle to a ROOT file, PDF, and PNG.\n"
        "\n"
        "Example: \n"
        "  g4MaterialAngleScan.py \\ \n"
        "      -c myDetector.xml \\ \n"
        "      -o materialScan.root \\ \n"
        "      --angleDef theta --minValue 10 --maxValue 170 -b 5 \\ \n"
        "      --eventsPerBin 50 \\ \n"
        "      -i Air Vacuum \\ \n"
        "      --colors 'kRed+2' 'cornflowerblue' '#00ff00' \\ \n"
        "      -t 600 \n"
        ),
    )

parser.add_argument(
    "--compactFile", "-c", dest="compact", type=str, help="name of the detector compact file to load geometry from"
    )

parser.add_argument(
    "--steeringFile", "-s", dest="steering", default=None, type=str, help="ddsim steering file (optional)"
    )

parser.add_argument(
    "--outputFile",
    "-o",
    dest="output",
    default="materialScan.root",
    type=str,
    help="name of output root file with extension (and prefix for exported canvases)",
    )

parser.add_argument(
    "--angleDef",
    dest="angleDef",
    default="theta",
    type=str,
    help="angle definition to use: eta, theta, cosTheta or thetaRad. Default: theta",
    )

parser.add_argument("--minValue", dest="minValue", default=-6, type=float, help="minimum value for eta/theta/cosTheta")

parser.add_argument("--maxValue", dest="maxValue", default=6, type=float, help="maximum value for eta/theta/cosTheta")

parser.add_argument(
    "--angleBinning", "-b", dest="angleBinning", default=0.05, type=float, help="eta/theta/cosTheta/thetaRad bin width"
    )

parser.add_argument(
    "--eventsPerBin",
    dest="eventsPerBin",
    default=100,
    type=int,
    help="target number of geantinos to average over per angle bin (approximate, because we are relying on ddsim gun random distribution in theta)",
    )

parser.add_argument("--seed", "-S", dest="seed", default=None, type=int, help="random seed for ddsim gun (optional)")

parser.add_argument("--timeOut", "-t", dest="timeOut", default=600, type=int, help="timeout for ddsim runs in seconds")

parser.add_argument(
    "--removeMatsSubstrings",
    "-r",
    dest="removeMatsSubstrings",
    nargs="+",
    default=[],
    help=(
        "Substrings to be removed from materials strings "
        "(e.g. DCH_ for drift chamber specific materials). "
        "Applied before --ignoreMats."
        ),
    )

parser.add_argument(
    "--ignoreMats",
    "-i",
    dest="ignoreMats",
    nargs="+",
    default=[],
    help="List of materials that should be ignored. Applied after --removeMatsSubstrings.",
    )

parser.add_argument(
    "-P",
    "--noPilot",
    action="store_true",
    dest="noPilot",
    default=False,
    help="don't run the pilot job (e.g. if you're sure the geometry is good)",
    )

parser.add_argument(
    "--colors",
    dest="colors",
    nargs="+",
    default=None,
    help=(
        "List of ROOT colours to use for materials. Accepts ROOT colour names (e.g. 'kRed' 'kBlue+2', case-sensitive), "
        "ROOT-style integers (e.g. '4' '8' '15' '16' '23' '42'), hex codes in quotes (e.g. '#ff0000' '#3b82f6'), "
        "or matplotlib names (e.g. 'red' 'steelblue' 'tab:blue'). If fewer colours are provided than "
        "materials are found, the list will be padded with default ROOT colours."
        ),
    )

args = parser.parse_args()

# ---------------------------------------------------------------------------
# valid angle definitions and helper functions
# ---------------------------------------------------------------------------

ANGLE_AND_DISTRIBUTION_DEFS = {
    "theta": "uniform",
    "eta": "eta",
    "cosTheta": "cosTheta",
    "thetaRad": "uniform",
    }
    
def direction_to_angleDef(dx, dy, dz, angle_def):
    theta = math.acos(dz / math.sqrt(dx**2 + dy**2 + dz**2)) 
    if angle_def == "theta":
        return math.degrees(theta)
    elif angle_def == "thetaRad":
        return theta
    elif angle_def == "cosTheta":
        return math.cos(theta)
    elif angle_def == "eta":
        if not (0.0 <= theta <= math.pi):
            raise ValueError("Reconstructed theta not in [0, pi]")
        if math.isclose(theta, 0.0, abs_tol=1e-12):
            return float("inf")
        if math.isclose(theta, math.pi, abs_tol=1e-12):
            return float("-inf")
        return -math.log(math.tan(theta / 2.0))



# ---------------------------------------------------------------------------
# default colours and colour list builder for ROOT histograms
# ---------------------------------------------------------------------------

DEFAULT_COLORS = [
    ROOT.kRed,
    ROOT.kBlue,
    ROOT.kGreen + 2,
    ROOT.kOrange + 1,
    ROOT.kMagenta,
    ROOT.kCyan + 1,
    ROOT.kYellow + 2,
    ROOT.kViolet + 1,
    ROOT.kTeal + 2,
    ROOT.kPink + 3,
    ROOT.kAzure + 2,
    ROOT.kSpring + 5,
    ]


def resolve_color(c):
    """
    Accept:
      - ROOT names:             kRed, kBlue+2, kAzure-1
      - ROOT-style integers:    2, 4, 6
      - hex codes:              #ff0000, #3b82f6
      - matplotlib names:       red, steelblue, tab:blue, ...
    Returns a ROOT color index (int) or None if the color cannot be resolved.
    """
    c = str(c).strip()

    # hex code
    if c.startswith("#"):
        return ROOT.TColor.GetColor(c)

    # plain integer
    try:
        return int(c)
    except ValueError:
        pass

    # ROOT name with optional offset: kGreen+2, kAzure-1
    for op in ("+", "-"):
        if op in c:
            name, offset = c.split(op, 1)
            try:
                return getattr(ROOT, name.strip()) + int(op + offset.strip())
            except (AttributeError, ValueError):
                pass
    try:
        return getattr(ROOT, c)
    except AttributeError:
        pass

    # matplotlib color name -> hex -> ROOT
    try:
        import matplotlib.colors as mcolors

        hex_col = mcolors.to_hex(c)  # handles 'red', 'steelblue', 'tab:blue', etc.
        return ROOT.TColor.GetColor(hex_col)
    except (ImportError, ValueError):
        pass

    print(f'WARNING: could not resolve color "{c}"')
    return None


def build_color_list(args_colors):
    """
    Create list from user input and fill with default colors if needed.
    Ensure no dupicated colors in the final list.
    """
    if args_colors is None:
        return DEFAULT_COLORS
    resolved = []
    resolved_set = set()
    for color in args_colors:
        resolved_color = resolve_color(color)
        if resolved_color is None:
            continue
        if resolved_color in resolved_set:
            print(f'WARNING: color "{resolved_color}" already in list, skipping it')
            continue
        resolved.append(resolved_color)
        resolved_set.add(resolved_color)

    # pad with default colors
    for col in DEFAULT_COLORS:
        if col not in resolved_set:
            resolved.append(col)
            resolved_set.add(col)

    return resolved


COLORS = build_color_list(args.colors)


# ---------------------------------------------------------------------------
# check that the requested inputs are valid
# ---------------------------------------------------------------------------

if not os.path.isfile(args.compact):
    print("ERROR: cannot find requested input geometry file", args.compact, file=sys.stderr)
    exit(1)
print("geometry file:", args.compact)

if args.steering is not None and not os.path.isfile(args.steering):
    print("ERROR: cannot find requested ddsim steering file", args.steering, file=sys.stderr)
    exit(1)
print("ddsim steering file:", args.steering)

angleDef = str(args.angleDef)
if angleDef not in ANGLE_AND_DISTRIBUTION_DEFS.keys():
    print("ERROR: unknown angle definition", angleDef, ". Choose from ", ANGLE_AND_DISTRIBUTION_DEFS.keys(), ".", file=sys.stderr)
    exit(1)
print(angleDef)

minValue = float(args.minValue)
maxValue = float(args.maxValue)
binning = float(args.angleBinning)

if angleDef == "theta":
    if minValue < 0:
        print("WARNING: lower theta bound is negative, setting to 0")
        minValue = 0
    if maxValue > 180:
        print("WARNING: upper theta bound is above 180, setting to 180")
        maxValue = 180

elif angleDef == "thetaRad":
    if minValue < 0:
        print("WARNING: lower theta bound is negative, setting to 0")
        minValue = 0
    if maxValue > 3.14159:
        print("WARNING: upper theta bound is above pi, setting to pi")
        maxValue = 3.14159

elif angleDef == "cosTheta":
    if minValue < -1:
        print("WARNING: lower cosTheta bound is below -1, setting to -1")
        minValue = -1
    if maxValue > 1:
        print("WARNING: upper cosTheta bound is above 1, setting to 1")
        maxValue = 1


# ---------------------------------------------------------------------------
# create bins
# ---------------------------------------------------------------------------

# build uniform edges in native angle variable
# converted to theta later for Geant4
edges = []
v = minValue
while v <= maxValue + 1e-9:
    edges.append(round(v, 12))
    v += binning
if edges[-1] < maxValue - 1e-12:
    edges.append(maxValue)
nBins = len(edges) - 1

if nBins < 1:
    print("ERROR: bin width larger than the requested range", file=sys.stderr)
    sys.exit(1)


# ---------------------------------------------------------------------------
# functions to run ddsim
# ---------------------------------------------------------------------------

def build_angle_args(angle_def, angle_min, angle_max):
    """
    Convert user input for min and max values of the scan range
    into the appropriate ddsim gun arguments for the requested angle definition
    """
    if angle_def == "eta":
        return [
            "--gun.etaMin", str(angle_min),
            "--gun.etaMax", str(angle_max),
        ]

    elif angle_def == "theta":
        return [
            "--gun.thetaMin", f"{angle_min}*deg",
            "--gun.thetaMax", f"{angle_max}*deg",
        ]

    elif angle_def == "thetaRad":
        return [
            "--gun.thetaMin", str(angle_min),
            "--gun.thetaMax", str(angle_max),
        ]

    elif angle_def == "cosTheta":
        theta_from_cos_min = math.acos(angle_min)
        theta_from_cos_max = math.acos(angle_max)

        # ensure that theta_min < theta_max, since acos is decreasing
        theta_min_rad = min(theta_from_cos_min, theta_from_cos_max)
        theta_max_rad = max(theta_from_cos_min, theta_from_cos_max)
        return [
            "--gun.thetaMin", str(theta_min_rad),
            "--gun.thetaMax", str(theta_max_rad),
        ]
    
    return []

def build_ddsim_cmd(with_stdbuf=False):

    cmd = (["stdbuf", "-oL"] if with_stdbuf else []) + [
        "ddsim",
        "--compactFile",
        args.compact,
        "--runType",
        "batch",
        "--enableGun",
        "-N",
        str(args.eventsPerBin*nBins),
        "--action.step",
        "Geant4MaterialScanner/MaterialScan",
        "--gun.particle",
        "geantino",
        "--gun.energy",
        "10*GeV",
        "--gun.position",
        "0,0,0",
        "--gun.distribution",
        f"{ANGLE_AND_DISTRIBUTION_DEFS[angleDef]}",
        "--gun.phiMin",
        "0*deg",
        "--gun.phiMax",
        "360*deg"
        ]
    cmd += build_angle_args(angleDef, minValue, maxValue)
    if args.steering is not None:
        cmd += ["--steeringFile", args.steering]
    if args.seed is not None:
        cmd += ["--random.seed", str(args.seed)]
    return cmd


def run_ddsim(timeout):
    cmd = build_ddsim_cmd(with_stdbuf=True)
    print("Running:", " ".join(cmd))
    try:
        return subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        sys.exit(f"ERROR: ddsim timed out after {timeout} s")


def run_ddsim_progress(timeout, n_events):
    import threading
    from tqdm import tqdm

    cmd = build_ddsim_cmd(with_stdbuf=True)
    print("Running:", " ".join(cmd))

    lines = []

    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)

    # Watchdog thread to enforce timeout
    def _watchdog():
        try:
            proc.wait(timeout=timeout)
        except subprocess.TimeoutExpired:
            print(f"\nERROR: ddsim timed out after {timeout} s, killing process.")
            proc.kill()

    watchdog = threading.Thread(target=_watchdog, daemon=True)
    watchdog.start()

    pbar = None
    print("  Waiting for Geant4 initialisation...", flush=True)

    for line in proc.stdout:
        lines.append(line)
        if "+++ Initializing event" in line:
            if pbar is None:
                # start progress bar only once the first event starts, to avoid counting Geant4 initialisation time in the progress bar
                print("  Initialisation complete, scanning...", flush=True)
                pbar = tqdm(total=n_events, unit="evt", desc="  ddsim")
                pbar.update(1)  # count the event we just saw
            else:
                pbar.update(1)

    if pbar is not None:
        pbar.close()

    proc.wait()

    if proc.returncode == -9:
        sys.exit("ERROR: ddsim was killed due to timeout.")
    if proc.returncode != 0:
        print(f"WARNING: ddsim exited with return code {proc.returncode}")

    # Create a simple object to hold the combined stdout for parsing later
    class Result:
        stdout = "".join(lines)

    return Result()


# ---------------------------------------------------------------------------
# pilot run
# ---------------------------------------------------------------------------

if not args.noPilot:
    print("\nRunning pilot job...")
    pr = run_ddsim(args.timeOut)
    n_scans = sum(1 for line in pr.stdout.splitlines() if "Material scan between" in line)
    n_finished = sum(1 for line in pr.stdout.splitlines() if "Finished run" in line)
    if n_scans < 1 or n_finished < 1:
        print("ERROR: pilot job did not finish successfully.")
        print("Re-run the following command to investigate:")
        print(" ".join(build_ddsim_cmd(with_stdbuf=False)))
        sys.exit(1)
    print("Pilot job OK.\n")

# ---------------------------------------------------------------------------
# main ddsim run
# ---------------------------------------------------------------------------

print(f"Running main job ({nBins * args.eventsPerBin} events)...")
result = run_ddsim_progress(args.timeOut, nBins * args.eventsPerBin)


# ---------------------------------------------------------------------------
# parse ddsim output
#
# The MaterialScanner prints one block per event. Example block:
#
#  +-------------------------------------------------------------------------------------------------------------------------------------------------- # noqa: E501
#  + Material scan between: (x0, y0, z0)[cm] and (x1, y1, z1)[cm]  TrackID:1: # noqa: E501
#  +-------------------------------------------------------------------------------------------------------------------------------------------------- # noqa: E501
#  |     \   Material           Atomic                 Radiation   Interaction               Path   Integrated  Integrated    Material # noqa: E501
#  | Num. \  Name          Number/Z   Mass/A  Density    Length       Length    Thickness   Length      X0        Lambda      Endpoint # noqa: E501
#  | Layer \                        [g/mole]  [g/cm3]     [cm]        [cm]          [cm]      [cm]     [cm]        [cm]     (     cm,     cm,     cm) # noqa: E501
#  +-------------------------------------------------------------------------------------------------------------------------------------------------- # noqa: E501
#  |     1 Air                    7   14.784   0.0012  30528.8402   71282.7920    183.325   183.32    0.006005    0.002572  ( -34.68,  -4.58, 179.96) # noqa: E501
#  ...
# GenerationInit   WARN  +++ Finished run 1 after ...
#
# ---------------------------------------------------------------------------

direction_re = re.compile(
    r"direction:\(\s*([-\d.]+)\s+([-\d.]+)\s+([-\d.]+)\)"
)
raw_data = []  # list of events; each event is a list of step tuples
current_evt = []
current_direction = None
in_scan = False

# Note: ddsim output is parsed for a second time now (first in run_ddsim_progress for the progress bar).
# In principle, we could unify these two steps, but this way the code is less cluttered/intertwined,
# and the parsing is very fast, so it should not be a problem.
print("\nParsing ddsim output...")

# keep track of materials that collapse to the same name when applying the 'removeMatSubtrings' option
# e.g. G4_Cu and Custom_Cu, if both G4_ and Custom_ are removed
conflicting_name_dict = defaultdict(set)

debug_counter = 0
for line in result.stdout.splitlines():
    # Get the geantino direction
    m = direction_re.search(line)
    if m and "Particle" in line:
        dx, dy, dz = (float(v) for v in m.groups())
        current_direction = (dx, dy, dz)

    if "Material scan between" in line:
        # starting a new event's scan; save the previous event first
        if current_evt:
            raw_data.append((current_direction, current_evt))
            debug_counter += 1
        current_evt = []
        # current_direction = None
        in_scan = True

    elif (
        in_scan and "(" in line and len(line.split("(")[0].split()) == 12 and line.split()[0] == "|"
        ):  # this line contains material information
        parts = line.split()
        try:
            int(parts[1])  # first token after '|' should be a step index
        except ValueError:
            continue  # skip header/separator lines
        try:
            x0_cm = float(parts[6])
            li_cm = float(parts[7])
            thick_cm = float(parts[8])
            # print(f"DEBUG: -> x0_cm={x0_cm}, li_cm={li_cm}, thick_cm={thick_cm}, direction={current_direction}, mat_name={parts[2]}")
        except (ValueError, IndexError):
            continue
        if x0_cm <= 0.0 or li_cm <= 0.0:
            continue

        original_name = parts[2]
        mat_name = original_name
        for substring in args.removeMatsSubstrings:
            mat_name = mat_name.replace(substring, "")
        conflicting_name_dict[mat_name].add(original_name)

        t_over_x0 = thick_cm / x0_cm
        t_over_li = thick_cm / li_cm
        thick_mm = thick_cm * 10.0
        current_evt.append((mat_name, t_over_x0, t_over_li, thick_mm))

# append the last event
if current_evt:
    # print(f"DEBUG: appending last event with {len(current_evt)} steps, direction={current_direction}")
    # print(f"DEBUG: last event steps: {current_evt}")
    raw_data.append((current_direction, current_evt))

# print out any material name conflicts
for mat_name, original_names in conflicting_name_dict.items():
    if len(original_names) > 1:
        print(
            f'WARNING: multiple material names collapse to "{mat_name}" '
            f'after applying --removeMatsSubstrings: {original_names}'
            )

n_received = len(raw_data)
n_expected = nBins * args.eventsPerBin
print(f"Parsed {n_received} scan events (expected {n_expected})")
if n_received != n_expected:
    print(f"WARNING: event count mismatch ({n_received} vs {n_expected}). " "Results may be incomplete.")


# ---------------------------------------------------------------------------
# accumulate per-bin, per-material sums, then average over phi
#
# bin_data[ib][mat_name] = [sum_x0, sum_li, sum_len_mm]
# Sums are over all nPhi shots for that bin; divided by nPhi at the end.
# ---------------------------------------------------------------------------

bin_data = [{} for _ in range(nBins)]
bin_counts = [0] * nBins  # count actual events per bin
bin_edges = [args.minValue + i * args.angleBinning for i in range(nBins + 1)]

def find_bin(angle_value, bin_edges):
    ib = bisect.bisect_right(bin_edges, angle_value) - 1

    if ib < 0 or ib >= len(bin_edges) - 1:
        return None

    return ib

for direction, steps in raw_data:
    if direction is None:
        continue
    dx, dy, dz = direction
    angle_value = direction_to_angleDef(dx, dy, dz, angleDef)
    print(f"DEBUG: direction={direction}, angle_value={angle_value}, angleDef={angleDef}")
    ib = find_bin(angle_value, bin_edges)
    if ib is None:
        print(f"WARNING: angle value {angle_value} out of range for binning, skipping event")
        continue
    # print(f"DEBUG: angle_value={angle_value} falls into bin {ib} (range {bin_edges[ib]} to {bin_edges[ib+1]})")
    
    bin_counts[ib] += 1

    for mat, t_x0, t_li, t_mm in steps:
        # Ignore certain materials if specified
        if mat in args.ignoreMats:
            continue
        if mat not in bin_data[ib]:
            bin_data[ib][mat] = [0.0, 0.0, 0.0]
        bin_data[ib][mat][0] += t_x0
        bin_data[ib][mat][1] += t_li
        bin_data[ib][mat][2] += t_mm
        print(f"DEBUG: bin {ib}, mat {mat}, t_x0={t_x0}, t_li={t_li}, t_mm={t_mm}, cumulative sums: {bin_data[ib][mat]}")

# divide by nPhi to get the phi-averaged value
for ib in range(nBins):
    n_events_in_bin = bin_counts[ib]
    if n_events_in_bin == 0:
        print(f"WARNING: no events found for bin {ib} (angle range {bin_edges[ib]} to {bin_edges[ib+1]}), skipping")
        continue
    for mat in bin_data[ib]:
        print(f"DEBUG: bin {ib}, mat {mat}, before averaging: x0={bin_data[ib][mat][0]}, li={bin_data[ib][mat][1]}, len_mm={bin_data[ib][mat][2]}, n_events_in_bin={n_events_in_bin}")
        bin_data[ib][mat][0] /= n_events_in_bin
        bin_data[ib][mat][1] /= n_events_in_bin
        bin_data[ib][mat][2] /= n_events_in_bin
        print(f"DEBUG: bin {ib}, mat {mat}, averaged values: x0={bin_data[ib][mat][0]}, li={bin_data[ib][mat][1]}, len_mm={bin_data[ib][mat][2]}")

all_mats = sorted({mat for ib in range(nBins) for mat in bin_data[ib]})
print(f"Materials found: {all_mats}")

# ---------------------------------------------------------------------------
# ROOT histograms
# ---------------------------------------------------------------------------

fout = ROOT.TFile(args.output, "recreate")

edges_arr = array.array("d", edges)
nbins_root = len(edges) - 1

AXIS_LABELS = {
    "theta": "#theta [deg]",
    "eta": "#eta",
    "cosTheta": "cos(#theta)",
    "thetaRad": "#theta [rad]",
    }
AXIS_LABEL = AXIS_LABELS[angleDef]


def make_stack_and_total(qty_idx, qty_name, y_title):
    """
    Build a THStack of per-material TH1D + a black 'Total' TH1D.
    qty_idx: 0 = x0, 1 = lambda, 2 = path length
    """
    stack = ROOT.THStack(f"hs_{qty_name}", f";{AXIS_LABEL};{y_title}")
    total = ROOT.TH1D(f"h_{qty_name}_TOTAL", f"Total;{AXIS_LABEL};{y_title}", nbins_root, edges_arr)
    total.SetLineWidth(2)
    total.SetLineColor(ROOT.kBlack)

    mat_hists = {}
    for ci, mat in enumerate(all_mats):
        h = ROOT.TH1D(f"h_{qty_name}_{mat}", f"{mat};{AXIS_LABEL};{y_title}", nbins_root, edges_arr)
        col = COLORS[ci % len(COLORS)]
        h.SetFillColor(col)
        h.SetLineColor(ROOT.kBlack)
        h.SetLineWidth(1)
        mat_hists[mat] = h

    for ib in range(nBins):
        root_bin = ib + 1  # ROOT 1-indexed
        for mat, vals in bin_data[ib].items():
            print(f"DEBUG: filling ROOT hist for bin {ib}, mat {mat}, vals={vals}")
            val = vals[qty_idx]
            mat_hists[mat].AddBinContent(root_bin, val)
            total.AddBinContent(root_bin, val)

    for mat in all_mats:
        stack.Add(mat_hists[mat])
        # mat_hists[mat].Write()

    stack.Write()
    # total.Write()
    return stack


stack_x0 = make_stack_and_total(0, "x0", "Number of X_{0}")
stack_li = make_stack_and_total(1, "lambda", "Number of #lambda_{I}")
stack_len = make_stack_and_total(2, "depth", "Material depth [mm]")

# ---------------------------------------------------------------------------
# ROOT canvases
# ---------------------------------------------------------------------------

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)


def draw_canvas(stack, canvas_name, title):
    c = ROOT.TCanvas(canvas_name + "_canvas", title, 900, 600)
    c.SetLeftMargin(0.12)
    stack.Draw("hist")
    # stack.GetXaxis().SetTitle(AXIS_LABEL)
    # total.Draw('hist same')
    leg = ROOT.TLegend(0.65, 0.58, 0.92, 0.90)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    if stack.GetHists():
        for mat in reversed(all_mats):  # reverse to match stack order
            h = stack.GetHists().FindObject(f"h_{canvas_name}_{mat}")
            if h:
                leg.AddEntry(h, mat, "f")
    # leg.AddEntry(total, 'Total', 'l')
    leg.Draw()

    export_name = str(args.output)
    if export_name.endswith(".root"):
        export_name = export_name[: -len(".root")]
    c.Print(export_name + "_" + canvas_name + ".pdf")
    c.Print(export_name + "_" + canvas_name + ".png")
    c.Write()


draw_canvas(stack_x0, "x0", "Radiation lengths")
draw_canvas(stack_li, "lambda", "Interaction lengths")
draw_canvas(stack_len, "depth", "Material depth [mm]")

fout.Write()
fout.Close()
print(f"\nDone. Results written to {args.output}")

