#include "moja/flint/externalpoolsimple.h"

#include "moja/flint/flintexceptions.h"

namespace moja::flint {

ExternalPoolSimple::ExternalPoolSimple(std::vector<double>& pools, const std::string& name,
                                       const std::string& description, const std::string& units, double scale,
                                       int order, int idx, const std::shared_ptr<ITransform> initValue, IPool* parent)
    : _metadata(name, description, units, scale, order),
      _idx(idx),
      _initValue(initValue),
      _value(pools[idx]),
      _parent(parent) {
   if (parent != nullptr) {
      parent->add_child(this);
   }
}

const std::string& ExternalPoolSimple::name() const { return _metadata.name(); }

const std::string& ExternalPoolSimple::description() const { return _metadata.description(); }

const std::string& ExternalPoolSimple::units() const { return _metadata.units(); }

double ExternalPoolSimple::scale() const { return _metadata.scale(); }

int ExternalPoolSimple::order() const { return _metadata.order(); }

std::optional<double> ExternalPoolSimple::initValue() const {
   const auto& value = _initValue->value();
   return (value.isEmpty() ? std::optional<double>{} : std::optional<double>(_initValue->value()));
}

void ExternalPoolSimple::init() {
   const auto& value = _initValue->value();
   _value = value.isEmpty() ? 0.0 : value.convert<double>();
}

void ExternalPoolSimple::add_child(IPool* pool) {
   throw std::runtime_error("Cant add child pool to pool with initial value set.");
}

}  // namespace moja::flint
