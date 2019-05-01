#ifndef MOJA_FLINT_LIBRARYINFOMANAGED_H_
#define MOJA_FLINT_LIBRARYINFOMANAGED_H_

#include <Poco/SharedLibrary.h>
#include "moja/flint/librarytype.h"
#include "moja/flint/libraryinfobase.h"

#include <string>

namespace moja {
namespace flint {

class LibraryInfoManaged : public LibraryInfoBase {
public:
	virtual ~LibraryInfoManaged() { }

	LibraryType GetLibraryType() const override { return LibraryType::Managed; }

	std::string originalFilename;

	/** File name of this module (.dll or .so file name). */
	std::string filename;

	/** Handle to this module (library handle), if it's currently loaded. */
    std::unique_ptr<Poco::SharedLibrary> handle;
};

}
} // moja::flint

#endif // MOJA_FLINT_LIBRARYINFOMANAGED_H_
