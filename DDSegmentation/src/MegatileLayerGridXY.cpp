/*
 * MegatileLayerGridXY.cpp
 *
 *  Created on: April 19, 2016
 *      Author: S. Lu, DESY
 *              D Jeans UTokyo
 */

//#define VERBOSEcoterra 1


#include "DDSegmentation/MegatileLayerGridXY.h"

#include <cassert>

namespace DD4hep {
  namespace DDSegmentation {

    /// default constructor using an encoding string
    MegatileLayerGridXY::MegatileLayerGridXY(const std::string& cellEncoding) :
      CartesianGrid(cellEncoding) {
      // define type and description
      _type = "MegatileLayerGridXY";
      // _description = "Cartesian segmentation in the local XY-plane for both Normal wafer and Magic wafer(depending on the layer dimensions)";
      _description = "Cartesian segmentation in the local XY-plane: megatiles with dead areas; integer number of megatiles and cells";

//////// setup in the compact-steering file (i.e., ILD_o3_v05.xml)//// 
// for example,
// <segmentation type="MegatileLayerGridXY" yer_nCells="36" layer_nStripsX="2" layer_nStripsY="36" deadWidth="0" layer_configuration="TL"/>
////
//
      registerParameter("nMegaY", "number of megatiles along Z", _nMegaY , 1 , SegmentationParameter::NoUnit, true);
//      _nMegaY is given by Ecal_n_wafers_per_tower from "model_parameters_ILD_o3_v05.xml"

      registerParameter("layer_nCells", "division of megatile into square tiles", _nCells, 36 , SegmentationParameter::NoUnit, true);
      registerParameter("layer_nStripsX", "division of megatile into strips (1)", _nStripsX, 4 , SegmentationParameter::NoUnit, true);
      registerParameter("layer_nStripsY", "division of megatile into strips (2)", _nStripsY, 36 , SegmentationParameter::NoUnit, true);

      registerParameter("deadWidth", "width of dead region at edge of megatile", _deadWidth, 0., SegmentationParameter::LengthUnit, true);

      registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "cellX");
      registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "cellY");

      registerParameter("identifier_wafer", "Cell encoding identifier for wafer", _identifierWafer, std::string("wafer"),
                        SegmentationParameter::NoUnit, true);

      registerParameter("identifier_layer", "Cell encoding identifier for layer", _identifierLayer, std::string("layer"),
                        SegmentationParameter::NoUnit, true);

      registerParameter("identifier_module", "Cell encoding identifier for module", _identifierModule, std::string("module"),
                        SegmentationParameter::NoUnit, true);

      registerParameter("layer_configuration", "layer configuration (S, T, L)", _layerConfig, std::string("TLS"), SegmentationParameter::NoUnit, true);

      _calculated=false;

    }

    /// destructor
    MegatileLayerGridXY::~MegatileLayerGridXY() {

    }


    void MegatileLayerGridXY::calculateDerivedQuantities() const {

			if (_calculated) return; // already up to date

      _megaSize  =   _totalSizeY / _nMegaY ; // size of square megatile in this layer
      _gridSizeL =  (_megaSize - 2.*_deadWidth) / _nStripsX ; // size of cell within megatile (x)
      _gridSizeT =  (_megaSize - 2.*_deadWidth) / _nStripsY ;
      _gridSizeS = (_megaSize - 2.*_deadWidth) / _nCells;

      std::cout << "calculateDerivedQuantities: total X width: " << _totalSizeX 
				<< " / nMega: " << _nMegaY << " = " << _megaSize 
				<< " ; dead width = " << _deadWidth << " ; nstripsX,Y, nCells : " << _nStripsX 
				<< " " << _nStripsY << " " << _nCells 
				<< " ; stripX,Y/cell size: " << _gridSizeL 
				<< " " << _gridSizeT << " " << _gridSizeS << std::endl;

      _calculated=true;

      return;
    }

    /// determine the position based on the cell ID
    Vector3D MegatileLayerGridXY::position(const CellID& cID) const {

      calculateDerivedQuantities();

      _decoder->setValue(cID);
      unsigned int LayerIndex;
      unsigned int WaferIndex;
      unsigned int CellIndexX;
      unsigned int CellIndexY;
			unsigned int ModuleIndex; 

      Vector3D cellPosition;

      LayerIndex = (*_decoder)[_identifierLayer];
      WaferIndex = (*_decoder)[_identifierWafer];
      CellIndexX = (*_decoder)[_xId];
      CellIndexY = (*_decoder)[_yId];
			ModuleIndex = (*_decoder)[_identifierModule];
			
			double regulatingEBUSizeX = _waferOffsetX[LayerIndex][WaferIndex] * 2;
			double regulatingEBUSizeY = _waferOffsetY[LayerIndex][WaferIndex] * 2;

      std::vector<double> celldims = layercellDimensions( LayerIndex );
			cellPosition.X = -regulatingEBUSizeX / 2.;
			cellPosition.Y = -regulatingEBUSizeY / 2.;   // _megaSize includes two _deadWidth.
      cellPosition.Y += (CellIndexY + 0.5) * celldims[1];
			double XfromEdge = 0;

#if VERBOSEcoterra
	if ( cellPosition.X > 100000. ) std::cout << "HERE too large cellPosition.X place 1" << std::endl;
	if ( _isRegulatingEBU[LayerIndex][WaferIndex] ) {
		std::cout << "regulatingEBUSizeX, regulatingEBUSizeY(layer) = " << regulatingEBUSizeX << ", " << regulatingEBUSizeY 
		<< "(" << LayerIndex << ")" << std::endl;
		 std::cout << "CellIndexX, Y = " << CellIndexX << ", " << CellIndexY << ", celldims[0], [1] = " << celldims[0] 
				<< ", " << celldims[1] << std::endl;
		std::cout << "cellPosition.X, Y = " << cellPosition.X << ", " << cellPosition.Y << std::endl;
	}
#endif


			int n_cellX = regulatingEBUSizeX / celldims[0];
			if  (  _isRegulatingEBU[LayerIndex][WaferIndex] && celldims[0] > celldims[1]*1.5 
						&&CellIndexX == (unsigned int)(n_cellX - 1) ) {
					XfromEdge = ( n_cellX -1) * celldims[0] + ( regulatingEBUSizeX-( n_cellX -1) * celldims[0] )/2;
#if VERBOSEcoterra
						std::cout << "This hit is out of regulation EBU=> merged into neighbour strip; XfromEdge = " << XfromEdge << std::endl;
#endif
			} else {
				 XfromEdge = (CellIndexX + 0.5) * celldims[0];
			}
	
      ////////////////cellPosition.X += _deadWidth + XfromEdge;
      cellPosition.X += XfromEdge;

#if VERBOSEcoterra
	std::cout << "XfromEdge = " << XfromEdge << ", CellIndexX = " << CellIndexX << std::endl;
  if ( cellPosition.X > 100000. ) std::cout << "HERE too large cellPosition.X place 2" << std::endl;
#endif
	
#if VERBOSEcoterra 
		if ( cellPosition.X  > 50000 || cellPosition.Y  > 50000 || cellPosition.Z  > 50000 
				|| cellPosition.X  < -50000 || cellPosition.Y < -50000 || cellPosition.Z < -50000 ) {
			std::cout << "+ _deadWidth + ( CellIndexX + 0.5 )*celldims[0]: cellPosition.X = " << cellPosition.X 
							<< ", cellPosition.Y = " << cellPosition.Y << std::endl;
			std::cout << "CellIndexX * celldims[0] = " << CellIndexX << " x " << celldims[0] 
							<< ", CellIndexY * celldims[1] = " << CellIndexY << " x " << celldims[1] 
							<< ", _isRegulatingEBU[" << LayerIndex << "][" << WaferIndex << "] = " << _isRegulatingEBU[LayerIndex][WaferIndex]
							<< std::endl;
			std::cout << "in MegatileLayerGridXY: regulatingEBUSizeX = " << regulatingEBUSizeX << std::endl;
			std::cout << "x,y,z = " << cellPosition.X << ", " << cellPosition.Y  << ", " << cellPosition.Z << std::endl;
	 		std::cout << "LayerIndex = " << LayerIndex << ", WaferIndex = " << WaferIndex << std::endl;
		}
#endif

/// for Endcaps coterra g728
			if ( ModuleIndex > 4 ) {
				double endcapx = cellPosition.Y;
				cellPosition.Y = cellPosition.X;
				cellPosition.X = endcapx;
#if VERBOSEcoterra
				if ( 192 < WaferIndex && WaferIndex < 197 ) {
					std::cout << "including BAD POSITION"<< std::endl;
					std::cout << "x,y,z = " << cellPosition.Y << ", " << cellPosition.X << ", " << cellPosition.Z << std::endl;
					std::cout << "LayerIndex = " << LayerIndex << ", WaferIndex = " << WaferIndex << ", CellIndexX = " << CellIndexX
										<< ", CellIndexY = " << CellIndexY << ", ModuleIndex = " << ModuleIndex 
										<< ", _isRegulatingEBU[LayerIndex][WaferIndex] = " << _isRegulatingEBU[LayerIndex][WaferIndex]  
										<< std::endl;
				}
#endif
			}

      return cellPosition;
    }

    /// determine the cell ID based on the position
    CellID MegatileLayerGridXY::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
      calculateDerivedQuantities();
      _decoder->setValue(vID);
      unsigned int layerIndex = (*_decoder)[_identifierLayer];
			unsigned int waferIndex = (*_decoder)[_identifierWafer];
			unsigned int moduleIndex = (*_decoder)[_identifierModule];

      double _x = localPosition.X; 
      double _y = localPosition.Y;

//// coterra g728.1857 for Endcaps
			if ( moduleIndex > 4 ) {
				_x = localPosition.Y;
				_y = localPosition.X;
			}
////

      int _cellIndexX(-1);
      int _cellIndexY(-1);

	//  std::cout << "hello3 " << _x << " " << _y << std::endl;
			std::vector<double> celldims = layercellDimensions( layerIndex );

#if VERBOSEcoterra
			std::cout << "in cellID before area check" << std::endl;
			std::cout << "_x = localPosition.X: " << _x << " = " << localPosition.X << ", _waferOffsetX[" 
								<< layerIndex << "][" << waferIndex << "] = " << _waferOffsetX[layerIndex][waferIndex] << std::endl;
			std::cout << "_y = localPosition.Y: " << _y << " = " << localPosition.Y << ", _waferOffsetY["
								<< layerIndex << "][" << waferIndex << "] = " << _waferOffsetY[layerIndex][waferIndex] << std::endl;
			std::cout << "_megaSize = " << _megaSize << std::endl;
#endif
			double WaferOffsetX = _waferOffsetX[layerIndex][waferIndex];
			double WaferOffsetY = _waferOffsetY[layerIndex][waferIndex];
#if VERBOSEcoterra
			std::cout << "WaferOffsetX = " << _waferOffsetX[layerIndex][waferIndex] 
			<< " layer, wafer = " << layerIndex << ", " << waferIndex
			<< std::endl;
#endif
      // _waferOffsetX has negative sign. <<<---??? not true!!
			if ( _x < -WaferOffsetX || _x > WaferOffsetX ||
	  	  	 _y < -WaferOffsetY || _y > WaferOffsetY ) {
#if VERBOSEcoterra
			std::cout << "This hit falls down in to dead width." << std::endl;
			std::cout << "localPosition.X/Y = " << localPosition.X << ", " << localPosition.Y << std::endl;
			std::cout << "layerIndex = " << layerIndex << ", waferIndex = " << waferIndex 
								<< ", _isRegulatingEBU[" << layerIndex << "][" << waferIndex << "] = " << _isRegulatingEBU[layerIndex][waferIndex] 
								<< std::endl; 
#endif

//      	 _x += WaferOffsetX;
//      	 _y += WaferOffsetY;

				} else {

			// localPosition.X is a point in a coordination which center is on the center of wafer.
      // _waferOffsetX is 1/2 of wafer_dim_x, 
      // and wafer_dim_x is already subtracted with the guard ring (deadWidth);
      // _x should be interpreted in a coodination who's origin is on the edge of wafre.

      	 _x += WaferOffsetX;
      	 _y += WaferOffsetY;

#if VERBOSEcoterra
  if ( _isRegulatingEBU[layerIndex][waferIndex] ) {
		if ( localPosition.X < -10000 || localPosition.X > 10000 || localPosition.Y < -10000 || localPosition.Y > 10000 ) {
			std::cout << "in cellID on hit " << std::endl;
			std::cout << "_x = localPosition.X + _waferOffsetX; => " << _x << " = " << localPosition.X << " + " << WaferOffsetX << std::endl;
			std::cout << "_y = localPosition.Y + _waferOffsetY; => " << _y << " = " << localPosition.Y << " + " << WaferOffsetY << std::endl;
			std::cout << "_megaSize = " << _megaSize << std::endl;
		}
	}
#endif

	  // edge of active area of megatile
	  			_cellIndexX = int ( _x / celldims[0] );
	  			_cellIndexY = int ( _y / celldims[1] );
#if VERBOSEcoterra
      std::cout << "in cellID after subtract other  wafers" << std::endl;
			std::cout << "_x = " << _x << " and _y = " << _y << " _deadWidth = " << _deadWidth << "; in cellID" << std::endl;
				if ( _isRegulatingEBU[layerIndex][waferIndex] && celldims[0] < celldims[1]*1.5 ) {
					int temp_n_cellX = WaferOffsetX * 2 / celldims[0];
			//		if ( _cellIndexX > temp_n_cellX - 1 ) {
						std::cout << "EXESS hit in transeverse layer WaferOffsetX = " << WaferOffsetX 
						<< ", celldims[0] = " << celldims[0]
						<< ", _cellIndexX = " << _cellIndexX
						<< ", _x = " << _x
						<< ", _megaSize = " << _megaSize
						<< ", temp_n_cellX = " << temp_n_cellX
						<< std::endl;
			//		}
				}
				//if ( _x > 10000 || _x < -10000 ) {
					std::cout << "HERE checking _x and celldims " << std::endl;
					std::cout << " _cellIndexX, Y = " << _cellIndexX << ", " << _cellIndexY << std::endl;
					std::cout << " celldims[0], [1] = " << celldims[0] << ", " << celldims[1] << std::endl;
				//}
#endif 
					if ( _isRegulatingEBU[layerIndex][waferIndex] && celldims[0] > celldims[1]*1.5 ) {
						int n_cellX = WaferOffsetX * 2 / celldims[0]; // How many strip-row can stay on this EBU.
						if ( _cellIndexX > n_cellX - 1 ) { // hit position is out of this EBU.
							_cellIndexX -= 1;
#if VERBOSEcoterra
						std::cout << "This hit is out of regulation EBU=> merged into neighbour strip; _cellIndexX = " << _cellIndexX << std::endl;
#endif
						}
					}
				}
	if ( _isRegulatingEBU[layerIndex][waferIndex] ) {
      	 //_cellIndexX = 0;
      	 //_cellIndexY = 0;
	}


      	(*_decoder)[_xId] = _cellIndexX;
      	(*_decoder)[_yId] = _cellIndexY;
     
#if VERBOSEcoterra
				int CellIndexX = (*_decoder)[_xId]; 
				if ( CellIndexX > 10 ) std::cout << "HERE check just before return _decoder: _cellIndexX = " << _cellIndexX << std::endl;
#endif
      	return _decoder->getValue();
    }

    std::vector<double> MegatileLayerGridXY::cellDimensions(const CellID& cID) const {

      _decoder->setValue( cID );
      unsigned int _layerIndex = (*_decoder)[_identifierLayer];

      return layercellDimensions(_layerIndex);

    }

    std::vector<double> MegatileLayerGridXY::layercellDimensions(const int layerIndex) const {

      calculateDerivedQuantities();

      unsigned int mm = layerIndex % _layerConfig.length();
      
      const char laytype = _layerConfig.at(mm);

      double xsize(0);
      double ysize(0);

      if ( laytype=='S' ) { // square cell
				xsize = ysize = _gridSizeS;
      } else if ( laytype=='T' ) { // transverse
				xsize = _gridSizeT;
				ysize = _gridSizeL;
      } else if ( laytype=='L' ) { // transverse
				xsize = _gridSizeL;
				ysize = _gridSizeT;
      } else {
				assert(0);
      }
      
      //      std::cout << laytype << " " << xsize << " " << ysize << std::endl;

#if __cplusplus >= 201103L
      //      return {_gridSizeX[_layerIndex], _gridSizeY[_layerIndex]};
      //      return {_gridSizeX, _gridSizeY};
      return {xsize, ysize};
#else
      std::vector<double> cellDims(2,0.0);
      //cellDims[0] = _gridSizeX[_layerIndex];
      //cellDims[1] = _gridSizeY[_layerIndex];
      //cellDims[0] = _gridSizeX;
      //cellDims[1] = _gridSizeY;
      cellDims[0] = xsize;
      cellDims[1] = ysize;
      return cellDims;
#endif
    }

    REGISTER_SEGMENTATION(MegatileLayerGridXY)

  } /* namespace DDSegmentation */
} /* namespace DD4hep */
