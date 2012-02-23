#-------------------------------------------------------------------------------------      
def detector_ILDExSIT(lcdd, det):
  sit = DetElement(lcdd, det.name, det.type, det.id)
  mother = lcdd.worldVolume()
  for layer in det.findall('layer'):
    support = layer.find('support')
    ladder  = layer.find('ladder')
    layername = det.name + '_layer%d'%layer.id
    nLadders = ladder.number
    dphi   = 2.*pi/nLadders
    zhalf  = ladder.zhalf
    sens_radius = ladder.radius
    sens_thick  = ladder.thickness
    supp_thick  = support.thickness
    support_radius = sens_radius + sens_thick/2. + supp_thick/2.
    width       = 2.*tan(dphi/2.)*(sens_radius-sens_thick/2.)

    ladderbox = Box(lcdd, layername+'_ladder_box',  (sens_thick+supp_thick)/2.,width/2.,zhalf)
    laddervol = Volume(lcdd, layername+'_ladder', ladderbox, lcdd.material('Air'))
    laddervol.setVisAttributes(lcdd.visAttributes(layer.vis))
    
    sensbox   = Box(lcdd, layername+'_sens_box', sens_thick/2.,width/2.,zhalf)
    sensvol   = Volume(lcdd, layername+'_sens_volume', sensbox, lcdd.material(ladder.material))
    sensvol.setVisAttributes(lcdd.visAttributes(layer.vis))
    laddervol.placeVolume(sensvol, Position(-(sens_thick+supp_thick)/2.+sens_thick/2.,0,0))
    
    suppbox   = Box(lcdd, layername+'_supp_box', supp_thick/2.,width/2.,zhalf)
    suppvol   = Volume(lcdd,layername+'_supp', suppbox,lcdd.material(support.material))
    suppvol.setVisAttributes(lcdd.visAttributes(support.vis))
    laddervol.placeVolume(suppvol, Position(-(sens_thick+supp_thick)/2.+sens_thick/2.+supp_thick/2.,0,0))
    
    for j in range(nLadders):
      laddername = layername + '_ladder%d' % j
      radius = sens_radius + ((sens_thick+supp_thick)/2. - sens_thick/2.)
      mother.placeVolume(laddervol, Position(radius*cos(j*dphi),radius*sin(j*dphi),0.), Rotation(0,0,j*dphi))
  return sit 
