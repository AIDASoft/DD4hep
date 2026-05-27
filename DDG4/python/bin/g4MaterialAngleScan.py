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

"""

import os
import sys
import argparse
import subprocess
import math
import random
import array
import ROOT

parser = argparse.ArgumentParser(description="Material budget scan in theta/eta/cosTheta")

parser.add_argument("--compactFile", 
                    "-c", 
                    dest="compact", 
                    type=str, 
                    help="name of file to read")

parser.add_argument("--steeringFile",
                    "-s",
                    dest="steering",
                    default=None,
                    type=str,
                    help="ddsim steering file (optional)")

parser.add_argument("--outputFile",
                    "-o",
                    dest="output",
                    default='materialScan.root',
                    type=str,
                    help="name of output root file to write")

parser.add_argument("--angleDef",
                    dest="angleDef",
                    default="eta",
                    type=str,
                    help="angle definition to use: eta, theta, cosTheta or thetaRad, default: eta")

parser.add_argument("--angleMin", 
                    dest="angleMin", 
                    default=-6, 
                    type=float, 
                    help="minimum eta/theta/cosTheta")

parser.add_argument("--angleMax", 
                    dest="angleMax", 
                    default=6, 
                    type=float, 
                    help="maximum eta/theta/cosTheta")

parser.add_argument("--angleBinning",
                    "-b",
                    dest="angleBinning",
                    default=0.05,
                    type=float,
                    help="eta/theta/cosTheta/thetaRad bin width")

parser.add_argument("--nPhi",
                    dest="nPhi",
                    default=100,
                    type=int,
                    help="number of phi values to scan for each eta/theta/cosTheta/thetaRad bin")

parser.add_argument("--timeOut",
                    "-t",
                    dest="timeOut",
                    default=600,
                    type=int,
                    help="timeout for ddsim runs in seconds")

# parser.add_argument("--x0max", 
#                     "-x", 
#                     dest="x0max", 
#                     default=0.0, 
#                     type=float, 
#                     help="Max of x0")

parser.add_argument("--removeMatsSubstrings",
                    "-r",                    
                    dest="removeMatsSubstrings",
                    nargs="+",
                    default=[],
                    help="Substrings to be removed from materials strings (e.g. '66D' for reduced density materials). Applied before --ignoreMats.")

parser.add_argument("--ignoreMats",
                    "-i",
                    dest="ignoreMats",
                    nargs="+",
                    default=[],
                    help="List of materials that should be ignored. Applied after --removeMatsSubstrings.")

parser.add_argument("-P", 
                    "--noPilot",
                    action="store_true", 
                    dest="noPilot", 
                    default=False,
                    help="don't run the pilot job (e.g. if you're sure the geometry is good)")

parser.add_argument("--colors",
                    dest="colors",
                    nargs="+",
                    default=None,
                    help="List of ROOT colours to use for materials, accepts ROOT colour names (e.g. kRed kBlue+2), ROOT-style integers (e.g. 4 8 15 16 23 42), hex codes in quotes (e.g. '#ff0000' '#3b82f6') or matplotlib names (e.g. red steelblue tab:blue). If fewer colours are provided than materials are found, the list will be padded with default ROOT colours that don't duplicate any user-specified ones.")

args = parser.parse_args()

# ---------------------------------------------------------------------------
# valid angle definitions and helper functions
# ---------------------------------------------------------------------------

ANGLE_DEFS = ['eta', 'theta', 'cosTheta', 'thetaRad']

def eta_to_theta_deg(eta):
    return math.degrees(2.0 * math.atan(math.exp(-eta)))
 
def costheta_to_theta_deg(costh):
    return math.degrees(math.acos(costh))

def theta_rad_to_theta_deg(theta_rad):
    return math.degrees(theta_rad)

def native_to_theta_deg(val):
    if angleDef == 'theta':
        return val
    elif angleDef == 'eta':
        return eta_to_theta_deg(val)
    elif angleDef == 'cosTheta':
        return costheta_to_theta_deg(val)
    elif angleDef == 'thetaRad':
        return theta_rad_to_theta_deg(val)
    
def direction_from_theta_phi(theta_deg, phi_deg):
    """unit direction vector from polar and azimuthal angles"""
    theta = math.radians(theta_deg)
    phi   = math.radians(phi_deg)
    x = math.sin(theta) * math.cos(phi)
    y = math.sin(theta) * math.sin(phi)
    z = math.cos(theta)
    return x, y, z


# ---------------------------------------------------------------------------
# default colours and colour list builder for ROOT histograms
# ---------------------------------------------------------------------------

def resolve_color(c):
    """
    Accept:
      - ROOT names:      kRed, kBlue+2, kAzure-1
      - plain integers:  2, 4, 628
      - hex codes:       #ff0000, #3b82f6
      - matplotlib names: red, steelblue, tab:blue, ...
    Returns a ROOT color index (int).
    """
    c = str(c).strip()

    # hex code
    if c.startswith('#'):
        return ROOT.TColor.GetColor(c)

    # plain integer
    try:
        return int(c)
    except ValueError:
        pass

    # ROOT name with optional offset: kGreen+2, kAzure-1
    for op in ('+', '-'):
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
        hex_col = mcolors.to_hex(c)   # handles 'red', 'steelblue', 'tab:blue', etc.
        return ROOT.TColor.GetColor(hex_col)
    except (ImportError, ValueError):
        pass

    print(f'WARNING: could not resolve color "{c}", falling back to kBlack')
    return ROOT.kBlack


DEFAULT_COLORS = [
    ROOT.kRed,      ROOT.kBlue,     ROOT.kGreen+2,  ROOT.kOrange+1,
    ROOT.kMagenta,  ROOT.kCyan+1,   ROOT.kYellow+2, ROOT.kViolet+1,
    ROOT.kTeal+2,   ROOT.kPink+3,   ROOT.kAzure+2,  ROOT.kSpring+5,
]

def build_color_list(args_colors):
    """
    Start from user-supplied colors (if any), then pad with defaults
    that don't duplicate anything already in the list.
    """
    if args_colors is None:
        return list(DEFAULT_COLORS)
    resolved = [resolve_color(c) for c in args_colors]
    resolved_set = set(resolved)
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
    print('ERROR: cannot find requested input geometry file', args.compact, file=sys.stderr)
    exit(1)
print('geometry file:', args.compact)

if args.steering is not None and not os.path.isfile(args.steering):
    print('ERROR: cannot find requested ddsim steering file', args.steering, file=sys.stderr)
    exit(1)
print('ddsim steering file:', args.steering)

angleDef = str(args.angleDef)
if angleDef not in ANGLE_DEFS:
    print('ERROR: unknown angle definition', angleDef, '. Choose from ', ANGLE_DEFS, '.', file=sys.stderr)
    exit(1)
print(angleDef)

angleMin = float(args.angleMin)
angleMax = float(args.angleMax)
binning  = float(args.angleBinning)

if angleDef == 'theta':
    if angleMin < 0:
        print('WARNING: lower theta bound is negative, setting to 0')
        angleMin = 0
    if angleMax > 180:
        print('WARNING: upper theta bound is above 180, setting to 180')
        angleMax = 180

elif angleDef == 'thetaRad':
    if angleMin < 0:
        print('WARNING: lower theta bound is negative, setting to 0')
        angleMin = 0
    if angleMax > 3.14159:
        print('WARNING: upper theta bound is above pi, setting to pi')
        angleMax = 3.14159

elif angleDef == 'cosTheta':
    if angleMin < -1:
        print('WARNING: lower cosTheta bound is below -1, setting to -1')
        angleMin = -1
    if angleMax > 1:
        print('WARNING: upper cosTheta bound is above 1, setting to 1')
        angleMax = 1


# ---------------------------------------------------------------------------
# create the bins
# ---------------------------------------------------------------------------

 
# build uniform edges in native angle variable
edges = []
v = angleMin
while v <= angleMax + 1e-9:
    edges.append(round(v, 12))
    v += binning
if edges[-1] < angleMax - 1e-12:
    edges.append(angleMax)
nBins = len(edges) - 1
 
if nBins < 1:
    print('ERROR: bin width larger than the requested range', file=sys.stderr)
    sys.exit(1)


bin_centres_native = [(edges[i] + edges[i+1]) / 2.0 for i in range(nBins)]

# for Geant4 particle gun, we need the angleDef as theta value, so convert
bin_theta_centres  = [native_to_theta_deg(c) for c in bin_centres_native]

# ---------------------------------------------------------------------------
# write Geant4 macro file 
# ---------------------------------------------------------------------------

macName   = '_thetaScan_'       + args.output + '.mac'
pilotName = '_thetaScan_pilot_' + args.output + '.mac'
 
def write_mac(filename, theta_centres):
    with open(filename, 'w') as f:
        f.write('/gun/particle geantino\n')
        f.write('/gun/energy 20 GeV\n')
        f.write('/gun/number 1\n')
        f.write('/gun/position 0 0 0 mm\n')
        for theta_c in theta_centres:
            phi_list = [random.uniform(0.0, 360.0) for _ in range(args.nPhi)]
            for phi in phi_list:
                x, y, z = direction_from_theta_phi(theta_c, phi)
                f.write(f'/gun/direction {x:.8f} {y:.8f} {z:.8f}\n')
                f.write('/run/beamOn\n')
        f.write('exit\n')

# full macro: all bins x all phi
write_mac(macName, bin_theta_centres)

# pilot macro: first bin, first phi only
write_mac(pilotName, [bin_theta_centres[0]])


# ---------------------------------------------------------------------------
# ddsim runner
# ---------------------------------------------------------------------------
 
def run_ddsim(mac_file, timeout):
    cmd = ['ddsim',
           '--compactFile', args.compact,
           '--runType',     'run',
           '--enableG4Gun',
           '--action.step', 'Geant4MaterialScanner/MaterialScan',
           '-M',            mac_file]
    if args.steering is not None:
        cmd += ['--steeringFile', args.steering]
    print('Running:', ' '.join(cmd))
    try:
        return subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        sys.exit(f'ERROR: ddsim timed out after {timeout} s')
        
def run_ddsim_progress(mac_file, timeout, n_events):
    import threading
    from tqdm import tqdm

    cmd = ['stdbuf', '-oL',
           'ddsim',
           '--compactFile', args.compact,
           '--runType',     'run',
           '--enableG4Gun',
           '--action.step', 'Geant4MaterialScanner/MaterialScan',
           '-M',            mac_file]
    if args.steering is not None:
        cmd += ['--steeringFile', args.steering]
    print('Running:', ' '.join(cmd))

    lines = []

    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT,
                            text=True)

    def _watchdog():
        try:
            proc.wait(timeout=timeout)
        except subprocess.TimeoutExpired:
            print(f'\nERROR: ddsim timed out after {timeout} s, killing process.')
            proc.kill()

    watchdog = threading.Thread(target=_watchdog, daemon=True)
    watchdog.start()

    pbar = None
    print('  Waiting for Geant4 initialisation...', flush=True)

    for line in proc.stdout:
        lines.append(line)
        if 'Finished run' in line:
            if pbar is None:
                # first event done — initialisation is over, start the bar
                print('  Initialisation complete, scanning...', flush=True)
                pbar = tqdm(total=n_events, unit='evt', desc='  ddsim')
                pbar.update(1)   # count the event we just saw
            else:
                pbar.update(1)

    if pbar is not None:
        pbar.close()

    proc.wait()

    if proc.returncode == -9:
        sys.exit('ERROR: ddsim was killed due to timeout.')
    if proc.returncode != 0:
        print(f'WARNING: ddsim exited with return code {proc.returncode}')

    class Result:
        stdout = ''.join(lines)
    return Result()

# ---------------------------------------------------------------------------
# pilot run
# ---------------------------------------------------------------------------
 
if not args.noPilot:
    print('\nRunning pilot job...')
    pr = run_ddsim(pilotName, args.timeOut)
    n_scans    = sum(1 for l in pr.stdout.splitlines() if 'Material scan between' in l)
    n_finished = sum(1 for l in pr.stdout.splitlines() if 'Finished run'          in l)
    if n_scans < 1 or n_finished < 1:
        print('ERROR: pilot job did not finish successfully.')
        print('Re-run the following command to investigate:')
        print('  ddsim --compactFile', args.compact,
              '--runType run --enableG4Gun',
              '--action.step Geant4MaterialScanner/MaterialScan',
              ['--steeringFile ' + args.steering if args.steering is not None else ''],
              '-M', pilotName)
        sys.exit(1)
    print('Pilot job OK.\n')

# ---------------------------------------------------------------------------
# main ddsim run
# ---------------------------------------------------------------------------
 
print(f'Running main job ({nBins * args.nPhi} events)...')
result = run_ddsim_progress(macName, args.timeOut, nBins * args.nPhi)
 

# ---------------------------------------------------------------------------
# parse ddsim output
#
# The MaterialScanner prints one block per event. Example block:
#
#  +--------------------------------------------------------------------------------------------------------------------------------------------------
#  + Material scan between: (x0, y0, z0)[cm] and (x1, y1, z1)[cm]  TrackID:1: 
#  +--------------------------------------------------------------------------------------------------------------------------------------------------
#  |     \   Material           Atomic                 Radiation   Interaction               Path   Integrated  Integrated    Material
#  | Num. \  Name          Number/Z   Mass/A  Density    Length       Length    Thickness   Length      X0        Lambda      Endpoint  
#  | Layer \                        [g/mole]  [g/cm3]     [cm]        [cm]          [cm]      [cm]     [cm]        [cm]     (     cm,     cm,     cm)
#  +--------------------------------------------------------------------------------------------------------------------------------------------------
#  |     1 Air                    7   14.784   0.0012  30528.8402   71282.7920    183.325   183.32    0.006005    0.002572  ( -34.68,  -4.58, 179.96)
#  ...
# GenerationInit   WARN  +++ Finished run 1 after ...
#
#
# Columns after splitting on whitespace (| is parts[0]):
#   parts[1]  = step index       (int)
#   parts[2]  = material name    (str)
#   parts[3]  = Z
#   parts[4]  = A
#   parts[5]  = density [g/cm3]
#   parts[6]  = X0      [cm]
#   parts[7]  = LambdaI [cm]
#   parts[8]  = thickness [cm]
#   parts[9]  = t/X0
#   parts[10] = (ex,ey,ez) — end position, embedded in the line
# ---------------------------------------------------------------------------

raw_data    = []   # list of events; each event is a list of step tuples
current_evt = []
in_scan     = False

print('\nParsing ddsim output...')

for line in result.stdout.splitlines():
    if 'Material scan between' in line:
        current_evt = []
        in_scan     = True
 
    elif 'Finished run' in line:
        raw_data.append(current_evt)
        in_scan = False
 
    elif in_scan and '(' in line and len(line.split('(')[0].split()) == 12 and line.split()[0] == '|':  # this line contains material information
        parts = line.split()
        if len(parts) < 10:
            continue
        try:
            int(parts[1])          # first token after '|' must be a step index
        except ValueError:
            continue               # skip header/separator lines
        try:
            x0_cm    = float(parts[6])
            li_cm    = float(parts[7])
            thick_cm = float(parts[8])
        except (ValueError, IndexError):
            continue
        if x0_cm <= 0.0 or li_cm <= 0.0:
            continue
        mat_name   = parts[2]
        for substring in args.removeMatsSubstrings:
            mat_name = mat_name.replace(substring, '')
        t_over_x0  = thick_cm / x0_cm
        t_over_li  = thick_cm / li_cm
        thick_mm   = thick_cm * 10.0
        current_evt.append((mat_name, t_over_x0, t_over_li, thick_mm))
 
n_received = len(raw_data)
n_expected = nBins * args.nPhi
print(f'Parsed {n_received} scan events (expected {n_expected})')
if n_received != n_expected:
    print(f'WARNING: event count mismatch ({n_received} vs {n_expected}). '
          'Results may be incomplete.')
    
    

# ---------------------------------------------------------------------------
# accumulate per-bin, per-material sums, then average over phi
#
# bin_data[ib][mat_name] = [sum_x0, sum_li, sum_len_mm]
# Sums are over all nPhi shots for that bin; divided by nPhi at the end.
# ---------------------------------------------------------------------------
 
bin_data = [{} for _ in range(nBins)]
 
for ev_idx, steps in enumerate(raw_data):
    ib = ev_idx // args.nPhi # get the angle bin index from the total event index and nPhi
    if ib >= nBins:
        break
    for (mat, t_x0, t_li, t_mm) in steps:
        # Ignore certain materials if specified
        if mat in args.ignoreMats:
            continue
        if mat not in bin_data[ib]:
            bin_data[ib][mat] = [0.0, 0.0, 0.0]
        bin_data[ib][mat][0] += t_x0
        bin_data[ib][mat][1] += t_li
        bin_data[ib][mat][2] += t_mm
 
# divide by nPhi to get the phi-averaged value
for ib in range(nBins):
    for mat in bin_data[ib]:
        bin_data[ib][mat][0] /= args.nPhi
        bin_data[ib][mat][1] /= args.nPhi
        bin_data[ib][mat][2] /= args.nPhi
 
all_mats = sorted({mat for ib in range(nBins) for mat in bin_data[ib]})
print(f'Materials found: {all_mats}')

# ---------------------------------------------------------------------------
# ROOT output
# ---------------------------------------------------------------------------
 
fout = ROOT.TFile(args.output, 'recreate')
 
edges_arr  = array.array('d', edges)
nbins_root = len(edges) - 1

AXIS_LABELS = {
    'theta':    '#theta [deg]',
    'eta':      '#eta',
    'cosTheta': 'cos(#theta)',
}
AXIS_LABEL = AXIS_LABELS[angleDef]

 
def make_stack_and_total(qty_idx, qty_name, y_title):
    """
    Build a THStack of per-material TH1D + a black 'Total' TH1D.
    qty_idx: 0 = x0, 1 = lambda, 2 = path length
    """
    stack = ROOT.THStack(f'hs_{qty_name}', f';{AXIS_LABEL};{y_title}')
    total = ROOT.TH1D(f'h_{qty_name}_TOTAL',
                      f'Total;{AXIS_LABEL};{y_title}',
                      nbins_root, edges_arr)
    total.SetLineWidth(2)
    total.SetLineColor(ROOT.kBlack)
 
    mat_hists = {}
    for ci, mat in enumerate(all_mats):
        h = ROOT.TH1D(f'h_{qty_name}_{mat}',
                      f'{mat};{AXIS_LABEL};{y_title}',
                      nbins_root, edges_arr)
        col = COLORS[ci % len(COLORS)]
        h.SetFillColor(col)
        h.SetLineColor(ROOT.kBlack)
        h.SetLineWidth(1)
        mat_hists[mat] = h
 
    for ib in range(nBins):
        root_bin = ib + 1   # ROOT 1-indexed
        for mat, vals in bin_data[ib].items():
            val = vals[qty_idx]
            mat_hists[mat].AddBinContent(root_bin, val)
            total.AddBinContent(root_bin, val)
 
    for mat in all_mats:
        stack.Add(mat_hists[mat])
        # mat_hists[mat].Write()
 
    stack.Write()
    # total.Write()
    return stack, total, mat_hists
 
stack_x0,  total_x0,  _ = make_stack_and_total(0, 'x0',  't/X_{0}')
stack_li,  total_li,  _ = make_stack_and_total(1, 'lambda', 'Number of #lambda_{I}')
stack_len, total_len, _ = make_stack_and_total(2, 'depth', 'Material depth [mm]')
 
# ---------------------------------------------------------------------------
# quick-look canvases
# ---------------------------------------------------------------------------
 
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
 
def draw_canvas(stack, total, canvas_name, title):
    c = ROOT.TCanvas(canvas_name+"_canvas", title, 900, 600)
    c.SetLeftMargin(0.12)
    stack.Draw('hist')
    # stack.GetXaxis().SetTitle(AXIS_LABEL)
    # total.Draw('hist same')
    leg = ROOT.TLegend(0.65, 0.58, 0.92, 0.90)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    if stack.GetHists():
        for mat in reversed(all_mats): # reverse to match stack order
            h = stack.GetHists().FindObject(f'h_{canvas_name}_{mat}')
            if h:
                leg.AddEntry(h, mat, 'f')
    # leg.AddEntry(total, 'Total', 'l')
    leg.Draw()

    export_name = str(args.output)[:-5] + "_" + canvas_name # remote .root extension
    c.Print(export_name + ".pdf")
    c.Print(export_name + ".png")
    c.Write()
 
draw_canvas(stack_x0,  total_x0,  'x0',  'Radiation lengths')
draw_canvas(stack_li,  total_li,  'lambda',  'Interaction lengths')
draw_canvas(stack_len, total_len, 'depth', 'Material depth [mm]')
 
fout.Write()
fout.Close()
print(f'\nDone. Results written to {args.output}')
 
# clean up temp macro files
for f in [macName, pilotName]:
    try:
        os.remove(f)
    except OSError:
        pass




