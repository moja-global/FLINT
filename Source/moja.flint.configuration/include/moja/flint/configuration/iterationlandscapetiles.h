#ifndef MOJA_FLINT_CONFIGURATION_ITERATIONLANDSCAPETILES_H_
#define MOJA_FLINT_CONFIGURATION_ITERATIONLANDSCAPETILES_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/configtile.h"
#include "moja/flint/configuration/iterationbase.h"

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API IterationLandscapeTiles : public IterationBase {
  public:
   IterationLandscapeTiles();
   virtual ~IterationLandscapeTiles() {}

   double tileSizeX() const { return _tileSizeX; }
   double tileSizeY() const { return _tileSizeY; }
   int xPixels() const { return _xPixels; }
   int yPixels() const { return _yPixels; }

   void set_tileSizeX(double val) { _tileSizeX = val; }
   void set_tileSizeY(double val) { _tileSizeY = val; }
   void set_xPixels(int val) { _xPixels = val; }
   void set_yPixels(int val) { _yPixels = val; }

   const std::vector<ConfigTile>& landscapeTiles() const { return _landscapeTiles; }
   std::vector<ConfigTile>& landscapeTiles() { return _landscapeTiles; }

   virtual void addConfigTile(ConfigTile index) { _landscapeTiles.push_back(index); }

  private:
   double _tileSizeX;
   double _tileSizeY;
   int _xPixels;
   int _yPixels;
   std::vector<ConfigTile> _landscapeTiles;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_ITERATIONLANDSCAPETILES_H_
