# ==========================================================================
#   Software for the ALEPH experiment
# --------------------------------------------------------------------------
#  Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
#  All rights reserved.
# 
#  For the licensing terms see ALSOFTINSTALL/LICENSE.
#  For the list of contributors see ALSOFTINSTALL/doc/CREDITS.
# 
#  Author     : M.Frank
# 
# ==========================================================================

__version__ = "1.0"
__author__  = "Markus Frank <Markus.Frank@cern.ch>"
__usage__   = f"""

python gen_header.py --bank FTFT --output . 
"""

import os
import sys
import cmath

# =========================================================================================
def to_len(value):
  return 10.0*value

# =========================================================================================
def to_angle(value):
  v = (value/360.0)*2.0*cmath.pi

# =========================================================================================
def _trim(line, replace_space=None):
  if not replace_space:
    return line.lstrip().rstrip()
  return line.lstrip().rstrip().replace(' ',replace_space)

# =========================================================================================
def to_vector(value_list):
  s = str(value_list).replace("'","").replace('"','').replace('[','{').replace(']','}')
  return s

# =========================================================================================
def _items(line):
  l = _trim(line)
  for i in range(10):
    l = l.replace('  ',' ')
  itm = l.split(' ')
  items = []
  for i in itm:
    if len(i): items.append(i)
  return items


"""
   \author  M.Frank
   \version 1.0
"""
class material:
  # =======================================================================================
  def __init__(self, geo, data):
    self.geometry  = geo
    self.data      = data
    self.index     = data['index']
    self.name      = _trim(data['name'],'_')
    self.A         = data['A']
    self.Z         = data['Z']
    self.density   = data['density']
    self.intlen    = data['intlen']
    self.radlen    = data['radlen']
    self.mix       = data['mix']

"""
   \author  M.Frank
   \version 1.0
"""
class medium:
  # =======================================================================================
  def __init__(self, geo, data):
    self.geometry  = geo
    self.data      = data
    self.index     = data['index']
    self.name      = _trim(data['name'],'_')
    self.idx_mat   = data['material']
    self.isvol     = data['isvol']
    self.ifield    = data['ifield']
    self.magfield  = data['magfield']
    self.tmaxfd    = data['tmaxfd']
    self.stepmax   = data['stepmax']
    self.deemax    = data['deemax']
    self.epsilon   = data['epsilon']
    self.stepmin   = data['stepmin']

  def material(self):
    return self.geometry.material[self.idx_mat]


"""
   \author  M.Frank
   \version 1.0
"""
class matrix:
  # =======================================================================================
  def __init__(self, geo, data):
    self.geometry  = geo
    self.data      = data
    self.index     = data['index']
    self.flag      = data['flag']
    m              = data['matrix']
    self.matrix    = [m[0], m[3], m[6], \
                      m[1], m[4], m[7], \
                      m[2], m[5], m[8] ]
    #self.matrix    = data['matrix']
    self.phi       = [data['phi1'],data['phi2'],data['phi3']]
    self.theta     = [data['theta1'],data['theta2'],data['theta3']]


"""
   \author  M.Frank
   \version 1.0
"""
class volume:
  # =======================================================================================
  def __init__(self, geo, data):
    self.geometry   = geo
    self.data       = data
    self.index      = data['index']
    self.name       = _trim(data['name'], '_')
    self.shape      = _trim(data['shape'])
    self.idx_medium = data['medium']
    self.params     = data['params']
    self.option     = data['only']

  def medium(self):
    return self.geometry.medium[self.idx_medium]


"""
   \author  M.Frank
   \version 1.0
"""
class placement:
  # =======================================================================================
  def __init__(self, geo, data):
    self.geometry  = geo
    self.data      = data
    self.index     = data['index']
    self.name      = data['name']
    self.mother    = data['mother']
    self.ind_rot   = data['rotation']
    self.copy_no   = data['copy']
    self.params    = data['params']
    self.option    = data['only']
    self.position  = (data['x'], data['y'], data['z'])
    self.code      = ''


"""
   \author  M.Frank
   \version 1.0
"""
class division:
  # =======================================================================================
  def __init__(self, geo, data):
    self.geometry   = geo
    self.data       = data
    self.name       = data['name']
    self.volume     = data['volume']
    self.ndiv       = data['ndiv']
    self.axis       = data['axis']
    self.start      = data['start']
    self.step       = data['step']
    self.idx_medium = data['idx_medium']
    self.option     = data['option']
  

# =========================================================================================
"""
   \author  M.Frank
   \version 1.0
"""
class geant3_geometry:
  # =======================================================================================
  def __init__(self):
    self.material   = {}
    self.medium     = {}
    self.volumes    = {}
    self.rotations  = {}
    self.placements = []
    self.divisions  = []
    
  # =======================================================================================
  def add_material(self, mat):
    self.material[mat['index']] = material(self, mat)
    
  # =======================================================================================
  def add_medium(self, med):
    self.medium[med['index']] = medium(self, med)
    
  # =======================================================================================
  def add_placement(self, place):
    self.placements.append(placement(self, place))

  # =======================================================================================
  def add_rotation(self, mat):
    self.rotations[mat['index']] = matrix(self, mat)

  # =======================================================================================
  def add_volume(self, vol):
    self.volumes[vol['index']] = volume(self, vol)

  # =======================================================================================
  def add_division(self, div):
    self.divisions.append(division(self, div))


# =========================================================================================
"""
   \author  M.Frank
   \version 1.0
"""
class dd4hep_geometry:

  # =======================================================================================
  def __init__(self, geo, output):
    self.print_volumes = False
    self.geant3_geo = geo
    self.output     = output
    self.material   = {}
    self.medium     = {}
    self.shapes     = {}
    self.volumes    = {}
    self.rotations  = {}
    self.placements = []

  # =======================================================================================
  def write_code(self, line):
    if self.output:
      self.output.write( str(line)+'\n' )
      return
    print(line)

  # =======================================================================================
  def extract_data(self):
    open_bracket  = '{'
    close_bracket = '}'
    for idx, vol in self.geant3_geo.volumes.items():
      typ   = vol.shape
      shape = 'None'
      error = None
      name  = vol.name + '_solid'
      if typ == 'BOX':
        shape = f'dd4hep::Box( std::string("{name}"), {vol.params[0]}*units::cm, {vol.params[1]}*units::cm, {vol.params[2]}*units::cm )'
      elif typ == 'TUBE':  #      Geant3 parameters:  rmin, rmax, dz
        shape = f'dd4hep::Tube( std::string("{name}"), {vol.params[0]}*units::cm, {vol.params[1]}*units::cm, {vol.params[2]}*units::cm )'
      elif typ == 'TUBS':  #      Geant3 parameters:  rmin, rmax, dz, start-phi, end-phi
        shape = f'dd4hep::Tube( std::string("{name}"), {vol.params[0]}*units::cm, {vol.params[1]}*units::cm, {vol.params[2]}*units::cm, ' + \
                f'{vol.params[3]}*units::degree, {vol.params[4]}*units::degree )'
      elif typ == 'CONE':  #      Geant3 parameters:  dz, rmin1, rmax2, rmin2, rmax2
        shape = f'dd4hep::Cone( std::string("{name}"), {vol.params[0]}*units::cm, ' + \
                f'{vol.params[1]}*units::cm, {vol.params[2]}*units::cm, ' + \
                f'{vol.params[3]}*units::cm, {vol.params[4]}*units::cm )'
      elif typ == 'CONS':  #      Geant3 parameters:  dz, rmin1, rmax2, rmin2, rmax2, start-phi, end-phi
        shape = f'dd4hep::ConeSegment( std::string("{name}"), {vol.params[0]}*units::cm, ' + \
                f'{vol.params[1]}*units::cm, {vol.params[2]}*units::cm, ' + \
                f'{vol.params[3]}*units::cm, {vol.params[4]}*units::cm, ' + \
                f'{vol.params[5]}*units::degree, {vol.params[6]}*units::degree )'
      elif typ == 'HYPE':  #      Geant3 parameters:  rmin, rmax, dz, theta
        shape = f'dd4hep::Hyperboloid( std::string("{name}"), {vol.params[0]}*units::cm, ' + \
                f'{vol.params[3]}*units::degree, {vol.params[1]}*units::cm, {vol.params[3]}*units::degree, {vol.params[2]}*units::cm )'
      elif typ == 'SPHE':  #      Geant3 parameters:  rmin, rmax, start-theta, end-theta, start-phi, end-phi
        shape = f'dd4hep::Sphere( std::string("{name}"), {vol.params[0]}*units::cm, {vol.params[1]}*units::cm, ' + \
                f'{vol.params[2]}*units::degree, {vol.params[3]}*units::degree, ' + \
                f'{vol.params[4]}*units::degree, {vol.params[5]}*units::degree )'
                
      elif typ == 'TRD1':  #      Geant3 parameters:  dx1, dx2, dy, dz
        shape = f'dd4hep::Trd1( std::string("{name}"), {vol.params[0]}*units::cm, {vol.params[1]}*units::cm, {vol.params[2]}*units::cm, {vol.params[3]}*units::cm )'
        
      elif typ == 'TRD2':  #      Geant3 parameters:  dx1, dx2, dy1, dy2, dz
        shape = f'dd4hep::Trd2( std::string("{name}"), {vol.params[0]}*units::cm, {vol.params[1]}*units::cm, {vol.params[2]}*units::cm, {vol.params[3]}*units::cm, {vol.params[4]}*units::cm )'
      elif typ == 'TRAP':  #      Geant3 parameters:  z, theta, phi,  h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2
        shape = f'dd4hep::Trap( std::string("{name}"), {vol.params[0]}*units::cm, {vol.params[1]}*units::degree, {vol.params[2]}*units::degree, ' + \
                f'{vol.params[3]}*units::degree, {vol.params[4]}*units::cm, {vol.params[5]}*units::cm, {vol.params[6]}*units::degree, ' + \
                f'{vol.params[7]}*units::degree, {vol.params[8]}*units::cm, {vol.params[9]}*units::cm, {vol.params[10]}*units::degree )'
      elif typ == 'PARA':  #      Geant3 parameters:  dx, dy, dz, alpha, theta, phi
        #shape = f'dd4hep::Parallelepiped( std::string("{name}"), {vol.params[0]}*units::cm, {vol.params[1]}*units::cm, {vol.params[2]}*units::cm, ' + \
        #        f'{vol.params[3]}*units::degree, {vol.params[4]}*units::degree, {vol.params[5]}*units::degree )'
        shape = f'dd4hep::Solid(new TGeoPara( "{name}", {vol.params[0]}*units::cm, {vol.params[1]}*units::cm, {vol.params[2]}*units::cm, ' + \
                f'{vol.params[3]}, {vol.params[4]}, {vol.params[5]} ))'
      elif typ == 'PCON':  #      Geant3 parameters:  phi, dphi, nz, (z, rmin, rmax)
        nz   = int(vol.params[2])
        z    = []
        rmin = []
        rmax = []
        for i in range(nz):
          z.append(    f'{vol.params[3+i*3]}*units::cm' )
          rmin.append( f'{vol.params[4+i*3]}*units::cm' )
          rmax.append( f'{vol.params[5+i*3]}*units::cm' )
        shape = f'dd4hep::Polycone( std::string("{name}"), {vol.params[0]}*units::degree, {vol.params[1]}*units::degree, ' + \
                f'{to_vector(rmin)}, {to_vector(rmax)}, {to_vector(z)} )'

      elif typ == 'PGON':  #      Geant3 parameters:  phi, dphi, nz, (z, rmin, rmax)
        nz   = int(vol.params[2])
        z    = []
        rmin = []
        rmax = []
        for i in range(nz):
          z.append(    f'{vol.params[3+i*3]}*units::cm' )
          rmin.append( f'{vol.params[4+i*3]}*units::cm' )
          rmax.append( f'{vol.params[5+i*3]}*units::cm' )
        shape = f'dd4hep::Polyhedra( std::string("{name}"), {nz}, {vol.params[0]}*units::degree, {vol.params[1]}*units::degree, ' + \
                f'{to_vector(z)}, {to_vector(rmin)}, {to_vector(rmax)} )'
                
      else:                # Unknown shape
        error = f'ERROR: Unknown shape {vol.name} {typ} '

      if error:
        print( f' FAILED SHAPE: {error}' )
      else:
        self.shapes[idx] = shape
        medium   = vol.medium()
        material = medium.material()
        self.material[material.name] = { 'g3': material, 'code': '' }
        self.medium[medium.name]     = { 'g3': medium, 'material': material.name, 'code': '' }
        self.volumes[idx]            = { 'g3': vol, 'shape': shape, 'medium': medium.name }
        if self.print_volumes:
          print( f'---> {vol.name} SHAPE:   {shape}' )
          print( f'          MEDIUM: "{medium.name}"  -> MATERIAL: "{material.name}" ' )

    for key, mat in self.geant3_geo.rotations.items():
      self.rotations[key] = {'g3': mat, 'code': ''}
    for pv in self.geant3_geo.placements:
      self.placements.append({'g3': pv, 'code': ''})

  # =======================================================================================
  def write_header(self):
    self.write_code(\
"""// ==========================================================================
//   Software for the ALEPH experiment
// --------------------------------------------------------------------------
//  Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
//  All rights reserved.
// 
//  For the licensing terms see ALSOFTINSTALL/LICENSE.
//  For the list of contributors see ALSOFTINSTALL/doc/CREDITS.
// 
//  Author     : M.Frank
// 
// ==========================================================================

#include <dd4hep/geant3_geometry.h>
""")

  # =======================================================================================
  def handle_materials(self):
    self.write_code( '/// Handle the conversion of the Geant3 materials: \n' + \
                     'void dd4hep::geant3_geometry_imp::handle_materials()  {        \n' + \
                     '  /// First add all pure materials:' )
    for name, mat in self.material.items():
      material = mat['g3']
      if len(material.mix) == 0:
        m = material.data
        self.write_code( f'  this->add_material({material.index}, "{name}", {m['Z']}, {m['A']}, {m['density']}, {m['intlen']}, {m['radlen']}); ' )
    self.write_code( f'  /// Now add all the composite materials:' )
    for name in self.material.keys():
      material = self.material[name]['g3']
      if len(material.mix) > 0:
        m = material.data
        val = to_vector(material.mix).replace('A: ','').replace('Z: ','').replace('wmix: ','')
        code = f'  this->add_mixture({material.index}, "{name}", {m['Z']}, {m['A']}, {m['density']}, {m['intlen']}, {m['radlen']}, {val});'
        material.code = code
        #print( str(material.data) )
        self.write_code( material.code )
    self.write_code( '} /// End geant3_geometry_imp::handle_materials  \n\n' )
    return self

  # =======================================================================================
  def handle_media(self):
    self.write_code( '/// Handle the conversion of the Geant3 media: \n' + \
                     'void dd4hep::geant3_geometry_imp::handle_media()  { \n' )
    for key in self.medium.keys():
      medium   = self.medium[key]['g3']
      material = medium.material()
      self.medium[key]['code'] = f'  this->add_medium( {medium.index}, "{medium.name}", "{material.name}" );'
      self.write_code( self.medium[key]['code'] )
    self.write_code( '} /// End geant3_geometry_imp::handle_media  \n\n' )
    return self

  # =======================================================================================
  def handle_rotations(self):
    self.write_code( '/// Handle the conversion of the Geant3 rotations: \n' + \
                     'void dd4hep::geant3_geometry_imp::handle_rotations()  {' )
    for key in self.rotations.keys():
      rot   = self.rotations[key]['g3']
      data  = rot.matrix
      index = rot.index
      self.rotations[key]['code'] = \
        f'  this->add_rotation( {index}, dd4hep::Rotation3D({str(data).replace('[','').replace(']','')}) );'
      self.write_code( self.rotations[key]['code'] )
    self.write_code( '} /// End geant3_geometry_imp::handle_rotations\n\n' )
    return self

  # =======================================================================================
  def handle_volumes(self):
    self.write_code( '/// Handle the conversion of the Geant3 volumes: \n' + \
                     'void dd4hep::geant3_geometry_imp::handle_volumes()  { \n' + \
                     '  dd4hep::Solid    solid;    \n' )
    for idx in self.volumes.keys():
      volume = self.volumes[idx]
      name   = _trim(volume['g3'].name)
      medium = _trim(volume['medium'])
      index  = volume['g3'].index
      shape  = volume['shape']
      opt    = _trim(volume['g3'].option)
      code = '\n' + \
             f'  solid = {shape}; \n' +\
             f'  this->add_volume( {index}, "{name}", "{medium}", solid, "{opt}");'
      volume['code'] = code
      self.write_code( code )
    self.write_code( '} /// End geant3_geometry_imp::handle_volumes  \n\n' )
    return self

  # =======================================================================================
  def handle_placements(self):
    self.write_code( '/// Handle the conversion of the Geant3 volumes: \n' + \
                     'void dd4hep::geant3_geometry_imp::handle_placements()  { \n' + \
                     '  std::vector<double> params; \n' )
    ob = '{'
    cb = '}'
    for pv in self.placements:
      place = pv['g3']
      if not len(place.params):
        code =  '  params.clear();\n'
      else:
        code = f'  params = {ob} {str(place.params)[1:-1]} {cb};\n'
      x = str(place.position[0]) + '*units::cm'
      y = str(place.position[1]) + '*units::cm'
      z = str(place.position[2]) + '*units::cm'
      code = code + \
             f'  this->add_placement({place.index}, "{_trim(place.mother)}", "{_trim(place.name)}", {place.copy_no},' + \
             f' dd4hep::Position({x},{y},{z}), {place.ind_rot}, params, "{_trim(place.option)}");'
      self.write_code( code )
    self.write_code( '} /// End geant3_geometry_imp::handle_placements  \n\n' )
    return self

  # =======================================================================================
  def handle_divisions(self):
    code = '/// Handle the conversion of the Geant3 volume divisions: \n' + \
                     'void dd4hep::geant3_geometry_imp::handle_divisions()  { \n'
    for d in self.geant3_geo.divisions:
      code = code + \
             f'  this->add_division("{_trim(d.name)}", "{_trim(d.volume)}", ' +\
             f' {d.axis}, {d.ndiv}, ' +\
             f' {d.start}*units::degree, {d.step}*units::degree, ' +\
             f' {d.idx_medium}, "{d.option}");\n'
    self.write_code( code )
    self.write_code( '} /// End geant3_geometry_imp::handle_divisions  \n\n' )
    return self

# =========================================================================================
import argparse
parser = argparse.ArgumentParser(allow_abbrev=False,
                                 prog='extract_geant3_geometry',
                                 formatter_class=argparse.RawDescriptionHelpFormatter,
                                 description='Create DD4hep geometry',
                                 epilog='Usage example: '+__usage__ )

#
#  Check debug flag
parser.add_argument(
  '-D',
  '--debug',
  action='store_true',
  dest='debug',
  default=False,
  help='Enable python debugging (invoke pdb)',
)
#
# geometry input file
parser.add_argument(
  '-I',
  '--input',
  type=str,
  dest='geometry_input',
  default='aleph.geometry.txt',
  help='Location of the geometry input file',
)
#
# geometry output file
parser.add_argument(
  '-O',
  '--output',
  type=str,
  dest='geometry_output',
  default=None,
  help='Location of the geometry output file',
)
#
#
#
args = parser.parse_args()

geometry_file = args.geometry_input
lines = open(geometry_file, 'r').readlines()

aleph_geometry = geant3_geometry()

if args.debug:
  import pdb
  pdb.set_trace()
  
num_err = 0
for line in lines:
  if line.find('aleph_geometry.add') >= 0:
    try:
      eval(line)
    except Exception as X:
      print( f'ERROR: {line} {str(X)}' )
      num_err = num_err + 1

if num_err == 0:
   print( f'+++ Successfully processed Geant3 input file: {geometry_file}')
   print( f'+++ Parsed  {len(aleph_geometry.material)}  material')
   print( f'+++         {len(aleph_geometry.medium)}  media')
   print( f'+++         {len(aleph_geometry.volumes)}  volumes')
   print( f'+++         {len(aleph_geometry.divisions)}  divisions')
   print( f'+++         {len(aleph_geometry.placements)}  placements')
   output = sys.stdout
   if args.geometry_output:
      output = open(args.geometry_output, 'w')
   dd4hep = dd4hep_geometry(aleph_geometry, output)
   dd4hep.extract_data()
   never = True
   dd4hep.write_header()
   dd4hep.handle_materials()
   if never:
     dd4hep.handle_media()
     dd4hep.handle_rotations()
     dd4hep.handle_volumes()
     dd4hep.handle_divisions()
     dd4hep.handle_placements()

else:
   print( f'+++ FAILED to process Geant3 input file: {geometry_file}.  {num_err} errors encountered.')
