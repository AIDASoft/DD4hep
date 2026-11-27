#!/usr/bin/env python3
"""
Check that the run number of an EDM4hep simulation output is the expected one in all events.
"""

import argparse
import sys

from podio.reading import get_reader


def main():
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument("output", help="EDM4hep output file of the simulation")
  parser.add_argument("run_number", type=int, help="expected run number")
  args = parser.parse_args()

  n_events = 0
  for event in get_reader(args.output).get("events"):
    run_number = event.get("EventHeader")[0].getRunNumber()
    if run_number != args.run_number:
      print(f"ERROR: event {n_events}: run number {run_number}, expected {args.run_number}")
      return 1
    n_events += 1

  if n_events == 0:
    print("ERROR: no events found in the output")
    return 1

  print(f"Run number is {args.run_number} in all {n_events} events")
  return 0


if __name__ == "__main__":
  sys.exit(main())
