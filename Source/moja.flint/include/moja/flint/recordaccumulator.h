#ifndef MOJA_FLINT_RECORDACCUMULATOR_H_
#define MOJA_FLINT_RECORDACCUMULATOR_H_

#include "moja/flint/record.h"
#include "moja/flint/recordutils.h"

#include <moja/types.h>

#include <tlx/container/btree_map.hpp>

#include <iterator>
#include <numeric>
#include <unordered_set>
#include <vector>

namespace moja {
namespace flint {

template <class TPersistable>
class RecordAccumulator {
  public:
   typedef std::unordered_set<Record<TPersistable>*, RecordHasher<TPersistable>, RecordComparer<TPersistable>>
       rec_accu_set;
   typedef std::vector<std::shared_ptr<Record<TPersistable>>> rec_accu_vec;
   typedef typename rec_accu_vec::size_type rec_accu_size_type;

   Record<TPersistable>* insert(Int64 id, std::shared_ptr<Record<TPersistable>> record) {
      // ID has been assigned by user, assume that we can run with this
      _nextId = id + 1;  // can't guarantee that this will be called in 'id increasing' order but a good guess perhaps
      record->setId(id);
      _records.push_back(record);
      _recordsIdx.insert(record.get());
      return record.get();
   }

   Record<TPersistable>* accumulate(std::shared_ptr<Record<TPersistable>> record) {
      auto record_ptr = record.get();
      auto it = _recordsIdx.find(record_ptr);
      if (it != _recordsIdx.end()) {
         // Found an existing ID for the key.
         auto existing = *it;
         existing->merge(record_ptr);
         return existing;
      }

      // First time seeing this key - assign an ID.
      record->setId(_nextId++);
      _records.push_back(record);
      _recordsIdx.insert(record_ptr);
      return record_ptr;
   }

   Record<TPersistable>* accumulate(std::shared_ptr<Record<TPersistable>> record, Int64 requestedId) {
      auto record_ptr = record.get();
      auto it = _recordsIdx.find(record_ptr);
      if (it != _recordsIdx.end()) {
         // Found an existing ID for the key.
         auto existing = *it;
         existing->merge(record_ptr);
         return existing;
      }
      // First time seeing this key - assign an ID.
      _nextId = requestedId +
                1;  // can't guarantee that this will be called in 'id increasing' order but a good guess perhaps
      record->setId(requestedId);
      _records.push_back(record);
      _recordsIdx.insert(record_ptr);
      return record_ptr;
   }

   Record<TPersistable>* search(std::shared_ptr<Record<TPersistable>> record) {
      auto record_ptr = record.get();
      auto it = _recordsIdx.find(record_ptr);
      if (it != _recordsIdx.end()) {
         // Found an existing ID for the key.
         auto existing = *it;
         return existing;
      }
      return nullptr;
   }

   std::vector<TPersistable> getPersistableCollection() const {
      std::vector<TPersistable> persistables;
      for (auto& record : _records) {
         persistables.push_back(record->asPersistable());
      }
      return persistables;
   }

   // std::vector<TPersistable> getPersistableCollection(size_t startIndex, size_t chunkSize) const {
   //	std::vector<TPersistable> persistables;
   //	if (startIndex > _records.size())
   //		return persistables;
   //	size_t chunkPosition = 0;
   //	for (auto it = _records.begin() + startIndex; it != _records.end() && chunkPosition++ < chunkSize; ++it) {
   //		persistables.push_back(record->asPersistable());
   //	}
   //	return persistables;
   //}

   void clear() {
      _recordsIdx.clear();
      _records.clear();
   }

   rec_accu_size_type size() const { return _records.size(); }

  private:
   Int64 _nextId = 1;
   rec_accu_set _recordsIdx;
   rec_accu_vec _records;
};

template <class TPersistable, class TRecord>
class RecordAccumulator2 {
   struct RecordComparer {
      bool operator()(const TRecord* lhs, const TRecord* rhs) const { return lhs->operator==(*rhs); }
   };
   struct RecordHasher {
      size_t operator()(const TRecord* record) const { return record->hash(); }
   };
   size_t compute_size() {
      if (_records.capacity() == 0) {
         return (std::max)(64 / sizeof(TRecord), size_t(1));
      }
      if (_records.capacity() > 4096 * 32 / sizeof(TRecord)) {
         return _records.capacity() * 2;
      }
      return (_records.capacity() * 3 + 1) / 2;
   }

  public:
   typedef std::unordered_set<TRecord*, RecordHasher, RecordComparer> rec_accu_set;
   typedef std::vector<TRecord> rec_accu_vec;
   typedef typename rec_accu_vec::size_type rec_accu_size_type;

   const TRecord* insert(Int64 id, TRecord record) {
      if (_records.size() == _records.capacity()) {
         _recordsIdx.clear();
         _records.reserve(compute_size());
         for (auto& rec : _records) {
            _recordsIdx.insert(&rec);
         }
      }
      // ID has been assigned by user, assume that we can run with this
      _nextId = id + 1;  // can't guarantee that this will be called in 'id increasing' order but a good guess perhaps
      record.setId(id);
      _records.push_back(record);
      auto& new_record = _records.back();
      _recordsIdx.insert(&new_record);
      return &new_record;
   }

   const TRecord* accumulate(TRecord record) { return accumulate(record, _nextId); }

   const TRecord* accumulate(TRecord record, Int64 requestedId) {
      auto it = _recordsIdx.find(&record);
      if (it != _recordsIdx.end()) {
         // Found an existing ID for the key.
         auto existing = *it;
         existing->merge(record);
         return existing;
      }
      return insert(requestedId, record);
   }

   const TRecord* search(TRecord record) {
      auto it = _recordsIdx.find(&record);
      if (it != _recordsIdx.end()) {
         // Found an existing ID for the key.
         auto existing = *it;
         return existing;
      }
      return nullptr;
   }

   const rec_accu_vec& getRecords() const { return _records; }

   std::vector<TPersistable> getPersistableCollection() const {
      std::vector<TPersistable> persistables;
      persistables.reserve(_records.size());
      for (const auto& record : _records) {
         persistables.emplace_back(record.asPersistable());
      }
      return persistables;
   }

   void clear() {
      _recordsIdx.clear();
      _records.clear();
   }

   rec_accu_size_type size() const { return _records.size(); }

   const rec_accu_vec& records() const { return _records; }

  private:
   Int64 _nextId = 1;
   rec_accu_set _recordsIdx;
   rec_accu_vec _records;
};

template <class TPersistable, class TRecordConv, class TKey, class TValue>
class RecordAccumulatorMap {
  public:
   typedef tlx::btree_map<TKey, TValue> rec_accu_map;
   typedef typename rec_accu_map::size_type rec_accu_size_type;

   void insert(Int64 id, const TKey& key, const TValue& value) {
      // ID has been assigned by user, assume that we can run with this
      _nextId = id + 1;  // can't guarantee that this will be called in 'id increasing' order but a good guess perhaps
      auto newValue = value;
      newValue._id = id;
      _records.insert(key, newValue);
   }

   Int64 accumulate(const TKey& key, const TValue& value) { return accumulate(key, value, _nextId); }

   Int64 accumulate(const TKey& key, const TValue& value, Int64 requestedId) {
      auto it = _records.find(key);
      if (it != _records.end()) {
         it->second += value;
         return it->second._id;
      }
      // First time seeing this key - assign an ID.
      _nextId = requestedId +
                1;  // can't guarantee that this will be called in 'id increasing' order but a good guess perhaps
      auto newValue = value;
      newValue._id = requestedId;
      _records.insert(std::make_pair(key, newValue));
      return newValue._id;
   }

   const rec_accu_map& getRecords() const { return _records; }

   std::vector<TPersistable> getPersistableCollection() const {
      std::vector<TPersistable> persistables;
      for (const auto& rec : _records) {
         persistables.emplace_back(TRecordConv::asPersistable(rec.first, rec.second));
      }
      return persistables;
   }

   std::vector<TPersistable> getPersistableCollectionRange(typename rec_accu_map::const_iterator& rangeStart,
                                                           size_t chunkSize) const {
      std::vector<TPersistable> persistables;
      size_t chunkPosition = 0;
      for (; (rangeStart != _records.end() && chunkPosition++ < chunkSize); ++rangeStart) {
         persistables.push_back(TRecordConv::asPersistable((*rangeStart).first, (*rangeStart).second));
      }
      return persistables;
   }

   void clear() { _records.clear(); }

   rec_accu_size_type size() const { return _records.size(); }

  private:
   Int64 _nextId = 1;
   rec_accu_map _records;
};

template <class TTuple, class TRecordConv, class TKey, class TValue>
class list_of_tuples {
   using map_type = tlx::btree_map<TKey, TValue>;
   const map_type& accumulator_map_;

  public:
   template <bool Const>
   class tuples_iterator {
      using iterator_type = std::conditional_t<Const, typename map_type::const_iterator, typename map_type::iterator>;
      iterator_type iterator_current_;
      iterator_type iterator_end_;
      TTuple record_{};

     public:
      explicit tuples_iterator(iterator_type iterator_begin, iterator_type iterator_end)
          : iterator_current_{iterator_begin}, iterator_end_(iterator_end) {
         if (iterator_current_ != iterator_end_) {
            const auto& record = *iterator_current_;
            record_ = TRecordConv::asTuple(record.first, record.second);
         }
      }
      using difference_type = std::ptrdiff_t;
      using value_type = TTuple;
      using pointer = std::conditional_t<Const, const value_type*, value_type*>;
      using reference = std::conditional_t<Const, const value_type&, value_type&>;
      using iterator_category = std::forward_iterator_tag;

      reference operator*() const { return record_; }
      pointer operator->() const { return &record_; }

      auto& operator++() {
         ++iterator_current_;
         if (iterator_current_ != iterator_end_) {
            const auto& record = *iterator_current_;
            record_ = TRecordConv::asTuple(record.first, record.second);
         }
         return *this;
      }

      auto operator++(int) {
         auto result = *this;
         ++*this;
         return result;
      }
      // Support comparison between iterator and const_iterator types
      template <bool R>
      bool operator==(const tuples_iterator<R>& rhs) const {
         return iterator_current_ == rhs.iterator_current_;
      }

      template <bool R>
      bool operator!=(const tuples_iterator<R>& rhs) const {
         return iterator_current_ != rhs.iterator_current_;
      }

      // Support implicit conversion of iterator to const_iterator
      // (but not vice versa)
      operator tuples_iterator<true>() const { return tuples_iterator<true>(iterator_current_, iterator_end_); }
   };

   using const_iterator = tuples_iterator<true>;
   using iterator = tuples_iterator<false>;

   list_of_tuples(const map_type& map) : accumulator_map_{map} {}

   // Begin and end member functions
   iterator begin() { return iterator{std::begin(accumulator_map_), std::end(accumulator_map_)}; }
   iterator end() { return iterator{std::end(accumulator_map_), std::end(accumulator_map_)}; }
   const_iterator begin() const { return const_iterator{std::cbegin(accumulator_map_), std::cend(accumulator_map_)}; }
   const_iterator end() const { return const_iterator{std::cend(accumulator_map_), std::cend(accumulator_map_)}; }

   // Other member operations
   const auto& front() const { return accumulator_map_.front().asPersistable(); }
   size_t size() const { return accumulator_map_.size(); }
};

template <class TPersistable, class TTuple, class TRecordConv, class TKey, class TValue>
class RecordAccumulatorMap2 {
  public:
   typedef tlx::btree_map<TKey, TValue> rec_accu_map;
   typedef typename rec_accu_map::size_type rec_accu_size_type;

   void insert(Int64 id, const TKey& key, const TValue& value) {
      // ID has been assigned by user, assume that we can run with this
      _nextId = id + 1;  // can't guarantee that this will be called in 'id increasing' order but a good guess perhaps
      auto newValue = value;
      newValue._id = id;
      _records.insert(key, newValue);
   }

   Int64 accumulate(const TKey& key, const TValue& value) { return accumulate(key, value, _nextId); }

   Int64 accumulate(const TKey& key, const TValue& value, Int64 requestedId) {
      auto it = _records.find(key);
      if (it != _records.end()) {
         it->second += value;
         return it->second._id;
      }
      // First time seeing this key - assign an ID.
      _nextId = requestedId +
                1;  // can't guarantee that this will be called in 'id increasing' order but a good guess perhaps
      auto newValue = value;
      newValue._id = requestedId;
      _records.insert(std::make_pair(key, newValue));
      return newValue._id;
   }

   const rec_accu_map& getRecords() const { return _records; }

   std::vector<TPersistable> getPersistableCollection() const {
      std::vector<TPersistable> persistables;
      persistables.reserve(_records.size());
      for (const auto& rec : _records) {
         persistables.emplace_back(TRecordConv::asPersistable(rec.first, rec.second));
      }
      return persistables;
   }

   std::vector<TPersistable> getPersistableCollectionRange(typename rec_accu_map::const_iterator& rangeStart,
                                                           size_t chunkSize) const {
      std::vector<TPersistable> persistables;
      persistables.reserve((std::min)(_records.size(), chunkSize));
      size_t chunkPosition = 0;
      for (; (rangeStart != _records.end() && chunkPosition++ < chunkSize); ++rangeStart) {
         persistables.emplace_back(TRecordConv::asPersistable((*rangeStart).first, (*rangeStart).second));
      }
      return persistables;
   }

   const auto tuples() const {
      return list_of_tuples<TTuple, TRecordConv, TKey, TValue>{_records};
   }

   [[deprecated("Replaced with tuples() method")]]
   std::vector<TTuple> getTupleCollection() {
      std::vector<TTuple> tuples;
      tuples.reserve(_records.size());
      for (const auto& rec : _records) {
         tuples.emplace_back(TRecordConv::asTuple(rec.first, rec.second));
      }
      return tuples;
   }

   [[deprecated("Replaced with tuples() method")]]
   std::vector<TTuple> getTupleCollectionRange(
       typename rec_accu_map::const_iterator& rangeStart, size_t chunkSize) {
      std::vector<TTuple> tuples;
      tuples.reserve((std::min)(_records.size(), chunkSize));
      size_t chunkPosition = 0;
      for (; (rangeStart != _records.end() && chunkPosition++ < chunkSize); ++rangeStart) {
         tuples.emplace_back(TRecordConv::asTuple((*rangeStart).first, (*rangeStart).second));
      }
      return tuples;
   }

   void clear() { _records.clear(); }

   rec_accu_size_type size() const { return _records.size(); }

  private:
   Int64 _nextId = 1;
   rec_accu_map _records;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_RECORDACCUMULATOR_H_