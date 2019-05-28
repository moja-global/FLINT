#ifndef MOJA_DATAREPOSITORY_SPATIALBLOCKSCOLLECTION_H_
#define MOJA_DATAREPOSITORY_SPATIALBLOCKSCOLLECTION_H_

#include "moja/datarepository/_datarepository_exports.h"
#include "moja/datarepository/tileblockcellindexer.h"

#include <iterator>

namespace moja {
namespace datarepository {

class DATAREPOSITORY_API SpatialBlocksCollection {
  public:
   template <bool is_const_iterator = true>
   class _iterator : public std::iterator<std::forward_iterator_tag, BlockIdx> {
     public:
      typedef _iterator self_type;
      typedef typename std::conditional<is_const_iterator, const value_type&, value_type&>::type ValueReferenceType;
      typedef typename std::conditional<is_const_iterator, const value_type*, value_type*>::type ValuePointerType;

      _iterator() = delete;
      explicit _iterator(const TileBlockCellIndexer& indexer, bool atEnd = false);
      _iterator(const Rectangle& areaOfInterest, const TileBlockCellIndexer& indexer);
      _iterator(const TileIdx& tileIdx, const TileBlockCellIndexer& indexer);

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
      BlockIdx _block;
      bool _hasAreaOfInterest;
      bool _hasTileIdx;

      unsigned _tileTop;
      unsigned _blockTop;
      unsigned _tileLeft;
      unsigned _blockLeft;
      unsigned _tileRight;
      unsigned _blockRight;
      unsigned _tileBottom;
      unsigned _blockBottom;
   };

   typedef _iterator<false> iterator;
   typedef _iterator<true> const_iterator;

   explicit SpatialBlocksCollection(const TileBlockCellIndexer& indexer)
       : _indexer(indexer), _tileIdx(0, indexer), _hasAreaOfInterest(false), _hasTileIdx(false) {}
   SpatialBlocksCollection(Rectangle& areaOfInterest, const TileBlockCellIndexer& indexer)
       : _indexer(indexer),
         _tileIdx(0, indexer),
         _areaOfInterest(areaOfInterest),
         _hasAreaOfInterest(true),
         _hasTileIdx(false) {}
   SpatialBlocksCollection(const TileIdx& tileIdx, const TileBlockCellIndexer& indexer)
       : _indexer(indexer), _tileIdx(tileIdx), _hasAreaOfInterest(false), _hasTileIdx(true) {}

   iterator begin();
   iterator end();

   const_iterator begin() const;
   const_iterator end() const;

   TileBlockCellIndexer _indexer;
   Rectangle _areaOfInterest;
   TileIdx _tileIdx;
   bool _hasAreaOfInterest;
   bool _hasTileIdx;
};

inline SpatialBlocksCollection::iterator SpatialBlocksCollection::begin() {
   // return _hasAreaOfInterest ? iterator(_areaOfInterest, _indexer) : iterator(_indexer);
   if (_hasAreaOfInterest) return iterator(_areaOfInterest, _indexer);
   if (_hasTileIdx) return iterator(_tileIdx, _indexer);
   return iterator(_indexer);
}

inline SpatialBlocksCollection::iterator SpatialBlocksCollection::end() { return iterator(_indexer, true); }

inline SpatialBlocksCollection::const_iterator SpatialBlocksCollection::begin() const {
   // return _hasAreaOfInterest ? const_iterator(_areaOfInterest, _indexer) : const_iterator(_indexer);
   if (_hasAreaOfInterest) return const_iterator(_areaOfInterest, _indexer);
   if (_hasTileIdx) return const_iterator(_tileIdx, _indexer);
   return const_iterator(_indexer);
}

inline SpatialBlocksCollection::const_iterator SpatialBlocksCollection::end() const {
   return const_iterator(_indexer, true);
}

template <bool is_const_iterator>
SpatialBlocksCollection::_iterator<is_const_iterator>::_iterator(const TileBlockCellIndexer& indexer, bool atEnd)
    : _block(0U, 0U, indexer), _hasAreaOfInterest(false) {
   if (atEnd) {
      _block.tileIdx = indexer.tileDesc.indexLimit;
      return;
   }
   _tileIndexLimit = indexer.tileDesc.indexLimit;
   _blockIndexLimit = indexer.blockDesc.indexLimit;
}

template <bool is_const_iterator>
SpatialBlocksCollection::_iterator<is_const_iterator>::_iterator(const Rectangle& areaOfInterest,
                                                                 const TileBlockCellIndexer& indexer)
    : _block(0U, 0U, indexer), _hasAreaOfInterest(true) {
   // Adjust the cell to point to the first cell in the areaOfInterest
   _tileIndexLimit = indexer.tileDesc.indexLimit;
   _blockIndexLimit = indexer.blockDesc.indexLimit;

   auto& TL = areaOfInterest.location();
   Point BR = {areaOfInterest.bottom() + indexer.cellDesc.lonSize, areaOfInterest.right() - indexer.cellDesc.latSize};
   auto _blockAOI_TL = indexer.getBlockIndexFromLatLon(TL);
   auto _blockAOI_BR = indexer.getBlockIndexFromLatLon(BR);

   _tileTop = _blockAOI_TL.tileY();
   _tileLeft = _blockAOI_TL.tileX();
   _tileBottom = _blockAOI_BR.tileY();
   _tileRight = _blockAOI_BR.tileX();

   _blockTop = _blockAOI_TL.blockY();
   _blockLeft = _blockAOI_TL.blockX();
   _blockBottom = _blockAOI_BR.blockY();
   _blockRight = _blockAOI_BR.blockX();

   _block = BlockIdx(_blockAOI_TL.tileIdx, 0, indexer);
}

template <bool is_const_iterator>
SpatialBlocksCollection::_iterator<is_const_iterator>::_iterator(const TileIdx& tileIdx,
                                                                 const TileBlockCellIndexer& indexer)
    : _block(0U, 0U, indexer), _hasAreaOfInterest(false), _hasTileIdx(true) {
   // Adjust the cell to point to the first cell in the areaOfInterest
   _tileIndexLimit = indexer.tileDesc.indexLimit;
   _blockIndexLimit = indexer.blockDesc.indexLimit;

   _tileTop = tileIdx.tileY();
   _tileLeft = tileIdx.tileX();
   _tileBottom = tileIdx.tileY();
   _tileRight = tileIdx.tileX();

   _blockTop = 0;
   _blockLeft = 0;
   _blockBottom = indexer.blockDesc.rows - 1;
   _blockRight = indexer.blockDesc.cols - 1;

   _block = BlockIdx(tileIdx.tileIdx, 0, indexer);
}

template <bool is_const_iterator>
inline typename SpatialBlocksCollection::_iterator<is_const_iterator>::self_type&
SpatialBlocksCollection::_iterator<is_const_iterator>::operator++() {
   increment();
   return *this;
}

template <bool is_const_iterator>
inline typename SpatialBlocksCollection::_iterator<is_const_iterator>::self_type
SpatialBlocksCollection::_iterator<is_const_iterator>::operator++(int) {
   const self_type old(*this);
   ++(*this);
   return old;
}

template <bool is_const_iterator>
inline typename SpatialBlocksCollection::_iterator<is_const_iterator>::ValueReferenceType
    SpatialBlocksCollection::_iterator<is_const_iterator>::operator*() {
   return _block;
}

template <bool is_const_iterator>
inline typename SpatialBlocksCollection::_iterator<is_const_iterator>::ValuePointerType
    SpatialBlocksCollection::_iterator<is_const_iterator>::operator->() {
   return &_block;
}

template <bool is_const_iterator>
inline bool SpatialBlocksCollection::_iterator<is_const_iterator>::operator==(const self_type& rhs) {
   return (_block.blockIdx == rhs._block.blockIdx && _block.tileIdx == rhs._block.tileIdx);
}

template <bool is_const_iterator>
inline bool SpatialBlocksCollection::_iterator<is_const_iterator>::operator!=(const self_type& rhs) {
   return !(*this == rhs);
}

template <bool is_const_iterator>
void SpatialBlocksCollection::_iterator<is_const_iterator>::increment() {
   if (++_block.blockIdx >= _blockIndexLimit) {
      _block.blockIdx = 0;
      if (++_block.tileIdx >= _tileIndexLimit) {
         _block.tileIdx = _tileIndexLimit;
      }
   }

   if (!_hasAreaOfInterest && !_hasTileIdx) return;

   auto foundNext = false;
   while (!foundNext) {
      // if Tile is not in areaOfInterest
      auto tileX = _block.tileX();
      auto tileY = _block.tileY();

      if (tileX < _tileLeft || tileY < _tileTop || tileX > _tileRight || tileY > _tileBottom) {
         _block.blockIdx = 0;
         if (++_block.tileIdx >= _tileIndexLimit) {  // We exceeded our data
            _block.tileIdx = _tileIndexLimit;
            return;
         }
         continue;
      }

      auto blockX = _block.blockX();
      auto blockY = _block.blockY();
      // if Block is not in areaOfInterest
      if ((tileX == _tileLeft && blockX < _blockLeft) || (tileY == _tileTop && blockY < _blockTop) ||
          (tileX == _tileRight && blockX > _blockRight) || (tileY == _tileBottom && blockY > _blockBottom)) {
         if (++_block.blockIdx >= _blockIndexLimit) {
            _block.blockIdx = 0;
            if (++_block.tileIdx >= _tileIndexLimit) {
               _block.tileIdx = _tileIndexLimit;
               return;
            }
         }
         continue;
      }
      foundNext = true;
   }
   return;
}
}  // namespace datarepository
}  // namespace moja
#endif  // MOJA_DATAREPOSITORY_SPATIALBLOCKSCOLLECTION_H_
