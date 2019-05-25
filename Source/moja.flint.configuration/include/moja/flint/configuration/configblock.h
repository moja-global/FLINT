#ifndef MOJA_FLINT_CONFIGURATION_BLOCK_H_
#define MOJA_FLINT_CONFIGURATION_BLOCK_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include <string>

namespace moja {
namespace flint {
namespace configuration {

class ConfigTile;

class CONFIGURATION_API ConfigBlock {
  public:
   ConfigBlock(const ConfigTile& tile, int row, int col, int blockSizeX, int blockSizeY);
   virtual ~ConfigBlock() = default;

   virtual std::string toString() const;
   virtual double left() const;
   virtual double top() const;
   virtual double xPixelSize() const;
   virtual double yPixelSize() const;
   virtual int xPixels() const;
   virtual int yPixels() const;

  private:
   const ConfigTile& _tile;
   int _row;
   int _col;
   int _blockSizeX;
   int _blockSizeY;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_BLOCK_H_
