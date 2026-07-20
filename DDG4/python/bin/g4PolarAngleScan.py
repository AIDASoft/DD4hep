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
The results are presented in material depth [mm], number of radiation lengths,
and number of interaction lengths.
For each of these three, the scan seperates contribution per found material,
creating a histogram for each material individually,
a stacked histogram with all contributions, and one histogram with only the
total value if only the sum of all contributions is of interest.

The behaviour of the script tries to follow the script in k4geo/utils/material_plots.py
in terms of configurable options.
"""

import os
import sys
import argparse
import subprocess
import math
import array
import ROOT
from collections import defaultdict
import threading
import textwrap

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from MaterialScanner_Parser import parse_materialscanner_output

parser = argparse.ArgumentParser(
    formatter_class=argparse.RawTextHelpFormatter,
    description=textwrap.dedent("""\
        Material budget scan in theta/eta/cosTheta/thetaRad for a given geometry,
        using Geant4 particle gun and the MaterialScanner stepping action.

        The scan shoots geantinos in random directions, parses the ddsim output,
        and saves THStacks of material depth, radiation length,
        and interaction length vs angle to a ROOT file, PDF, and PNG.
        Due to the random nature of the ddsim particle gun, a fixed number
        of events per bin cannot be guaranteed. It is recommended to set the target
        value for the number of events per bin large enough to not be affected
        by Poissonian fluctuations. Mitigated by the script slightly by using Halton sequence for gun.

        Example:
          g4PolarAngleScan.py \\
              -c myDetector.xml \\
              -o materialScan.root \\
              --angleDef theta --minValue 10 --maxValue 170 -b 2 \\
              --eventsPerBin 1000 \\
              -i Air Vacuum \\
              --colors 'kRed+2' 'cornflowerblue' '#00ff00' \\
              -t 600 -p
        """),
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
    help="name of output root file with extension (used also as prefix for exported canvases)",
    )

parser.add_argument(
    "--angleDef",
    dest="angleDef",
    default="theta",
    type=str,
    choices=["eta", "theta", "cosTheta", "thetaRad"],
    help=textwrap.dedent("""\
        angle definition to use: eta, theta, cosTheta or thetaRad. Default: theta.
        This choice determines the underlying distribution of the ddsim gun:
        uniform in theta for theta/thetaRad, uniform in eta for eta, and uniform in cos(theta) for cosTheta.
        """),
    )

parser.add_argument(
    "--minValue",
    dest="minValue",
    default=10,
    type=float,
    help=textwrap.dedent("""\
        minimum value for eta/theta/cosTheta, float in corresponding units:
        deg for theta, rad for thetaRad, unitless for eta and cosTheta
        (e.g. --angleDef theta --minValue 10 --maxValue 170  for a scan from 10 to 170 degrees)
        """),
    )

parser.add_argument(
    "--maxValue",
    dest="maxValue",
    default=170,
    type=float,
    help=textwrap.dedent("""\
        maximum value for eta/theta/cosTheta, float in corresponding units:
        deg for theta, rad for thetaRad, unitless for eta and cosTheta
        (e.g. --angleDef thetaRad --minValue 0.175 --maxValue 2.967  for a scan from 0.175 to 2.967 radians)
        """),
    )

parser.add_argument(
    "--angleBinning",
    "-b",
    dest="angleBinning",
    default=2,
    type=float,
    help=textwrap.dedent("""\
        eta/theta/cosTheta/thetaRad bin width in corresponding units:
        deg for theta, rad for thetaRad, unitless for eta and cosTheta
        (e.g. --angleDef cosTheta --minValue -0.9 --maxValue 0.9 --b 0.1
        for a scan from -0.9 to 0.9 in cosTheta with bin width of 0.1)
        Adjusted automatically if the range is not divisible by bin width!
        """),
    )

parser.add_argument(
    "--eventsPerBin",
    dest="eventsPerBin",
    default=1000,
    type=int,
    help=textwrap.dedent("""\
        target number of geantinos to average over per angle bin
        (approximate, because we are relying on ddsim gun random distribution in theta)
        """),
    )

parser.add_argument("--seed", "-S", dest="seed", default=None, type=int, help="random seed for ddsim gun (optional)")

parser.add_argument("--timeOut", "-t", dest="timeOut", default=3600, type=int, help="timeout for ddsim runs in seconds")

parser.add_argument(
    "--removeMatsSubstrings",
    "-r",
    dest="removeMatsSubstrings",
    nargs="+",
    default=[],
    help=textwrap.dedent("""\
        Substrings to be removed from materials strings
        (e.g. DCH_ for drift chamber specific materials).
        Applied before --ignoreMats.
        """),
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
    "--noPilot",
    "-P",
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
    help=textwrap.dedent("""\
        List of ROOT colours to use for materials. Accepts ROOT colour names (e.g. 'kRed' 'kBlue+2', case-sensitive),
        ROOT-style integers (e.g. '4' '8' '15' '16' '23' '42'), hex codes in quotes (e.g. '#ff0000' '#3b82f6'),
        or matplotlib names (e.g. 'red' 'steelblue' 'tab:blue'). If fewer colours are provided than
        materials are found, the list will be padded with default ROOT colours.
        """),
    )

parser.add_argument(
    "--saveRawData",
    action="store_true",
    dest="saveRawData",
    default=False,
    help="save the unbinned raw data to a txt file for debugging/inspection",
    )

parser.add_argument(
    "--progressBar",
    "-p",
    action="store_true",
    dest="progressBar",
    default=False,
    help="show a progress bar for the main ddsim run (requires tqdm)",
    )

args = parser.parse_args()

# ---------------------------------------------------------------------------
# valid angle definitions and helper function
# ---------------------------------------------------------------------------

ANGLE_AND_DISTRIBUTION_DEFS = {
    "theta": "uniform",
    "eta": "eta",
    "cosTheta": "cos(theta)",
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
    Ensure no duplicated colors in the final list.
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
print("angle definition:", angleDef)

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

# adjust bin width to fit an integer number of bins
nBins = max(1, round((maxValue - minValue) / binning))
actual_binning = (maxValue - minValue) / nBins

if not math.isclose(actual_binning, binning, abs_tol=1e-6):
    print(f"WARNING: bin width adjusted from {binning} to {actual_binning:.6g} "
          f"to fit exactly {nBins} bins in [{minValue}, {maxValue}]")

bin_edges = [minValue + i * actual_binning for i in range(nBins + 1)]
bin_edges[-1] = maxValue  # avoid floating point rounding issues for the last bin edge


def find_bin(angle_value, min_value, actual_binning, n_bins):
    ib = int((angle_value - min_value) / actual_binning)
    if ib < 0 or ib >= n_bins:
        return None

    return ib

# ---------------------------------------------------------------------------
# functions to run ddsim
# ---------------------------------------------------------------------------


def build_angle_args(angle_def, angle_min, angle_max):
    """
    Convert user input for min and max values of the scan range
    into the correct ddsim gun arguments
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
        str(args.eventsPerBin * nBins),
        "--outputFile",
        "sim_scan_output.root",  # not needed, deleted at the end
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
        "--gun.halton",  # use Halton sequence for better coverage of the angular space
        "True",
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


# ddsim call for the pilot run (no progress bar, no parsing of output)
def run_ddsim(timeout):
    cmd = build_ddsim_cmd(with_stdbuf=True)
    print("Running:", " ".join(cmd))
    try:
        return subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        sys.exit(f"ERROR: ddsim timed out after {timeout} s")


# ddsims call for the main run, with progress bar and parsing of output
def run_ddsim_progress(timeout, n_events):

    cmd = build_ddsim_cmd(with_stdbuf=True)
    print("Running:", " ".join(cmd))

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

    progress_bar = None
    print("  Starting the initialisation and scan...", flush=True)

    bin_data = [{} for _ in range(nBins)]
    bin_counts = [0] * nBins  # count actual events per bin

    # keep track of materials that collapse to the same name when applying the 'removeMatSubtrings' option
    # e.g. G4_Cu and Custom_Cu, if both G4_ and Custom_ are removed
    conflicting_name_dict = defaultdict(set)

    # Save any warnings to a list to be printed out at the end, to avoid messing up the progress bar
    warning_list = []

    raw_file = None
    if args.saveRawData:
        raw_data_path = args.output.replace(".root", "_rawdata.txt")
        raw_file = open(raw_data_path, "w")
        raw_file.write("# angle_value mat_name x0 lambda depth_mm\n")

    for event in parse_materialscanner_output(proc.stdout):
        if args.progressBar:
            if progress_bar is None:
                from tqdm import tqdm
                # start progress bar only once the first event starts,
                # to avoid counting Geant4 initialisation time in progress bar
                print("  Initialisation complete, scanning...", flush=True)
                progress_bar = tqdm(total=n_events, unit="evt", desc="  ddsim")
            progress_bar.update(1)

        if event.direction is None:
            continue

        dx, dy, dz = event.direction
        angle_value = direction_to_angleDef(dx, dy, dz, angleDef)
        ib = find_bin(angle_value, minValue, actual_binning, nBins)
        if ib is None:
            warning_list.append(f"WARNING: angle value {angle_value} out of range, skipping")
            continue

        # accumulate thicknesses per material for this event
        mat_dict = {}
        for step in event.steps:
            # Remove any substrings from the material name as requested by the user
            original_name = step.name
            mat_name = original_name
            for substring in args.removeMatsSubstrings:
                mat_name = mat_name.replace(substring, "")
            # Save the original name for bookkeeping of conflicting names
            conflicting_name_dict[mat_name].add(original_name)

            if mat_name not in args.ignoreMats:
                if mat_name not in mat_dict:
                    mat_dict[mat_name] = [0, 0, 0]
                # compute the thicknesses in units of radiation lengths and interaction lengths
                # mat_dict accumulates for one event the information of each step
                mat_dict[mat_name][0] += step.thickness_cm / step.x0_cm
                mat_dict[mat_name][1] += step.thickness_cm / step.li_cm
                mat_dict[mat_name][2] += step.thickness_cm * 10.0

        bin_counts[ib] += 1
        for mat, sums in mat_dict.items():
            if mat in args.ignoreMats:
                continue

            # bin_data[ib][mat] accumulates for one bin the sums of x0, li, and depth_mm across events
            if mat not in bin_data[ib]:
                bin_data[ib][mat] = [0.0, 0.0, 0.0]
            bin_data[ib][mat][0] += sums[0]
            bin_data[ib][mat][1] += sums[1]
            bin_data[ib][mat][2] += sums[2]

            # if requested, write out the unbinned raw data to a text file for debugging/inspection
            if raw_file is not None:
                raw_file.write(f"{angle_value:.6f} {mat} {sums[0]:.6f} {sums[1]:.6f} {sums[2]:.6f}\n")

    if raw_file is not None:
        raw_file.close()

    if progress_bar is not None:
        progress_bar.close()

    # print accumulated warnings
    print(*warning_list, sep="\n", flush=True)

    proc.wait()

    if proc.returncode == -9:
        sys.exit("ERROR: ddsim was killed, either due to timeout or other means.")
    if proc.returncode != 0:
        print(f"ERROR: ddsim exited with return code {proc.returncode}")
        print("Re-run the following command to investigate:")
        print(" ".join(build_ddsim_cmd(with_stdbuf=False)))
        sys.exit(1)

    return bin_data, bin_counts, conflicting_name_dict


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
bin_data, bin_counts, conflicting_name_dict = run_ddsim_progress(args.timeOut, nBins * args.eventsPerBin)

# print out any material name conflicts
for mat_name, original_names in conflicting_name_dict.items():
    if len(original_names) > 1:
        print(
            f'WARNING: multiple material names collapse to "{mat_name}" '
            f'after applying --removeMatsSubstrings: {original_names}'
            )

n_received = sum(bin_counts)
n_expected = nBins * args.eventsPerBin
if n_received != n_expected:
    print(f"WARNING: event count mismatch (Received {n_received} vs expected {n_expected}). Results may be incomplete.")


# get the phi-averaged value
for ib in range(nBins):
    n_events_in_bin = bin_counts[ib]
    if n_events_in_bin == 0:
        print(f"WARNING: no events found for bin {ib} (angle range {bin_edges[ib]} to {bin_edges[ib + 1]}), skipping")
        continue
    for mat in bin_data[ib]:
        bin_data[ib][mat][0] /= n_events_in_bin
        bin_data[ib][mat][1] /= n_events_in_bin
        bin_data[ib][mat][2] /= n_events_in_bin

seen = set()
all_mats = []
for ib in range(nBins):
    for mat in bin_data[ib]:
        if mat not in seen:
            seen.add(mat)
            all_mats.append(mat)
print(f"Materials found: {all_mats}")

# ---------------------------------------------------------------------------
# ROOT histograms
# ---------------------------------------------------------------------------

fout = ROOT.TFile(args.output, "recreate")

bin_edges_arr = array.array("d", bin_edges)
nbins_root = len(bin_edges) - 1

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
    total = ROOT.TH1D(f"h_{qty_name}_TOTAL", f"Total;{AXIS_LABEL};{y_title}", nbins_root, bin_edges_arr)
    total.SetLineWidth(2)
    total.SetLineColor(ROOT.kBlack)

    mat_hists = {}
    for ci, mat in enumerate(all_mats):
        h = ROOT.TH1D(f"h_{qty_name}_{mat}", f"{mat};{AXIS_LABEL};{y_title}", nbins_root, bin_edges_arr)
        col = COLORS[ci % len(COLORS)]
        h.SetFillColor(col)
        h.SetLineColor(ROOT.kBlack)
        h.SetLineWidth(1)
        mat_hists[mat] = h

    for ib in range(nBins):
        root_bin = ib + 1  # ROOT 1-indexed
        for mat, vals in bin_data[ib].items():
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

# ---------------------------------------------------------------------------
# Clean up
# ---------------------------------------------------------------------------

# remove ddsim output file
if os.path.exists("sim_scan_output.root"):
    os.remove("sim_scan_output.root")
