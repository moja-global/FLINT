#ifndef Configuration_IterationBase_INCLUDED
#define Configuration_IterationBase_INCLUDED

#include "moja/flint/configuration/_configuration_exports.h"

#include <string>

namespace moja {
	namespace InT {
		namespace Configuration {

			enum class LocalDomainIterationType {
				NotAnIteration,		// i.e Point sim
				LandscapeTiles,
				AreaOfInterest,
				TileIndex,
				BlockIndex
			};

			static LocalDomainIterationType convertStrToLocalDomainIterationType(std::string iterationTypeStr) {
				if (iterationTypeStr == "LandscapeTiles")
					return LocalDomainIterationType::LandscapeTiles;
				else if (iterationTypeStr == "AreaOfInterest")
					return LocalDomainIterationType::LandscapeTiles;
				else if (iterationTypeStr == "TileIndex")
					return LocalDomainIterationType::TileIndex;
				else if (iterationTypeStr == "BlockIndex")
					return LocalDomainIterationType::BlockIndex;
				return LocalDomainIterationType::NotAnIteration;
			}

			class CONFIGURATION_API IterationBase {
			public:
				IterationBase(LocalDomainIterationType iterationType);
				virtual ~IterationBase() { }

				virtual inline LocalDomainIterationType iterationType() const { return _iterationType; }

			private:
				LocalDomainIterationType _iterationType;
			};

		}
	}
} // namespace moja::flint::Configuration

#endif // Configuration_IterationBase_INCLUDED
