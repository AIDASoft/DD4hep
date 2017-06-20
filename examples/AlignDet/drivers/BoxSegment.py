def detector_BoxSegment(description, det):
  box    = det.find('box')
  mat    = det.find('material')
  pos    = det.find('position')
  rot    = det.find('rotation')
  mother = description.worldVolume()
  de     = DetElement(description, det.name, det.type, det.id)
  sha    = Box(description, det.name+'_envelope', box.x, box.y, box.z)
  vol    = Volume(description, det.name+'_envelope_volume', sha, description.material(mat.name))
  phv    = mother.placeVolume(vol, Position(pos.x, pos.y, pos.z), 
                                   Rotation(rot.x, rot.y, rot.z))
  vol.setVisAttributes(description, det.vis)
  phv.addPhysVolID('id',det.id)
  de.addPlacement(phv)
  return de
