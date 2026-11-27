#!/usr/bin/env python3
"""
Check that the event weight of an EDM4hep simulation output is 1 in all events.

Meant for outputs of inputs that do not provide event weights, e.g. the particle gun.
"""

import argparse
import sys

from podio.reading import get_reader


def main():
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument("output", help="EDM4hep output file of the simulation")
  args = parser.parse_args()

  n_events = 0
  for event in get_reader(args.output).get("events"):
    weight = event.get("EventHeader")[0].getWeight()
    if weight != 1.0:
      print(f"ERROR: event {n_events}: weight {weight}, expected 1.0")
      return 1
    n_events += 1

  if n_events == 0:
    print("ERROR: no events found in the output")
    return 1

  print(f"Event weight is 1 in all {n_events} events")
  return 0


if __name__ == "__main__":
  sys.exit(main())
