#!/usr/bin/env python3
"""
Check the merging of the calorimeter hit contributions in the EDM4hep output (MergeCaloContributions).

The files must contain the same events, simulated with the same seed, without and with the merging (and
optionally with the merging in time bins, CaloContributionTimeWindow). For every SimCalorimeterHit the hits
(cellID and energy) must be identical and the energy deposited by each MCParticle must be the same. With the
merging each MCParticle must have only one contribution per hit, with the merging in time bins only one
contribution per hit and time bin [k*window, (k+1)*window).
"""

import argparse
import math
import sys

from podio.reading import get_reader


def particle_energies(hit, window=None):
  """Energy deposited in a hit by each MCParticle, and the number of contributions of each MCParticle (and time
  bin, if a time window is given)"""
  energies, counts = {}, {}
  for contribution in hit.getContributions():
    index = contribution.getParticle().getObjectID().index
    energies[index] = energies.get(index, 0.) + contribution.getEnergy()
    key = (index, math.floor(contribution.getTime() / window)) if window else index
    counts[key] = counts.get(key, 0) + 1
  return energies, counts


def compare(file_default, file_merged, tolerance, window=None):
  """Compare the file with merged contributions to the default one, return the number of errors"""
  events_default = get_reader(file_default).get("events")
  events_merged = get_reader(file_merged).get("events")
  if len(events_default) != len(events_merged):
    print(f"Different number of events: {len(events_default)} vs {len(events_merged)}")
    return 1

  errors = 0
  n_hits = n_contributions_default = n_contributions_merged = 0
  for frame_default, frame_merged in zip(events_default, events_merged):
    for name in frame_default.getAvailableCollections():
      collection_default = frame_default.get(name)
      if collection_default.getTypeName() != "edm4hep::SimCalorimeterHitCollection":
        continue
      collection_merged = frame_merged.get(name)
      if len(collection_default) != len(collection_merged):
        print(f"{name}: different number of hits: {len(collection_default)} vs {len(collection_merged)}")
        errors += 1
        continue
      for hit_default, hit_merged in zip(collection_default, collection_merged):
        n_hits += 1
        if hit_default.getCellID() != hit_merged.getCellID() or hit_default.getEnergy() != hit_merged.getEnergy():
          print(f"{name}: different hits: cellID {hit_default.getCellID()} vs {hit_merged.getCellID()}, "
                f"energy {hit_default.getEnergy()} vs {hit_merged.getEnergy()}")
          errors += 1
          continue
        energies_default, counts_default = particle_energies(hit_default)
        energies_merged, counts_merged = particle_energies(hit_merged, window)
        n_contributions_default += sum(counts_default.values())
        n_contributions_merged += sum(counts_merged.values())
        if set(energies_default) != set(energies_merged):
          print(f"{name}: hit {hit_default.getCellID()} has contributions from different MCParticles")
          errors += 1
          continue
        for index, energy in energies_default.items():
          if abs(energies_merged[index] - energy) > tolerance * max(abs(energy), 1e-12):
            print(f"{name}: hit {hit_default.getCellID()}, MCParticle {index}: "
                  f"energy {energy} vs {energies_merged[index]}")
            errors += 1
        if any(count != 1 for count in counts_merged.values()):
          what = "MCParticle and time bin" if window else "MCParticle"
          print(f"{name}: hit {hit_default.getCellID()} has more than one merged contribution per {what}")
          errors += 1

  print(f"{file_merged}: {n_hits} calorimeter hits, {n_contributions_default} contributions without merging, "
        f"{n_contributions_merged} with merging" + (f" in time bins of {window} ns" if window else ""))
  if n_hits == 0 or n_contributions_merged >= n_contributions_default:
    print("No calorimeter hits, or the merging did not reduce the number of contributions")
    errors += 1
  return errors


def main():
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument("default", help="output file without merging of the contributions")
  parser.add_argument("merged", help="output file with merging of the contributions")
  parser.add_argument("--binned", help="output file with merging of the contributions in time bins")
  parser.add_argument("--time-window", type=float, default=1.0, help="width of the time bins of --binned (ns)")
  parser.add_argument("--tolerance", type=float, default=1e-5,
                      help="relative tolerance on the energy of each MCParticle in each hit")
  args = parser.parse_args()

  errors = compare(args.default, args.merged, args.tolerance)
  if args.binned:
    errors += compare(args.default, args.binned, args.tolerance, args.time_window)
  if errors:
    print(f"TEST_FAILED: {errors} errors")
    return 1
  print("TEST_PASSED")
  return 0


if __name__ == "__main__":
  sys.exit(main())
