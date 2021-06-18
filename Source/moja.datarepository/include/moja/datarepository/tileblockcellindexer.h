#ifndef MOJA_DATAREPOSITORY_TILEBLOCKCELLINDEXER_H_
#define MOJA_DATAREPOSITORY_TILEBLOCKCELLINDEXER_H_

#include "moja/datarepository/_datarepository_exports.h"

#include <moja/hash.h>
#include <moja/mathex.h>
#include <moja/utility.h>

#include <chrono>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <stdexcept>

namespace moja {
namespace datarepository {

struct TileIdx;
struct BlockIdx;
struct CellIdx;

class DATAREPOSITORY_API Rectangle {  // works from top,left corner
  public:
   Rectangle() = default;
   Rectangle(Point p, Size s);
   Rectangle(double x, double y, double width, double height);
   ~Rectangle();

   double x() const;  // x-coordinate of the upper-left corner of this Rectangle structure
   double y() const;  // y-coordinate of the upper-left corner of this Rectangle structure
   double bottom()
       const;  // y-coordinate that is the sum of the Y and Height property values of this Rectangle structure
   double right() const;   // x-coordinate that is the sum of X and Width property values of this Rectangle structure
   double height() const;  // height of this Rectangle structure
   double width() const;   // width of this Rectangle structure
   const Point& location() const;

  private:
   Point _location;  // coordinates of the upper-left corner of this Rectangle structure (lat,lon)
   Size _size;
};

inline double Rectangle::x() const { return _location.lon; };
inline double Rectangle::y() const { return _location.lat; };
inline double Rectangle::bottom() const { return _location.lat - _size.height; };
inline double Rectangle::right() const { return _location.lon + _size.width; };
inline double Rectangle::height() const { return _size.height; };
inline double Rectangle::width() const { return _size.width; };
inline const Point& Rectangle::location() const { return _location; };

// --------------------------------------------------------------------------------------------

struct TileBlockCellLayerDesc {
   std::string name;
   double latSize;
   double lonSize;
   double latSizeInv;
   double lonSizeInv;
   UInt32 rows;
   UInt32 cols;
   UInt32 indexLimit;  // rows*cols
};

// --------------------------------------------------------------------------------------------

class DATAREPOSITORY_API TileBlockCellIndexer {  // works from top,left corner
  public:
   TileBlockCellIndexer(double tileLatSize, double tileLonSize, double blockLatSize, double blockLonSize,
                        double cellLatSize, double cellLonSize);
   TileBlockCellIndexer(UInt32 tileRows, UInt32 tileCols, UInt32 blockRows, UInt32 blockCols, UInt32 cellRows,
                        UInt32 cellCols);

   void SetGlobalData();

   void SanityChecksLatLon(double tileLatSize, double tileLonSize, double blockLatSize, double blockLonSize,
                           double cellLatSize, double cellLonSize);
   void SanityChecksIndex(UInt32 tileRows, UInt32 tileCols, UInt32 blockRows, UInt32 blockCols, UInt32 cellRows,
                          UInt32 cellCols);

   std::string toString() const;

   Point getLatLonFromIndex(const CellIdx& cellIndex) const;
   Point getLatLonFromIndex(const BlockIdx& blockIndex) const;
   Point getLatLonFromIndex(const TileIdx& tileIndex) const;

   double getLatFromIndex(const CellIdx& cellIndex) const;
   double getLatFromIndex(const BlockIdx& blockIndex) const;
   double getLatFromIndex(const TileIdx& tileIndex) const;

   Point getOriginFromIndex(const CellIdx& cellIndex) const;
   Point getOriginFromIndex(const BlockIdx& blockIndex) const;
   Point getOriginFromIndex(const TileIdx& tileIndex) const;

   CellIdx getCellIndexFromLatLon(const Point& point) const;
   BlockIdx getBlockIndexFromLatLon(const Point& point) const;
   TileIdx getTileIndexFromLatLon(const Point& point) const;

   CellIdx convertIndex(const CellIdx& cellIndex) const;

   bool compare(const TileBlockCellIndexer& indexer) const {
      return (cellResCompareValueLon == indexer.cellResCompareValueLon &&
              cellResCompareValueLat == indexer.cellResCompareValueLat);
   }
   bool compare(TileBlockCellIndexer& indexer) const {
      return (cellResCompareValueLon == indexer.cellResCompareValueLon &&
              cellResCompareValueLat == indexer.cellResCompareValueLat);
   }

   TileBlockCellLayerDesc globalDesc;
   TileBlockCellLayerDesc tileDesc;
   TileBlockCellLayerDesc blockDesc;
   TileBlockCellLayerDesc cellDesc;

  private:
   Int32 cellResCompareValueLon;
   Int32 cellResCompareValueLat;

   static double convertFromLonOrigin(double orig);
   static double convertFromLatOrigin(double orig);

   static double convertToLonOrigin(double orig);
   static double convertToLatOrigin(double orig);

   // Convert Origin to Index
   static UInt32 convOtoI(double o, double partSize);
};

inline double TileBlockCellIndexer::convertFromLonOrigin(double orig) { return orig - 180.0; }
inline double TileBlockCellIndexer::convertFromLatOrigin(double orig) { return orig * -1 + 90.0; }
inline double TileBlockCellIndexer::convertToLonOrigin(double value) { return value + 180.0; }
inline double TileBlockCellIndexer::convertToLatOrigin(double value) { return value * -1 + 90.0; }
inline UInt32 TileBlockCellIndexer::convOtoI(double o, double partSize) {
   return UInt32((o / partSize) + MathEx::k0Plus);
}

// --------------------------------------------------------------------------------------------

// The TileBlockCellIndexer assumes it's working on a global data set for referencing purposes
// Starting at the global level (90 -> -90 (Lat), -180 -> 180 (Lon)) -> 180 (Y),360 (X)
// Then Tile  Lat size & Lon size (user defined tile  size that fits in the global size a whole number of times),
// i.e. 1.0 Then Block Lat size & Lon size (user defined block size that fits in the tile   size a whole number of
// times), i.e. 0.1 Then Cell  Lat size & Lon size (user defined cell  size that fits in the block  size a whole number
// of times), i.e. 0.00025
struct DATAREPOSITORY_API TileIdx {
   TileIdx() = delete;

   TileIdx(const Point point, const TileBlockCellIndexer& indexer) : tileCols(indexer.tileDesc.cols) {
      auto tile = indexer.getTileIndexFromLatLon(point);
      tileIdx = tile.tileIdx;
   }

   TileIdx(const XYIndex tile, const TileBlockCellIndexer& indexer)
       : tileCols(indexer.tileDesc.cols), tileIdx(tile.Y * tileCols + tile.X) {}

   TileIdx(const UInt32 tileIdx, const TileBlockCellIndexer& indexer)
       : tileCols(indexer.tileDesc.cols), tileIdx(tileIdx) {}

   std::string toString() const;

   UInt32 tileX() const;
   UInt32 tileY() const;

   UInt32 tileCols;
   UInt32 tileIdx;
};

inline UInt32 TileIdx::tileX() const { return (tileIdx % tileCols); }
inline UInt32 TileIdx::tileY() const { return (tileIdx / tileCols); }

// --------------------------------------------------------------------------------------------

struct DATAREPOSITORY_API BlockIdx {
   BlockIdx() = delete;

   BlockIdx(const BlockIdx& src)
       : tileCols(src.tileCols), tileIdx(src.tileIdx), blockCols(src.blockCols), blockIdx(src.blockIdx) {}

   BlockIdx(const XYIndex tile, const XYIndex block, const TileBlockCellIndexer& indexer)
       : tileCols(indexer.tileDesc.cols), blockCols(indexer.blockDesc.cols) {
      auto t_idx = tile.Y * tileCols + tile.X;
      auto b_idx = block.Y * blockCols + block.X;

      if (t_idx > indexer.tileDesc.indexLimit - 1) {
         std::stringstream ss;
         ss << "tile index > index limit " << "tile.X:" << tile.X << " tile.Y:" << tile.Y;
         throw std::range_error(ss.str());
      }

      if (b_idx > indexer.blockDesc.indexLimit - 1) {
         std::stringstream ss;
         ss << "block index > index limit block.X:" << block.X << "block.Y:" << block.Y;
         throw std::range_error(ss.str());
      }

      tileIdx = t_idx;
      blockIdx = b_idx;
   }

   BlockIdx(const UInt32 tileIdx, const UInt32 blockIdx, const TileBlockCellIndexer& indexer)
       : tileCols(indexer.tileDesc.cols), blockCols(indexer.blockDesc.cols) {
      if (tileIdx > indexer.tileDesc.indexLimit - 1) {
         std::stringstream ss;
         ss << "tile index > index limit tileIdx:" << tileIdx;
         throw std::invalid_argument(ss.str());
      }

      if (blockIdx > indexer.blockDesc.indexLimit - 1) {
         std::stringstream ss;
         ss << "block index > index limit blockIdx:" << blockIdx;
         throw std::invalid_argument(ss.str());
      }

      this->tileIdx = tileIdx;
      this->blockIdx = blockIdx;
   }

   std::string toString() const;

   UInt32 tileX() const;
   UInt32 tileY() const;
   UInt32 blockX() const;
   UInt32 blockY() const;

   UInt32 tileCols;
   UInt32 tileIdx;
   UInt32 blockCols;
   UInt32 blockIdx;
};

inline UInt32 BlockIdx::tileX() const { return (tileIdx % tileCols); }
inline UInt32 BlockIdx::tileY() const { return (tileIdx / tileCols); }
inline UInt32 BlockIdx::blockX() const { return (blockIdx % blockCols); }
inline UInt32 BlockIdx::blockY() const { return (blockIdx / blockCols); }

// --------------------------------------------------------------------------------------------

struct DATAREPOSITORY_API CellIdx {
   CellIdx() = delete;

   CellIdx(const XYIndex tile, const XYIndex block, const XYIndex cell, const TileBlockCellIndexer& indexer)
       : tileCols(indexer.tileDesc.cols),
         tileIdx(tile.Y * tileCols + tile.X),
         blockCols(indexer.blockDesc.cols),
         blockIdx(block.Y * blockCols + block.X),
         cellCols(indexer.cellDesc.cols),
         cellIdx(cell.Y * cellCols + cell.X),
         _indexer(&indexer) {}

   CellIdx(const UInt32 tileIdx, const UInt32 blockIdx, const UInt32 cellIdx, const TileBlockCellIndexer& indexer)
       : tileCols(indexer.tileDesc.cols),
         tileIdx(tileIdx),
         blockCols(indexer.blockDesc.cols),
         blockIdx(blockIdx),
         cellCols(indexer.cellDesc.cols),
         cellIdx(cellIdx),
         _indexer(&indexer) {}

   std::string toString() const;

   UInt32 tileX() const;
   UInt32 tileY() const;
   UInt32 blockX() const;
   UInt32 blockY() const;
   UInt32 cellX() const;
   UInt32 cellY() const;
   size_t hash() const;

   UInt32 tileCols;
   UInt32 tileIdx;
   UInt32 blockCols;
   UInt32 blockIdx;
   UInt32 cellCols;
   UInt32 cellIdx;
   const TileBlockCellIndexer* _indexer;
};

inline size_t CellIdx::hash() const { return hash::hash_combine(tileIdx, blockIdx, cellIdx); }

inline UInt32 CellIdx::tileX() const { return (tileIdx % tileCols); }
inline UInt32 CellIdx::tileY() const { return (tileIdx / tileCols); }
inline UInt32 CellIdx::blockX() const { return (blockIdx % blockCols); }
inline UInt32 CellIdx::blockY() const { return (blockIdx / blockCols); }
inline UInt32 CellIdx::cellX() const { return UInt32(cellIdx % cellCols); }
inline UInt32 CellIdx::cellY() const { return UInt32(cellIdx / cellCols); }

class CellIdxHasher {
  public:
   size_t operator()(const CellIdx& ci) const { return hash::hash_combine(ci.tileIdx, ci.blockIdx, ci.cellIdx); }
};

}  // namespace datarepository

// --------------------------------------------------------------------------------------------

// Tile simulation statistics utility structure
struct DATAREPOSITORY_API blockStats {
   using block_clock = std::chrono::steady_clock;
   using block_time_point = std::chrono::time_point<block_clock>;

   using cell_clock = std::chrono::high_resolution_clock;
   using cell_time_point = std::chrono::time_point<cell_clock>;
   using duration = std::chrono::nanoseconds;

   blockStats() : cellProcessing(0), cells(0), processed(0), errors(0) {
      tileIdx = -1;
      blockIdx = -1;
   }

   void startBlock(const datarepository::CellIdx& cell);
   void startBlock(const datarepository::BlockIdx& block);
   void endBlock();
   void startCell();
   void endCell();

   // Data members
   block_time_point startTime;
   block_time_point endTime;
   cell_time_point cellStart;
   duration cellProcessing;
   Int64 cells;
   Int64 processed;
   Int64 errors;

   UInt32 tileIdx;
   UInt32 blockIdx;
};
inline void blockStats::startBlock(const datarepository::CellIdx& cell) {
   tileIdx = cell.tileIdx;
   blockIdx = cell.blockIdx;
   startTime = block_clock::now();
}
inline void blockStats::startBlock(const datarepository::BlockIdx& block) {
   tileIdx = block.tileIdx;
   blockIdx = block.blockIdx;
   startTime = block_clock::now();
}
inline void blockStats::endBlock() {
   endTime = block_clock::now();
   tileIdx = -1;
   blockIdx = -1;
}
inline void blockStats::startCell() {
   ++processed;
   cellStart = cell_clock::now();
}
inline void blockStats::endCell() { cellProcessing += cell_clock::now() - cellStart; }

// --------------------------------------------------------------------------------------------

struct DATAREPOSITORY_API blockCollectionStats {
   blockCollectionStats()
       : curTileIdx(-1), curBlockIdx(-1), curBlock(nullptr), blocksProcessed(0), cellsInspected(0), cellsProcessed(0) {}

   // return true if a block starts
   template <typename T>
   bool checkStartBlock(const T& idx) {
      if (curTileIdx != idx.tileIdx || curBlockIdx != idx.blockIdx) {
         checkEndBlock(idx);  // end previous block
         curTileIdx = idx.tileIdx;
         curBlockIdx = idx.blockIdx;
         const auto blockKey = std::make_tuple(curTileIdx, curBlockIdx);
         const auto it = blockStatMap.find(blockKey);
         if (it != blockStatMap.end()) {
            BOOST_THROW_EXCEPTION(moja::datarepository::DataRepositoryException());
         }
         _blockStats.emplace_back(blockStats{});
         curBlock = &_blockStats.back();
         blockStatMap.emplace(std::pair<BlockKey, blockStats*>{blockKey, curBlock});
         curBlock->startBlock(idx);
         return true;
      }
      return false;
   }

   void endCurBlock() {
      if (curBlock) {
         curBlock->endBlock();  // assume that we must've had this block start, so don't check it exists in map
      }
      curBlock = nullptr;
      curTileIdx = -1;
      curBlockIdx = -1;
   }
   template <typename T>
   bool checkEndBlock(const T& idx) {
      if ((curTileIdx != -1 && curBlockIdx != -1) && (curTileIdx != idx.tileIdx || curBlockIdx != idx.blockIdx)) {
         endCurBlock();
         return true;
      }
      return false;
   }

   void addInspectedCell() const;
   void startProcessedCell() const;
   void endProcessedCell() const;

   void calculateStats() {
      blocksProcessed = 0;
      cellsInspected = 0;
      cellsProcessed = 0;

      for (auto& block : _blockStats) {
         blocksProcessed++;
         cellsProcessed += block.processed;
         cellsInspected += block.cells;
      }
   }

   // Data members
   UInt32 curTileIdx;
   UInt32 curBlockIdx;
   blockStats* curBlock;

   int blocksProcessed;
   Int64 cellsInspected;
   Int64 cellsProcessed;

   typedef std::tuple<UInt32, UInt32> BlockKey;  // tile, block
   std::vector<blockStats> _blockStats;
   std::unordered_map<BlockKey, blockStats*, moja::Hash> blockStatMap;
};
inline void blockCollectionStats::startProcessedCell() const { curBlock->startCell(); }
inline void blockCollectionStats::endProcessedCell() const { curBlock->endCell(); }
inline void blockCollectionStats::addInspectedCell() const { curBlock->cells++; }

// --------------------------------------------------------------------------------------------

// Tile simulation statistics utility structure
struct DATAREPOSITORY_API tileStats {
   using clock = std::chrono::steady_clock;
   using timepoint = std::chrono::time_point<clock>;

   tileStats() : startTime(clock::now()), endTime(clock::now()), tileIdx(-1) {}

   void startTile(const datarepository::CellIdx& cell) {
      startTime = clock::now();
      tileIdx = cell.tileIdx;
   }

   void startTile(const datarepository::TileIdx& tile) {
      startTime = clock::now();
      tileIdx = tile.tileIdx;
   }

   void endTile() {
      endTime = clock::now();
      tileIdx = -1;
   }

   // Data members
   timepoint startTime;
   timepoint endTime;
   UInt32 tileIdx;
};

// --------------------------------------------------------------------------------------------

// LocalDomain simulation statistics utility structure
struct DATAREPOSITORY_API tileCollectionStats {
   using clock = std::chrono::steady_clock;
   using timepoint = std::chrono::time_point<clock>;
   using duration = std::chrono::nanoseconds;

   tileCollectionStats()
       : startTime(clock::now()),
         endTime(clock::now()),
         tilesProcessed(0),
         totalTileTime(0),
         curTileIdx(-1),
         curTile(nullptr) {}

   template <typename T>
   bool checkStartTile(const T& tile) {
      if (curTileIdx != tile.tileIdx) {
         checkEndTile();  // end previous tile cell.tileIdx not current
         curTileIdx = tile.tileIdx;
         auto tileIt = tileStatMap.find(tile.tileIdx);
         if (tileIt != tileStatMap.end()) {
            // Error - started a tile again!
            curTile = nullptr;
            return false;
         }
         _tileStats.emplace_back(tileStats{});
         curTile = &_tileStats.back();
         tileStatMap.emplace({tile.tileIdx, curTile});
         return true;
      }
      return false;
   }

   bool checkEndTile() {
      if (curTileIdx != -1) {
         if (curTile) {
            curTile->endTile();  // assume that we must've had this tile start, so don't check it exists in map
         }
         curTile = nullptr;
         curTileIdx = -1;
         return true;
      }
      return false;
   }

   void startGroup() {
      startTime = clock::now();
      endTime = clock::now();
      tilesProcessed = 0;
      totalTileTime = duration(0);
   }

   void endGroup() {
      endTime = clock::now();
      tilesProcessed = 0;
      totalTileTime = duration(0);
      for (auto& tile : _tileStats) {
         tilesProcessed++;
         totalTileTime += (tile.endTime - tile.startTime);
      }
   }

   // Data members
   timepoint startTime;
   timepoint endTime;

   int tilesProcessed;
   duration totalTileTime;

   UInt32 curTileIdx;
   tileStats* curTile;
   std::vector<tileStats> _tileStats;
   std::unordered_map<UInt32, tileStats*> tileStatMap;
};

}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_TILEBLOCKCELLINDEXER_H_
