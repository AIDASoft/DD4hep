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

// Framework includes
#include <DD4hep/Printout.h>
#include <DD4hep/FieldTypes.h>
#include <DD4hep/MatrixHelpers.h>
#include <DD4hep/DetFactoryHelper.h>
#include <cmath>

using namespace dd4hep;

// ROOT include files
#include <TH2.h>
#include <TPad.h>
#include <TRint.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TArrow.h>
#include <TCanvas.h>
#include <THistPainter.h>

#include "Hoption.h"
#include "Hparam.h"

extern Hoption_t Hoption;
extern Hparam_t  Hparam;

namespace  {
  void help_bfield(int argc, char** argv)   {
    std::cout <<
      "Usage: DD4hep_DrawBField -arg [-arg]                                 \n"
      "  The plugin implicitly assumes that the geometry is already loaded. \n"
      "     -area  <string>  Define area for which the B-field will be drawn\n"
      "     -nx:   <number>  Number of bins in X direction                  \n"
      "     -ny:   <number>  Number of bins in Y direction                  \n"
      "     -dx:   <range>   Definition of the range in X                   \n"
      "     -dy:   <range>   Definition of the range in Y                   \n"
      "     -dz:   <range>   Definition of the range in Z                   \n"
      "     Range definition as tuple: min,max                              \n"
      "                 min:  lower boundary                                \n"
      "                 max:  upper boundary                                \n"
      "\tArguments given: " << arguments(argc,argv) << std::endl << std::flush;
    ::exit(EINVAL);
  }

  struct range_t  {
    double rmin=0e0, rmax=0e0;
  };
  struct field_t  {
    Position  position;
    Direction bfield;
  };

  range_t get_range(const std::string& s, int argc, char** argv)   {
    int ival=0;
    range_t range;
    std::string val;
    for(std::size_t i=0; i<s.length()+1; ++i)   {
      char c = s.c_str()[i];
      if ( c == ',' || i == s.length() )  {
        switch(ival)   {
        case 0:
          range.rmin = _toDouble(val);
          break;
        case 1:
          range.rmax = _toDouble(val);
          break;
        default:
          except("","+++ Too many value for range descriptor provided: %s", s.c_str());
          break;
        }
        val = "";
        ++ival;
        continue;
      }
      val += c;
    }
    if ( ival != 2 )   {
      help_bfield(argc, argv);
    }
    return range;
  }

  class MyHistPainter : public THistPainter {
  public:
    using THistPainter::THistPainter;
    TH2* paintArrows(TH2* histo, const std::vector<field_t>& points)   {
      fYaxis->SetTitle("Y coordinate [cm]");
      fXaxis->SetTitle("X coordinate [cm]");
      for(const auto& p : points)    {
        double strength2 = p.bfield.mag2();
        if ( strength2 > 1e-8 )   {
          double strength = sqrt(p.bfield.X()*p.bfield.X()+p.bfield.Y()*p.bfield.Y());
          int ix = fXaxis->FindBin(p.position.X());
          int iy = fYaxis->FindBin(p.position.Y());
          double xlo = fXaxis->GetBinLowEdge(ix);
          double xhi = fXaxis->GetBinLowEdge(ix+1);
          double ylo = fYaxis->GetBinLowEdge(iy);
          double yhi = fYaxis->GetBinLowEdge(iy+1);
          auto norm_field = p.bfield * ((xhi-xlo) / strength);
          double x1  = xlo + (xhi-xlo)/2.0;
          double x2  = x1 + norm_field.X();
          double y1  = ylo + (yhi-ylo)/2.0;
          double y2  = y1 + norm_field.Y();
          auto* arrow = new TArrow(x1, y1, x2, y2, 0.015, "|>");
          arrow->SetAngle(25);
          arrow->SetFillColor(kRed);
          arrow->SetLineColor(kRed);
          arrow->Draw();
          histo->Fill(p.position.X(), p.position.X(), strength);
          dd4hep::detail::printf("Arrow %+15.8e  %+15.8e  %+15.8e  %+15.8e    %+15.8e  %+15.8e\n",
                                 x1, y1, x2, y2, x2-x1, y2-y1);
          //delete arrow;
        }
      }
      PaintPalette();
      return histo;
    }
  };
  
  /// Basic entry point to interprete an XML document
  /**
   *  - The file URI to be opened 
   *    is passed as first argument to the call.
   *  - The processing hint (build type) is passed as optional 
   *    second argument.
   *
   *  Factory: DD4hep_CompactLoader
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  static long draw_bfield(Detector& description, int argc, char** argv) {
    std::vector<std::string> srange_x, srange_y, srange_z;
    std::size_t nbin_x = 100, nbin_y = 100, nbin_z = 1;
    std::string output;
    double z_value = 0e0;
    bool draw = false;

    printout(WARNING,"DrawBField","This is a TEST. It does not function properly!");

    for(int i = 0; i < argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-rx",argv[i],4) )
        srange_x.push_back(argv[++i]);
      else if ( 0 == ::strncmp("-ry",argv[i],3) )
        srange_y.push_back(argv[++i]);
      else if ( 0 == ::strncmp("-rz",argv[i],3) )
        srange_z.push_back(argv[++i]);
      else if ( 0 == ::strncmp("-nx",argv[i],3) )
        nbin_x = _toULong(argv[++i]);
      else if ( 0 == ::strncmp("-ny",argv[i],3) )
        nbin_y = _toULong(argv[++i]);
      else if ( 0 == ::strncmp("-nz",argv[i],3) )
        nbin_z = _toULong(argv[++i]);
      else if ( 0 == ::strncmp("-z",argv[i],2) )
        z_value = _toDouble(argv[++i]);
      else if ( 0 == ::strncmp("-draw",argv[i],4) )
        draw = true;
      else if ( 0 == ::strncmp("-output",argv[i],4) )
        output = argv[++i];
      else if ( 0 == ::strncmp("-help",argv[i],2) )
        help_bfield(argc, argv);
    }
    if ( srange_x.empty() || srange_y.empty() )   {
      help_bfield(argc, argv);
    }
    std::vector<range_t> range_x, range_y, range_z;
    for(const auto& r : srange_x)  {
      range_t rg = get_range(r, argc, argv);
      range_x.push_back(rg);
    }
    for(const auto& r : srange_y)  {
      range_t rg = get_range(r, argc, argv);
      range_y.push_back(rg);
    }
    for(const auto& r : srange_z)  {
      range_t rg = get_range(r, argc, argv);
      range_z.push_back(rg);
    }
    
    double ma = std::numeric_limits<double>::max();
    range_t envelope_x  { ma, -ma };
    range_t envelope_y  { ma, -ma };
    range_t envelope_z  { ma, -ma };
    for( const auto& range : range_x )   {
      envelope_x.rmin = std::min(range.rmin, envelope_x.rmin);
      envelope_x.rmax = std::max(range.rmax, envelope_x.rmax);
    }
    for( const auto& range : range_y )   {
      envelope_y.rmin = std::min(range.rmin, envelope_y.rmin);
      envelope_y.rmax = std::max(range.rmax, envelope_y.rmax);
    }
    for( const auto& range : range_z )   {
      envelope_z.rmin = std::min(range.rmin, envelope_z.rmin);
      envelope_z.rmax = std::max(range.rmax, envelope_z.rmax);
    }

    double dx = (envelope_x.rmax - envelope_x.rmin) / double(nbin_x);
    double dy = (envelope_y.rmax - envelope_y.rmin) / double(nbin_y);
    double dz = nbin_z == 1 ? 0e0 : (envelope_z.rmax - envelope_z.rmin) / double(nbin_z);
    dd4hep::detail::printf("Range(x) min:%4ld bins %+15.8e cm max:%+15.8e cm dx:%+15.8e cm\n",
                           nbin_x, envelope_x.rmin/cm, envelope_x.rmax/cm, dx/cm);
    dd4hep::detail::printf("Range(y) min:%4ld bins %+15.8e cm max:%+15.8e cm dx:%+15.8e cm\n",
                           nbin_y, envelope_y.rmin/cm, envelope_y.rmax/cm, dy/cm);
    if ( nbin_z > 1 ) dd4hep::detail::printf("Range(z) min:%4ld bins %+15.8e cm max:%+15.8e cm dx:%+15.8e cm\n",
                                             nbin_z, envelope_z.rmin/cm, envelope_z.rmax/cm, dz/cm);

    FILE* out_file = ::fopen(output.empty() ? "/dev/null" : output.c_str(), "w");
    ::fprintf(out_file,"#######################################################################################################\n");
    ::fprintf(out_file,"      x[cm]            y[cm]            z[cm]          Bx[Tesla]        By[Tesla]        Bz[Tesla]     \n");
    std::vector<field_t> field_values;
    for( std::size_t i = 0; i < nbin_x; ++i )   {
      float x = envelope_x.rmin + double(i)*dx + dx/2e0;
      for( std::size_t j = 0; j < nbin_y; ++j )   {
        float y = envelope_y.rmin + double(j)*dy + dy/2e0;
        for( std::size_t k = 0; k < nbin_z; ++k )   {
          float z = nbin_z == 1 ? z_value : envelope_z.rmin + double(k)*dz + dz/2e0;
          field_t value;
          value.position = { x, y, z };
          value.bfield   = { 0e0, 0e0, 0e0 };
          value.bfield = description.field().magneticField(value.position);
          ::fprintf(out_file, " %+15.8e  %+15.8e  %+15.8e  %+15.8e  %+15.8e  %+15.8e\n",
                 value.position.X()/cm, value.position.Y()/cm,  value.position.Z()/cm,
                 value.bfield.X()/dd4hep::tesla, value.bfield.Y()/dd4hep::tesla, value.bfield.Z()/dd4hep::tesla);
          field_values.emplace_back(value);
        }
      }
    }
    ::fclose(out_file);
    if ( draw )   {
      if ( 0 == gApplication )  {
        std::pair<int, char**> a(argc,argv);
        gApplication = new TRint("DD4hepRint", &a.first, a.second);
        printout(INFO,"DD4hepRint","++ Created ROOT interpreter instance for DD4hepUI.");
      }
      auto* histo = new TH2F("Bfield", "B-Field strength in Tesla",
                            nbin_x, envelope_x.rmin, envelope_x.rmax,
                            nbin_y, envelope_y.rmin, envelope_y.rmax);
      MyHistPainter paint;
      paint.SetHistogram(histo);
      TCanvas* c1 = new TCanvas("B-Field");
      c1->SetWindowSize(1000,1000);
      //paint.Paint();
      histo->SetStats(kFALSE);
      //histo->Draw();
      paint.paintArrows(histo, field_values);
      histo->Draw("COLZ SAME");
      gPad->SetGridx();
      gPad->SetGridy();
      if ( !gApplication->IsRunning() )  {
        gApplication->Run();
      }
    }
    return 1;
  }
}
DECLARE_APPLY(DD4hep_DrawBField,draw_bfield)
