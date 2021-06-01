#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/record.h"

namespace moja::flint {

template <class TPersistable>
struct RecordHasher {
   std::size_t operator()(Record<TPersistable>* record) const { return record->hash(); }
};

template <class TPersistable>
struct RecordComparer {
   bool operator()(Record<TPersistable>* lhs, const Record<TPersistable>* rhs) const { return lhs->operator==(*rhs); }
};

}  // namespace moja::flint
