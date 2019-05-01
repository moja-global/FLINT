#ifndef MOJA_FLINT_RECORDACCUMULATORUUID_H_
#define MOJA_FLINT_RECORDACCUMULATORUUID_H_

#include "moja/types.h"
#include "moja/flint/recordutils.h"

#include "Poco/Mutex.h"

#include <unordered_set>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace moja {
namespace flint {

template<class TPersistable, class TRecord>
class RecordAccumulatorUuid {
	struct RecordComparer {
		bool operator()(const TRecord* lhs, const TRecord* rhs) const {
			return lhs->operator==(*rhs);
		}
	};

	struct RecordHasher {
		std::size_t operator()(const TRecord* record) const {
			return record->hash();
		}
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
	typedef std::vector <TRecord> rec_accu_vec;
	typedef typename rec_accu_vec::size_type rec_accu_size_type;

	RecordAccumulatorUuid() {}

	const TRecord* accumulate(TRecord record) {
		Poco::Mutex::ScopedLock lock(_mutex);
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
		record.setId(_uuidGenerator());
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

	const TRecord* searchId(boost::uuids::uuid id) const {
		for (auto& record : _records) {
			if (record.getId() == id)
				return &record;
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

	rec_accu_size_type size() {
		return _records.size();
	}

	const rec_accu_vec& records() const {
		return _records;
	};

private:
	Poco::Mutex _mutex;
	rec_accu_set _recordsIdx;
	rec_accu_vec _records;
    boost::uuids::random_generator _uuidGenerator;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_RECORDACCUMULATORUUID_H_