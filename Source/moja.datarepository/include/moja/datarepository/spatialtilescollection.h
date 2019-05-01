#ifndef MOJA_DATAREPOSITORY_SPATIALTILESCOLLECTION_H_
#define MOJA_DATAREPOSITORY_SPATIALTILESCOLLECTION_H_

#include "moja/datarepository/_datarepository_exports.h"
#include "moja/datarepository/tileblockcellindexer.h"

#include <iterator>

namespace moja {
namespace datarepository {

class DATAREPOSITORY_API SpatialTilesCollection {
public:
	template<bool is_const_iterator = true>
	class _iterator : public std::iterator < std::forward_iterator_tag, TileIdx > {
	public:
		typedef _iterator self_type;
		typedef typename std::conditional<is_const_iterator, const value_type&, value_type&>::type ValueReferenceType;
		typedef typename std::conditional<is_const_iterator, const value_type*, value_type*>::type ValuePointerType;

		_iterator() = delete;
		explicit _iterator(const TileBlockCellIndexer& indexer, bool atEnd = false);

		_iterator(const Rectangle& areaOfInterest, const TileBlockCellIndexer& indexer);

		self_type& operator++();
		self_type operator++(int);

		ValueReferenceType operator*();
		ValuePointerType operator->();

		bool operator==(const self_type& rhs);

		bool operator!=(const self_type& rhs);
		friend class _iterator < true >;

	private:
		void increment();

		UInt32 _tileIndexLimit;
		UInt32 _blockIndexLimit;
		TileIdx _tile;
		bool _hasAreaOfInterest;

		unsigned _tileTop;
		unsigned _tileLeft;
		unsigned _tileRight;
		unsigned _tileBottom;
	};

	typedef _iterator<false> iterator;
	typedef _iterator<true> const_iterator;

	explicit SpatialTilesCollection(const TileBlockCellIndexer& indexer) :_indexer(indexer), _hasAreaOfInterest(false) {}
	SpatialTilesCollection(Rectangle& areaOfInterest, const TileBlockCellIndexer& indexer) :_indexer(indexer), _areaOfInterest(areaOfInterest), _hasAreaOfInterest(true) {}

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

	TileBlockCellIndexer _indexer;
	Rectangle _areaOfInterest;
	bool _hasAreaOfInterest;
};

inline SpatialTilesCollection::iterator SpatialTilesCollection::begin() {
	return _hasAreaOfInterest ? iterator(_areaOfInterest, _indexer) : iterator(_indexer);
}

inline SpatialTilesCollection::iterator SpatialTilesCollection::end() {
	return iterator(_indexer, true);
}

inline SpatialTilesCollection::const_iterator SpatialTilesCollection::begin() const {
	return _hasAreaOfInterest ? const_iterator(_areaOfInterest, _indexer) : const_iterator(_indexer);
}

inline SpatialTilesCollection::const_iterator SpatialTilesCollection::end() const {
	return const_iterator(_indexer, true);
}

template <bool is_const_iterator>
SpatialTilesCollection::_iterator<is_const_iterator>::_iterator(const TileBlockCellIndexer& indexer, bool atEnd) :
	_tile(0U, indexer), _hasAreaOfInterest(false) {
	if (atEnd) {
		_tile.tileIdx = indexer.tileDesc.indexLimit;
		return;
	}
	_tileIndexLimit = indexer.tileDesc.indexLimit;
	_blockIndexLimit = indexer.blockDesc.indexLimit;
}

template <bool is_const_iterator>
SpatialTilesCollection::_iterator<is_const_iterator>::_iterator(const Rectangle& areaOfInterest, const TileBlockCellIndexer& indexer) :
	_tile(0U, indexer), _hasAreaOfInterest(true) {
	// Adjust the cell to point to the first cell in the areaOfInterest
	_tileIndexLimit = indexer.tileDesc.indexLimit;
	_blockIndexLimit = indexer.blockDesc.indexLimit;

	auto& TL = areaOfInterest.location();
	Point BR = { areaOfInterest.bottom() + indexer.cellDesc.lonSize, areaOfInterest.right() - indexer.cellDesc.latSize };

	auto _tileAOI_TL = indexer.getTileIndexFromLatLon(TL);
	auto _tileAOI_BR = indexer.getTileIndexFromLatLon(BR);

	_tileTop = _tileAOI_TL.tileY();
	_tileLeft = _tileAOI_TL.tileX();
	_tileBottom = _tileAOI_BR.tileY();
	_tileRight = _tileAOI_BR.tileX();

	_tile = TileIdx(_tileAOI_TL.tileIdx, indexer);
}

template <bool is_const_iterator>
inline typename SpatialTilesCollection::_iterator<is_const_iterator>::self_type& SpatialTilesCollection::_iterator<is_const_iterator>::operator++() {
	increment();
	return *this;
}

template <bool is_const_iterator>
inline typename SpatialTilesCollection::_iterator<is_const_iterator>::self_type SpatialTilesCollection::_iterator<is_const_iterator>::operator++(int) {
	const self_type old(*this);
	++(*this);
	return old;
}

template <bool is_const_iterator>
inline typename SpatialTilesCollection::_iterator<is_const_iterator>::ValueReferenceType SpatialTilesCollection::_iterator<is_const_iterator>::operator*() { return _tile; }

template <bool is_const_iterator>
inline typename SpatialTilesCollection::_iterator<is_const_iterator>::ValuePointerType SpatialTilesCollection::_iterator<is_const_iterator>::operator->() { return &_tile; }

template <bool is_const_iterator>
inline bool SpatialTilesCollection::_iterator<is_const_iterator>::operator==(const self_type& rhs) {
	return (_tile.tileIdx == rhs._tile.tileIdx);
}

template <bool is_const_iterator>
inline bool SpatialTilesCollection::_iterator<is_const_iterator>::operator!=(const self_type& rhs) { return !(*this == rhs); }

template <bool is_const_iterator>
void SpatialTilesCollection::_iterator<is_const_iterator>::increment() {

	if (++_tile.tileIdx >= _tileIndexLimit) {
		_tile.tileIdx = _tileIndexLimit;
	}

	if (!_hasAreaOfInterest)
		return;

	auto foundNext = false;
	while (!foundNext) {
		// if Tile is not in areaOfInterest
		auto tileX = _tile.tileX();
		auto tileY = _tile.tileY();

		if (tileX < _tileLeft
			|| tileY < _tileTop
			|| tileX > _tileRight
			|| tileY > _tileBottom) {

			if (++_tile.tileIdx >= _tileIndexLimit) {	// We exceeded our data
				_tile.tileIdx = _tileIndexLimit;
				return;
			}
			continue;
		}
		foundNext = true;
	}
	return;
}

}} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_SPATIALTILESCOLLECTION_H_
