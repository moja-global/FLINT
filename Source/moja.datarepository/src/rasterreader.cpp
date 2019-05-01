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
#include <fstream>
#include <sstream>

namespace moja {
	namespace datarepository {

		// --------------------------------------------------------------------------------------------

		FlintMetaDataRasterReader::FlintMetaDataRasterReader(
			const std::string& path, 
			const std::string& prefix, 
			const DynamicObject& settings) :
			MetaDataRasterReaderInterface(path, prefix, settings) {
			_metaPath = (boost::format("%1%%2%%3%.json") % path % Poco::Path::separator() % prefix).str();
		}

		DynamicObject FlintMetaDataRasterReader::readMetaData() const {
			if (file_exists(_metaPath)) {
				Poco::JSON::Parser jsonMetadataParser;
				//Poco::Dynamic::Var parsedMetadata;

				std::ifstream metadataFile(_metaPath, std::ifstream::in);
				jsonMetadataParser.parse(metadataFile);

				auto metadata = jsonMetadataParser.result();
				auto layerMetadata = parsePocoJSONToDynamic(metadata).extract<const DynamicObject>();
				return layerMetadata;
			} 
			else
			{
				BOOST_THROW_EXCEPTION(FileNotFoundException() << FileName(_metaPath));
			}
		}

		// --------------------------------------------------------------------------------------------

		FlintTileRasterReader::FlintTileRasterReader(const std::string& path, const Point& origin,
			const std::string& prefix,
			const TileBlockCellIndexer& indexer,
			const DynamicObject& settings) :
			TileRasterReaderInterface(path, origin, prefix, indexer, settings) {
			_tilePath = (boost::format("%1%%2%%3%_%4%.blk") % path % Poco::Path::separator() % prefix % tile_id(origin)).str();
		}

		FlintTileRasterReader::~FlintTileRasterReader() {}

		void FlintTileRasterReader::readBlockData(const BlockIdx& blkIdx, std::vector<Int8>* block) const {
			readFlintBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int8));
		}

		void FlintTileRasterReader::readBlockData(const BlockIdx& blkIdx, std::vector<UInt8>* block) const {
			readFlintBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt8));
		}

		void FlintTileRasterReader::readBlockData(const BlockIdx& blkIdx, std::vector<Int16>* block) const {
			readFlintBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int16));
		}

		void FlintTileRasterReader::readBlockData(const BlockIdx& blkIdx, std::vector<UInt16>* block) const {
			readFlintBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt16));
		}

		void FlintTileRasterReader::readBlockData(const BlockIdx& blkIdx, std::vector<Int32>* block) const {
			readFlintBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int32));
		}

		void FlintTileRasterReader::readBlockData(const BlockIdx& blkIdx, std::vector<UInt32>* block) const {
			readFlintBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt32));
		}

		void FlintTileRasterReader::readBlockData(const BlockIdx& blkIdx, std::vector<Int64>* block) const {
			readFlintBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int64));
		}

		void FlintTileRasterReader::readBlockData(const BlockIdx& blkIdx, std::vector<UInt64>* block) const {
			readFlintBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt64));
		}

		void FlintTileRasterReader::readBlockData(const BlockIdx& blkIdx, std::vector<float>* block) const {
			readFlintBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(float));
		}

		void FlintTileRasterReader::readBlockData(const BlockIdx& blkIdx, std::vector<double>* block) const {
			readFlintBlockData(blkIdx, reinterpret_cast<char*>(block->data()), block->size() * sizeof(double));
		}

		void FlintTileRasterReader::readFlintBlockData(const BlockIdx& blk_idx, char* block_data, size_t block_size) const {
			std::ifstream fileStream;
			try {
				fileStream.open(_tilePath, std::ios::binary);
				std::streampos blockStart = blk_idx.blockIdx * block_size;
				if (fileStream.fail()) {
					BOOST_THROW_EXCEPTION(FileReadException()
						<< FileName(_tilePath)
						<< Message(strerror(errno)));
				}

				fileStream.seekg(blockStart);
				fileStream.read(block_data, block_size);
				fileStream.close();
			}
			catch (...) {
				fileStream.close();
				throw;
			}
		}

		// --------------------------------------------------------------------------------------------

		FlintStackRasterReader::FlintStackRasterReader(
			const std::string& path, 
			const Point& origin,
			const std::string& prefix,
			const TileBlockCellIndexer& indexer,
			const DynamicObject& settings) :
			StackRasterReaderInterface(path, origin, prefix, indexer, settings) {
			_tilePath = (boost::format("%1%%2%%3%_%4%.blk") % path % Poco::Path::separator() % prefix % stack_id(origin)).str();
		}

		FlintStackRasterReader::~FlintStackRasterReader() {}

		void FlintStackRasterReader::readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<Int8>* block) const {
			readFlintBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int8));
		}

		void FlintStackRasterReader::readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<UInt8>* block) const {
			readFlintBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt8));
		}

		void FlintStackRasterReader::readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<Int16>* block) const {
			readFlintBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int16));
		}

		void FlintStackRasterReader::readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<UInt16>* block) const {
			readFlintBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt16));
		}

		void FlintStackRasterReader::readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<Int32>* block) const {
			readFlintBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int32));
		}

		void FlintStackRasterReader::readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<UInt32>* block) const {
			readFlintBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt32));
		}

		void FlintStackRasterReader::readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<Int64>* block) const {
			readFlintBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(Int64));
		}

		void FlintStackRasterReader::readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<UInt64>* block) const {
			readFlintBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(UInt64));
		}

		void FlintStackRasterReader::readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<float>* block) const {
			readFlintBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(float));
		}

		void FlintStackRasterReader::readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<double>* block) const {
			readFlintBlockData(blkIdx, nSeries, reinterpret_cast<char*>(block->data()), block->size() * sizeof(double));
		}

		void FlintStackRasterReader::readFlintBlockData(const BlockIdx& blk_idx, int nSeries, char* block_data, size_t block_size) const {
			std::ifstream fileStream;
			try {
				fileStream.open(_tilePath, std::ios::binary);
				std::streampos blockStart = blk_idx.blockIdx * block_size;
				if (fileStream.fail()) {
					BOOST_THROW_EXCEPTION(FileReadException()
						<< FileName(_tilePath)
						<< Message(strerror(errno)));
				}

				fileStream.seekg(blockStart);
				fileStream.read(block_data, block_size);
				fileStream.close();
			}
			catch (...) {
				fileStream.close();
				throw;
			}
		}

		// --------------------------------------------------------------------------------------------

		std::unique_ptr<MetaDataRasterReaderInterface> FlintRasterReaderFactory::getMetaDataRasterReader(
			const std::string& path, 
			const std::string& prefix, 
			const DynamicObject& settings) const
		{
			return std::make_unique<FlintMetaDataRasterReader>(path, prefix, settings);
		}

		// --------------------------------------------------------------------------------------------

		std::unique_ptr<TileRasterReaderInterface> FlintRasterReaderFactory::getTileRasterReader(
			const std::string& path,
			const Point& origin,
			const std::string& prefix,
			const TileBlockCellIndexer&
			indexer,
			const DynamicObject& settings) const
		{
			return std::make_unique<FlintTileRasterReader>(path, origin, prefix, indexer, settings);
		}

		// --------------------------------------------------------------------------------------------

		std::unique_ptr<StackRasterReaderInterface> FlintRasterReaderFactory::getStackRasterReader(
			const std::string& path,
			const Point& origin,
			const std::string& prefix,
			const TileBlockCellIndexer&
			indexer,
			const DynamicObject& settings) const
		{
			return std::make_unique<FlintStackRasterReader>(path, origin, prefix, indexer, settings);
		}
	}
} // moja::datarepository
