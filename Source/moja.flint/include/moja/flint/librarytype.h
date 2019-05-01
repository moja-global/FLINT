#ifndef MOJA_FLINT_LIBRARYTYPE_H_
#define MOJA_FLINT_LIBRARYTYPE_H_

namespace moja {
namespace flint {

enum class LibraryType {
	Unknown,
	Internal,
	Managed,
	External,
    Python
};

}
} // moja::flint

#endif // MOJA_FLINT_LIBRARYTYPE_H_