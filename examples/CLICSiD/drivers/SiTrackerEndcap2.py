def detector_SiTrackerEndcap2(lcdd, det):
  sdet    = DetElement(lcdd, det.name, det.type, det.id)
  mother  = lcdd.trackingVolume()
  modules = {}
  #---Loop over modules------------------------------------------------------------------
  for mod in det.findall('module'):
    trd   = mod.find('trd')
    comps = mod.findall('module_component')
    total_thickness = 0
    for c in comps: total_thickness += c.thickness
    m_envelope = Trapezoid(lcdd, mod.name+'Trd', trd.x1, trd.x2, 
                           total_thickness/2, total_thickness/2, trd.z)
    m_volume   = Volume(lcdd, mod.name+'Volume', m_envelope, lcdd.material('Vacuum'))
    if mod.vis : m_volume.setVisAttributes(lcdd.visAttributes(mod.vis))
    #---Loop open components-----------------------------------------------------------
    n_sensor, c_id, posY = 0, 0, -total_thickness/2 
    for c in comps:
      c_name =  mod.name+'_component%d'%c_id
      shp = Trapezoid(lcdd, c_name+"_trd", trd.x1, trd.x2, c.thickness/2e0, c.thickness/2e0, trd.z)
      vol = Volume(lcdd, c_name, shp, lcdd.material(c.material))
      if mod.vis : vol.setVisAttributes(lcdd.visAttributes(mod.vis))
      phv = m_volume.placeVolume(vol, Position(0e0, posY + c.thickness/2e0, 0e0))
      phv.addPhysVolID('component', c_id)
      if c.getB('sensitive'):
        if n_sensor > 1 : print 'SiTrackerEndcap2::fromCompact: '+c_name+' Max of 2 modules allowed!'
        phv.addPhysVolID('sensor',c_id)
        #vol.setSensitiveDetector(sens)
        ++n_sensor
      posY += c.thickness
      c_id += 1
    modules[mod.name] = m_volume
  #--Loop over Layers------------------------------------------------------------------
  for layer in det.findall('layer'):
    mod_num = 0
    for ring in layer.findall('ring'):
      nmodules = ring.getI('nmodules')
      m_nam = ring.get('module')
      m_vol = modules[m_nam]
      iphi, phi = 2*pi/nmodules, ring.getF('phi0',0)
      for k in range(nmodules):
        m_base = det.name+'_layer%d'%layer.id+'_module%d'%mod_num
        x = ring.r * cos(phi)
        y = ring.r * sin(phi)
        pv = mother.placeVolume(m_vol, Position(x,y,ring.zstart+ring.dz),
                                       Rotation(-pi/2,-pi/2+phi,0))
        pv.addPhysVolID('system',det.id).addPhysVolID('barrel',1)
        pv.addPhysVolID('layer', layer.id).addPhysVolID('module',mod_num)
        module = DetElement(lcdd, m_base, det.type+"/Module", det.id)
        module.addPlacement(pv)
        sdet.add(module)
        if det.getB('reflect'):
          pv = mother.placeVolume(m_vol, Position(x,y,-ring.zstart-ring.dz),
                                         Rotation(-pi/2,-pi/2-phi,pi))
          pv.addPhysVolID('system',det.id).addPhysVolID('barrel',2)
          pv.addPhysVolID('layer', layer.id).addPhysVolID('module',mod_num)
          r_module = DetElement(lcdd, m_base+'_reflect', det.type+"/Module", det.id)
          r_module.addPlacement(pv)
          sdet.add(r_module)
        phi += iphi
        mod_num += 1            
  return sdet

