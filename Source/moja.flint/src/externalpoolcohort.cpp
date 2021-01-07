#include "moja/flint/externalpoolcohort.h"

#include "moja/flint/flintexceptions.h"

namespace moja::flint {

ExternalPoolCohort::ExternalPoolCohort(std::vector<double>& pools, const std::string& name, int idx,
                                                   std::shared_ptr<ITransform> initValue)
    : _metadata(name, "", "", 1.0, idx), idx_(idx), init_value_(initValue), value_(pools[idx]) {
   value_ = 0.0;
}

ExternalPoolCohort::ExternalPoolCohort(std::vector<double>& pools, const std::string& name,
                                                   const std::string& description, const std::string& units,
                                                   double scale, int order, int idx,
                                                   std::shared_ptr<ITransform> initValue)
    : _metadata(name, description, units, scale, order), idx_(idx), init_value_(initValue), value_(pools[idx]) {
   value_ = 0.0;
}

const std::string& ExternalPoolCohort::name() const { return _metadata.name(); }

const std::string& ExternalPoolCohort::description() const { return _metadata.description(); }

const std::string& ExternalPoolCohort::units() const { return _metadata.units(); }

double ExternalPoolCohort::scale() const { return _metadata.scale(); }

int ExternalPoolCohort::order() const { return _metadata.order(); }

double ExternalPoolCohort::initValue() const { return init_value_->value(); }

void ExternalPoolCohort::init() {
   const auto& value = init_value_->value();
   value_ = value.isEmpty() ? 0.0 : value.convert<double>();
}

}  // namespace moja::flint
