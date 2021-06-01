#pragma once

#include "moja/flint/libraryinfobase.h"
#include "moja/flint/librarytype.h"

namespace moja::flint {

class LibraryInfoInternal : public LibraryInfoBase {
  public:
   virtual ~LibraryInfoInternal() {}

   LibraryType GetLibraryType() const override { return LibraryType::Internal; }
};

}  // namespace moja::flint
