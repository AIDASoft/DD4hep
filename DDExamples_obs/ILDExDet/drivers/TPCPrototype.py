from ROOT import DD4hep

#-------------------------------------------------------------------------------------
def detector_TPCPrototype(lcdd, det):
  tube     = det.find('tubs')
  material = det.find('material')
  tpc      = DetElement(det.name, det.type, det.id, DD4hep.TPCData())
  tpcdata  = tpc._data();
  
  tpc_tube = Tube(tube.rmin, tube.rmax, tube.zhalf)
  tpc_vol  = Volume(det.name+'_envelope', tpc_tube, lcdd.material(material.name))
  
  for px in det.findall('detector'):
    px_tube = px.find('tubs')
    px_pos  = px.find('position')
    px_rot  = px.find('rotation')
    px_mat  = px.find('material')
    part_det  = DetElement(px.name, px.type, px.id)
    part_tube = Tube(px_tube.rmin, px_tube.rmax, px_tube.zhalf)
    part_mat  = lcdd.material(px_mat.name)
    part_vol  = Volume(px.name, part_tube, part_mat)
    #part_vol.setSensitiveDetector(sens)
    part_vol.setVisAttributes(lcdd.visAttributes(px.vis))
    if   px.id == 2 : tpcdata.innerWall = part_det
    elif px.id == 3 : tpcdata.outerWall = part_det
    elif px.id == 4 : tpcdata.gasVolume = part_det
    elif px.id == 5 : tpcdata.cathode = part_det
    #---Endplate-------------------------------------
    if px.id == 0 :
      tpcdata.endplate = part_det
      mdcount = 0
      for modules in px.findall('modules'):
        
        for row in modules.findall('row') :
          nmodules = row.getI('nModules')
          rowID = row.getI('RowID')
          pitch = row.getF('modulePitch')
          width = row.getF('moduleWidth')
          height = row.getF('moduleHeight')
          zhalf=px_tube.zhalf
          mr_name = modules.name + '_Row%d' % rowID
          mr_vol = Volume(mr_name, Box(width/2,height/2,zhalf), part_mat)
          mr_mat = lcdd.material(px_mat.name)
          xml_pads = lcdd.readout(row.get('pads'))
          
          for md in range(nmodules):
            m_name = modules.name + '_Row%d_M%d' % (rowID,md)
            module = DetElement(part_det, m_name, mdcount)
            mdcount += 1
            posx = row.getF('modulePosX') + md*(width/2+pitch)
            posy = row.getF('modulePosY')
            m_phv = part_vol.placeVolume(mr_vol, Position(posx,posy,0), Rotation(0,0,0))
            m_phv.addPhysVolID('module',md)
            module.setPlacement(m_phv)
            module.setReadout(xml_pads)
    #-->module.addExtension<PadLayout>(new FixedPadAngleDiskLayout(module))
  
    part_phv = tpc_vol.placeVolume(part_vol, getPosition(px_pos), getRotation(px_rot))
    part_phv.addPhysVolID(px.name, px.id)
    part_det.setPlacement(part_phv)
    tpc.add(part_det)
    #--- now reflect it
    if px.getB('reflect'):
      r_pos = Position(px_pos.x, px_pos.y, -px_pos.z)
      r_rot = Rotation(pi, 0, pi)
      part_phv2 = tpc_vol.placeVolume(part_vol, r_pos, r_rot)
      part_phv2.addPhysVolID(px.name+"_negativ",px.id+1);
      rdet = part_det.clone(px.name+"_negativ",px.id+1)
      rdet.setPlacement(part_phv2)
      tpcdata. endplate2 = rdet
      tpc.add(rdet)

  tpc_vol.setVisAttributes(lcdd.visAttributes(det.vis))
  phv = lcdd.worldVolume().placeVolume(tpc_vol)
  tpc.setPlacement(phv)
  return tpc
