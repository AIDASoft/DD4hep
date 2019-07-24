from __future__ import absolute_import, unicode_literals

def detector_Shelf(description, det):

  plane  = det.find('planes')
  mat    = det.find('material')
  #pos   = det.find('position')
  #rot   = det.find('rotation')
  book   = det.find('books')
  
  #---Construct the ensamble plane+books volume-------------------------------------------------------------
  e_vol  = Volume(description, 'ensemble', Box(description,'box', plane.x, plane.y+book.y, plane.z), description.material('Air'))
  e_vol.setVisAttributes(description,'InvisibleWithDaughters')
  
  #---Construct the plane and place it----------------------------------------------------------------------
  p_vol   = Volume(description, 'plane', Box(description, 'plane', plane.x, plane.y, plane.z), description.material(mat.name))
  p_vol.setVisAttributes(description, plane.vis)
  e_vol.placeVolume(p_vol, Position(0,-book.y,0))
  
  #---Construct a book and place it number of times---------------------------------------------------------
  b_vol = Volume(description, 'book',Box(description, 'book', book.x, book.y, book.z), description.material('Carbon'))
  b_vol.setVisAttributes(description, book.vis)
  x,y,z = plane.x-book.x, plane.y, -plane.z+book.z
  for n in range(book.number):
    e_vol.placeVolume(b_vol, Position(x,y,z))
    z += 2*book.z + book.getF('dz')
  
  #--Construct the overal envelope and Detector element-----------------------------------------------------
  g_x, g_y, g_z = plane.x, plane.number*plane.getF('dy'), plane.z
  g_vol  = Volume(description, det.name, Box(description,'box', g_x, g_y, g_z), description.material('Air'))
  g_vol.setVisAttributes(description,'InvisibleWithDaughters')
  de     = DetElement(description, det.name, det.type, det.id)
  phv = description.worldVolume().placeVolume(g_vol, Position(g_x,g_y,g_z))
  phv.addPhysVolID('id',det.id)
  de.addPlacement(phv)
  x,y,z = 0,book.y+plane.y-2*plane.getF('dy'),0
  for n in range(plane.number):
    g_vol.placeVolume(e_vol, Position(x,y,z))
    y += plane.getF('dy')
  #---Return detector element---------------------------------------------------------------------------------
  return de
