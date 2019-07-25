#ifndef MOJA_FLINT_RECORD_UTILS_H_
#define MOJA_FLINT_RECORD_UTILS_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/record.h"

#include <moja/types.h>

#include <Poco/Tuple.h>

#include <vector>

namespace moja {
namespace flint {

template <class TPersistable>
struct RecordHasher {
   std::size_t operator()(Record<TPersistable>* record) const { return record->hash(); }
};

template <class TPersistable>
struct RecordComparer {
   bool operator()(Record<TPersistable>* lhs, const Record<TPersistable>* rhs) const { return lhs->operator==(*rhs); }
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_RECORD_UTILS_H_