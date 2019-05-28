#ifndef MOJA_FLINT_LIBRARYINFOINTERNAL_H_
#define MOJA_FLINT_LIBRARYINFOINTERNAL_H_

#include "moja/flint/libraryinfobase.h"
#include "moja/flint/librarytype.h"

namespace moja {
namespace flint {

class LibraryInfoInternal : public LibraryInfoBase {
  public:
   virtual ~LibraryInfoInternal() {}

   inline LibraryType GetLibraryType() const override { return LibraryType::Internal; }
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_LIBRARYINFOINTERNAL_H_
