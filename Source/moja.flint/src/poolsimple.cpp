#include "moja/flint/poolsimple.h"

#include "moja/flint/flintexceptions.h"

namespace moja {
namespace flint {

PoolSimple::PoolSimple(std::vector<double>& pools, const std::string& name, const std::string& description,
                       const std::string& units, double scale, int order, int idx, std::optional<double> value,
                       IPool* parent)
    : _metadata(name, description, units, scale, order),
      _idx(idx),
      _initValue(value),
      _value(pools[_idx]),
      _parent(parent) {
   if (parent != nullptr) {
      parent->add_child(this);
   }
}

const std::string& PoolSimple::name() const { return _metadata.name(); }

const std::string& PoolSimple::description() const { return _metadata.description(); }

const std::string& PoolSimple::units() const { return _metadata.units(); }

double PoolSimple::scale() const { return _metadata.scale(); }

int PoolSimple::order() const { return _metadata.order(); }

std::optional<double> PoolSimple::initValue() const { return _initValue; }

void PoolSimple::init() { _value = _initValue.value_or(0.0); }

void PoolSimple::add_child(IPool* pool) {
   if (_initValue.has_value()) {
      throw std::runtime_error("Cant add child pool to pool with initial value set.");
   }
   _children.emplace_back(pool);
}

}  // namespace flint
}  // namespace moja
