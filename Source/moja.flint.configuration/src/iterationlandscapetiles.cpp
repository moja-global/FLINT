#include "moja/flint/configuration/iterationlandscapetiles.h"

namespace moja {
namespace flint {
namespace configuration {

	IterationLandscapeTiles::IterationLandscapeTiles()
		: IterationBase(LocalDomainIterationType::LandscapeTiles), _tileSizeX(0), _tileSizeY(0), _xPixels(0), _yPixels(0) {
	}

}
}
} // namespace moja::flint::configuration
