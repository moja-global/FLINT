#ifndef MOJA_FLINT_POOLSIMPLE_H_
#define MOJA_FLINT_POOLSIMPLE_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ipool.h"
#include <vector>

namespace moja {
namespace flint {

class FLINT_API PoolSimple : public IPool {
public:
	PoolSimple() = delete;
	PoolSimple(std::vector<double>& pools, const std::string& name, int idx, double value);
	PoolSimple(std::vector<double>& pools, const std::string& name, const std::string& description, const std::string& units, double scale, int order, int idx, double value);
	~PoolSimple() = default;

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
	double& _value;
};

inline double PoolSimple::value() const {
	return _value;
}

inline void PoolSimple::set_value(double value) {
	_value = value;
}

inline int PoolSimple::idx() const {
	return _idx;
}

}
} // moja::flint

#endif // MOJA_FLINT_POOLSIMPLE_H_
