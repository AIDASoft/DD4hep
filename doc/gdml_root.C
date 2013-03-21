#include <fstream>
#include <cstring>
#include <cerrno>
#include <string>
#include <sstream>
#include <map>

#include <TGeoManager.h>
#include <TColor.h>
#include <TObjArray.h>
#include <TGeoVolume.h>
#include <TGDMLParse.h>

using namespace std;

TGeoVolume* gdml_root(const char* sys_name) {
  bool debug_processing = false;
  TGeoManager* geo = new TGeoManager();
  string system    = sys_name;
  string gdml_file = system+".gdml";
  string vis_file  = system+".vis.csv";
  map<string,TGeoVolume*> vol_map;
  ifstream in(vis_file.c_str());
  TGDMLParse parse;

  cout << "++ Processing gdml file:" << gdml_file << endl;

  TGeoVolume* top_vol = parse.GDMLReadFile(gdml_file.c_str());
  TObjArray*  vols    = gGeoManager->GetListOfVolumes();
  for(int i=0;i<vols->GetEntries();++i) {
    TGeoVolume* v=(TGeoVolume*)vols->At(i);
    vol_map.insert(make_pair(v->GetName(),v));
  }

  if ( !in.good() )
    cout << "++ Failed to open visualization file:" << vis_file 
	 << " :: " << strerror(errno) << endl;
  else
    cout << "++ Processing visualization file:" << vis_file << endl;

  while(in.good() ) {
    stringstream debug;
    char   text[1024], *line;
    const char* vol_name="";
    float  red = 0, blue = 0, green = 0;
    bool   visible = false, show_daughters = false;
    string line_style="solid", drawing_style="solid";
    map<string,TGeoVolume*>::iterator vit;

    in.getline(text,sizeof(text),'\n');
    line = strtok(text,";");
    debug << "+ Process:";
    for(int count=0; line != 0; ++count, line=::strtok(0,";"))  {
      debug << line << " ";
      switch(count) {
      case 0:
	vol_name = line+::strlen("vol:");
	break;
      case 1:
	break;
      case 2:
	line += ::strlen("visible:");
	visible = *line=='1';
	break;
      case 3:
	line += ::strlen("r:");
	red = atof(line);
	break;
      case 4:
	line += ::strlen("g:");
	green = atof(line);
	break;
      case 5:
	line += ::strlen("b:");
	blue = atof(line);
	break;
      case 6:
	line += ::strlen("alpha:");
	//alpha = atof(line);
	break;
      case 7:
	line_style = line+::strlen("line_style:");
	break;
      case 8:
	drawing_style = line+::strlen("drawing_style:");
	break;
      case 9:
	line += ::strlen("show_daughters:");
	show_daughters = *line=='1';
	break;
      default:
	break;
      }
    }
    if ( debug_processing ) cout << debug.str() << endl;
    debug.str("");
    if ( vol_name && ::strlen(vol_name) && (vit=vol_map.find(vol_name)) != vol_map.end() ) {
      TGeoVolume* volume = (*vit).second;
      int     color  = TColor::GetColor(red,green,blue);
      Color_t bright = TColor::GetColorBright(color);
      Color_t dark   = TColor::GetColorDark(color);
      debug << "+ \tr:" << red << " g:" << green << " b:" << blue << " col:" << color 
	    << " line_style:" << line_style << " drawing_style:" << drawing_style 
	    << " visible:" << visible << " show_daughters:" << show_daughters;
      volume->SetLineColor(dark);
      if ( drawing_style == "solid" )  {
	volume->SetFillColor(bright);
	volume->SetFillStyle(1001); // Root: solid
      }
      else {
	//volume->SetFillColor(bright);
	volume->SetFillColor(0);
	volume->SetFillStyle(0);    // Root: hollow
      }
      if ( line_style == "unbroken" )
	volume->SetFillStyle(1);
      else
	volume->SetFillStyle(2);
      
      volume->SetLineWidth(10);
      volume->SetVisibility(visible ? kTRUE : kFALSE);
      volume->SetAttBit(TGeoAtt::kVisContainers,kTRUE);
      volume->SetVisDaughters(show_daughters ? kTRUE : kFALSE);
    }
    else {
      cout << endl << "++ Failed to find volume with name:" << vol_name;
    }
    if ( debug_processing ) cout << debug.str() << endl;
  }
  cout << "++ Closing geometry and starting display...." << endl;
  geo->SetTopVolume(top_vol);
  geo->CloseGeometry();
  geo->SetVisLevel(4);
  geo->SetVisOption(1);
  top_vol->Draw("ogl");
  return top_vol;
}
