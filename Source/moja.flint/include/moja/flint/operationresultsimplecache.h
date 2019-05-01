#ifndef MOJA_FLINT_OPERATIONRESULTSIMPLECACHE_H_
#define MOJA_FLINT_OPERATIONRESULTSIMPLECACHE_H_

#include "moja/flint/ioperationresult.h"
#include "moja/flint/operationresultfluxcollection.h"
#include "moja/flint/operationresultfluxsimplecache.h"
#include "moja/flint/timing.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

class OperationResultSimpleCache : public IOperationResult {
	friend class OperationManagerSimpleCache;
	friend class StockOperationSimpleCache;
	friend class ProportionalOperationSimpleCache;
public:
	typedef std::shared_ptr<OperationResultSimpleCache> Ptr;

	explicit OperationResultSimpleCache(OperationTransferType transferType, ModuleMetaData* metaData);
	explicit OperationResultSimpleCache(IOperation& operation);
	virtual ~OperationResultSimpleCache() = default;

	virtual OperationResultFluxCollection operationResultFluxCollection() override;

	virtual OperationTransferType transferType() const override;
	virtual const ModuleMetaData* metaData() const override;
	virtual const DynamicVar& dataPackage() const override;
	virtual bool hasDataPackage() const override;

	virtual const Timing& timingWhenApplied() const override { return _timingWhenApplied; };
	virtual void setTimingWhenApplied(const Timing& timing) override { _timingWhenApplied = timing; };

private:
	void addFlux(int source, int sink, double value);

protected:
	OperationTransferType _transferType;
	const ModuleMetaData* _metaData;
	const DynamicVar _dataPackage;
	const bool _hasDataPackage;
	std::vector<OperationResultFluxSimpleCache> _fluxes;

	Timing _timingWhenApplied;
};

#undef USE_INT_ITERATOR

}
} // moja::flint

#endif // MOJA_FLINT_OPERATIONRESULTSIMPLECACHE_H_
