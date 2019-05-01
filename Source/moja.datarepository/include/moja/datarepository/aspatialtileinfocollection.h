#ifndef MOJA_DATAREPOSITORY_ASPATIALTILEINFOCOLLECTION_H_
#define MOJA_DATAREPOSITORY_ASPATIALTILEINFOCOLLECTION_H_

#include "moja/datarepository/_datarepository_exports.h"
#include "moja/datarepository/aspatialtileinfo.h"

#include "moja/dynamic.h"

//#include <string>
#include <map>

namespace moja {
namespace datarepository {
class IProviderRelationalInterface;
class AspatialTileInfoCollectionIterator;
class AspatialTileInfo;

class DATAREPOSITORY_API AspatialTileInfoCollection {
public:

	template<bool is_const_iterator = true>
	class _iterator : public std::iterator<std::forward_iterator_tag, LandUnitInfo> {
	public:
		typedef _iterator self_type;
		typedef typename std::conditional<is_const_iterator, const value_type&, value_type&>::type ValueReferenceType;
		typedef typename std::conditional<is_const_iterator, const value_type*, value_type*>::type ValuePointerType;

		_iterator() = delete;
		explicit _iterator(const AspatialTileInfoCollection& tileInfoCollection, bool start = true);

		self_type& operator++();
		self_type operator++(int);

		ValueReferenceType operator*();
		ValuePointerType operator->();

		bool operator==(const self_type& rhs);

		bool operator!=(const self_type& rhs);
		friend class _iterator<true>;

	private:
		void increment();

		int _currentTileIndex = 0;
		const AspatialTileInfoCollection* _tileInfoCollection;
		std::shared_ptr<AspatialTileInfo> _currentTile;
		AspatialTileInfoIterator _currentTileIter;
		AspatialTileInfoIterator _currentTileIterEnd;
	};

	typedef _iterator<false> iterator;
	typedef _iterator<true> const_iterator;

	AspatialTileInfoCollection(const IProviderRelationalInterface& provider, int maxTileSize, int tileCacheSize = 1000);
	virtual ~AspatialTileInfoCollection() = default;

	virtual int tileCount() const;
	virtual int luCount(int tileId) const;

    virtual std::shared_ptr<std::map<Int64, DynamicVar>> fetchLUBlock(Int64 id) const;

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

	Int64 getTotalLUCount() const;
    std::vector<AspatialTileInfo> getTiles() const;

private:
	const IProviderRelationalInterface* _provider;
	int _maxTileSize;
	int _tileCacheSize;
	mutable int _tileCount = -1;
};

inline AspatialTileInfoCollection::iterator AspatialTileInfoCollection::begin() {
	return iterator(*this);
}

inline AspatialTileInfoCollection::iterator AspatialTileInfoCollection::end() {
	return iterator(*this, false);
}

inline AspatialTileInfoCollection::const_iterator AspatialTileInfoCollection::begin() const {
	return const_iterator(*this);
}

inline AspatialTileInfoCollection::const_iterator AspatialTileInfoCollection::end() const {
	return const_iterator(*this, false);
}

template <bool is_const_iterator>
AspatialTileInfoCollection::_iterator<is_const_iterator>::_iterator(
	const AspatialTileInfoCollection& tileInfoCollection, bool start) : _tileInfoCollection(&tileInfoCollection) {

	if (start) {
		_currentTile = std::make_shared<AspatialTileInfo>(tileInfoCollection, 1);
		_currentTileIter = _currentTile->begin();
		_currentTileIterEnd = _currentTile->end();
	}
	else {
		_currentTile = std::make_shared<AspatialTileInfo>(tileInfoCollection, tileInfoCollection.tileCount());
		_currentTileIter = _currentTile->end();
		_currentTileIterEnd = _currentTile->end();
	}
}

template <bool is_const_iterator>
inline typename AspatialTileInfoCollection::_iterator<is_const_iterator>::self_type& AspatialTileInfoCollection::_iterator<is_const_iterator>::operator++() {
	increment();
	return *this;
}

template <bool is_const_iterator>
inline typename AspatialTileInfoCollection::_iterator<is_const_iterator>::self_type AspatialTileInfoCollection::_iterator<is_const_iterator>::operator++(int) {
	const self_type old(*this);
	++(*this);
	return old;
}

template <bool is_const_iterator>
inline typename AspatialTileInfoCollection::_iterator<is_const_iterator>::ValueReferenceType AspatialTileInfoCollection::_iterator<is_const_iterator>::operator*() {
	return *_currentTileIter;
}

template <bool is_const_iterator>
inline typename AspatialTileInfoCollection::_iterator<is_const_iterator>::ValuePointerType AspatialTileInfoCollection::_iterator<is_const_iterator>::operator->() {
	return &(*_currentTileIter);
}

template <bool is_const_iterator>
inline bool AspatialTileInfoCollection::_iterator<is_const_iterator>::operator==(const self_type& rhs) {
	return _currentTileIter == rhs._currentTileIter;
}

template <bool is_const_iterator>
inline bool AspatialTileInfoCollection::_iterator<is_const_iterator>::operator!=(const self_type& rhs) {
	return !(*this == rhs);
}

template <bool is_const_iterator>
void AspatialTileInfoCollection::_iterator<is_const_iterator>::increment() {
	++_currentTileIter;
	if (_currentTileIter == _currentTileIterEnd) {
		if (++_currentTileIndex < _tileInfoCollection->tileCount()) {
			_currentTile = std::make_shared<AspatialTileInfo>(*_tileInfoCollection, _currentTileIndex + 1);
			_currentTileIter = _currentTile->begin();
			_currentTileIterEnd = _currentTile->end();
		}
	}
}

}} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_ASPATIALTILEINFOCOLLECTION_H_
