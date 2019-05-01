#ifndef MOJA_MODULES_ZIPPER_RASTERREADER_H_
#define MOJA_MODULES_ZIPPER_RASTERREADER_H_

#include "moja/modules/zipper/_modules.zipper_exports.h"
#include "moja/utility.h"

#include <moja/datarepository/rasterreaderinterface.h>

#include <string>
#include <vector>

namespace moja {
namespace datarepository {
	class TileBlockCellIndexer;
	struct BlockIdx;
}
namespace modules {
namespace zipper {


// --------------------------------------------------------------------------------------------
// *** METADATA METHODS ***
// --------------------------------------------------------------------------------------------

//class ZIPPER_API FlintMetaDataRasterReader : public datarepository::MetaDataRasterReaderInterface {
//public:
//	FlintMetaDataRasterReader(const std::string& path, const std::string& prefix);
//	~FlintMetaDataRasterReader() override {}
//
//	DynamicObject readMetaData() const override;
//
//private:
//	std::string _metaPath;
//};

// --------------------------------------------------------------------------------------------

class ZIPPER_API CompressedMetaDataRasterReader : public datarepository::MetaDataRasterReaderInterface {
public:
	CompressedMetaDataRasterReader(
		const std::string& path, 
		const std::string& prefix,
		const DynamicObject& settings);
	~CompressedMetaDataRasterReader() override {}

	DynamicObject readMetaData() const override;

private:
	std::string _metaZipPath;
	std::string _metaPath;
};


// --------------------------------------------------------------------------------------------
// *** TILE METHODS ***
// --------------------------------------------------------------------------------------------

class ZIPPER_API CompressedTileRasterReader : public datarepository::TileRasterReaderInterface {
public:
	CompressedTileRasterReader(
		const std::string& path, 
		const Point& origin, 
		const std::string& prefix,
		const datarepository::TileBlockCellIndexer& indexer,
		const DynamicObject& settings);

	~CompressedTileRasterReader() override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int8>*	block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt8>*	block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int16>*	block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt16>* block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int32>*	block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt32>* block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int64>*	block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt64>* block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<float>*	block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<double>* block) const override;
private:
	void readCompressedBlockData(const datarepository::BlockIdx& blkIdx, char* block_data, size_t block_size) const;

	std::string _layerPath;
	std::string _tileName;
};

//// --------------------------------------------------------------------------------------------
//
//class ZIPPER_API FlintTileRasterReader : public datarepository::TileRasterReaderInterface {
//public:
//	FlintTileRasterReader(const std::string& path, const Point& origin, const std::string& prefix,
//		const datarepository::TileBlockCellIndexer& indexer);
//	~FlintTileRasterReader() override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int8>*	block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt8>*	block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int16>*	block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt16>* block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int32>*	block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt32>* block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int64>*	block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt64>* block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<float>*	block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<double>* block) const override;
//private:
//	void readFlintBlockData(const datarepository::BlockIdx& blkIdx, char* block_data, size_t block_size) const;
//
//	std::string _tilePath;
//};
//
// --------------------------------------------------------------------------------------------
// *** STACK METHODS ***
// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------

class ZIPPER_API CompressedStackRasterReader : public datarepository::StackRasterReaderInterface {
public:
	CompressedStackRasterReader(
		const std::string& path, 
		const Point& origin, 
		const std::string& prefix, 
		const datarepository::TileBlockCellIndexer& indexer,
		const DynamicObject& settings);

	~CompressedStackRasterReader() override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<Int8>*		block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<UInt8>*		block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<Int16>*		block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<UInt16>*	block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<Int32>*		block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<UInt32>*	block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<Int64>*		block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<UInt64>*	block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<float>*		block) const override;
	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<double>*	block) const override;
private:
	void readCompressedBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, char* block_data, size_t block_size) const;

	std::string _layerPath;
	std::string _tileName;
};

//// --------------------------------------------------------------------------------------------
//
//class ZIPPER_API FlintStackRasterReader : public datarepository::StackRasterReaderInterface {
//public:
//	FlintStackRasterReader(const std::string& path, const Point& origin, const std::string& prefix, const datarepository::TileBlockCellIndexer& indexer);
//	~FlintStackRasterReader() override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<Int8>*		block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<UInt8>*		block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<Int16>*		block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<UInt16>*	block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<Int32>*		block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<UInt32>*	block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<Int64>*		block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<UInt64>*	block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<float>*		block) const override;
//	void readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<double>*	block) const override;
//private:
//	void readFlintBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, char* block_data, size_t block_size) const;
//
//	std::string _tilePath;
//};
//

// --------------------------------------------------------------------------------------------

class ZIPPER_API ZipperRasterReaderFactory : public datarepository::RasterReaderFactoryInterface {
public:
	ZipperRasterReaderFactory() {}

	bool hasMetaDataReader() const override {
		return true;
	}

	bool hasTileRasterReader() const override {
		return true;
	}

	bool hasStackRasterReader()	const override {
		return true;
	}

	std::unique_ptr<datarepository::MetaDataRasterReaderInterface> getMetaDataRasterReader(
		const std::string& path, 
		const std::string& prefix,
		const DynamicObject& settings) const override;

	std::unique_ptr<datarepository::TileRasterReaderInterface> getTileRasterReader(
		const std::string& path, 
		const Point& origin,
		const std::string& prefix,
		const datarepository::TileBlockCellIndexer& indexer,
		const DynamicObject& settings) const override;

	std::unique_ptr<datarepository::StackRasterReaderInterface> getStackRasterReader(
		const std::string& path, 
		const Point& origin,
		const std::string& prefix,
		const datarepository::TileBlockCellIndexer& indexer,
		const DynamicObject& settings) const override;
};

}}}

#endif // MOJA_MODULES_ZIPPER_RASTERREADER_H_