#ifndef MOJA_DATAREPOSITORY_RASTERREADERINTERFACE_H_
#define MOJA_DATAREPOSITORY_RASTERREADERINTERFACE_H_

#include "moja/datarepository/_datarepository_exports.h"

#include <moja/dynamic.h>
#include <moja/utility.h>
#include <moja/filesystem.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace fs = moja::filesystem;

namespace moja {
namespace datarepository {

class TileBlockCellIndexer;
struct BlockIdx;

// --------------------------------------------------------------------------------------------

class DATAREPOSITORY_API MetaDataRasterReaderInterface {
  public:
   MetaDataRasterReaderInterface(const std::string& path, const std::string& prefix, const DynamicObject& settings)
       : _settings(settings){};
   virtual ~MetaDataRasterReaderInterface() = default;

   virtual DynamicObject readMetaData() const = 0;

   static bool file_exists(const std::string& path);
   static std::string tile_id(const Point& origin);

   DynamicObject _settings;
};

// --------------------------------------------------------------------------------------------

inline bool MetaDataRasterReaderInterface::file_exists(const std::string& path) {
   return fs::exists(path);
}

// --------------------------------------------------------------------------------------------

inline std::string MetaDataRasterReaderInterface::tile_id(const Point& origin) {
   std::ostringstream str;
   str << std::setfill('0');
   str << (origin.lon < 0 ? "-" : "") << std::setw(3) << std::abs(origin.lon) << "_" << (origin.lat < 0 ? "-" : "")
       << std::setw(3) << std::abs(origin.lat);
   return str.str();
}

// --------------------------------------------------------------------------------------------

class DATAREPOSITORY_API TileRasterReaderInterface {
  public:
   TileRasterReaderInterface(const std::string& path, const Point& origin, const std::string& prefix,
                             const TileBlockCellIndexer& indexer, const DynamicObject& settings)
       : _origin(origin), _indexer(indexer), _settings(settings) {}
   virtual ~TileRasterReaderInterface() = default;

   virtual void readBlockData(const BlockIdx& blkIdx, std::vector<Int8>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, std::vector<UInt8>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, std::vector<Int16>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, std::vector<UInt16>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, std::vector<Int32>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, std::vector<UInt32>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, std::vector<Int64>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, std::vector<UInt64>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, std::vector<float>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, std::vector<double>* block) const = 0;

   static bool file_exists(const std::string& path);
   static std::string tile_id(const Point& origin);

  protected:
   Point _origin;
   const TileBlockCellIndexer& _indexer;
   DynamicObject _settings;
};

// --------------------------------------------------------------------------------------------

inline bool TileRasterReaderInterface::file_exists(const std::string& path) {
   return fs::exists(path);
}

// --------------------------------------------------------------------------------------------

inline std::string TileRasterReaderInterface::tile_id(const Point& origin) {
   std::ostringstream str;
   str << std::setfill('0');
   str << (origin.lon < 0 ? "-" : "") << std::setw(3) << std::abs(origin.lon) << "_" << (origin.lat < 0 ? "-" : "")
       << std::setw(3) << std::abs(origin.lat);
   return str.str();
}

// --------------------------------------------------------------------------------------------

class DATAREPOSITORY_API StackRasterReaderInterface {
  public:
   StackRasterReaderInterface(const std::string& path, const Point& origin, const std::string& prefix,
                              const TileBlockCellIndexer& indexer, const DynamicObject& settings)
       : _origin(origin), _indexer(indexer), _settings(settings) {}
   virtual ~StackRasterReaderInterface() = default;

   virtual void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<Int8>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<UInt8>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<Int16>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<UInt16>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<Int32>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<UInt32>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<Int64>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<UInt64>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<float>* block) const = 0;
   virtual void readBlockData(const BlockIdx& blkIdx, int nSeries, std::vector<double>* block) const = 0;

   static bool file_exists(const std::string& path);
   static std::string stack_id(const Point& origin);

  protected:
   Point _origin;
   const TileBlockCellIndexer& _indexer;
   DynamicObject _settings;
};

// --------------------------------------------------------------------------------------------

inline bool StackRasterReaderInterface::file_exists(const std::string& path) {
   return fs::exists(path);
}

// --------------------------------------------------------------------------------------------

inline std::string StackRasterReaderInterface::stack_id(const Point& origin) {
   std::ostringstream str;
   str << std::setfill('0');
   str << (origin.lon < 0 ? "-" : "") << std::setw(3) << std::abs(origin.lon) << "_" << (origin.lat < 0 ? "-" : "")
       << std::setw(3) << std::abs(origin.lat);
   return str.str();
}

// --------------------------------------------------------------------------------------------

class DATAREPOSITORY_API RasterReaderFactoryInterface {
  public:
   RasterReaderFactoryInterface() {}
   virtual ~RasterReaderFactoryInterface() = default;

   virtual bool hasMetaDataReader() const = 0;
   virtual bool hasTileRasterReader() const = 0;
   virtual bool hasStackRasterReader() const = 0;

   virtual std::unique_ptr<MetaDataRasterReaderInterface> getMetaDataRasterReader(
       const std::string& path, const std::string& prefix, const DynamicObject& settings) const = 0;
   virtual std::unique_ptr<TileRasterReaderInterface> getTileRasterReader(const std::string& path, const Point& origin,
                                                                          const std::string& prefix,
                                                                          const TileBlockCellIndexer& indexer,
                                                                          const DynamicObject& settings) const = 0;
   virtual std::unique_ptr<StackRasterReaderInterface> getStackRasterReader(const std::string& path,
                                                                            const Point& origin,
                                                                            const std::string& prefix,
                                                                            const TileBlockCellIndexer& indexer,
                                                                            const DynamicObject& settings) const = 0;
};

}  // namespace datarepository
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_RASTERREADERINTERFACE_H_