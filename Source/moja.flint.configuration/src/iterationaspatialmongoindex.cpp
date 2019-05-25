#include "moja/flint/configuration/iterationaspatialmongoindex.h"

namespace moja {
namespace flint {
namespace configuration {

IterationASpatialMongoIndex::IterationASpatialMongoIndex()
    : IterationBase(LocalDomainIterationType::ASpatialMongoIndex), _processAllDocuments(false) {}

}  // namespace configuration
}  // namespace flint
}  // namespace moja
