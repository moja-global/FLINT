#include "moja/flint/poolsimple.h"

#include "moja/flint/flintexceptions.h"

namespace moja {
namespace flint {

PoolSimple::PoolSimple(std::vector<double>& pools, const std::string& name, int idx, double value)
    : _metadata(name, "", "", 1.0, idx), _idx(idx), _initValue(value), _value(pools[_idx]) {
   _value = _initValue;
}

PoolSimple::PoolSimple(std::vector<double>& pools, const std::string& name, const std::string& description,
                       const std::string& units, double scale, int order, int idx, double value)
    : _metadata(name, description, units, scale, order), _idx(idx), _initValue(value), _value(pools[_idx]) {
   _value = _initValue;
}

const std::string& PoolSimple::name() const { return _metadata.name(); }

const std::string& PoolSimple::description() const { return _metadata.description(); }

const std::string& PoolSimple::units() const { return _metadata.units(); }

double PoolSimple::scale() const { return _metadata.scale(); }

int PoolSimple::order() const { return _metadata.order(); }

double PoolSimple::initValue() const { return _initValue; }

void PoolSimple::init() { _value = _initValue; }

}  // namespace flint
}  // namespace moja
