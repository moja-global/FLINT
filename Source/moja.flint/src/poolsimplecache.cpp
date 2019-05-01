#include "moja/flint/poolsimplecache.h"
#include "moja/flint/flintexceptions.h"

namespace moja {
namespace flint {

PoolSimpleCache::PoolSimpleCache(std::vector<double>& pools, const std::string& name, int idx, double value)
	: _metadata(name, "", "", 1.0, idx), _idx(idx), _initValue(value), _initialised(true), _pools(&pools) {
	(*_pools)[_idx] = _initValue;
}

PoolSimpleCache::PoolSimpleCache(std::vector<double>& pools, const std::string& name, const std::string& description, const std::string& units, double scale, int order, int idx, double value)
	: _metadata(name, description, units, scale, order), _idx(idx), _initValue(value), _initialised(true), _pools(&pools) {
	(*_pools)[_idx] = _initValue;
}

const std::string& PoolSimpleCache::name() const {
	return _metadata.name();
}

const std::string& PoolSimpleCache::description() const {
	return _metadata.description();
}

const std::string& PoolSimpleCache::units() const {
	return _metadata.units();
}

double PoolSimpleCache::scale() const {
	return _metadata.scale();
}

int PoolSimpleCache::order() const {
	return _metadata.order();
}

double PoolSimpleCache::initValue() const {
	return _initValue;
}

int PoolSimpleCache::idx() const {
	return _idx;
}

double PoolSimpleCache::value() const {
	if (!_initialised) {
		throw PreconditionViolatedException() << Precondition("PoolSimpleCache has not been initialised");
	}
	return (*_pools)[_idx];
}

void PoolSimpleCache::set_value(double value) {
	(*_pools)[_idx] = value;
}

void PoolSimpleCache::init() {
	(*_pools)[_idx] = _initValue;
	_initialised = true;
}

}
} // moja::flint


