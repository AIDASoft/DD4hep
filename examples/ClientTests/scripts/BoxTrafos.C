// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//  ROOT interpreted macro to test world to local and local to world
//  transformations using 2 simple boxes.
//
//  usage: $> root.exe <path>/BoxTrafos.C
//
//
//====================================================================

namespace {
  struct Loader {  Loader() { gSystem->Load("libDDCore"); }  } _load;
}

using namespace dd4hep::detail;

void printPos(const char* com, const Position& p)  {
  printf("%-24s:  %7.3f %7.3f %7.3f\n",com,p.x(),p.y(),p.z());
}

void printCoord(const Position& global, const Position& local)  {
  printPos("Global",global);
  printPos("Local",local);
}

void local_to_world(const char* com,const DetElement de, const Position& pos,const Position& loc)  {
  Position glob;
  printf("Local to World: Transformation '%s'(%7.3f, %7.3f, %7.3f)->world (Should be: (%7.3f, %7.3f, %7.3f) :\n",
	 com, loc.x(),loc.y(),loc.z(),
	 loc.x()+pos.x(),loc.y()+pos.y(),loc.z()+pos.z() );
  de.localToWorld(loc,glob);
  printCoord(glob,loc);
}

void world_to_local(const char* com,const DetElement de, const Position& pos,const Position& glob)  {
  Position loc;
  printf("World to Local: Transformation '%s'(%7.3f, %7.3f, %7.3f)->world (Should be: (%7.3f, %7.3f, %7.3f) :\n",
	 com, glob.x(),glob.y(),glob.z(),
	 glob.x()-pos.x(),glob.y()-pos.y(),glob.z()-pos.z() );
  de.worldToLocal(glob,loc);
  printCoord(glob,loc);
}

int BoxTrafos()  {
  string xml = "file:";
  xml += gSystem->Getenv("DD4hepINSTALL");
  xml += "/examples/ClientTests/compact/BoxTrafos.xml";
  const char* argv[] = {xml.c_str(), "BUILD_DEFAULT", 0};

  gSystem->Load("libDDCore");
  Detector& description = Detector::getInstance();
  description.apply("dd4hepCompactLoader",2,(char**)argv);
  description.apply("dd4hepGeometryDisplay",0,0);


  DetElement de = description.detector("B3");
  PlacedVolume pv = de.placement();
  Volume vol = pv.volume();
  Solid solid = vol.solid();
  TGeoBBox* box = (TGeoBBox*)(solid.ptr());
  Position glob,loc, pos(-10,30,10);

  printf("\n++++  local->world:\n\n");

  loc = Position(-pos.x(),-pos.y(),-pos.z());
  local_to_world("origine",de,pos,loc);

  loc = Position();
  local_to_world("center",de,pos,loc);

  loc = Position(box->GetDX(),box->GetDY(),box->GetDZ());
  local_to_world("top edge",de,pos,loc);

  loc = Position(box->GetDX(),box->GetDY(),-box->GetDZ());
  local_to_world("top edge",de,pos,loc);

  loc = Position(-box->GetDX(),box->GetDY(),box->GetDZ());
  local_to_world("top edge",de,pos,loc);

  loc = Position(-box->GetDX(),box->GetDY(),-box->GetDZ());
  local_to_world("top edge",de,pos,loc);

  loc = Position(box->GetDX(),-box->GetDY(),box->GetDZ());
  local_to_world("bottom edge",de,pos,loc);

  loc = Position(box->GetDX(),-box->GetDY(),-box->GetDZ());
  local_to_world("bottom edge",de,pos,loc);

  loc = Position(-box->GetDX(),-box->GetDY(),box->GetDZ());
  local_to_world("bottom edge",de,pos,loc);

  loc = Position(-box->GetDX(),-box->GetDY(),-box->GetDZ());
  local_to_world("bottom edge",de,pos,loc);

  printf("\n++++  world->local:\n\n");

  glob = Position(0,0,0);
  world_to_local("world center",de,pos,glob);

  glob = Position(pos.x(),pos.y(),pos.z());
  world_to_local("position",de,pos,glob);

  glob = Position( box->GetDX()+pos.x(),  box->GetDY()+pos.y(),  box->GetDZ()+pos.z());
  world_to_local("top edge",de,pos,glob);

  glob = Position( box->GetDX()+pos.x(),  box->GetDY()+pos.y(), -box->GetDZ()+pos.z());
  world_to_local("top edge",de,pos,glob);

  glob = Position(-box->GetDX()+pos.x(),  box->GetDY()+pos.y(),  box->GetDZ()+pos.z());
  world_to_local("top edge",de,pos,glob);

  glob = Position(-box->GetDX()+pos.x(),  box->GetDY()+pos.y(), -box->GetDZ()+pos.z());
  world_to_local("top edge",de,pos,glob);

  glob = Position( box->GetDX()+pos.x(), -box->GetDY()+pos.y(),  box->GetDZ()+pos.z());
  world_to_local("bottom edge",de,pos,glob);

  glob = Position( box->GetDX()+pos.x(), -box->GetDY()+pos.y(), -box->GetDZ()+pos.z());
  world_to_local("bottom edge",de,pos,glob);

  glob = Position(-box->GetDX()+pos.x(), -box->GetDY()+pos.y(),  box->GetDZ()+pos.z());
  world_to_local("bottom edge",de,pos,glob);

  glob = Position(-box->GetDX()+pos.x(), -box->GetDY()+pos.y(), -box->GetDZ()+pos.z());
  world_to_local("bottom edge",de,pos,glob);

  return 1;
}
