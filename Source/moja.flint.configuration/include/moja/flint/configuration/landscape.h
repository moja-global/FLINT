#ifndef MOJA_FLINT_CONFIGURATION_LANDSCAPE_H_
#define MOJA_FLINT_CONFIGURATION_LANDSCAPE_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/iterationareaofinterest.h"
#include "moja/flint/configuration/iterationaspatialindex.h"
#include "moja/flint/configuration/iterationaspatialmongoindex.h"
#include "moja/flint/configuration/iterationbase.h"
#include "moja/flint/configuration/iterationblockindex.h"
#include "moja/flint/configuration/iterationcellindex.h"
#include "moja/flint/configuration/iterationlandscapetiles.h"
#include "moja/flint/configuration/iterationtileindex.h"

#include <string>

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API Landscape {
  public:
   explicit Landscape(const std::string& providerName, LocalDomainIterationType iterationType);
   virtual ~Landscape() {}

   virtual const std::string& providerName() const { return _providerName; }
   virtual LocalDomainIterationType iterationType() const { return _iterationType; }

   virtual void set_providerName(const std::string& str) { _providerName = str; }
   virtual void set_iterationType(LocalDomainIterationType value) { _iterationType = value; }

   void setIteration(LocalDomainIterationType iterationType);

   void setIterationAreaOfInterest();
   void setIterationASpatialIndex();
   void setIterationASpatialMongoIndex();
   void setIterationLandscapeTiles();
   void setIterationTileIndex();
   void setIterationBlockIndex();
   void setIterationCellIndex();

   IterationAreaOfInterest* iterationAreaOfInterest() { return _iterationAreaOfInterest.get(); }
   IterationASpatialIndex* iterationASpatialIndex() { return _iterationASpatialIndex.get(); }
   IterationASpatialMongoIndex* iterationASpatialMongoIndex() { return _iterationASpatialMongoIndex.get(); }
   IterationLandscapeTiles* iterationLandscapeTiles() { return _iterationLandscapeTiles.get(); }
   IterationTileIndex* iterationTileIndex() { return _iterationTileIndex.get(); }
   IterationBlockIndex* iterationBlockIndex() { return _iterationBlockIndex.get(); }
   IterationCellIndex* iterationCellIndex() { return _iterationCellIndex.get(); }

   const IterationAreaOfInterest* iterationAreaOfInterest() const { return _iterationAreaOfInterest.get(); }
   const IterationASpatialIndex* iterationASpatialIndex() const { return _iterationASpatialIndex.get(); }
   const IterationASpatialMongoIndex* iterationASpatialMongoIndex() const { return _iterationASpatialMongoIndex.get(); }
   const IterationLandscapeTiles* iterationLandscapeTiles() const { return _iterationLandscapeTiles.get(); }
   const IterationTileIndex* iterationTileIndex() const { return _iterationTileIndex.get(); }
   const IterationBlockIndex* iterationBlockIndex() const { return _iterationBlockIndex.get(); }
   const IterationCellIndex* iterationCellIndex() const { return _iterationCellIndex.get(); }

  private:
   std::string _providerName;
   LocalDomainIterationType _iterationType;

   std::shared_ptr<IterationAreaOfInterest> _iterationAreaOfInterest;
   std::shared_ptr<IterationASpatialIndex> _iterationASpatialIndex;
   std::shared_ptr<IterationASpatialMongoIndex> _iterationASpatialMongoIndex;
   std::shared_ptr<IterationLandscapeTiles> _iterationLandscapeTiles;
   std::shared_ptr<IterationTileIndex> _iterationTileIndex;
   std::shared_ptr<IterationBlockIndex> _iterationBlockIndex;
   std::shared_ptr<IterationCellIndex> _iterationCellIndex;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_LANDSCAPE_H_
