from ROOT import DD4hep

#-------------------------------------------------------------------------------------      
def detector_ILDExTPC(lcdd, det):
  name     = det.get('name')
  mat      = lcdd.material(det.find('material').get('name'))
  tube     = det.find('tubs')
  tpc      = DD4hep.ILDExTPC(lcdd, det.get('name'), det.get('type'), det.getI('id'))
  tpc_tube = Tube(lcdd,name+'_envelope', tube.getF('rmin'), tube.getF('rmax'), tube.getF('zhalf')) 
  tpc_vol  = Volume(lcdd, name+'_envelope_volume', tpc_tube, mat)
  lcdd.add(tpc_tube).add(tpc_vol);
  tpc.setEnvelope(tpc_tube).setVolume(tpc_vol)
  
  for px_det in det.findall('detector'):
    px_tube = px_det.find('tubs')
    px_pos  = px_det.find('position')
    px_rot  = px_det.find('rotation')
    part_name = px_det.get('name')
    part_mat  = lcdd.material(px_det.find('material').get('name'))
    part_det  = DetElement(lcdd,part_name,px_det.get('type'),px_det.getI('id'))
    part_tube = Tube(lcdd,part_name+'_tube',px_tube.getF('rmin'),px_tube.getF('rmax'),px_tube.getF('zhalf'))
    part_pos  = Position(lcdd,part_name+'_position',px_pos.getF('x'),px_pos.getF('y'),px_pos.getF('z'))
    part_rot  = Rotation(lcdd,part_name+'_rotation',px_pos.getF('x'),px_pos.getF('y'),px_pos.getF('z'))
    part_vol  = Volume(lcdd,part_name,part_tube,part_mat)
    
    part_det.setVolume(part_vol).setEnvelope(part_tube)
    part_det.setVisAttributes(lcdd,px_det.get('vis'), part_vol)
    lcdd.add(part_tube).add(part_pos).add(part_rot).add(part_vol)
    
    tpc_vol.addPhysVol(part_vol,part_pos,part_rot)
    
    if   part_det.id() == 0 : tpc.setInnerWall(part_det)
    elif part_det.id() == 1 : tpc.setOuterWall(part_det)
    elif part_det.id() == 2 : tpc.setGasVolume(part_det)
    tpc.add(part_det)
    
  tpc.setVisAttributes(lcdd, det.get('vis'), tpc_vol)
  lcdd.pickMotherVolume(tpc).addPhysVol(tpc_vol,lcdd.identity())
  return tpc
