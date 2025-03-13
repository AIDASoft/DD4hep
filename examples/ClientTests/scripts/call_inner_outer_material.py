import os
import argparse
import dd4hep
import DDRec

argparser = argparse.ArgumentParser()
argparser.add_argument('--call-inner-material', action='store_true', default=False)

args = argparser.parse_args()

det = dd4hep.Detector.getInstance()
install_dir = os.environ['DD4hepExamplesINSTALL']
det.fromCompact(f'{install_dir}/examples/ClientTests/compact/InnerOuterMaterial.xml')

sm = det.extension[DDRec.SurfaceManager]()
multimap = sm.map('world')
d = {}
for k, v in multimap:
    if args.call_inner_material:
        innermat = v.innerMaterial()
    outermat = v.outerMaterial()
    d[k] = outermat.name()

with open(f'surfaces_{"bad" if args.call_inner_material else "good"}.txt', 'w') as f:
    for k, v in d.items():
        f.write(f'{k} {v}\n')
