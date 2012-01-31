#-------------------------------------------------------------------------------------      
def detector_ILDExVXD(lcdd, det):
  doc    = lcdd.document()
  name   = det.get('name')
  vdx    = Subdetector(doc, det.get('name'), det.get('type'), det.getI('id'))
  
  mother = lcdd.pickMotherVolume(vdx);
  for layer in det.findall('layer'):
    support = layer.find('support')
    ladder = layer.find('ladder')
    layername = name + '_layer%d' % layer.getI('id')
    nLadders = ladder.getI('number')
    dphi   = 2.*pi/nLadders
    zhalf  = ladder.getF('zhalf')
    offset = ladder.getF('offset')
    sens_radius = ladder.getF('radius')
    sens_thick  = ladder.getF('thickness')
    supp_thick  = support.getF('thickness')
    supp_radius = sens_radius + sens_thick/2. + supp_thick/2.
    width       = 2.*tan(dphi/2.)*(sens_radius-sens_thick/2.)
    sens_mat    = lcdd.material(ladder.get('material'))
    supp_mat    = lcdd.material(support.get('material'))
    ladderbox = Box(doc, layername+'_ladder_solid',  (sens_thick+supp_thick)/2.,width/2.,zhalf)
    laddervol = Volume(doc, layername+'_ladder_volume', ladderbox, sens_mat)
    sensbox   = Box(doc, layername+'_sens_solid', sens_thick/2.,width/2.,zhalf)
    sensvol   = Volume(doc, layername+'_sens_volume', sensbox, sens_mat)
    senspos   = Position(doc,layername+'_sens_position',-(sens_thick+supp_thick)/2.+sens_thick/2.,0,0)
    suppbox   = Box(doc, layername+'_supp_solid', supp_thick/2.,width/2.,zhalf)
    suppvol   = Volume(doc,layername+'_supp_volume', suppbox,supp_mat)
    supppos   = Position(doc,layername+'_supp_position',-(sens_thick+supp_thick)/2.+sens_thick/2.+supp_thick/2.,0,0)
    
    lcdd.add(ladderbox).add(sensbox).add(senspos).add(suppbox).add(supppos)
    lcdd.add(laddervol).add(sensvol).add(suppvol)
    
    laddervol.setVisAttributes(lcdd.visAttributes(layer.get('vis')))
    suppvol.setVisAttributes(lcdd.visAttributes(layer.get('vis')))
    sensvol.setVisAttributes(lcdd.visAttributes(support.get('vis')))
    
    laddervol.addPhysVol(PhysVol(sensvol),senspos)
    laddervol.addPhysVol(PhysVol(suppvol),supppos)
    
    for j in range(nLadders):
      laddername = layername + '_ladder%d' % j
      radius = sens_radius + ((sens_thick+supp_thick)/2. - sens_thick/2.)
      rot = Rotation(doc,laddername+'_rotation',0,0,j*dphi)
      pos = Position(doc,laddername+'_position',
                     radius*cos(j*dphi) - offset*sin(j*dphi),
                     radius*sin(j*dphi) - offset*cos(j*dphi),0.)
      lcdd.add(rot).add(pos)
      ladder_physvol = PhysVol(laddervol)
      mother.addPhysVol(ladder_physvol,pos,rot)
  
  vdx.setVisAttributes(lcdd, det.get('vis'), laddervol);
  return vdx 
