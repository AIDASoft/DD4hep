###############################################################
# make a 2d slice through a dd4hep detector model
#  present results as a series of TH2F
#    - density (1/X0), materials
# in each bin, we consider the material along 2 lines,
#  parallel to the histogram axes and passing through the bin center
#
# the g4 geometry is scanned by shooting a geantino along various paths
#
# D. Jeans, KEK. 2025/2/3
#
# for usage instructions:
#   python3 g4GraphicalScan.py -h
#
# e.g. for a scan at z=1000mm, in the range -10mm < x,y < 10mm, with 100x100 bins :
# > python3 g4GraphicalScan.py -c myModel.xml -s XY -x -10,10 -y -10,10 -z 1000 -n 100 -o scanOutput.root
#
###############################################################
import os
import sys
import optparse
import subprocess
import ROOT

# define the input parameters

parser = optparse.OptionParser()
parser.formatter.width = 132
parser.description = '2-dimensional material scan using Geant4.'
parser.add_option('-c', '--compact', dest='compact', default=None,
                  help='compact xml input file',
                  metavar='<FILE>')
parser.add_option('-s', '--sliceType',
                  dest='sliceType', default='ZX',
                  help='slice plane [XY, ZX, or ZY]',
                  metavar='<string>')
parser.add_option('-x', '--xRange',
                  dest='xRange', default='-1000.,1000',
                  help='range to scan in x [in mm; give tuple "min,max" as string, or just "val" in case of ZY]',
                  metavar='<tuple>')
parser.add_option('-y', '--yRange',
                  dest='yRange', default='-1000.,1000',
                  help='range to scan in y [in mm; give tuple "min,max" as string, or just "val" in case of ZX]',
                  metavar='<tuple>')
parser.add_option('-z', '--zRange',
                  dest='zRange', default='-1000.,1000',
                  help='range to scan in z [in mm; give tuple "min,max" as string, or just "val" in case of XY]',
                  metavar='<tuple>')
parser.add_option('-n', '--nBins',
                  dest='nBins', default='100',
                  help='number of bins in output histograms',
                  metavar='<int>')
parser.add_option('-o', '--outputFile',
                  dest='outFile', default='output.root',
                  help='name of ouput root file',
                  metavar='<string>')

(opts, args) = parser.parse_args()
#
# check that the requested inputs are valid
#
infileName = str(opts.compact)
if not os.path.isfile(infileName):
    print('ERROR: cannot find requested input geometry file', infileName, file=sys.stderr)
    exit(1)
print(infileName)

sliceType = str(opts.sliceType)
if sliceType != 'XY' and sliceType != 'ZX' and sliceType != 'ZY':
    print('ERROR: unknown slice Type', sliceType, '. Choose XY, ZX or ZY.', file=sys.stderr)
    exit(1)
print(sliceType)

planePos = -99999.
planeAxis = ''

aa = str(opts.xRange).split(',')
if len(aa) == 2 and sliceType != 'ZY':
    xRange = (float(aa[0]), float(aa[1]))
    if xRange[1] <= xRange[0]:
        print('ERROR, xmin is larger than xmax', file=sys.stderr)
        exit(1)
elif len(aa) == 1 and sliceType == 'ZY':
    xRange = (float(aa[0]))
    planePos = xRange
    planeAxis = 'X'
else:
    print('ERROR: could not determine xRange, or inconsistent with sliceType', file=sys.stderr)
    exit(1)
print('xRange', xRange)

aa = str(opts.yRange).split(',')
if len(aa) == 2 and sliceType != 'ZX':
    yRange = (float(aa[0]), float(aa[1]))
    if yRange[1] <= yRange[0]:
        print('ERROR, ymin is larger than ymax', file=sys.stderr)
        exit(1)
elif len(aa) == 1 and sliceType == 'ZX':
    yRange = (float(aa[0]))
    planePos = yRange
    planeAxis = 'Y'
else:
    print('ERROR: could not determine yRange, or inconsistent with sliceType', file=sys.stderr)
    exit(1)
print('yRange', yRange)

aa = str(opts.zRange).split(',')
if len(aa) == 2 and sliceType != 'XY':
    zRange = (float(aa[0]), float(aa[1]))
    if zRange[1] <= zRange[0]:
        print('ERROR, zmin is larger than zmax', file=sys.stderr)
        exit(1)
elif len(aa) == 1 and sliceType == 'XY':
    zRange = (float(aa[0]))
    planePos = zRange
    planeAxis = 'Z'
else:
    print('ERROR: could not determine zRange, or inconsistent with sliceType', file=sys.stderr)
    exit(1)
print('zRange', zRange)

nBins = int(opts.nBins)
print('nBins', nBins)

if nBins < 1:
    print('ERROR: crazy number of bins requested', nBins, file=sys.stderr)
    exit(1)

outFileName = str(opts.outFile)
#
# define the "mother" histogram according to the requested slice type, ranges, number of bins
#
fout = ROOT.TFile(outFileName, 'recreate')

if sliceType == 'XY':
    h2 = ROOT.TH2F('hMat', 'h2', nBins, xRange[0], xRange[1], nBins, yRange[0], yRange[1])
    h2.GetXaxis().SetTitle('x [mm]')
    h2.GetYaxis().SetTitle('y [mm]')
elif sliceType == 'ZX':
    h2 = ROOT.TH2F('hMat', 'h2', nBins, zRange[0], zRange[1], nBins, xRange[0], xRange[1])
    h2.GetXaxis().SetTitle('z [mm]')
    h2.GetYaxis().SetTitle('x [mm]')
elif sliceType == 'ZY':
    h2 = ROOT.TH2F('hMat', 'h2', nBins, zRange[0], zRange[1], nBins, yRange[0], yRange[1])
    h2.GetXaxis().SetTitle('z [mm]')
    h2.GetYaxis().SetTitle('y [mm]')
h2.Fill(0, 0, 0.0)  # to ensure there is at least one entry...otherwise doesn't get drawn...

edgeOfWorld = -15000.  # mm
#
# this is where the materials will be stored
#
mats = {}
#
# we make scans along the X and Y axes of the mother histogram
# prepare the input macro to ddsim
#
steerName = '_' + outFileName + '.mac'
steerMac = open(steerName, 'w')
steerMac.write('/gun/particle geantino' + '\n')
steerMac.write('/gun/energy 20 GeV' + '\n')
steerMac.write('/gun/number 1' + '\n')

for iDir in range(0, 2):
    mats[iDir] = {}
    if iDir == 0:
        axis = h2.GetXaxis()
    else:
        axis = h2.GetYaxis()
    #
    # the direction of the gun
    #
    if sliceType == 'XY':
        if iDir == 0:
            dirn = '0 1 0'
        else:
            dirn = '1 0 0'
    elif sliceType == 'ZX':
        if iDir == 0:
            dirn = '1 0 0'
        else:
            dirn = '0 0 1'
    elif sliceType == 'ZY':
        if iDir == 0:
            dirn = '0 1 0'
        else:
            dirn = '0 0 1'
    steerMac.write('/gun/direction ' + dirn + '\n')
    #
    # loop over the bins in this axis
    #
    for iX in range(1, nBins + 1):

        mats[iDir][iX] = {}
        #
        # define the starting position of the gun
        #
        X = axis.GetBinCenter(iX)
        if iDir == 0:
            if sliceType == 'XY':
                startPos = str(X) + ' '
                startPos += str(edgeOfWorld) + ' '
                startPos += str(zRange)
            elif sliceType == 'ZX':
                startPos = str(edgeOfWorld) + ' '
                startPos += str(yRange) + ' '
                startPos += str(X)
            elif sliceType == 'ZY':
                startPos = str(xRange) + ' '
                startPos += str(edgeOfWorld) + ' '
                startPos += str(X)
        else:
            if sliceType == 'XY':
                startPos = str(edgeOfWorld) + ' '
                startPos += str(X) + ' '
                startPos += str(zRange)
            elif sliceType == 'ZX':
                startPos = str(X) + ' '
                startPos += str(yRange) + ' '
                startPos += str(edgeOfWorld)
            elif sliceType == 'ZY':
                startPos = str(xRange) + ' '
                startPos += str(X) + ' '
                startPos += str(edgeOfWorld)

        steerMac.write('/gun/position ' + startPos + ' mm \n')
        steerMac.write('/run/beamOn' + '\n')

steerMac.write('exit')
steerMac.close()

#
# run ddsim with this macro
#
cmd = ['ddsim', '--compactFile', infileName, '--runType', 'run', '--enableG4Gun',
       '--action.step', 'Geant4MaterialScanner/MaterialScan', '-M', steerName]
result = subprocess.run(cmd, capture_output=True, text=True)
#
# parse the results
#
iscan = 1
inScan = False
iDir = 0
for line in result.stdout.splitlines():
    if 'Material scan between' in line:
        gg = line.split(')')[0].split('(')[1].split(',')
        startx = 10 * float(gg[0])  # convert cm -> mm
        starty = 10 * float(gg[1])
        startz = 10 * float(gg[2])
        inScan = True
    elif 'Finished run' in line:
        iscan += 1
        if iscan == nBins + 1:   # now move to the second set of scans
            iDir = 1
            iscan = 1
        inScan = False
    elif r"+-----------------" in line:  # comment line
        continue
    elif r"|     \   Material" in line:  # comment line
        continue
    elif r"| Num. \  Name" in line:      # comment line
        continue
    elif r"| Layer \ " in line:          # comment line
        continue
    elif inScan:   # this line contains material information
        index = int(line.split()[1])
        material = line.split()[2]
        radlen = 10 * float(line.split()[6])     # cm->mm
        thickness = 10 * float(line.split()[8])  # cm->mm
        endpos = line.split('(')[1].split(')')[0].split(',')
        endx = 10 * float(endpos[0])             # cm -> mm
        endy = 10 * float(endpos[1])
        endz = 10 * float(endpos[2])
        mats[iDir][iscan][index] = [material, radlen, thickness, endx, endy, endz]
#
# now all data is collected: fill the histograms
#
h2.SetTitle('materialScan at ' + planeAxis + '=' + str(planePos) + ' mm : 1/X_{0}')

hists = {}
hists['x0'] = h2

for iDir in range(0, 2):   # the two directions
    if iDir == 1:
        mainaxis = h2.GetYaxis()  # perpendicular to the scan direction
        scanaxis = h2.GetXaxis()  # parallel to the scan direction
    elif iDir == 0:
        mainaxis = h2.GetXaxis()
        scanaxis = h2.GetYaxis()

    for jj in range(1, mainaxis.GetNbins() + 1):  # loop over the scan lines
        mainaxis.GetBinCenter(jj)
        scandat = mats[iDir][jj]

        hxn = scanaxis.GetNbins()
        hxl = scanaxis.GetBinLowEdge(1)
        hxh = scanaxis.GetBinUpEdge(hxn)
        hxbw = scanaxis.GetBinWidth(1)

        curpos = edgeOfWorld

        for value in scandat.values():
            begpos = curpos

            endx = value[3]
            endy = value[4]
            endz = value[5]

            if sliceType == 'XY':
                if iDir == 0:
                    endpos = endy
                elif iDir == 1:
                    endpos = endx
            elif sliceType == 'ZX':
                if iDir == 0:
                    endpos = endx
                elif iDir == 1:
                    endpos = endz
            elif sliceType == 'ZY':
                if iDir == 0:
                    endpos = endy
                elif iDir == 1:
                    endpos = endz

            radlen = value[1]
            thick = value[2]
            matStr = value[0]

            if begpos < hxl and endpos < hxl:  # not in histo range (below)
                pass
            elif begpos > hxh and endpos > hxh:  # not in histo range (above)
                pass
            else:
                if matStr not in hists.keys():  # not yet seen material: make a new histogram for it
                    hists[matStr] = h2.Clone(h2.GetName() + '_' + matStr)
                    hists[matStr].SetTitle(hists[matStr].GetTitle().replace('1/X_{0}', matStr))
                    hists[matStr].Reset()
                    hists[matStr].Fill(0, 0, 0.0)  # ensure at least one entry...otherwise doesn't get drawn..

                iy1 = scanaxis.FindBin(begpos)
                iy2 = scanaxis.FindBin(endpos)

                if iy1 == iy2:   # this step entirely within one histo bin
                    if iDir == 1:
                        hists['x0']  .AddBinContent(iy1, jj, thick / radlen / hxbw)
                        hists[matStr].AddBinContent(iy1, jj, thick / hxbw)
                    else:
                        hists['x0']  .AddBinContent(jj, iy1, thick / radlen / hxbw)
                        hists[matStr].AddBinContent(jj, iy1, thick / hxbw)
                else:   # this step extends over two or more bins
                    firstBinStubLength = scanaxis.GetBinUpEdge(iy1) - begpos
                    lastBinStubLength = endpos - scanaxis.GetBinLowEdge(iy2)
                    if iDir == 1:
                        hists['x0'].AddBinContent(iy1, jj, firstBinStubLength / radlen / hxbw)
                        hists['x0'].AddBinContent(iy2, jj, lastBinStubLength / radlen / hxbw)
                        hists[matStr].AddBinContent(iy1, jj, firstBinStubLength / hxbw)
                        hists[matStr].AddBinContent(iy2, jj, lastBinStubLength / hxbw)
                    else:
                        hists['x0'].AddBinContent(jj, iy1, firstBinStubLength / radlen / hxbw)
                        hists['x0'].AddBinContent(jj, iy2, lastBinStubLength / radlen / hxbw)
                        hists[matStr].AddBinContent(jj, iy1, firstBinStubLength / hxbw)
                        hists[matStr].AddBinContent(jj, iy2, lastBinStubLength / hxbw)

                    if iy2 - iy1 > 1:  # fill the bins in between first and last
                        for i in range(iy1 + 1, iy2):
                            if iDir == 1:
                                hists['x0']  .AddBinContent(i, jj, 1. / radlen)
                                hists[matStr].AddBinContent(i, jj, 1.)
                            else:
                                hists['x0']  .AddBinContent(jj, i, 1. / radlen)
                                hists[matStr].AddBinContent(jj, i, 1.)
            curpos = endpos

for mm in hists.keys():
    hists[mm].Scale(1. / 2)  # average of the two direction scans

fout.Write()
fout.Close()

# clean up the macro file
os.remove(steerName)
