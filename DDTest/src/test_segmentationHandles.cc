#include "DD4hep/Segmentations.h"
#include "DD4hep/CartesianGridXY.h"
#include "DD4hep/CartesianGridYZ.h"
#include "DD4hep/CartesianGridXZ.h"
#include "DD4hep/CartesianGridXYZ.h"
#include "DD4hep/DDTest.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <exception>
#include <cmath>


static DD4hep::DDTest test( "CellDimensions" ) ;
using namespace DD4hep;
using namespace DD4hep::Geometry;

int main() {
  try{
    BitField64 bf("system:8,barrel:3,layer:8,slice:5,x:16,y:16");
    Segmentation base("CartesianGridXY","Test",&bf);
    CartesianGridXY seg(base);
    const double xSize=12343.43243;
    const double ySize=M_PI;

    test( seg.isValid(), " Assignment from Segmentation -> "+typeName(typeid(seg))+" worked" );

    seg.setGridSizeX(xSize);
    seg.setGridSizeY(ySize);

    DD4hep::DDSegmentation::VolumeID volID = 0;

    test( fabs(seg.cellDimensions(volID)[0] - xSize )  < 1e-11, " CG_XY: Dimension for X" );
    test( fabs(seg.cellDimensions(volID)[1] - ySize )  < 1e-11, " CG_XY: Dimension for Y" );

  } catch( std::exception &e ){
    test.log( e.what() );
    test.error( "exception occurred" );
  }

  try{
    BitField64 bf("system:8,barrel:3,layer:8,slice:5,x:16,z:16");
    Segmentation base("CartesianGridXZ","Test",&bf);
    CartesianGridXZ seg(base);

    const double xSize=12343.43243;
    const double zSize=M_PI;

    test( seg.isValid(), " Assignment from Segmentation -> "+typeName(typeid(seg))+" worked" );

    seg.setGridSizeX(xSize);
    seg.setGridSizeZ(zSize);

    VolumeID volID = 0;

    test( fabs(seg.cellDimensions(volID)[0] - xSize )  < 1e-11, " CG_XZ: Dimension for X" );
    test( fabs(seg.cellDimensions(volID)[1] - zSize )  < 1e-11, " CG_XZ: Dimension for Z" );

  } catch( std::exception &e ){
    test.log( e.what() );
    test.error( "exception occurred" );
  }

  try{
    BitField64 bf("system:8,barrel:3,layer:8,slice:5,y:16,z:16");
    Segmentation base("CartesianGridYZ","Test",&bf);
    CartesianGridYZ seg = base;

    test( seg.isValid(), " Assignment from Segmentation -> "+typeName(typeid(seg))+" worked" );

    const double ySize=12343.43243;
    const double zSize=M_PI;

    seg.setGridSizeY(ySize);
    seg.setGridSizeZ(zSize);

    DD4hep::DDSegmentation::VolumeID volID = 0;

    test( fabs(seg.cellDimensions(volID)[0] - ySize )  < 1e-11, " CG_YZ: Dimension for Y" );
    test( fabs(seg.cellDimensions(volID)[1] - zSize )  < 1e-11, " CG_YZ: Dimension for Z" );

  } catch( std::exception &e ){
    test.log( e.what() );
    test.error( "exception occurred" );
  }

  try{
    BitField64 bf("system:8,barrel:3,layer:8,slice:7,x:10,y:10,z:10");
    Segmentation base("CartesianGridXYZ","Test",&bf);
    CartesianGridXYZ seg = base;

    test( seg.isValid(), " Assignment from Segmentation -> "+typeName(typeid(seg))+" worked" );

    const double xSize=42.24;
    const double ySize=12343.43243;
    const double zSize=M_PI;

    seg.setGridSizeX(xSize);
    seg.setGridSizeY(ySize);
    seg.setGridSizeZ(zSize);

    DD4hep::DDSegmentation::VolumeID volID = 0;

    test( fabs(seg.cellDimensions(volID)[0] - xSize )  < 1e-11, " CG_XYZ: Dimension for X" );
    test( fabs(seg.cellDimensions(volID)[1] - ySize )  < 1e-11, " CG_XYZ: Dimension for Y" );
    test( fabs(seg.cellDimensions(volID)[2] - zSize )  < 1e-11, " CG_XYZ: Dimension for Z" );

  } catch( std::exception &e ){
    test.log( e.what() );
    test.error( "exception occurred" );
  }
  return 0;
}
