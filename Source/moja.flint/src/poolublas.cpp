#include "moja/flint/poolublas.h"
#include "moja/flint/flintexceptions.h"

namespace moja {
namespace flint {

PoolUblas::PoolUblas(moja_ublas_vector& pools, const std::string& name, int idx, double value)
	: _metadata(name, "", "", 1.0, idx), _idx(idx), _initValue(value), _initialised(true), _pools(&pools) {
	(*_pools)[_idx] = _initValue;
}

PoolUblas::PoolUblas(moja_ublas_vector& pools, const std::string& name, const std::string& description, const std::string& units, double scale, int order, int idx, double value)
	: _metadata(name, description, units, scale, order), _idx(idx), _initValue(value), _initialised(true), _pools(&pools) {
	(*_pools)[_idx] = _initValue;
}

const std::string& PoolUblas::name() const {
	return _metadata.name();
}

const std::string& PoolUblas::description() const {
	return _metadata.description();
}

const std::string& PoolUblas::units() const {
	return _metadata.units();
}

double PoolUblas::scale() const {
	return _metadata.scale();
}

int PoolUblas::order() const {
	return _metadata.order();
}

double PoolUblas::initValue() const {
	return _initValue;
}

int PoolUblas::idx() const {
	return _idx;
}

double PoolUblas::value() const {
	if (!_initialised) {
		throw PreconditionViolatedException() << Precondition("PoolUblas has not been initialised");
	}
	return (*_pools)[_idx];
}

void PoolUblas::set_value(double value) {
	(*_pools)[_idx] = value;
}

void PoolUblas::init() {
	(*_pools)[_idx] = _initValue;
	_initialised = true;
}

}
} // moja::flint


