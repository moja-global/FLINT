#ifndef MOJA_DATAREPOSITORY_SPATIALCELLSCOLLECTION_H_
#define MOJA_DATAREPOSITORY_SPATIALCELLSCOLLECTION_H_

#include "moja/datarepository/_datarepository_exports.h"
#include "moja/datarepository/tileblockcellindexer.h"

#include <iterator>

namespace moja {
namespace datarepository {

class DATAREPOSITORY_API SpatialCellsCollection {
  public:
   template <bool is_const_iterator = true>
   class _iterator : public std::iterator<std::forward_iterator_tag, CellIdx> {
     public:
      typedef _iterator self_type;
      typedef typename std::conditional<is_const_iterator, const value_type&, value_type&>::type ValueReferenceType;
      typedef typename std::conditional<is_const_iterator, const value_type*, value_type*>::type ValuePointerType;

      _iterator() = delete;
      explicit _iterator(const TileBlockCellIndexer& indexer, bool atEnd = false);
      _iterator(const Rectangle& areaOfInterest, const TileBlockCellIndexer& indexer);
      _iterator(const TileIdx& tileIdx, const TileBlockCellIndexer& indexer);
      _iterator(const BlockIdx& blockIdx, const TileBlockCellIndexer& indexer);

      self_type& operator++();
      self_type operator++(int);

      ValueReferenceType operator*();
      ValuePointerType operator->();

      bool operator==(const self_type& rhs);

      bool operator!=(const self_type& rhs);
      friend class _iterator<true>;

     private:
      void increment();

      UInt32 _tileIndexLimit;
      UInt32 _blockIndexLimit;
      UInt32 _cellIndexLimit;
      CellIdx _cell;
      bool _hasAreaOfInterest;
      bool _hasTileIdx;
      bool _hasBlockIdx;

      unsigned _tileTop;
      unsigned _blockTop;
      unsigned _cellTop;
      unsigned _tileLeft;
      unsigned _blockLeft;
      unsigned _cellLeft;
      unsigned _tileRight;
      unsigned _blockRight;
      unsigned _cellRight;
      unsigned _tileBottom;
      unsigned _blockBottom;
      unsigned _cellBottom;
   };

   typedef _iterator<false> iterator;
   typedef _iterator<true> const_iterator;

   explicit SpatialCellsCollection(const TileBlockCellIndexer& indexer)
       : _indexer(indexer),
         _tileIdx(0, indexer),
         _blockIdx(0, 0, indexer),
         _hasAreaOfInterest(false),
         _hasTileIdx(false),
         _hasBlockIdx(false) {}
   SpatialCellsCollection(Rectangle& areaOfInterest, const TileBlockCellIndexer& indexer)
       : _indexer(indexer),
         _tileIdx(0, indexer),
         _blockIdx(0, 0, indexer),
         _areaOfInterest(areaOfInterest),
         _hasAreaOfInterest(true),
         _hasTileIdx(false),
         _hasBlockIdx(false) {}
   SpatialCellsCollection(const TileIdx& tileIdx, const TileBlockCellIndexer& indexer)
       : _indexer(indexer),
         _tileIdx(tileIdx),
         _blockIdx(0, 0, indexer),
         _hasAreaOfInterest(false),
         _hasTileIdx(true),
         _hasBlockIdx(false) {}
   SpatialCellsCollection(const BlockIdx& blockIdx, const TileBlockCellIndexer& indexer)
       : _indexer(indexer),
         _tileIdx(0, indexer),
         _blockIdx(blockIdx),
         _hasAreaOfInterest(false),
         _hasTileIdx(false),
         _hasBlockIdx(true) {}

   iterator begin();
   iterator end();

   const_iterator begin() const;
   const_iterator end() const;

   TileBlockCellIndexer _indexer;
   Rectangle _areaOfInterest;
   TileIdx _tileIdx;
   BlockIdx _blockIdx;
   bool _hasAreaOfInterest;
   bool _hasTileIdx;
   bool _hasBlockIdx;
};

inline SpatialCellsCollection::iterator SpatialCellsCollection::begin() {
   if (_hasAreaOfInterest) return iterator(_areaOfInterest, _indexer);
   if (_hasTileIdx) return iterator(_tileIdx, _indexer);
   if (_hasBlockIdx) return iterator(_blockIdx, _indexer);
   return iterator(_indexer);
}

inline SpatialCellsCollection::iterator SpatialCellsCollection::end() { return iterator(_indexer, true); }

inline SpatialCellsCollection::const_iterator SpatialCellsCollection::begin() const {
   if (_hasAreaOfInterest) return const_iterator(_areaOfInterest, _indexer);
   if (_hasTileIdx) return const_iterator(_tileIdx, _indexer);
   if (_hasBlockIdx) return const_iterator(_blockIdx, _indexer);
   return const_iterator(_indexer);
}

inline SpatialCellsCollection::const_iterator SpatialCellsCollection::end() const {
   return const_iterator(_indexer, true);
}

template <bool is_const_iterator>
SpatialCellsCollection::_iterator<is_const_iterator>::_iterator(const TileBlockCellIndexer& indexer, bool atEnd)
    : _cell(0U, 0U, 0U, indexer), _hasAreaOfInterest(false), _hasTileIdx(false), _hasBlockIdx(false) {
   if (atEnd) {
      _cell.tileIdx = indexer.tileDesc.indexLimit;
      return;
   }
   _tileIndexLimit = indexer.tileDesc.indexLimit;
   _blockIndexLimit = indexer.blockDesc.indexLimit;
   _cellIndexLimit = indexer.cellDesc.indexLimit;
}

template <bool is_const_iterator>
SpatialCellsCollection::_iterator<is_const_iterator>::_iterator(const Rectangle& areaOfInterest,
                                                                const TileBlockCellIndexer& indexer)
    : _cell(0U, 0U, 0U, indexer), _hasAreaOfInterest(true), _hasTileIdx(false), _hasBlockIdx(false) {
   // Adjust the cell to point to the first cell in the areaOfInterest
   _tileIndexLimit = indexer.tileDesc.indexLimit;
   _blockIndexLimit = indexer.blockDesc.indexLimit;
   _cellIndexLimit = indexer.cellDesc.indexLimit;
   auto& TL = areaOfInterest.location();
   Point BR = {areaOfInterest.bottom() + indexer.cellDesc.lonSize, areaOfInterest.right() - indexer.cellDesc.latSize};

   auto _cellAOI_TL = indexer.getCellIndexFromLatLon(TL);
   auto _cellAOI_BR = indexer.getCellIndexFromLatLon(BR);

   _tileTop = _cellAOI_TL.tileY();
   _tileLeft = _cellAOI_TL.tileX();
   _tileBottom = _cellAOI_BR.tileY();
   _tileRight = _cellAOI_BR.tileX();

   _blockTop = _cellAOI_TL.blockY();
   _blockLeft = _cellAOI_TL.blockX();
   _blockBottom = _cellAOI_BR.blockY();
   _blockRight = _cellAOI_BR.blockX();

   _cellTop = _cellAOI_TL.cellY();
   _cellLeft = _cellAOI_TL.cellX();
   _cellBottom = _cellAOI_BR.cellY();
   _cellRight = _cellAOI_BR.cellX();

   _cell = CellIdx(_cellAOI_TL.tileIdx, 0, 0, indexer);
}

template <bool is_const_iterator>
SpatialCellsCollection::_iterator<is_const_iterator>::_iterator(const TileIdx& tileIdx,
                                                                const TileBlockCellIndexer& indexer)
    : _cell(0U, 0U, 0U, indexer), _hasAreaOfInterest(false), _hasTileIdx(true), _hasBlockIdx(false) {
   // Adjust the cell to point to the first cell in the areaOfInterest
   _tileIndexLimit = indexer.tileDesc.indexLimit;
   _blockIndexLimit = indexer.blockDesc.indexLimit;
   _cellIndexLimit = indexer.cellDesc.indexLimit;

   _tileTop = tileIdx.tileY();
   _tileLeft = tileIdx.tileX();
   _tileBottom = tileIdx.tileY();
   _tileRight = tileIdx.tileX();

   _blockTop = 0;
   _blockLeft = 0;
   _blockBottom = indexer.blockDesc.rows - 1;
   _blockRight = indexer.blockDesc.cols - 1;

   _cellTop = 0;
   _cellLeft = 0;
   _cellBottom = indexer.cellDesc.rows - 1;
   _cellRight = indexer.cellDesc.cols - 1;

   _cell = CellIdx(tileIdx.tileIdx, 0, 0, indexer);
}

template <bool is_const_iterator>
SpatialCellsCollection::_iterator<is_const_iterator>::_iterator(const BlockIdx& blockIdx,
                                                                const TileBlockCellIndexer& indexer)
    : _cell(0U, 0U, 0U, indexer), _hasAreaOfInterest(false), _hasTileIdx(false), _hasBlockIdx(true) {
   // Adjust the cell to point to the first cell in the areaOfInterest
   _tileIndexLimit = indexer.tileDesc.indexLimit;
   _blockIndexLimit = indexer.blockDesc.indexLimit;
   _cellIndexLimit = indexer.cellDesc.indexLimit;

   _tileTop = blockIdx.tileY();
   _tileLeft = blockIdx.tileX();
   _tileBottom = blockIdx.tileY();
   _tileRight = blockIdx.tileX();

   _blockTop = blockIdx.blockY();
   _blockLeft = blockIdx.blockX();
   _blockBottom = blockIdx.blockY();
   _blockRight = blockIdx.blockX();

   _cellTop = 0;
   _cellLeft = 0;
   _cellBottom = indexer.cellDesc.rows - 1;
   _cellRight = indexer.cellDesc.cols - 1;

   _cell = CellIdx(blockIdx.tileIdx, blockIdx.blockIdx, 0, indexer);
}

template <bool is_const_iterator>
typename SpatialCellsCollection::_iterator<is_const_iterator>::self_type&
SpatialCellsCollection::_iterator<is_const_iterator>::operator++() {
   increment();
   return *this;
}

template <bool is_const_iterator>
typename SpatialCellsCollection::_iterator<is_const_iterator>::self_type
SpatialCellsCollection::_iterator<is_const_iterator>::operator++(int) {
   const self_type old(*this);
   ++(*this);
   return old;
}

template <bool is_const_iterator>
typename SpatialCellsCollection::_iterator<is_const_iterator>::ValueReferenceType
    SpatialCellsCollection::_iterator<is_const_iterator>::operator*() {
   return _cell;
}

template <bool is_const_iterator>
typename SpatialCellsCollection::_iterator<is_const_iterator>::ValuePointerType
    SpatialCellsCollection::_iterator<is_const_iterator>::operator->() {
   return &_cell;
}

template <bool is_const_iterator>
bool SpatialCellsCollection::_iterator<is_const_iterator>::operator==(const self_type& rhs) {
   return (_cell.cellIdx == rhs._cell.cellIdx && _cell.blockIdx == rhs._cell.blockIdx &&
           _cell.tileIdx == rhs._cell.tileIdx);
}

template <bool is_const_iterator>
bool SpatialCellsCollection::_iterator<is_const_iterator>::operator!=(const self_type& rhs) {
   return !(*this == rhs);
}

template <bool is_const_iterator>
void SpatialCellsCollection::_iterator<is_const_iterator>::increment() {
   if (++_cell.cellIdx >= _cellIndexLimit) {
      _cell.cellIdx = 0;
      if (++_cell.blockIdx >= _blockIndexLimit) {
         _cell.blockIdx = 0;
         if (++_cell.tileIdx >= _tileIndexLimit) {
            _cell.tileIdx = _tileIndexLimit;
         }
      }
   }

   if (!_hasAreaOfInterest && !_hasTileIdx && !_hasBlockIdx) return;

   auto foundNext = false;
   while (!foundNext) {
      // if Tile is not in areaOfInterest
      const auto tileX = _cell.tileX();
      const auto tileY = _cell.tileY();

      if (tileX < _tileLeft || tileY < _tileTop || tileX > _tileRight || tileY > _tileBottom) {
         _cell.cellIdx = 0;
         _cell.blockIdx = 0;
         if (++_cell.tileIdx >= _tileIndexLimit) {  // We exceeded our data
            _cell.tileIdx = _tileIndexLimit;
            return;
         }
         continue;
      }

      const auto blockX = _cell.blockX();
      const auto blockY = _cell.blockY();
      // if Block is not in areaOfInterest
      if ((tileX == _tileLeft && blockX < _blockLeft) || (tileY == _tileTop && blockY < _blockTop) ||
          (tileX == _tileRight && blockX > _blockRight) || (tileY == _tileBottom && blockY > _blockBottom)) {
         _cell.cellIdx = 0;
         if (++_cell.blockIdx >= _blockIndexLimit) {
            _cell.blockIdx = 0;
            if (++_cell.tileIdx >= _tileIndexLimit) {
               _cell.tileIdx = _tileIndexLimit;
               return;
            }
         }
         continue;
      }

      const auto cellX = _cell.cellX();
      const auto cellY = _cell.cellY();
      // if Cell is not in areaOfInterest
      if ((tileX == _tileLeft && blockX == _blockLeft && cellX < _cellLeft) ||
          (tileY == _tileTop && blockY == _blockTop && cellY < _cellTop) ||
          (tileX == _tileRight && blockX == _blockRight && cellX > _cellRight) ||
          (tileY == _tileBottom && blockY == _blockBottom && cellY > _cellBottom)) {
         if (++_cell.cellIdx >= _cellIndexLimit) {
            _cell.cellIdx = 0;
            if (++_cell.blockIdx >= _blockIndexLimit) {
               _cell.blockIdx = 0;
               if (++_cell.tileIdx >= _tileIndexLimit) {
                  _cell.tileIdx = _tileIndexLimit;
                  return;
               }
            }
         }
         continue;
      }
      foundNext = true;
   }
}

}  // namespace datarepository
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_SPATIALCELLSCOLLECTION_H_
