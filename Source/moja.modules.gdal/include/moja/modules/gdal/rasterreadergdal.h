#pragma once
#include "moja/datarepository/rasterreaderinterface.h"

#include "moja/modules/gdal/_modules.gdal_exports.h"
namespace moja {

namespace flint {
class SpatialLocationInfo;
}  // namespace flint

namespace modules {
namespace gdal {

class GDAL_API MetaDataRasterReaderGDAL : public datarepository::MetaDataRasterReaderInterface {
  public:
   MetaDataRasterReaderGDAL(const std::string& path, const std::string& prefix, const DynamicObject& settings);
   ~MetaDataRasterReaderGDAL() override = default;

   DynamicObject readMetaData() const override;

  private:
   std::string _path;
   std::string _prefix;
};

class GDAL_API TileRasterReaderGDAL : public datarepository::TileRasterReaderInterface {
  public:
   TileRasterReaderGDAL(const std::string& path, const Point& origin, const std::string& prefix,
                       const datarepository::TileBlockCellIndexer& indexer, const DynamicObject& settings);
   ~TileRasterReaderGDAL() override;

   void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int8>* block) const override;
   void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt8>* block) const override;
   void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int16>* block) const override;
   void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt16>* block) const override;
   void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int32>* block) const override;
   void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt32>* block) const override;
   void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<Int64>* block) const override;
   void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<UInt64>* block) const override;
   void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<float>* block) const override;
   void readBlockData(const datarepository::BlockIdx& blkIdx, std::vector<double>* block) const override;

  private:
   void readBlockData(const datarepository::BlockIdx& blkIdx, char* block_data, size_t block_size) const;

   std::string _path;
   std::string _prefix;
   std::string _variable;
   std::string _runId;
};

class GDAL_API RasterReaderFactoryGDAL : public moja::datarepository::RasterReaderFactoryInterface {
  public:
   RasterReaderFactoryGDAL() = default;

   bool hasMetaDataReader() const override { return false; }

   bool hasTileRasterReader() const override { return true; }

   bool hasStackRasterReader() const override { return false; }

   std::unique_ptr<datarepository::MetaDataRasterReaderInterface> getMetaDataRasterReader(
       const std::string& path, const std::string& prefix, const DynamicObject& settings) const override {
      return nullptr;
   }

   std::unique_ptr<datarepository::TileRasterReaderInterface> getTileRasterReader(
       const std::string& path, const Point& origin, const std::string& prefix,
       const datarepository::TileBlockCellIndexer& indexer, const DynamicObject& settings) const override {
      return std::make_unique<TileRasterReaderGDAL>(path, origin, prefix, indexer, settings);
   }

   std::unique_ptr<datarepository::StackRasterReaderInterface> getStackRasterReader(
       const std::string& path, const Point& origin, const std::string& prefix,
       const datarepository::TileBlockCellIndexer& indexer, const DynamicObject& settings) const override {
      return nullptr;
   }

   std::string _variable;
};
}  // namespace gdal
}  // namespace modules
}