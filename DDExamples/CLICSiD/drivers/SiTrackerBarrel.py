def getRotation(rot):
  return Rotation(rot.getF('x',0.0),rot.getF('y',0.0), rot.getF('z',0.0))

def getPosition(pos):
  return Position(pos.getF('x',0.0),pos.getF('y',0.0), pos.getF('z',0.0))

def detector_SiTrackerBarrel(lcdd, det):
  sdet    = DetElement(lcdd, det.name, det.type, det.id)
  mother  = lcdd.trackingVolume()

  #---Loop over modules------------------------------------------------------------------
  for mod in det.findall('module'):
    env = mod.find('module_envelope')
    m_nam = det.name+'_'+mod.name
    m_box = Box(lcdd, m_nam+'_box', env.width, env.length, env.thickness)
    m_vol = Volume(lcdd, m_nam, m_box, lcdd.material('Air'))
    m_de  = DetElement(lcdd,m_nam,det.type+'/Module',det.id)
    
    #---Loop over components-----------------------------------------------------------
    n_comp, n_sensor = 0, 0
    for c in mod.findall('module_component'):
      c_nam = m_nam+'_component%d' % n_comp
      c_box = Box(lcdd, c_nam+'_box', c.width/2, c.length/2, c.thickness/2)
      c_vol = Volume(lcdd, c_nam, c_box, lcdd.material(c.material))
      c_de  = DetElement(lcdd,c_nam, det.type+'/Module/Component', det.id)
      pos = c.find('position')
      rot = c.find('rotation')
      if pos and rot :
        c_phv = m_vol.placeVolume(c_vol, getPosition(pos), getRotation(rot))
      elif pos :
        c_phv = m_vol.placeVolume(c_vol, getPosition(pos))
      elif rot :
        c_phv = m_vol.placeVolume(c_vol, getRotation(rot))
      else:
        c_phv = m_vol.placeVolume(c_vol)
      if c.getB('sensitive'):
        c_phv.addPhysVolID('sensor',n_sensor)
        #c_vol.setSensitiveDetector(sens)
        n_sensor += 1
      c_de.setAttributes(lcdd, c_vol, c.get('region',''), c.get('limits',''), mod.get('vis',''))
      m_de.add(c_de)
      n_comp += 1
    #--Endloop components------------------------------------------------------------
    m_vol.setVisAttributes(lcdd.visAttributes(mod.vis))
    sdet.add(m_de)
  #---Loop over layers---------------------------------------------------------------
  for layer in det.findall('layer'):
    barrel   = layer.find('barrel_envelope')
    x_layout = layer.find('rphi_layout')
    z_layout = layer.find('z_layout')
    m_nam    = det.name+'_'+layer.module
    m_de     = sdet.child(m_nam)
    m_vol    = lcdd.volume(m_nam);
    lay_nam  = det.name+'_layer%d'%layer.id
    lay_tub  = Tube(lcdd, lay_nam+'_tube', barrel.inner_r, barrel.outer_r, barrel.z_length)
    lay_vol  = Volume(lcdd, lay_nam,lay_tub,  lcdd.material(c.material))
    phi0     = x_layout.getF('phi0')      # Starting phi of first module.
    phi_tilt = x_layout.getF('phi_tilt')  # Phi tilt of a module.
    rc       = x_layout.getF('rc')        # Radius of the module center.
    nphi     = x_layout.getI('nphi')      # Number of modules in phi.
    rphi_dr  = x_layout.getF('dr')        # The delta radius of every other module.
    phi_incr = (pi * 2) / nphi            # Phi increment for one module.
    phic     = phi0                       # Phi of the module center.
    z0       = z_layout.getF('z0')        # Z position of first module in phi.
    nz       = z_layout.getF('nz')        # Number of modules to place in z.
    z_dr     = z_layout.getF('dr')        # Radial displacement parameter, of every other module.
      
    # Z increment for module placement along Z axis.
    # Adjust for z0 at center of module rather than the end of cylindrical envelope.
    z_incr   = nz > 1 and (2.0 * z0) / (nz - 1) or 0.0
    # Starting z for module placement along Z axis.
    module_z = -z0
    module = 0
      
    #---Loop over the number of modules in phi.
    for ii in range(nphi):
      dx = z_dr * cos(phic + phi_tilt)	# Delta x of module position.
      dy = z_dr * sin(phic + phi_tilt)	# Delta y of module position.
      x = rc * cos(phic)                # Basic x module position.
      y = rc * sin(phic)               	# Basic y module position.
        
      #---Loop over the number of modules in z.
      for j in range(nz) :
        # Create a unique name for the module in this logical volume, layer, phi, and z.
        m_place = lay_nam + '_phi%d'%ii + '_z%d'%j
        z = module_z;
        # Module PhysicalVolume.
        m_physvol = lay_vol.placeVolume(m_vol,Position(x,y,z),
                                              Rotation(pi/2,-((pi/2)-phic-phi_tilt),0))
        m_physvol.addPhysVolID('module', module)
        module += 1
          
        # Adjust the x and y coordinates of the module.
        x += dx
        y += dy
        # Flip sign of x and y adjustments.
        dx *= -1
        dy *= -1
        # Add z increment to get next z placement pos.
        module_z += z_incr
      # Increment the phi placement of module.
      phic += phi_incr
      # Increment the center radius according to dr parameter.
      rc += rphi_dr
      # Flip sign of dr parameter.
      rphi_dr *= -1
      # Reset the Z placement parameter for module.
      module_z = -z0
      m_de.setAttributes(lcdd,lay_vol,layer.get('region',''), layer.get('limits',''), layer.get('vi',''))
      
  # Create the PhysicalVolume for the layer.
  lpv = mother.placeVolume(lay_vol)       # Place layer in mother
  lpv.addPhysVolID('system', det.id)      # Set the subdetector system ID.
  lpv.addPhysVolID('barrel', 0)           # Flag this as a barrel subdetector.
  lpv.addPhysVolID('layer', layer.id)       # Set the layer ID.    
  return sdet
