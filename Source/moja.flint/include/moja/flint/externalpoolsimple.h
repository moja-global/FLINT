#ifndef MOJA_FLINT_EXTERNALPOOLSIMPLE_H_
#define MOJA_FLINT_EXTERNALPOOLSIMPLE_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ipool.h"
#include "moja/flint/itransform.h"
#include <vector>

namespace moja {
namespace flint {

class FLINT_API ExternalPoolSimple : public IPool {
public:
	ExternalPoolSimple() = delete;
	ExternalPoolSimple(std::vector<double>& pools, const std::string& name, int idx, const std::shared_ptr<ITransform> initValue);
    ExternalPoolSimple(std::vector<double>& pools, const std::string& name, const std::string& description, const std::string& units, double scale, int order, int idx, const std::shared_ptr<ITransform> initValue);
    ~ExternalPoolSimple() = default;

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
	std::shared_ptr<ITransform> _initValue;
	double& _value;
};

inline double ExternalPoolSimple::value() const {
	return _value;
}

inline void ExternalPoolSimple::set_value(double value) {
	_value = value;
}

inline int ExternalPoolSimple::idx() const {
	return _idx;
}

}
} // moja::flint

#endif // MOJA_FLINT_EXTERNALPOOLSIMPLE_H_
