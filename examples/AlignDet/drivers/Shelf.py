from __future__ import absolute_import, unicode_literals
from ddsix.moves import range


def detector_Shelf(description, det):

  plane = det.find('planes')
  mat = det.find('material')
  # pos   = det.find('position')
  # rot   = det.find('rotation')
  book = det.find('books')

  # ---Construct the ensamble plane+books volume-------------------------------------------------------------
  e_vol = Volume(description, 'ensemble',  # noqa: F821
                 Box(description, 'box', plane.x, plane.y + book.y, plane.z),   # noqa: F821
                     description.material('Air'))
  e_vol.setVisAttributes(description, 'InvisibleWithDaughters')

  # ---Construct the plane and place it----------------------------------------------------------------------
  p_vol = Volume(description, 'plane',  # noqa: F821
                 Box(description, 'plane', plane.x, plane.y, plane.z),  # noqa: F821
                     description.material(mat.name))
  p_vol.setVisAttributes(description, plane.vis)
  e_vol.placeVolume(p_vol, Position(0, -book.y, 0))  # noqa: F821

  # ---Construct a book and place it number of times---------------------------------------------------------
  b_vol = Volume(description, 'book',  # noqa: F821
                 Box(description, 'book', book.x, book.y, book.z),  # noqa: F821
                     description.material('Carbon'))
  b_vol.setVisAttributes(description, book.vis)
  x, y, z = plane.x - book.x, plane.y, -plane.z + book.z
  for n in range(book.number):
    e_vol.placeVolume(b_vol, Position(x, y, z))  # noqa: F821
    z += 2 * book.z + book.getF('dz')

  # --Construct the overal envelope and Detector element-----------------------------------------------------
  g_x, g_y, g_z = plane.x, plane.number * plane.getF('dy'), plane.z
  g_vol = Volume(description, det.name,  # noqa: F821
                 Box(description, 'box', g_x, g_y, g_z), description.material('Air'))  # noqa: F821
  g_vol.setVisAttributes(description, 'InvisibleWithDaughters')
  de = DetElement(description, det.name, det.type, det.id)  # noqa: F821
  phv = description.worldVolume().placeVolume(g_vol, Position(g_x, g_y, g_z))  # noqa: F821
  phv.addPhysVolID('id', det.id)
  de.addPlacement(phv)
  x, y, z = 0, book.y + plane.y - 2 * plane.getF('dy'), 0
  for n in range(plane.number):
    g_vol.placeVolume(e_vol, Position(x, y, z))  # noqa: F821
    y += plane.getF('dy')
  # ---Return detector element---------------------------------------------------------------------------------
  return de
