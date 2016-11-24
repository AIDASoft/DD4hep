#include "DDSegmentation/Segmentation.h"
#include "DDSegmentation/CartesianGridXY.h"
#include "DDSegmentation/CartesianGridYZ.h"
#include "DDSegmentation/CartesianGridXZ.h"
#include "DDSegmentation/CartesianGridXYZ.h"
#include "DD4hep/DDTest.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <exception>
#include <cmath>


static DD4hep::DDTest test( "CellDimensions" ) ;


int main() {
  try{

    DD4hep::DDSegmentation::CartesianGridXY seg("system:8,barrel:3,layer:8,slice:5,x:16,y:16");

    const double xSize=12343.43243;
    const double ySize=M_PI;

    seg.setGridSizeX(xSize);
    seg.setGridSizeY(ySize);

    DD4hep::DDSegmentation::VolumeID volID = 0;

    test( fabs(seg.cellDimensions(volID)[0] - xSize )  < 1e-11, " CG_XY: Dimension for X" );
    test( fabs(seg.cellDimensions(volID)[1] - ySize )  < 1e-11, " CG_XY: Dimension for Y" );

  } catch( std::exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

  try{

    DD4hep::DDSegmentation::CartesianGridXZ seg("system:8,barrel:3,layer:8,slice:5,x:16,z:16");

    const double xSize=12343.43243;
    const double zSize=M_PI;

    seg.setGridSizeX(xSize);
    seg.setGridSizeZ(zSize);

    DD4hep::DDSegmentation::VolumeID volID = 0;

    test( fabs(seg.cellDimensions(volID)[0] - xSize )  < 1e-11, " CG_XZ: Dimension for X" );
    test( fabs(seg.cellDimensions(volID)[1] - zSize )  < 1e-11, " CG_XZ: Dimension for Z" );

  } catch( std::exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

  try{

    DD4hep::DDSegmentation::CartesianGridYZ seg("system:8,barrel:3,layer:8,slice:5,y:16,z:16");

    const double ySize=12343.43243;
    const double zSize=M_PI;

    seg.setGridSizeY(ySize);
    seg.setGridSizeZ(zSize);

    DD4hep::DDSegmentation::VolumeID volID = 0;

    test( fabs(seg.cellDimensions(volID)[0] - ySize )  < 1e-11, " CG_YZ: Dimension for Y" );
    test( fabs(seg.cellDimensions(volID)[1] - zSize )  < 1e-11, " CG_YZ: Dimension for Z" );

  } catch( std::exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

  try{

    DD4hep::DDSegmentation::CartesianGridXYZ seg("system:8,barrel:3,layer:8,slice:7,x:10,y:10,z:10");

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
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

  return 0;
}
