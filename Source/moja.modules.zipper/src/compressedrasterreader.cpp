#include "moja/modules/zipper/compressedrasterreader.h"
#include "moja/datarepository/rasterreader.h"
#include "moja/datarepository/tileblockcellindexer.h"
#include "moja/datarepository/datarepositoryexceptions.h"

#include "moja/pocojsonutils.h"

#include <Poco/Path.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/ParseHandler.h>
#include <Poco/JSON/Stringifier.h>
#include <Poco/Path.h>
#include <Poco/File.h>

#include "moja/utility.h"

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <zipper/unzipper.h>
#include <fstream>
#include <sstream>
#include <zipper/unzipper.h>

using ::zipper::Unzipper;
using ::zipper::ZipEntry;

namespace moja {
namespace modules {
namespace zipper {

CompressedMetaDataRasterReader::CompressedMetaDataRasterReader(
	const std::string& path, 
	const std::string& prefix, 
	const DynamicObject& settings) :
	MetaDataRasterReaderInterface(path, prefix, settings) {
	_metaZipPath = (boost::format("%1%.zip") % path).str();
	_metaPath	 = (boost::format("%1%.json") % prefix).str();
}

DynamicObject CompressedMetaDataRasterReader::readMetaData() const {
	if (file_exists(_metaZipPath)) {
		Poco::JSON::Parser jsonMetadataParser;

		Unzipper unzipper(_metaZipPath);
		try {
			auto metaPath = _metaPath;
			// Zip entries are case-sensitive, but can't trust that the provider config took that into account.
			std::vector<ZipEntry> entries = unzipper.entries();
			auto it = std::find_if(entries.begin(), entries.end(), [&metaPath](const ZipEntry entry) {
				return boost::iequals(metaPath, entry.name);
			});

			std::stringstream jsonFileData;
			unzipper.extractEntryToStream(it->name, jsonFileData);
			unzipper.close();
			jsonMetadataParser.parse(jsonFileData);
			auto metadata = jsonMetadataParser.result();
			auto layerMetadata = parsePocoJSONToDynamic(metadata).extract<const DynamicObject>();
			return layerMetadata;
		}
		catch (...) {
			unzipper.close();
		}
	}
	else
	{
		BOOST_THROW_EXCEPTION(datarepository::FileNotFoundException() << datarepository::FileName(_metaPath));
	}
	return DynamicObject();
}

// --------------------------------------------------------------------------------------------

CompressedTileRasterReader::CompressedTileRasterReader(
	const std::string& path, 
	const Point& origin,
	const std::string& prefix,
	const datarepository::TileBlockCellIndexer& indexer,
	const DynamicObject& settings) :
	TileRasterReaderInterface(path, origin, prefix, indexer, settings) {
    _layerPath = path;
    _tileName = (boost::format("%1%_%2%") % prefix % tile_id(origin)).str();
}

CompressedTileRasterReader::~CompressedTileRasterReader() {
}

void CompressedTileRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int8>* block) const {
	readCompressedBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int8));
}

void CompressedTileRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt8>* block) const {
	readCompressedBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt8));
}

void CompressedTileRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int16>* block) const {
	readCompressedBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int16));
}

void CompressedTileRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt16>* block) const {
	readCompressedBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt16));
}

void CompressedTileRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int32>* block) const {
	readCompressedBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int32));
}

void CompressedTileRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt32>* block) const {
	readCompressedBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt32));
}

void CompressedTileRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int64>* block) const {
	readCompressedBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int64));
}

void CompressedTileRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt64>* block) const {
	readCompressedBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt64));
}

void CompressedTileRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<float>* block) const {
	readCompressedBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(float));
}

void CompressedTileRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<double>* block) const {
	readCompressedBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(double));
}

void CompressedTileRasterReader::readCompressedBlockData(
	const datarepository::BlockIdx& blkIdx, 
	char* block_data,
	size_t block_size) const {
    std::string zipPath;
    std::string blockPath;
    if (Poco::File(_layerPath).exists()) { // Layer is a directory of zipped tiles with block entries
        zipPath = (boost::format("%1%%2%%3%.zip") % _layerPath % Poco::Path::separator() % _tileName).str();
        blockPath = (boost::format("%1%.blk") % blkIdx.blockIdx).str();
    } else { // Layer is a zip file where blocks are tile/block entries
        zipPath = (boost::format("%1%.zip") % _layerPath).str();
        blockPath = (boost::format("%1%/%2%.blk") % _tileName % blkIdx.blockIdx).str();
    }

	Unzipper unzipper(zipPath);
	try {
		// Zip entries are case-sensitive, but can't trust that the provider config took that into account.
		std::vector<ZipEntry> entries = unzipper.entries();
		auto it = std::find_if(entries.begin(), entries.end(), [&blockPath](const ZipEntry entry) {
			return boost::iequals(blockPath, entry.name);
		});

		std::stringstream blockData;
		unzipper.extractEntryToStream(it->name, blockData);
		blockData.read(block_data, block_size);
		unzipper.close();
	}
	catch (...) {
		unzipper.close();
		throw;
	}
}

// --------------------------------------------------------------------------------------------

CompressedStackRasterReader::CompressedStackRasterReader(
	const std::string& path, 
	const Point& origin,
	const std::string& prefix,
	const datarepository::TileBlockCellIndexer& indexer,
	const DynamicObject& settings) :
	StackRasterReaderInterface(path, origin, prefix, indexer, settings) {
    _layerPath = path;
    _tileName = (boost::format("%1%_%2%") % prefix % stack_id(origin)).str();
}

CompressedStackRasterReader::~CompressedStackRasterReader() {
}

void CompressedStackRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<Int8>* block) const {
	readCompressedBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int8));
}

void CompressedStackRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<UInt8>* block) const {
	readCompressedBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt8));
}

void CompressedStackRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<Int16>* block) const {
	readCompressedBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int16));
}

void CompressedStackRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<UInt16>* block) const {
	readCompressedBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt16));
}

void CompressedStackRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<Int32>* block) const {
	readCompressedBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int32));
}

void CompressedStackRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<UInt32>* block) const {
	readCompressedBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt32));
}

void CompressedStackRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<Int64>* block) const {
	readCompressedBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int64));
}

void CompressedStackRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<UInt64>* block) const {
	readCompressedBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt64));
}

void CompressedStackRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<float>* block) const {
	readCompressedBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(float));
}

void CompressedStackRasterReader::readBlockData(const datarepository::BlockIdx& blkIdx, int nSeries, std::vector<double>* block) const {
	readCompressedBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(double));
}

void CompressedStackRasterReader::readCompressedBlockData(
	const datarepository::BlockIdx& blkIdx, 
	int nSeries, 
	char* block_data,
	size_t block_size) const {
    std::string zipPath;
    std::string blockPath;
    if (Poco::File(_layerPath).exists()) { // Layer is a directory of zipped tiles with block entries
        zipPath = (boost::format("%1%%2%%3%.zip") % _layerPath % Poco::Path::separator() % _tileName).str();
        blockPath = (boost::format("%1%.blk") % blkIdx.blockIdx).str();
    } else { // Layer is a zip file where blocks are tile/block entries
        zipPath = (boost::format("%1%.zip") % _layerPath).str();
        blockPath = (boost::format("%1%/%2%.blk") % _tileName % blkIdx.blockIdx).str();
    }

    Unzipper unzipper(zipPath);
    try {
        // Zip entries are case-sensitive, but can't trust that the provider config took that into account.
		std::vector<ZipEntry> entries = unzipper.entries();
		auto it = std::find_if(entries.begin(), entries.end(), [&blockPath](const ZipEntry entry) {
			return boost::iequals(blockPath, entry.name);
		});

		std::stringstream blockData;
		unzipper.extractEntryToStream(it->name, blockData);
		blockData.read(block_data, block_size);
		unzipper.close();
	}
	catch (...) {
		unzipper.close();
		throw;
	}
}

// --------------------------------------------------------------------------------------------

std::unique_ptr<datarepository::MetaDataRasterReaderInterface> ZipperRasterReaderFactory::getMetaDataRasterReader(
	const std::string& path, 
	const std::string& prefix,
	const DynamicObject& settings) const {
	if (datarepository::MetaDataRasterReaderInterface::file_exists(path + ".zip")) {
		return std::make_unique<CompressedMetaDataRasterReader>(path, prefix, settings);
	}

	return std::make_unique<datarepository::FlintMetaDataRasterReader>(path, prefix, settings);
}

// --------------------------------------------------------------------------------------------

std::unique_ptr<datarepository::TileRasterReaderInterface> ZipperRasterReaderFactory::getTileRasterReader(
    const std::string& path,
    const Point& origin,
    const std::string& prefix,
    const datarepository::TileBlockCellIndexer& indexer,
    const DynamicObject& settings) const {

    auto tileName = (boost::format("%1%_%2%") % prefix % datarepository::TileRasterReaderInterface::tile_id(origin)).str();
    for (auto zipPath : {
        (boost::format("%1%%2%%3%.zip") % path % Poco::Path::separator() % tileName).str(),
        (boost::format("%1%.zip") % path).str()
    }) {
        if (datarepository::TileRasterReaderInterface::file_exists(zipPath)) {
            return std::make_unique<CompressedTileRasterReader>(path, origin, prefix, indexer, settings);
        }
    }

    return std::make_unique<datarepository::FlintTileRasterReader>(path, origin, prefix, indexer, settings);
}

// --------------------------------------------------------------------------------------------

std::unique_ptr<datarepository::StackRasterReaderInterface> ZipperRasterReaderFactory::getStackRasterReader(
    const std::string& path,
    const Point& origin,
    const std::string& prefix,
    const datarepository::TileBlockCellIndexer&
    indexer,
    const DynamicObject& settings) const {

    auto tileName = (boost::format("%1%_%2%") % prefix % datarepository::TileRasterReaderInterface::tile_id(origin)).str();
    for (auto zipPath : {
        (boost::format("%1%%2%%3%.zip") % path % Poco::Path::separator() % tileName).str(),
        (boost::format("%1%.zip") % path).str()
    }) {
        if (datarepository::StackRasterReaderInterface::file_exists(zipPath)) {
            return std::make_unique<CompressedStackRasterReader>(path, origin, prefix, indexer, settings);
        }
    }

    return std::make_unique<datarepository::FlintStackRasterReader>(path, origin, prefix, indexer, settings);
}

}}}
