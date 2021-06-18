#ifndef MOJA_FLINT_RECORDACCUMULATORWITHMUTEX_H_
#define MOJA_FLINT_RECORDACCUMULATORWITHMUTEX_H_

#include "moja/flint/record.h"
#include "moja/flint/recordutils.h"

#include <moja/types.h>


#include <atomic>
#include <mutex>
#include <unordered_set>

namespace moja {
namespace flint {

template <class TPersistable>
class RecordAccumulatorWithMutex {
  public:
   typedef std::unordered_set<Record<TPersistable>*, RecordHasher<TPersistable>, RecordComparer<TPersistable>>
       rec_accu_set;
   typedef std::vector<std::shared_ptr<Record<TPersistable>>> rec_accu_vec;
   typedef typename rec_accu_vec::size_type rec_accu_size_type;

   Record<TPersistable>* insert(Int64 id, std::shared_ptr<Record<TPersistable>> record) {
      std::unique_lock<std::mutex> lock(_mutex);
      // ID has been assigned by user, assume that we can run with this
      _nextId = id + 1;  // can't guarantee that this will be called in 'id increasing' order but a good guess perhaps
      record->setId(id);
      _records.push_back(record);
      _recordsIdx.insert(record.get());
      return record.get();
   }

   Record<TPersistable>* accumulate(std::shared_ptr<Record<TPersistable>> record) {
      std::unique_lock<std::mutex> lock(_mutex);
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
      std::unique_lock<std::mutex> lock(_mutex);
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

   Record<TPersistable>* searchId(Int64 id) {
      for (auto& record : _records) {
         if (record->getId() == id) return record.get();
      }
      return nullptr;
   }

   std::vector<TPersistable> getPersistableCollection() {
      std::vector<TPersistable> persistables;
      for (auto& record : _records) {
         persistables.push_back(record->asPersistable());
      }
      return persistables;
   }

   void clear() {
      _recordsIdx.clear();
      _records.clear();
   }

   rec_accu_size_type size() { return _records.size(); }

   const rec_accu_vec& records() const { return _records; };

  private:
   std::mutex _mutex;
   Int64 _nextId = 1;
   rec_accu_set _recordsIdx;
   rec_accu_vec _records;
};

// template <class TPersistable, class TRecord, typename Int64>
template <class TPersistable, class TRecord>
class RecordAccumulatorWithMutex2 {
   struct RecordComparer {
      bool operator()(const TRecord* lhs, const TRecord* rhs) const { return lhs->operator==(*rhs); }
   };

   struct RecordHasher {
      std::size_t operator()(const TRecord* record) const { return record->hash(); }
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

   RecordAccumulatorWithMutex2() : _nextId(1) {}

   const TRecord* insert(Int64 id, TRecord record) {
      std::unique_lock<std::mutex> lock(_mutex);
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

   const TRecord* accumulate(TRecord record) { return accumulate(record, -1); }

   const TRecord* accumulate(TRecord record, Int64 requestedId) {
      std::unique_lock<std::mutex> lock(_mutex);
      auto it = _recordsIdx.find(&record);
      if (it != _recordsIdx.end()) {
         // Found an existing ID for the key.
         auto existing = *it;
         existing->merge(record);
         return existing;
      }

      if (_records.size() == _records.capacity()) {
         _recordsIdx.clear();
         _records.reserve(compute_size());
         for (auto& rec : _records) {
            _recordsIdx.insert(&rec);
         }
      }

      // First time seeing this key - assign an ID.
      if (requestedId == -1) {
         requestedId = _nextId;
      }
      _nextId = requestedId +
                1;  // can't guarantee that this will be called in 'id increasing' order but a good guess perhaps

      record.setId(requestedId);
      _records.push_back(record);
      auto& new_record = _records.back();
      _recordsIdx.insert(&new_record);
      return &new_record;
   }

   const TRecord* search(TRecord record) const {
      auto it = _recordsIdx.find(&record);
      if (it != _recordsIdx.end()) {
         // Found an existing ID for the key.
         auto existing = *it;
         return existing;
      }
      return nullptr;
   }

   const TRecord* searchId(Int64 id) const {
      for (auto& record : _records) {
         if (record.getId() == id) return &record;
      }
      return nullptr;
   }

   std::vector<TPersistable> getPersistableCollection() {
      std::vector<TPersistable> persistables;
      for (const auto& record : _records) {
         persistables.push_back(record.asPersistable());
      }
      return persistables;
   }

   void clear() {
      _recordsIdx.clear();
      _records.clear();
   }

   void shrink_to_fit() {
      _recordsIdx.clear();
      _records.shrink_to_fit();
   }

   rec_accu_size_type size() { return _records.size(); }

   const rec_accu_vec& records() const { return _records; };

  private:
   std::mutex _mutex;
   std::atomic<Int64> _nextId;
   rec_accu_set _recordsIdx;
   rec_accu_vec _records;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_RECORDACCUMULATORWITHMUTEX_H_