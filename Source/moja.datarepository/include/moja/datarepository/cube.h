#ifndef MOJA_DATAREPOSITORY_CUBE_H_
#define MOJA_DATAREPOSITORY_CUBE_H_

#include <moja/utility.h>
#include <string>

namespace moja {
namespace datarepository {

// Used to make a filename withthe Tiles top, left lat/lon
struct Cube {
	static const unsigned width = 360;

	explicit Cube(Point point);
	std::string cubeId() const;
	Point point;
};

}} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_CUBE_H_
