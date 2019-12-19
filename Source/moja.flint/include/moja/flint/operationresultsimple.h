#ifndef MOJA_FLINT_OPERATIONRESULTSIMPLE_H_
#define MOJA_FLINT_OPERATIONRESULTSIMPLE_H_

#include "moja/flint/ioperationresult.h"
#include "moja/flint/operationresultfluxsimple.h"
#include "moja/flint/timing.h"

#include <moja/dynamic.h>

namespace moja {
namespace flint {
class IOperation;
// --------------------------------------------------------------------------------------------

class OperationResultSimple : public IOperationResult {
   friend class OperationManagerSimple;
   friend class StockOperationSimple;
   friend class ProportionalOperationSimple;

  public:
   typedef std::shared_ptr<OperationResultSimple> Ptr;

   explicit OperationResultSimple(IOperation& operation);
   virtual ~OperationResultSimple() = default;
   OperationResultSimple(const OperationResultSimple&) = delete;

   virtual OperationResultFluxCollection operationResultFluxCollection() override;

   virtual OperationTransferType transferType() const override;
   virtual const ModuleMetaData* metaData() const override;
   virtual const DynamicVar& dataPackage() const override;
   virtual bool hasDataPackage() const override;

   virtual const Timing& timingWhenApplied() const override { return _timingWhenApplied; };
   virtual void setTimingWhenApplied(const Timing& timing) override { _timingWhenApplied = timing; };

   std::vector<OperationResultFluxSimple>& fluxes() { return _fluxes; }

  private:
   void addFlux(int source, int sink, double value);

  protected:
   OperationTransferType _transferType;
   const ModuleMetaData* _metaData;
   const DynamicVar _dataPackage;
   const bool _hasDataPackage;
   std::vector<OperationResultFluxSimple> _fluxes;

   Timing _timingWhenApplied;
};

#undef USE_INT_ITERATOR

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OPERATIONRESULTSIMPLE_H_
