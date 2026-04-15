#!/usr/bin/env python3
"""
Compare two DD4hep simulation output files for equivalence.

This script validates that single-threaded and multi-threaded simulations
produce identical physics results by comparing:
- Event counts
- Hit collection sizes
- Hit positions and energies
- MCParticle properties
"""

import argparse
import sys
def _coord(v, attr):
    """Get a coordinate from a vector, handling both attributes and callables."""
    val = getattr(v, attr)
    return val() if callable(val) else val


def compare_vectors(v1, v2, tolerance, description=""):
    """Compare two 3-vectors component by component."""
    # Try lowercase attributes first (plain structs), then uppercase methods (ROOT math types)
    for x_attr, y_attr, z_attr in (('x', 'y', 'z'), ('X', 'Y', 'Z')):
        if hasattr(v1, x_attr) and hasattr(v2, x_attr):
            try:
                x1, x2 = _coord(v1, x_attr), _coord(v2, x_attr)
                y1, y2 = _coord(v1, y_attr), _coord(v2, y_attr)
                z1, z2 = _coord(v1, z_attr), _coord(v2, z_attr)
                if abs(x1 - x2) > tolerance or abs(y1 - y2) > tolerance or abs(z1 - z2) > tolerance:
                    print(f"  {description} mismatch: ({x1}, {y1}, {z1}) vs ({x2}, {y2}, {z2})")
                    return False
                return True
            except Exception:
                continue
    return True


def compare_hit_collections(file1, file2, tolerance=1e-9, tree_name='EVENT'):
    """Compare hit collections between two ROOT files."""
    try:
        import ROOT
    except ImportError:
        print("ERROR: ROOT/PyROOT not available")
        return False

    if ROOT.gSystem.Load("libDDG4IO") < 0:
        raise RuntimeError("Failed to load libDDG4IO. Ensure DD4hep is installed and LD_LIBRARY_PATH is set.")

    # Open files
    f1 = ROOT.TFile.Open(file1)
    f2 = ROOT.TFile.Open(file2)

    if not f1 or f1.IsZombie():
        print(f"ERROR: Cannot open {file1}")
        return False
    if not f2 or f2.IsZombie():
        print(f"ERROR: Cannot open {file2}")
        return False

    # Get trees - try common tree names
    t1 = f1.Get(tree_name)
    if not t1:
        # Try common alternative names
        for name in ['events', 'EVENT', 'Events']:
            t1 = f1.Get(name)
            if t1:
                tree_name = name
                break
        if not t1:
            # Try to find any TTree
            for key in f1.GetListOfKeys():
                obj = key.ReadObj()
                if obj.InheritsFrom("TTree") and 'event' in key.GetName().lower():
                    t1 = obj
                    tree_name = key.GetName()
                    break

    t2 = f2.Get(tree_name)

    if not t1:
        print(f"ERROR: Cannot find tree in {file1}")
        return False
    if not t2:
        print(f"ERROR: Cannot find tree {tree_name} in {file2}")
        return False

    n_entries = t1.GetEntries()
    if n_entries != t2.GetEntries():
        print(f"ERROR: Different number of entries: {n_entries} vs {t2.GetEntries()}")
        return False

    print(f"Comparing {file1} vs {file2}")
    print(f"Tree: {tree_name}, Entries: {n_entries}")

    # Get list of branches (collections)
    branches1 = set(b.GetName() for b in t1.GetListOfBranches())
    branches2 = set(b.GetName() for b in t2.GetListOfBranches())

    if branches1 != branches2:
        print("WARNING: Different branches")
        print(f"  Only in file1: {branches1 - branches2}")
        print(f"  Only in file2: {branches2 - branches1}")
        # Continue with common branches
        branches = branches1 & branches2
    else:
        branches = branches1

    # Build event ID index for file2 to handle out-of-order events
    # This is critical for MT comparison where events may be written in different orders
    print("Building event ID index for file2...")
    event_id_map = {}
    for i in range(n_entries):
        t2.GetEntry(i)
        # Try to get EventHeader collection
        try:
            if hasattr(t2, 'EventHeader') and hasattr(t2.EventHeader, 'size'):
                if t2.EventHeader.size() > 0:
                    evt_hdr = t2.EventHeader[0]
                    # Try different methods to get event number
                    event_id = None
                    if hasattr(evt_hdr, 'getEventNumber'):
                        event_id = evt_hdr.getEventNumber()
                    elif hasattr(evt_hdr, 'eventNumber'):
                        event_id = evt_hdr.eventNumber
                    if event_id is not None:
                        event_id_map[event_id] = i
        except Exception as e:
            print(f"WARNING: Failed to get event ID from entry {i}: {e}")
            pass

    # If we couldn't build event ID map, fall back to sequential comparison
    use_event_id_matching = len(event_id_map) == n_entries
    if use_event_id_matching:
        print(f"Using event ID matching ({len(event_id_map)} events indexed)")
    else:
        print("WARNING: Could not build event ID index, using sequential comparison")
        print("This may fail if events are in different orders!")

    mismatches = 0
    total_comparisons = 0
    total_elements = 0

    # Track which collections we've seen to report
    collection_types = set()

    # Event-by-event comparison
    for i in range(n_entries):
        t1.GetEntry(i)

        # Find corresponding event in file2
        if use_event_id_matching:
            # Get event ID from file1
            try:
                if hasattr(t1, 'EventHeader') and hasattr(t1.EventHeader, 'size'):
                    if t1.EventHeader.size() > 0:
                        evt_hdr = t1.EventHeader[0]
                        # Try different methods to get event number
                        event_id = None
                        if hasattr(evt_hdr, 'getEventNumber'):
                            event_id = evt_hdr.getEventNumber()
                        elif hasattr(evt_hdr, 'eventNumber'):
                            event_id = evt_hdr.eventNumber

                        if event_id is None:
                            print(f"ERROR: Could not get event ID from entry {i} in file1")
                            mismatches += 1
                            continue
                        if event_id not in event_id_map:
                            print(f"ERROR: Event ID {event_id} from file1 not found in file2")
                            mismatches += 1
                            continue
                        j = event_id_map[event_id]
                        t2.GetEntry(j)
                    else:
                        print(f"ERROR: EventHeader is empty at entry {i} in file1")
                        mismatches += 1
                        continue
                else:
                    print(f"ERROR: No EventHeader found at entry {i} in file1")
                    mismatches += 1
                    continue
            except Exception as e:
                print(f"ERROR: Could not get event ID from entry {i} in file1: {e}")
                mismatches += 1
                continue
        else:
            # Sequential comparison
            t2.GetEntry(i)
            event_id = i  # Use entry number as pseudo-event ID for reporting

        for branch_name in sorted(branches):
            # Skip metadata branches and EventHeader (already used for matching)
            if branch_name.startswith('_') or 'objIdx' in branch_name or branch_name == 'EventHeader':
                continue

            try:
                # Get the collection objects from the tree
                coll1 = getattr(t1, branch_name, None)
                coll2 = getattr(t2, branch_name, None)

                if coll1 is None or coll2 is None:
                    continue

                # Check if it's a collection (has size())
                if not hasattr(coll1, 'size'):
                    # Scalar branch - compare directly
                    if hasattr(coll1, 'value') and hasattr(coll2, 'value'):
                        if abs(coll1.value - coll2.value) > tolerance:
                            print(f"Event {event_id}, {branch_name}: {coll1.value} != {coll2.value}")
                            mismatches += 1
                        total_comparisons += 1
                    continue

                # Compare collection sizes
                size1 = coll1.size()
                size2 = coll2.size()

                if size1 != size2:
                    print(f"Event {event_id}, {branch_name}: size mismatch {size1} != {size2}")
                    mismatches += 1
                    continue

                total_comparisons += 1
                collection_types.add(branch_name)

                # Compare collection elements
                for j in range(size1):
                    elem1 = coll1[j]
                    elem2 = coll2[j]
                    total_elements += 1
                    elem_mismatch = False

                    # Compare common EDM4hep properties
                    # Check for position (SimTrackerHit, CalorimeterHit, etc.)
                    if hasattr(elem1, 'getPosition'):
                        pos1 = elem1.getPosition()
                        pos2 = elem2.getPosition()
                        if not compare_vectors(pos1, pos2, tolerance, f"Event {event_id}, {branch_name}[{j}] position"):
                            elem_mismatch = True
                    elif hasattr(elem1, 'position'):
                        pos1 = elem1.position
                        pos2 = elem2.position
                        if not compare_vectors(pos1, pos2, tolerance, f"Event {event_id}, {branch_name}[{j}] position"):
                            elem_mismatch = True

                    # Check for energy/EDep
                    if hasattr(elem1, 'getEDep'):
                        e1 = elem1.getEDep()
                        e2 = elem2.getEDep()
                        if abs(e1 - e2) > tolerance:
                            print(f"Event {event_id}, {branch_name}[{j}] EDep: {e1} != {e2}")
                            elem_mismatch = True
                    elif hasattr(elem1, 'EDep'):
                        if abs(elem1.EDep - elem2.EDep) > tolerance:
                            print(f"Event {event_id}, {branch_name}[{j}] EDep: {elem1.EDep} != {elem2.EDep}")
                            elem_mismatch = True
                    elif hasattr(elem1, 'getEnergy'):
                        e1 = elem1.getEnergy()
                        e2 = elem2.getEnergy()
                        if abs(e1 - e2) > tolerance:
                            print(f"Event {event_id}, {branch_name}[{j}] Energy: {e1} != {e2}")
                            elem_mismatch = True
                    elif hasattr(elem1, 'energy'):
                        if abs(elem1.energy - elem2.energy) > tolerance:
                            print(f"Event {event_id}, {branch_name}[{j}] energy: {elem1.energy} != {elem2.energy}")
                            elem_mismatch = True

                    # Check for momentum (MCParticle)
                    if hasattr(elem1, 'getMomentum'):
                        mom1 = elem1.getMomentum()
                        mom2 = elem2.getMomentum()
                        if not compare_vectors(mom1, mom2, tolerance, f"Event {event_id}, {branch_name}[{j}] momentum"):
                            elem_mismatch = True
                    elif hasattr(elem1, 'momentum'):
                        desc = f"Event {event_id}, {branch_name}[{j}] momentum"
                        if not compare_vectors(elem1.momentum, elem2.momentum, tolerance, desc):
                            elem_mismatch = True

                    # Check for vertex (MCParticle)
                    if hasattr(elem1, 'getVertex'):
                        vtx1 = elem1.getVertex()
                        vtx2 = elem2.getVertex()
                        if not compare_vectors(vtx1, vtx2, tolerance, f"Event {event_id}, {branch_name}[{j}] vertex"):
                            elem_mismatch = True
                    elif hasattr(elem1, 'vertex'):
                        desc = f"Event {event_id}, {branch_name}[{j}] vertex"
                        if not compare_vectors(elem1.vertex, elem2.vertex, tolerance, desc):
                            elem_mismatch = True

                    # Check for time
                    if hasattr(elem1, 'getTime'):
                        t1_val = elem1.getTime()
                        t2_val = elem2.getTime()
                        if abs(t1_val - t2_val) > tolerance:
                            print(f"Event {event_id}, {branch_name}[{j}] time: {t1_val} != {t2_val}")
                            elem_mismatch = True
                    elif hasattr(elem1, 'time'):
                        if abs(elem1.time - elem2.time) > tolerance:
                            print(f"Event {event_id}, {branch_name}[{j}] time: {elem1.time} != {elem2.time}")
                            elem_mismatch = True

                    # Check for PDG (MCParticle)
                    if hasattr(elem1, 'getPDG'):
                        pdg1 = elem1.getPDG()
                        pdg2 = elem2.getPDG()
                        if pdg1 != pdg2:
                            print(f"Event {event_id}, {branch_name}[{j}] PDG: {pdg1} != {pdg2}")
                            elem_mismatch = True
                    elif hasattr(elem1, 'PDG'):
                        if elem1.PDG != elem2.PDG:
                            print(f"Event {event_id}, {branch_name}[{j}] PDG: {elem1.PDG} != {elem2.PDG}")
                            elem_mismatch = True

                    # Check for cellID (tracker/calo hits)
                    if hasattr(elem1, 'getCellID'):
                        cell1 = elem1.getCellID()
                        cell2 = elem2.getCellID()
                        if cell1 != cell2:
                            print(f"Event {event_id}, {branch_name}[{j}] cellID: {cell1} != {cell2}")
                            elem_mismatch = True
                    elif hasattr(elem1, 'cellID'):
                        if elem1.cellID != elem2.cellID:
                            print(f"Event {event_id}, {branch_name}[{j}] cellID: {elem1.cellID} != {elem2.cellID}")
                            elem_mismatch = True

                    if elem_mismatch:
                        mismatches += 1

            except Exception as e:
                print(f"Event {event_id}, branch {branch_name}: comparison error: {e}")
                import traceback
                traceback.print_exc()
                mismatches += 1

    print(f"Performed {total_comparisons} comparisons on {total_elements} elements")
    print(f"Collections compared: {', '.join(sorted(collection_types))}")

    if mismatches > 0:
        print(f"FAILURE: {mismatches} mismatches found")
        return False

    print("SUCCESS: Files are equivalent within tolerance")
    return True


def main():
    parser = argparse.ArgumentParser(description='Compare DD4hep simulation outputs')
    parser.add_argument('file1', help='First ROOT file (reference, typically ST)')
    parser.add_argument('file2', help='Second ROOT file (test, typically MT)')
    parser.add_argument('--tolerance', type=float, default=1e-9,
                        help='Numerical comparison tolerance')
    parser.add_argument('--tree', default='EVENT',
                        help='Tree name to compare')

    args = parser.parse_args()

    success = compare_hit_collections(args.file1, args.file2, args.tolerance, args.tree)
    return 0 if success else 1


if __name__ == '__main__':
    sys.exit(main())
