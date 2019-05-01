#ifndef MOJA_FLINT_CONFIGURATION_TILE_H_
#define MOJA_FLINT_CONFIGURATION_TILE_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include <string>
#include <vector>

namespace moja {
namespace flint {
namespace configuration {

class ConfigBlock;

class CONFIGURATION_API ConfigTile {
public:
	ConfigTile() = default;
	ConfigTile(int xIndex, int yIndex, double xSize, double ySize, int xPixels, int yPixels);

	virtual ~ConfigTile() = default;

	virtual std::string toString() const;
	virtual std::vector<std::shared_ptr<ConfigBlock>> split(int blockSizeX, int blockSizeY) const;

	virtual inline int xIndex() const { return _xIndex; }
	virtual inline int yIndex() const { return _yIndex; }
	virtual inline double xSize() const { return _xSize; }
	virtual inline double ySize() const { return _ySize; }
	virtual inline int xPixels() const { return _xPixels; }
	virtual inline int yPixels() const { return _yPixels; }
	virtual inline double xPixelSize() const { return _xPixelSize; }
	virtual inline double yPixelSize() const { return _yPixelSize; }
	virtual inline double xOrigin() const { return _xOrigin; }
	virtual inline double yOrigin() const { return _yOrigin; }

private:
	int _xIndex;
	int _yIndex;
	double _xOrigin;
	double _xPixelSize;
	int _xPixels;
	double _xSize;
	double _yOrigin;
	double _yPixelSize;
	int _yPixels;
	double _ySize;
};

}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_TILE_H_