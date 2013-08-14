#-------------------------------------------------------------------------------------      
def detector_ILDExVXD(lcdd, det):
  vdx = DetElement(det.name, det.type, det.id)
  mother = lcdd.worldVolume()
  for layer in det.findall('layer'):
    support = layer.find('support')
    ladder  = layer.find('ladder')
    layername = det.name + '_layer%d' % layer.id
    nLadders = ladder.getI('number')
    dphi   = 2.*pi/nLadders
    sens_thick  = ladder.thickness
    supp_thick  = support.thickness
    supp_radius = ladder.radius + sens_thick/2. + supp_thick/2.
    width       = 2.*tan(dphi/2.)*(ladder.radius-sens_thick/2.)
    
    ladderbox = Box(layername+'_ladder_box',  (sens_thick+supp_thick)/2., width/2., ladder.zhalf)
    laddervol = Volume(layername+'_ladder', ladderbox, lcdd.material('Air'))
    
    sensbox   = Box(layername+'_sens_box', sens_thick/2., width/2., ladder.zhalf)
    sensvol   = Volume(layername+'_sens', sensbox, lcdd.material(ladder.material))
    sensvol.setVisAttributes(lcdd.visAttributes(layer.vis))
    laddervol.placeVolume(sensvol, Position(-(sens_thick+supp_thick)/2.+sens_thick/2.,0,0))
    
    suppbox   = Box(layername+'_supp_box', supp_thick/2.,width/2.,ladder.zhalf)
    suppvol   = Volume(layername+'_supp', suppbox, lcdd.material(support.material))
    suppvol.setVisAttributes(lcdd.visAttributes(support.vis))
    laddervol.placeVolume(suppvol, Position(-(sens_thick+supp_thick)/2.+sens_thick/2.+supp_thick/2.,0,0))
    
    for j in range(nLadders):
      laddername = layername + '_ladder%d' % j
      radius = ladder.radius + ((sens_thick+supp_thick)/2. - sens_thick/2.)
      rot = Rotation(0,0,j*dphi)
      pos = Position(radius*cos(j*dphi) - ladder.offset*sin(j*dphi),
                     radius*sin(j*dphi) - ladder.offset*cos(j*dphi),0.)
      mother.placeVolume(laddervol, pos, rot)
  return vdx 
