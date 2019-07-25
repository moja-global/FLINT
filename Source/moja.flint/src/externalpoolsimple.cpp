#include "moja/flint/externalpoolsimple.h"

#include "moja/flint/flintexceptions.h"

namespace moja {
namespace flint {

ExternalPoolSimple::ExternalPoolSimple(std::vector<double>& pools, const std::string& name, int idx,
                                       const std::shared_ptr<ITransform> initValue)
    : _metadata(name, "", "", 1.0, idx), _idx(idx), _initValue(initValue), _value(pools[idx]) {
   _value = 0.0;
}

ExternalPoolSimple::ExternalPoolSimple(std::vector<double>& pools, const std::string& name,
                                       const std::string& description, const std::string& units, double scale,
                                       int order, int idx, const std::shared_ptr<ITransform> initValue)
    : _metadata(name, description, units, scale, order), _idx(idx), _initValue(initValue), _value(pools[idx]) {
   _value = 0.0;
}

const std::string& ExternalPoolSimple::name() const { return _metadata.name(); }

const std::string& ExternalPoolSimple::description() const { return _metadata.description(); }

const std::string& ExternalPoolSimple::units() const { return _metadata.units(); }

double ExternalPoolSimple::scale() const { return _metadata.scale(); }

int ExternalPoolSimple::order() const { return _metadata.order(); }

double ExternalPoolSimple::initValue() const { return _initValue->value(); }

void ExternalPoolSimple::init() {
   const auto& value = _initValue->value();
   _value = value.isEmpty() ? 0.0 : value.convert<double>();
}

}  // namespace flint
}  // namespace moja
