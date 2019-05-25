#include "moja/flint/libraryinfobase.h"

#include "moja/flint/librarymanager.h"

namespace moja {
namespace flint {

LibraryInfoBase::LibraryInfoBase() {
   loadOrder = LibraryManager::currentLoadOrder++;
   moduleCount = 0;
   transformCount = 0;
   flintDataCount = 0;
   flintDataFactoryCount = 0;
   providerCount = 0;
   libraryHandles = std::make_shared<FlintLibraryHandles>();
};

}  // namespace flint
}  // namespace moja
