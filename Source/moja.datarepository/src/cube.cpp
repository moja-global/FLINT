#include "moja/datarepository/cube.h"

#include <iomanip>
#include <sstream>
#include <moja/mathex.h>

namespace moja {
namespace datarepository {

Cube::Cube(Point p) :
	// was attempting to move to boundry of 1.0 degree tiles
	// doesn't make sense now with tiles that can be set to different dimensions (0.9), etc.
	// perhaps we could just use the Index?
	//_y_index(unsigned(p.lat + (p.lat > 0 ? MathEx::k0Plus : -MathEx::k0Plus))),
	//_x_index(unsigned(p.lon + (p.lon > 0 ? MathEx::k0Plus : -MathEx::k0Plus))) {}
	point(p) {}

std::string Cube::cubeId() const {
	std::ostringstream str;
	str << std::setfill('0');
	str << (point.lon < 0 ? "-" : "") << std::setw(3) << std::abs(point.lon) << "_" << (point.lat < 0 ? "-" : "") << std::setw(3) << std::abs(point.lat);
	return str.str();
}

}} // namespace moja::datarepository