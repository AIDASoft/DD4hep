// $Id: $
//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DDEve/DDEveEventData.h"

using namespace dd4hep;

/// Default constructor
DDEveHit::DDEveHit()
  : particle(0), x(0.0), y(0.0), z(0.0), deposit(0.0) 
{
}

/// Initializing constructor
DDEveHit::DDEveHit(int p, float xx, float yy, float zz, float d) 
  : particle(p), x(xx), y(yy), z(zz), deposit(d) 
{
}

/// Copy constructor
DDEveHit::DDEveHit(const DDEveHit& c)
  : particle(c.particle), x(c.x), y(c.y), z(c.z), deposit(c.deposit) 
{
}

/// Default destructor
DDEveHit::~DDEveHit()  {
}

/// Assignment operator
DDEveHit& DDEveHit::operator=(const DDEveHit& c)  {
  if ( this != &c )  {
    x = c.x;
    y = c.y;
    z = c.z;
    deposit = c.deposit;
  }
  return *this;
}

/// Copy constructor
DDEveParticle::DDEveParticle(const DDEveParticle& c)
  : id(c.id), parent(c.parent), pdgID(c.pdgID),
    vsx(c.vsx), vsy(c.vsy), vsz(c.vsz), vex(c.vex), vey(c.vey), vez(c.vez), 
    psx(c.psx), psy(c.psy), psz(c.psz), 
    energy(c.energy), time(c.time), daughters(c.daughters)	{
}

/// Default constructor
DDEveParticle::DDEveParticle()
  : id(0), parent(0), pdgID(0),
    vsx(0.0), vsy(0.0), vsz(0.0), vex(0.0), vey(0.0), vez(0.0), 
    psx(0.0), psy(0.0), psz(0.0), 
    energy(0.0), time(0.0), daughters()	{
}

/// Default destructor
DDEveParticle::~DDEveParticle()  
{
}

/// Assignment operator
DDEveParticle& DDEveParticle::operator=(const DDEveParticle& c)   {
  if ( this != &c )  {
    id = c.id; 
    parent = c.parent;
    pdgID = c.pdgID;
    vsx = c.vsx;
    vsy = c.vsy;
    vsz = c.vsz; 
    vex = c.vex;
    vey = c.vey;
    vez = c.vez; 
    psx = c.psx;
    psy = c.psy;
    psz = c.psz; 
    energy = c.energy;
    time = c.time;
    daughters = c.daughters;
  }
  return *this;
}


