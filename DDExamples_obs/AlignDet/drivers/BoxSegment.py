def detector_BoxSegment(lcdd, det):
  box    = det.find('box')
  mat    = det.find('material')
  pos    = det.find('position')
  rot    = det.find('rotation')
  mother = lcdd.worldVolume()
  de     = DetElement(lcdd, det.name, det.type, det.id)
  sha    = Box(lcdd, det.name+'_envelope', box.x, box.y, box.z)
  vol    = Volume(lcdd, det.name+'_envelope_volume', sha, lcdd.material(mat.name))
  phv    = mother.placeVolume(vol, Position(pos.x, pos.y, pos.z), 
                                   Rotation(rot.x, rot.y, rot.z))
  vol.setVisAttributes(lcdd, det.vis)
  phv.addPhysVolID('id',det.id)
  de.addPlacement(phv)
  return de
