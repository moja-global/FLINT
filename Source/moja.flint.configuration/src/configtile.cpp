#include "moja/flint/configuration/configtile.h"

#include "moja/flint/configuration/configblock.h"

#include <moja/mathex.h>

#include <boost/format.hpp>
#include <boost/format/group.hpp>

#include <iomanip>

namespace moja {
namespace flint {
namespace configuration {

ConfigTile::ConfigTile(int xIndex, int yIndex, double xSize, double ySize, int xPixels, int yPixels) {
   if (xSize <= 0.0) {
      throw std::invalid_argument("Error in config tile xSize <= 0");
   }

   if (ySize <= 0.0) {
      throw std::invalid_argument("Error in config tile ySize <= 0");
   }

   if (xPixels < 1) {
      throw std::invalid_argument("Error in config tile xPixels < 1");
   }

   if (yPixels < 1) {
      throw std::invalid_argument("Error in config tile yPixels < 1");
   }

   _xIndex = xIndex;
   _yIndex = yIndex;
   _xSize = xSize;
   _ySize = ySize;
   _xPixels = xPixels;
   _yPixels = yPixels;
   _xPixelSize = xSize / xPixels;
   _yPixelSize = ySize / yPixels;
   _xOrigin = xIndex;
   _yOrigin = yIndex;
}

std::string ConfigTile::toString() const {
   return (boost::format("%1%%2% %3%%4%") % (_xIndex < 0 ? "-" : "") %
           boost::io::group(std::setfill('0'), std::setw(3), std::abs(_xIndex)) % (_yIndex < 0 ? "-" : "") %
           boost::io::group(std::setfill('0'), std::setw(3), std::abs(_yIndex)))
       .str();
}

std::vector<std::shared_ptr<ConfigBlock>> ConfigTile::split(int blockSizeX, int blockSizeY) const {
   int cols = int(ceil(double(_xPixels) / blockSizeX) + MathEx::k0Plus);
   int rows = int(ceil(double(_yPixels) / blockSizeY) + MathEx::k0Plus);

   std::vector<std::shared_ptr<ConfigBlock>> blocks;
   for (int row = 0; row < rows; row++) {
      for (int col = 0; col < cols; col++) {
         blocks.push_back(std::make_shared<ConfigBlock>(*this, row, col, blockSizeX, blockSizeY));
      }
   }

   return blocks;
}

}  // namespace configuration
}  // namespace flint
}  // namespace moja
