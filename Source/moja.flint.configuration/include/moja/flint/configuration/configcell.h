#ifndef MOJA_FLINT_CONFIGURATION_CONFIGCELL_H_
#define MOJA_FLINT_CONFIGURATION_CONFIGCELL_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include <string>

namespace moja {
namespace flint {
namespace configuration {

class ConfigBlock;

class CONFIGURATION_API ConfigCell {
public:
	ConfigCell(const ConfigBlock& block, int row, int col);
	virtual ~ConfigCell() = default;

	virtual std::string toString() const;
	virtual double area() const;
	virtual double lon() const;
	virtual double lat() const;

	virtual int id() const { return _id; }

private:
	const double Pi = 3.141592653590;
	const double RadiusOfEarthInMeters = 6378137.0;
	const double DiameterOfEarthInMetersPerDeg = 2.0 * Pi * RadiusOfEarthInMeters / 360.0;
	const double DegToRadMultiplier = Pi / 180.0;

	const ConfigBlock& _block;
	int _row;
	int _col;
	int _id;
};

}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_CONFIGCELL_H_