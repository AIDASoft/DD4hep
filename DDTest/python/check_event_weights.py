#!/usr/bin/env python3
"""
Check that the event weights of an EDM4hep input file reach the simulation output.

Compares EventHeader.weight and EventHeader.weights event by event.
"""

import argparse
import sys

from podio.reading import get_reader


def main():
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument("input", help="EDM4hep input file of the simulation")
  parser.add_argument("output", help="EDM4hep output file of the simulation")
  args = parser.parse_args()

  input_events = get_reader(args.input).get("events")
  output_events = get_reader(args.output).get("events")

  n_events = 0
  for in_event, out_event in zip(input_events, output_events):
    in_header = in_event.get("EventHeader")[0]
    out_header = out_event.get("EventHeader")[0]
    if out_header.getWeight() != in_header.getWeight():
      print(f"ERROR: event {n_events}: weight {out_header.getWeight()}, expected {in_header.getWeight()}")
      return 1
    if list(out_header.getWeights()) != list(in_header.getWeights()):
      print(f"ERROR: event {n_events}: weights {list(out_header.getWeights())}, "
            f"expected {list(in_header.getWeights())}")
      return 1
    n_events += 1

  if n_events == 0:
    print("ERROR: no events found in the output")
    return 1

  print(f"Event weights of {n_events} events agree")
  return 0


if __name__ == "__main__":
  sys.exit(main())
