#ifndef MOJA_FLINT_LIBRARYINFOEXTERNAL_H_
#define MOJA_FLINT_LIBRARYINFOEXTERNAL_H_

#include "moja/flint/libraryinfobase.h"
#include "moja/flint/librarytype.h"

#include <Poco/SharedLibrary.h>

#include <string>

namespace moja {
namespace flint {

class LibraryInfoExternal : public LibraryInfoBase {
  public:
   LibraryInfoExternal() : handle(nullptr) {}
   virtual ~LibraryInfoExternal() {}

   LibraryType GetLibraryType() const override { return LibraryType::External; }

   std::string originalFilename;

   /** File name of this module (.dll or .so file name) */
   std::string filename;

   /** Handle to this module (library handle), if it's currently loaded */
   std::unique_ptr<Poco::SharedLibrary> handle;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_LIBRARYINFOEXTERNAL_H_
