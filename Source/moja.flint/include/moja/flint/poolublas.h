#ifndef MOJA_FLINT_POOLUBLAS_H_
#define MOJA_FLINT_POOLUBLAS_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ipool.h"
#include "moja/flint/matrixublas.h"
#include <string>

namespace moja {
namespace flint {

class FLINT_API PoolUblas : public IPool {
public:
	PoolUblas() = delete;
	PoolUblas(moja_ublas_vector& pools, const std::string& name, int idx, double value);
	PoolUblas(moja_ublas_vector& pools, const std::string& name, const std::string& description, const std::string& units, double scale, int order, int idx, double value);
	~PoolUblas() = default;

	const std::string& name() const override;
	const std::string& description() const override;
	const std::string& units() const override;
	double scale() const override;
	int order() const override;
	double initValue() const override;
	int idx() const override;

	double value() const override;
	void set_value(double value) override;

	void init() override;

protected:
	PoolMetaData _metadata;

	int _idx;
	double _initValue;
	bool _initialised = false;

	moja_ublas_vector* _pools;
};

}
} // moja::flint

#endif // MOJA_FLINT_POOLUBLAS_H_
