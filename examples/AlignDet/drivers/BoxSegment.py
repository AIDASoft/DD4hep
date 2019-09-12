from __future__ import absolute_import, unicode_literals


def detector_BoxSegment(description, det):
  box = det.find('box')
  mat = det.find('material')
  pos = det.find('position')
  rot = det.find('rotation')
  mother = description.worldVolume()
  de = DetElement(description, det.name, det.type, det.id)  # noqa: F821
  sha = Box(description, det.name + '_envelope', box.x, box.y, box.z)  # noqa: F821
  vol = Volume(description, det.name + '_envelope_volume', sha, description.material(mat.name))  # noqa: F821
  phv = mother.placeVolume(vol, Position(pos.x, pos.y, pos.z),  # noqa: F821
                           Rotation(rot.x, rot.y, rot.z))  # noqa: F821
  vol.setVisAttributes(description, det.vis)
  phv.addPhysVolID('id', det.id)
  de.addPlacement(phv)
  return de
