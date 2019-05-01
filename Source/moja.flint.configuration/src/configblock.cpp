#include "moja/flint/configuration/configblock.h"
#include "moja/flint/configuration/configtile.h"
#include "moja/flint/configuration/configurationexceptions.h"

#include <iomanip>
#include <boost/format.hpp>
#include <boost/format/group.hpp>

namespace moja {
namespace flint {
namespace configuration {

	ConfigBlock::ConfigBlock(const ConfigTile& tile, int row, int col, int blockSizeX, int blockSizeY) : _tile(tile) {
		if (row < 0) {
			BOOST_THROW_EXCEPTION(LandscapeDefinitionException() << Component("row") << Constraint(">= 0"));
		}

		if (col < 0) {
			BOOST_THROW_EXCEPTION(LandscapeDefinitionException() << Component("col") << Constraint(">= 0"));
		}

		if (blockSizeX <= 0) {
			BOOST_THROW_EXCEPTION(LandscapeDefinitionException() << Component("blockSizeX") << Constraint("> 0"));
		}

		if (blockSizeY <= 0) {
			BOOST_THROW_EXCEPTION(LandscapeDefinitionException() << Component("blockSizeY") << Constraint("> 0"));
		}

		_row = row;
		_col = col;
		_blockSizeX = blockSizeX;
		_blockSizeY = blockSizeY;
	}

	std::string ConfigBlock::toString() const {
		return (boost::format("Row: %1% Col: %2%")
			% boost::io::group(std::setfill(' '), std::setw(3), std::abs(_row))
			% boost::io::group(std::setfill(' '), std::setw(3), std::abs(_col)))
			.str();
	}

	double ConfigBlock::left() const {
		return _tile.xOrigin()
			+ _col * _blockSizeX * _tile.xPixelSize();
	}

	double ConfigBlock::top() const {
		return _tile.yOrigin()
			+ _tile.ySize()
			- _row * _blockSizeY * _tile.yPixelSize();
	}

	double ConfigBlock::xPixelSize() const {
		return _tile.xPixelSize();
	}

	double ConfigBlock::yPixelSize() const {
		return _tile.yPixelSize();
	}

	int ConfigBlock::xPixels() const {
		int overflow = _blockSizeX * (_col + 1) - _tile.xPixels();
		if (overflow > 0) {
			return _blockSizeX - overflow;
		}

		return _blockSizeX;
	}

	int ConfigBlock::yPixels() const {
		int overflow = _blockSizeY * (_row + 1) - _tile.yPixels();
		if (overflow > 0) {
			return _blockSizeY - overflow;
		}

		return _blockSizeY;
	}

}
}
} // namespace moja::flint::configuration
