#ifndef MOJA_FLINT_LIBRARYINFOPYTHON_H_
#define MOJA_FLINT_LIBRARYINFOPYTHON_H_

#include "moja/flint/libraryinfobase.h"
#include "moja/flint/librarytype.h"

#include <Poco/SharedLibrary.h>

#include <boost/python.hpp>

#include <string>

namespace moja {
namespace flint {

class LibraryInfoPython : public LibraryInfoBase {
  public:
   LibraryInfoPython() {}
   virtual ~LibraryInfoPython() {}

   LibraryType GetLibraryType() const override { return LibraryType::Python; }

   std::string originalFilename;

   /** File name of this module (.py file name) */
   std::string filename;

   std::vector<std::shared_ptr<boost::python::object>> moduleHandles;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_LIBRARYINFOPYTHON_H_
