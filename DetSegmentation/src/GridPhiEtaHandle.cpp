// Framework include files
#include "DetSegmentation/GridPhiEtaHandle.h"
#include "DD4hep/objects/SegmentationsInterna.h"
#include "DetSegmentation/GridPhiEta.h"
#include "DD4hep/Handle.inl"

using namespace std;
using namespace DD4hep::Geometry;

/// determine the position based on the cell ID
Position GridPhiEta::position(const CellID& id) const   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
DD4hep::CellID GridPhiEta::cellID(const Position& local,
                                     const Position& global,
                                     const VolumeID& volID) const
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in R
double GridPhiEta::gridSizeEta() const  {
  return access()->implementation->gridSizeEta();
}

/// access the grid size in Phi
int GridPhiEta::phiBins() const  {
  return access()->implementation->phiBins();
}

/// access the coordinate offset in R
double GridPhiEta::offsetEta() const  {
  return access()->implementation->offsetEta();
}

/// access the coordinate offset in Phi
double GridPhiEta::offsetPhi() const  {
  return access()->implementation->offsetPhi();
}

/// set the coordinate offset in R
void GridPhiEta::setOffsetEta(double offset) const  {
  access()->implementation->setOffsetEta(offset);
}

/// set the coordinate offset in Phi
void GridPhiEta::setOffsetPhi(double offset) const  {
  access()->implementation->setOffsetPhi(offset);
}

/// set the grid size in R
void GridPhiEta::setGridSizeEta(double cellSize) const  {
  access()->implementation->setGridSizeEta(cellSize);
}

/// set the grid size in Phi
void GridPhiEta::setPhiBins(int cellSize) const  {
  access()->implementation->setPhiBins(cellSize);
}

/// access the field name used for R
const string& GridPhiEta::fieldNameEta() const  {
  return access()->implementation->fieldNameEta();
}

/// access the field name used for Phi
const string& GridPhiEta::fieldNamePhi() const  {
  return access()->implementation->fieldNamePhi();
}

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in x
    -# size in z
*/
vector<double> GridPhiEta::cellDimensions(const CellID& id) const  {
  return {0};// access()->implementation->cellDimensions(id);
}

DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DD4hep::DDSegmentation::GridPhiEta);
