#include "moja/datarepository/tileblockcellindexer.h"

#include <boost/format.hpp>
#include <boost/format/group.hpp>

#include <iomanip>

namespace moja {
namespace datarepository {

// --------------------------------------------------------------------------------------------

Rectangle::Rectangle(Point p, Size s) : _location(p), _size(s) {}
Rectangle::Rectangle(double x, double y, double width, double height) : _location({ y, x }), _size({ height, width }) {}
Rectangle::~Rectangle() {}

// --------------------------------------------------------------------------------------------

TileBlockCellIndexer::TileBlockCellIndexer(
	double tileLatSize, double tileLonSize,
	double blockLatSize, double blockLonSize,
	double cellLatSize, double cellLonSize) {
	// TODO: Check all sizes fit evenly, can't have part tiles/blocks/cells
	// throw an exception if we have an issue
	SanityChecksLatLon(tileLatSize, tileLonSize, blockLatSize, blockLonSize, cellLatSize, cellLonSize);

	SetGlobalData();

	tileDesc.latSize = tileLatSize;
	tileDesc.lonSize = tileLonSize;
	tileDesc.latSizeInv = 1.0 / tileDesc.latSize;
	tileDesc.lonSizeInv = 1.0 / tileDesc.lonSize;
	tileDesc.cols = convOtoI(globalDesc.lonSize, tileLonSize);
	tileDesc.rows = convOtoI(globalDesc.latSize, tileLatSize);
	tileDesc.indexLimit = tileDesc.cols * tileDesc.rows;

	blockDesc.latSize = blockLatSize;
	blockDesc.lonSize = blockLonSize;
	blockDesc.latSizeInv = 1.0 / blockDesc.latSize;
	blockDesc.lonSizeInv = 1.0 / blockDesc.lonSize;
	blockDesc.cols = convOtoI(tileDesc.lonSize, blockLonSize);
	blockDesc.rows = convOtoI(tileDesc.latSize, blockLatSize);
	blockDesc.indexLimit = blockDesc.cols * blockDesc.rows;

	cellDesc.latSize = cellLatSize;
	cellDesc.lonSize = cellLonSize;
	cellDesc.latSizeInv = 1.0 / cellDesc.latSize;
	cellDesc.lonSizeInv = 1.0 / cellDesc.lonSize;
	cellDesc.cols = convOtoI(blockDesc.lonSize, cellLonSize);
	cellDesc.rows = convOtoI(blockDesc.latSize, cellLatSize);
	cellDesc.indexLimit = cellDesc.cols * cellDesc.rows;

	cellResCompareValueLon = Int32((cellDesc.lonSize * 100000) + MathEx::k0Plus); //  0.00025;
	cellResCompareValueLat = Int32((cellDesc.latSize * 100000) + MathEx::k0Plus);
}

// --------------------------------------------------------------------------------------------

TileBlockCellIndexer::TileBlockCellIndexer(
	UInt32 tileRows, UInt32 tileCols,
	UInt32 blockRows, UInt32 blockCols,
	UInt32 cellRows, UInt32 cellCols) {
	// TODO: Check all sizes fit evenly, can't have part tiles/blocks/cells
	// throw an exception if we have an issue
	SanityChecksIndex(tileRows, tileCols, blockRows, blockCols, cellRows, cellCols);
	SetGlobalData();

	tileDesc.cols = tileCols;
	tileDesc.rows = tileRows;
	tileDesc.latSize = globalDesc.latSize / tileRows;	 // TODO: do I use MathEx::k0Plus here?
	tileDesc.lonSize = globalDesc.lonSize / tileCols;	 // TODO: do I use MathEx::k0Plus here?
	tileDesc.latSizeInv = 1.0 / tileDesc.latSize;
	tileDesc.lonSizeInv = 1.0 / tileDesc.lonSize;
	tileDesc.indexLimit = tileDesc.cols * tileDesc.rows;

	blockDesc.cols = blockCols;
	blockDesc.rows = blockRows;
	blockDesc.latSize = tileDesc.latSize / blockRows;	// TODO: do I use MathEx::k0Plus here?
	blockDesc.lonSize = tileDesc.lonSize / blockCols;	// TODO: do I use MathEx::k0Plus here?
	blockDesc.latSizeInv = 1.0 / blockDesc.latSize;
	blockDesc.lonSizeInv = 1.0 / blockDesc.lonSize;
	blockDesc.indexLimit = blockDesc.cols * blockDesc.rows;

	cellDesc.cols = cellCols;
	cellDesc.rows = cellRows;
	cellDesc.latSize = blockDesc.latSize / cellRows;	// TODO: do I use MathEx::k0Plus here?
	cellDesc.lonSize = blockDesc.lonSize / cellCols;	// TODO: do I use MathEx::k0Plus here?
	cellDesc.latSizeInv = 1.0 / cellDesc.latSize;
	cellDesc.lonSizeInv = 1.0 / cellDesc.lonSize;
	cellDesc.indexLimit = cellDesc.cols * cellDesc.rows;

	cellResCompareValueLon = Int32((cellDesc.lonSize * 100000) + MathEx::k0Plus); //  0.00025;
	cellResCompareValueLat = Int32((cellDesc.latSize * 100000) + MathEx::k0Plus);
}

// --------------------------------------------------------------------------------------------

void TileBlockCellIndexer::SetGlobalData() {
	globalDesc.name = "Global";
	tileDesc.name = "Tile";
	blockDesc.name = "Block";
	cellDesc.name = "Cell";

	globalDesc.latSize = 180.0;
	globalDesc.lonSize = 360.0;
	globalDesc.latSizeInv = 1.0 / 180.0;
	globalDesc.lonSizeInv = 1.0 / 360.0;
	globalDesc.cols = 1;
	globalDesc.rows = 1;
	globalDesc.indexLimit = 1;
}

// --------------------------------------------------------------------------------------------

void TileBlockCellIndexer::SanityChecksLatLon(
	double tileLatSize, double tileLonSize,
	double blockLatSize, double blockLonSize,
	double cellLatSize, double cellLonSize) {

	// Can't get these to work as the floating point MOD has issues
#if 0
	double result = fmod(180.0, tileLatSize);
	if (!FloatCmp::equalTo(result, 0.0)) {
		std::stringstream ss;
		ss << "fmod(" << tileLatSize << ", 180.0) = " << result;
		BOOST_THROW_EXCEPTION(TileBlockCellIndexerInvalidParameterException() << Component(ss.str()) << Constraint("180.0 % tileLatSize == 0"));
	}
	result = fmod(360.0, tileLonSize);
	if (!FloatCmp::equalTo(result, 0.0)) {
		std::stringstream ss;
		ss << "fmod(" << tileLonSize << ", 360.0) = " << result;
		BOOST_THROW_EXCEPTION(TileBlockCellIndexerInvalidParameterException() << Component(ss.str()) << Constraint("360.0 % tileLonSize == 0"));
	}

	result = fmod(tileLatSize, blockLatSize);
	if (!FloatCmp::equalTo(result, 0.0)) {
		std::stringstream ss;
		ss << "fmod(" << blockLatSize << ", tileLatSize) = " << result;
		BOOST_THROW_EXCEPTION(TileBlockCellIndexerInvalidParameterException() << Component(ss.str()) << Constraint("tileLatSize % blockLatSize == 0"));
	}

	result = fmod(tileLonSize, blockLonSize);
	if (!FloatCmp::equalTo(result, 0.0)) {
		std::stringstream ss;
		ss << "fmod(" << blockLonSize << ", tileLonSize) = " << result;
		BOOST_THROW_EXCEPTION(TileBlockCellIndexerInvalidParameterException() << Component(ss.str()) << Constraint("tileLonSize % blockLonSize == 0"));
	}

	result = fmod(blockLatSize, cellLatSize);
	if (!FloatCmp::equalTo(result, 0.0)) {
		std::stringstream ss;
		ss << "fmod(" << cellLatSize << ", blockLatSize) = " << result;
		BOOST_THROW_EXCEPTION(TileBlockCellIndexerInvalidParameterException() << Component(ss.str()) << Constraint("blockLatSize % cellLatSize == 0"));
	}

	result = fmod(blockLonSize, cellLonSize);
	if (!FloatCmp::equalTo(result, 0.0)) {
		std::stringstream ss;
		ss << "fmod(" << cellLonSize << ", blockLonSize) = " << result;
		BOOST_THROW_EXCEPTION(TileBlockCellIndexerInvalidParameterException() << Component(ss.str()) << Constraint("blockLonSize % cellLonSize == 0"));
	}
#endif
}

// --------------------------------------------------------------------------------------------

void TileBlockCellIndexer::SanityChecksIndex(
	UInt32 tileRows, UInt32 tileCols,
	UInt32 blockRows, UInt32 blockCols,
	UInt32 cellRows, UInt32 cellCols) {}

// --------------------------------------------------------------------------------------------

std::string TileBlockCellIndexer::toString() const {
	std::string text;
	const TileBlockCellLayerDesc* _globalDesc[4] = { &globalDesc, &tileDesc, &blockDesc, &cellDesc };
	for (auto i = 0; i < 4; i++) {
		text += (boost::format(
			"%1%,%2%,%3%,%4%,%5%\n")
			% _globalDesc[i]->name
			% boost::io::group(std::setprecision(15), _globalDesc[i]->latSize)
			% boost::io::group(std::setprecision(15), _globalDesc[i]->lonSize)
			% _globalDesc[i]->rows
			% _globalDesc[i]->cols
			)
			.str();
	}
	return text;
}

// --------------------------------------------------------------------------------------------

Point TileBlockCellIndexer::getLatLonFromIndex(const CellIdx& cellIndex) const {
	auto originLon = (cellIndex.tileX() * tileDesc.lonSize) + (cellIndex.blockX() * blockDesc.lonSize) + (cellIndex.cellX() * cellDesc.lonSize);
	auto originLat = (cellIndex.tileY() * tileDesc.latSize) + (cellIndex.blockY() * blockDesc.latSize) + (cellIndex.cellY() * cellDesc.latSize);
	return Point({ convertFromLatOrigin(originLat), convertFromLonOrigin(originLon) });
}

// --------------------------------------------------------------------------------------------

Point TileBlockCellIndexer::getLatLonFromIndex(const BlockIdx& blockIndex) const {
	auto originLon = (blockIndex.tileX() * tileDesc.lonSize) + (blockIndex.blockX() * blockDesc.lonSize);
	auto originLat = (blockIndex.tileY() * tileDesc.latSize) + (blockIndex.blockY() * blockDesc.latSize);
	return Point({ convertFromLatOrigin(originLat), convertFromLonOrigin(originLon) });
}

// --------------------------------------------------------------------------------------------

Point TileBlockCellIndexer::getLatLonFromIndex(const TileIdx& tileIndex) const {
	auto originLon = tileIndex.tileX() * tileDesc.lonSize;
	auto originLat = tileIndex.tileY() * tileDesc.latSize;
	return Point({ convertFromLatOrigin(originLat), convertFromLonOrigin(originLon) });
}


// --------------------------------------------------------------------------------------------

double TileBlockCellIndexer::getLatFromIndex(const CellIdx& cellIndex) const {
	auto originLat = (cellIndex.tileY() * tileDesc.latSize) + (cellIndex.blockY() * blockDesc.latSize) + (cellIndex.cellY() * cellDesc.latSize);
	return convertFromLatOrigin(originLat);
}

// --------------------------------------------------------------------------------------------

double TileBlockCellIndexer::getLatFromIndex(const BlockIdx& blockIndex) const {
	auto originLat = (blockIndex.tileY() * tileDesc.latSize) + (blockIndex.blockY() * blockDesc.latSize);
	return convertFromLatOrigin(originLat);
}

// --------------------------------------------------------------------------------------------

double TileBlockCellIndexer::getLatFromIndex(const TileIdx& tileIndex) const {
	auto originLat = tileIndex.tileY() * tileDesc.latSize;
	return convertFromLatOrigin(originLat);
}

// --------------------------------------------------------------------------------------------

Point TileBlockCellIndexer::getOriginFromIndex(const CellIdx& cellIndex) const {
	auto oLon = (cellIndex.tileX() * tileDesc.lonSize) + (cellIndex.blockX() * blockDesc.lonSize) + (cellIndex.cellX() * cellDesc.lonSize);
	auto oLat = (cellIndex.tileY() * tileDesc.latSize) + (cellIndex.blockY() * blockDesc.latSize) + (cellIndex.cellY() * cellDesc.latSize);
	return Point({ oLat, oLon });
}

// --------------------------------------------------------------------------------------------

Point TileBlockCellIndexer::getOriginFromIndex(const BlockIdx& blockIndex) const {
	auto oLon = (blockIndex.tileX() * tileDesc.lonSize) + (blockIndex.blockX() * blockDesc.lonSize);
	auto oLat = (blockIndex.tileY() * tileDesc.latSize) + (blockIndex.blockY() * blockDesc.latSize);
	return Point({ oLat, oLon });
}

// --------------------------------------------------------------------------------------------

Point TileBlockCellIndexer::getOriginFromIndex(const TileIdx& tileIndex) const {
	auto oLon = (tileIndex.tileX() * tileDesc.lonSize);
	auto oLat = (tileIndex.tileY() * tileDesc.latSize);
	return Point({ oLat, oLon });
}

// --------------------------------------------------------------------------------------------

CellIdx TileBlockCellIndexer::getCellIndexFromLatLon(const Point& point) const {
	auto oLon = convertToLonOrigin(point.lon);
	auto oLat = convertToLatOrigin(point.lat);
	auto tileX = convOtoI(oLon, tileDesc.lonSize);
	auto tileY = convOtoI(oLat, tileDesc.latSize);
	auto blockX = convOtoI(oLon - (tileX * tileDesc.lonSize), blockDesc.lonSize);
	auto blockY = convOtoI(oLat - (tileY * tileDesc.latSize), blockDesc.latSize);
	auto cellX = convOtoI(oLon - (tileX * tileDesc.lonSize) - (blockX * blockDesc.lonSize), cellDesc.lonSize);
	auto cellY = convOtoI(oLat - (tileY * tileDesc.latSize) - (blockY * blockDesc.latSize), cellDesc.latSize);
	CellIdx cellIdx(XYIndex({ tileX, tileY }), XYIndex({ blockX, blockY }), XYIndex({ cellX, cellY }), *this);
	return cellIdx;
}

// --------------------------------------------------------------------------------------------

BlockIdx TileBlockCellIndexer::getBlockIndexFromLatLon(const Point& point) const {
	auto oLon = convertToLonOrigin(point.lon);
	auto oLat = convertToLatOrigin(point.lat);
	auto tileX = convOtoI(oLon, tileDesc.lonSize);
	auto tileY = convOtoI(oLat, tileDesc.latSize);
	auto blockX = convOtoI(oLon - (tileX * tileDesc.lonSize), blockDesc.lonSize);
	auto blockY = convOtoI(oLat - (tileY * tileDesc.latSize), blockDesc.latSize);
	BlockIdx blockIdx(XYIndex({ tileX, tileY }), XYIndex({ blockX, blockY }), *this);
	return blockIdx;
}

// --------------------------------------------------------------------------------------------

TileIdx TileBlockCellIndexer::getTileIndexFromLatLon(const Point& point) const {
	auto oLon = convertToLonOrigin(point.lon);
	auto oLat = convertToLatOrigin(point.lat);
	auto tileX = convOtoI(oLon, tileDesc.lonSize);
	auto tileY = convOtoI(oLat, tileDesc.latSize);
	TileIdx tileIdx(XYIndex({ tileX, tileY }), *this);
	return tileIdx;
}

// --------------------------------------------------------------------------------------------

CellIdx TileBlockCellIndexer::convertIndex(const CellIdx& cell) const {
	if (compare(*cell._indexer))
		return cell;
	auto cellX = convOtoI(cell.cellX()*cell._indexer->cellDesc.lonSize, cellDesc.lonSize);
	auto cellY = convOtoI(cell.cellY()*cell._indexer->cellDesc.latSize, cellDesc.latSize);
	auto cellIdx = (cellY*cellDesc.cols + cellX);
	return CellIdx(cell.tileIdx, cell.blockIdx, cellIdx, *this);
}

// --------------------------------------------------------------------------------------------

std::string TileIdx::toString() const {
	std::string text;

#if 1
	text += (boost::format(
		"(%1%)")
		% boost::io::group(std::setw(6), tileIdx)
		)
		.str();
#else
	text += (boost::format(
		"(%1%:[%2%,%3%])")
		% boost::io::group(std::setw(6), tileIdx)
		% boost::io::group(std::setw(3), tileX())
		% boost::io::group(std::setw(3), tileY())
		)
		.str();
#endif
	return text;
}

// --------------------------------------------------------------------------------------------

std::string BlockIdx::toString() const {
	std::string text;

#if 1
	text += (boost::format(
		"(%1%,%2%)")
		% boost::io::group(std::setw(6), tileIdx)
		% boost::io::group(std::setw(3), blockIdx)
		)
		.str();
#else
	text += (boost::format(
		"(%1%,%2%:[%3%,%4%],[%5%,%6%])")
		% boost::io::group(std::setw(6), tileIdx)
		% boost::io::group(std::setw(3), blockIdx)
		% boost::io::group(std::setw(3), tileX())
		% boost::io::group(std::setw(3), tileY())
		% boost::io::group(std::setw(3), blockX())
		% boost::io::group(std::setw(3), blockY())
		)
		.str();
#endif
	return text;
}

// --------------------------------------------------------------------------------------------

std::string CellIdx::toString() const {
	std::string text;

#if 1
	text += (boost::format(
		"(%1%,%2%,%3%)")
		% boost::io::group(std::setw(6), tileIdx)
		% boost::io::group(std::setw(3), blockIdx)
		% boost::io::group(std::setw(6), cellIdx)
		)
		.str();
#else
	text += (boost::format(
		"(%1%,%2%,%3%:[%4%,%5%],[%6%,%7%],[%8%,%9%])")
		% boost::io::group(std::setw(6), tileIdx)
		% boost::io::group(std::setw(3), blockIdx)
		% cellIdx
		% boost::io::group(std::setw(3), tileX())
		% boost::io::group(std::setw(3), tileY())
		% boost::io::group(std::setw(3), blockX())
		% boost::io::group(std::setw(3), blockY())
		% boost::io::group(std::setw(4), cellX())
		% boost::io::group(std::setw(4), cellY())
		)
		.str();
#endif
	return text;
}

}} // namespace moja::datarepository
