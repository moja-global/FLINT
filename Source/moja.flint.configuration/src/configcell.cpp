#include "moja/flint/configuration/configcell.h"

#include "moja/flint/configuration/configblock.h"
#include "moja/flint/configuration/configurationexceptions.h"

#include <boost/format.hpp>
#include <boost/format/group.hpp>

#include <cmath>
#include <iomanip>

namespace moja {
namespace flint {
namespace configuration {

ConfigCell::ConfigCell(const ConfigBlock& block, int row, int col) : _block(block) {
   if (row < 0) {
      BOOST_THROW_EXCEPTION(LandscapeDefinitionException() << Component("row") << Constraint(">= 0"));
   }

   if (col < 0) {
      BOOST_THROW_EXCEPTION(LandscapeDefinitionException() << Component("col") << Constraint(">= 0"));
   }

   _row = row;
   _col = col;
   _id = row * block.xPixels() + col + 1;
}

double ConfigCell::area() const {
   double x = _block.yPixelSize() * DiameterOfEarthInMetersPerDeg;
   return x * x * std::cos(lat() * DegToRadMultiplier) * 0.0001;
}

double ConfigCell::lon() const { return _block.left() + _block.xPixelSize() * _col; }

double ConfigCell::lat() const { return _block.top() - _block.yPixelSize() * (_row + 1); }

std::string ConfigCell::toString() const {
   return (boost::format("%1%,%2% (%3%,%4%)") % _row % _col % boost::io::group(std::setprecision(15), lon()) %
           boost::io::group(std::setprecision(15), lat()))
       .str();
}

}  // namespace configuration
}  // namespace flint
}  // namespace moja
