#include "moja/flint/poolcohort.h"

#include "moja/flint/flintexceptions.h"

namespace moja::flint {

PoolCohort::PoolCohort(std::vector<double>& pools, const std::string& name, int idx, double value)
    : _metadata(name, "", "", 1.0, idx), idx_(idx), init_value_(value), value_(pools[idx_]) {
   value_ = init_value_;
}

PoolCohort::PoolCohort(std::vector<double>& pools, const std::string& name, const std::string& description,
                       const std::string& units, double scale, int order, int idx, double value)
    : _metadata(name, description, units, scale, order), idx_(idx), init_value_(value), value_(pools[idx_]) {
   value_ = init_value_;
}

const std::string& PoolCohort::name() const { return _metadata.name(); }

const std::string& PoolCohort::description() const { return _metadata.description(); }

const std::string& PoolCohort::units() const { return _metadata.units(); }

double PoolCohort::scale() const { return _metadata.scale(); }

int PoolCohort::order() const { return _metadata.order(); }

double PoolCohort::initValue() const { return init_value_; }

void PoolCohort::init() { value_ = init_value_; }

}  // namespace flint
