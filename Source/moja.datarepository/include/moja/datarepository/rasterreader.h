#ifndef MOJA_DATAREPOSITORY_RASTERREADER_H_
#define MOJA_DATAREPOSITORY_RASTERREADER_H_

#include "moja/datarepository/_datarepository_exports.h"
#include "moja/utility.h"
#include "rasterreaderinterface.h"

#include <string>
#include <vector>

namespace moja {
	namespace datarepository {

		class TileBlockCellIndexer;
		struct BlockIdx;


		// --------------------------------------------------------------------------------------------
		// *** METADATA METHODS ***
		// --------------------------------------------------------------------------------------------

		class DATAREPOSITORY_API FlintMetaDataRasterReader : public MetaDataRasterReaderInterface {
		public:
			FlintMetaDataRasterReader(const std::string& path, const std::string& prefix, const DynamicObject& settings);
			~FlintMetaDataRasterReader() override {}

			DynamicObject readMetaData() const override;

		private:
			std::string _metaPath;
		};

		// --------------------------------------------------------------------------------------------
		// *** TILE METHODS ***
		// --------------------------------------------------------------------------------------------

		// --------------------------------------------------------------------------------------------

		class DATAREPOSITORY_API FlintTileRasterReader : public TileRasterReaderInterface {
		public:
			FlintTileRasterReader(
				const std::string& path, 
				const Point& origin, 
				const std::string& prefix,
				const TileBlockCellIndexer& indexer, 
				const DynamicObject& settings);
			~FlintTileRasterReader() override;
			void readBlockData(const BlockIdx& blkIdx, std::vector<Int8>*	block) const override;
			void readBlockData(const BlockIdx& blkIdx, std::vector<UInt8>*	block) const override;
			void readBlockData(const BlockIdx& blkIdx, std::vector<Int16>*	block) const override;
			void readBlockData(const BlockIdx& blkIdx, std::vector<UInt16>* block) const override;
			void readBlockData(const BlockIdx& blkIdx, std::vector<Int32>*	block) const override;
			void readBlockData(const BlockIdx& blkIdx, std::vector<UInt32>* block) const override;
			void readBlockData(const BlockIdx& blkIdx, std::vector<Int64>*	block) const override;
			void readBlockData(const BlockIdx& blkIdx, std::vector<UInt64>* block) const override;
			void readBlockData(const BlockIdx& blkIdx, std::vector<float>*	block) const override;
			void readBlockData(const BlockIdx& blkIdx, std::vector<double>* block) const override;
		private:
			void readFlintBlockData(const BlockIdx& blkIdx, char* block_data, size_t block_size) const;

			std::string _tilePath;
		};

		// --------------------------------------------------------------------------------------------
		// *** STACK METHODS ***
		// --------------------------------------------------------------------------------------------

		// --------------------------------------------------------------------------------------------

		class DATAREPOSITORY_API FlintStackRasterReader : public StackRasterReaderInterface {
		public:
			FlintStackRasterReader(
				const std::string& path, 
				const Point& origin, 
				const std::string& prefix, 
				const TileBlockCellIndexer& indexer,
				const DynamicObject& settings);
			~FlintStackRasterReader() override;
			void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<Int8>*		block) const override;
			void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<UInt8>*		block) const override;
			void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<Int16>*		block) const override;
			void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<UInt16>*	block) const override;
			void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<Int32>*		block) const override;
			void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<UInt32>*	block) const override;
			void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<Int64>*		block) const override;
			void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<UInt64>*	block) const override;
			void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<float>*		block) const override;
			void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<double>*	block) const override;
		private:
			void readFlintBlockData(const BlockIdx& blkIdx, int nSeries, char* block_data, size_t block_size) const;

			std::string _tilePath;
		};


		// --------------------------------------------------------------------------------------------

		class DATAREPOSITORY_API FlintRasterReaderFactory : public RasterReaderFactoryInterface {
		public:
			FlintRasterReaderFactory() {}

			bool hasMetaDataReader() const override {
				return true;
			}

			bool hasTileRasterReader() const override {
				return true;
			}

			bool hasStackRasterReader()	const override {
				return true;
			}

			std::unique_ptr<MetaDataRasterReaderInterface> getMetaDataRasterReader(
				const std::string& path, 
				const std::string& prefix, 
				const DynamicObject& settings) const override;

			std::unique_ptr<TileRasterReaderInterface> getTileRasterReader(
				const std::string& path, 
				const Point& origin,
				const std::string& prefix,
				const TileBlockCellIndexer& indexer,
				const DynamicObject& settings) const override;

			std::unique_ptr<StackRasterReaderInterface> getStackRasterReader(
				const std::string& path, 
				const Point& origin,
				const std::string& prefix,
				const TileBlockCellIndexer& indexer,
				const DynamicObject& settings) const override;
		};

	}
} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_RASTERREADER_H_