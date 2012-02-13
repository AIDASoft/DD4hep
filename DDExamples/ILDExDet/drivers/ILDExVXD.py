#-------------------------------------------------------------------------------------      
def detector_ILDExVXD(lcdd, det):
  name   = det.get('name')
  vdx    = DetElement(lcdd, det.get('name'), det.get('type'), det.getI('id'))
  
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
    ladderbox = Box(lcdd, layername+'_ladder_solid',  (sens_thick+supp_thick)/2.,width/2.,zhalf)
    laddervol = Volume(lcdd, layername+'_ladder_volume', ladderbox, sens_mat)
    sensbox   = Box(lcdd, layername+'_sens_solid', sens_thick/2.,width/2.,zhalf)
    sensvol   = Volume(lcdd, layername+'_sens_volume', sensbox, sens_mat)
    senspos   = Position(lcdd,layername+'_sens_position',-(sens_thick+supp_thick)/2.+sens_thick/2.,0,0)
    suppbox   = Box(lcdd, layername+'_supp_solid', supp_thick/2.,width/2.,zhalf)
    suppvol   = Volume(lcdd,layername+'_supp_volume', suppbox,supp_mat)
    supppos   = Position(lcdd,layername+'_supp_position',-(sens_thick+supp_thick)/2.+sens_thick/2.+supp_thick/2.,0,0)
    
    lcdd.add(ladderbox).add(sensbox).add(senspos).add(suppbox).add(supppos)
    lcdd.add(laddervol).add(sensvol).add(suppvol)
    
    laddervol.setVisAttributes(lcdd.visAttributes(layer.get('vis')))
    suppvol.setVisAttributes(lcdd.visAttributes(layer.get('vis')))
    sensvol.setVisAttributes(lcdd.visAttributes(support.get('vis')))
    
    laddervol.addPhysVol(sensvol,senspos)
    laddervol.addPhysVol(suppvol,supppos)
    
    for j in range(nLadders):
      laddername = layername + '_ladder%d' % j
      radius = sens_radius + ((sens_thick+supp_thick)/2. - sens_thick/2.)
      rot = Rotation(lcdd,laddername+'_rotation',0,0,j*dphi)
      pos = Position(lcdd,laddername+'_position',
                     radius*cos(j*dphi) - offset*sin(j*dphi),
                     radius*sin(j*dphi) - offset*cos(j*dphi),0.)
      lcdd.add(rot).add(pos)
      mother.addPhysVol(laddervol,pos,rot)
  
  vdx.setVisAttributes(lcdd, det.get('vis'), laddervol);
  return vdx 
