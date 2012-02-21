from ROOT import DD4hep

#-------------------------------------------------------------------------------------      
def detector_ILDExTPC(lcdd, det):
  tube     = det.find('tubs')
  material = det.find('material')
  tpc_de   = DD4hep.ILDExTPC(lcdd, det.name, det.type, det.id)
  tpc_tube = Tube(lcdd, det.name+'_tube', tube.rmin, tube.rmax, tube.zhalf) 
  tpc_vol  = Volume(lcdd, det.name+'_envelope', tpc_tube, lcdd.material(material.name))
  
  for px in det.findall('detector'):
    px_tube = px.find('tubs')
    px_pos  = px.find('position')
    px_rot  = px.find('rotation')
    px_mat  = px.find('material')
    part_det  = DetElement(lcdd, px.name, px.type, px.id)
    part_tube = Tube(lcdd,px.name+'_tube',px_tube.rmin, px_tube.rmax, px_tube.zhalf)
    part_vol  = Volume(lcdd, px.name, part_tube, lcdd.material(px_mat.name))
    print 'Vis for %s ' %det.name, px.vis
    part_vol.setVisAttributes(lcdd.visAttributes(px.vis))
    part_det.addPlacement(tpc_vol.placeVolume(part_vol, getPosition(px_pos), getRotation(px_rot)))
    if   px.id == 0 : tpc_de.setInnerWall(part_det)
    elif px.id == 1 : tpc_de.setOuterWall(part_det)
    elif px.id == 5 : tpc_de.setGasVolume(part_det)
    tpc_de.add(part_det)
  print 'vis = ', det.vis  
  tpc_vol.setVisAttributes(lcdd.visAttributes(det.vis))
  lcdd.pickMotherVolume(tpc_de).placeVolume(tpc_vol)
  #--additonal TPC data-----------------------------
  mod = det.find('module')
  tpc_de.setNModules(mod.getI('number'))
  tpc_de.setDriftLength(mod.getF('driftlength'))

  return tpc_de
