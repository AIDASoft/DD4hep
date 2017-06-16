#!/bin/python
#==========================================================================
#  AIDA Detector description implementation 
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================

import sys, errno, optparse

parser = optparse.OptionParser()
parser.description = "TGeo Geometry checking."
parser.formatter.width = 132
parser.add_option("-c", "--compact", dest="compact", default=None,
                  help="Define LCCDD style compact xml input",
		  metavar="<FILE>")

parser.add_option("-f","--full",
		  dest="full", default=False,
                  help="Full geometry checking",
		  metavar="<boolean>")

parser.add_option("-n", "--ntracks",
		  dest="num_tracks", default=1000000,
                  help="Number of tracks [requires '--full']",
		  metavar="<integer>")

parser.add_option("-x", "--vx",
		  dest="vx", default=0.0,
                  help="X-position of track origine vertex [requires '--full']",
		  metavar="<double>")

parser.add_option("-y", "--vy",
		  dest="vy", default=0.0,
                  help="Y-position of track origine vertex [requires '--full']",
		  metavar="<double>")

parser.add_option("-z", "--vz",
		  dest="vz", default=0.0,
                  help="Z-position of track origine vertex [requires '--full']",
		  metavar="<double>")

parser.add_option("-o", "--option", dest="option", default='ob',
                  help="TGeoManager geometry check option default:ob",
		  metavar="<string>")

(opts, args) = parser.parse_args()

if opts.compact is None:
  print "   ",parser.format_help()
  sys.exit(1)

try:
  import ROOT
  from ROOT import gROOT
  gROOT.SetBatch(1)
except ImportError,X:
  print 'PyROOT interface not accessible:',X
  sys.exit(errno.ENOENT)

try:
  import dd4hep
except ImportError,X:
  print 'dd4hep python interface not accessible:',X
  sys.exit(errno.ENOENT)

dd4hep.setPrintLevel(dd4hep.OutputLevel.ERROR)
print '+++%s\n+++ Loading compact geometry:%s\n+++%s'%(120*'=',opts.compact,120*'=')
description = dd4hep.Detector.getInstance()
description.fromXML(opts.compact)
opts.num_tracks = int(opts.num_tracks)
opts.vx = float(opts.vx)
opts.vy = float(opts.vy)
opts.vz = float(opts.vz)
print '+++%s\n+++ Checking geometry:%s full-check:%s\n+++%s'%(120*'=',opts.compact,opts.full,120*'=')
if opts.full:
  print '+++ # tracks:%d vertex:(%7.3f, %7.3f, %7.3f) [cm]'%(opts.num_tracks,opts.vx,opts.vy,opts.vz,)
  description.manager().CheckGeometryFull(opts.num_tracks,opts.vx,opts.vy,opts.vz,opts.option)
else:
  description.manager().CheckGeometry()
#
#
print '+++ Execution finished...'
sys.exit(0)
