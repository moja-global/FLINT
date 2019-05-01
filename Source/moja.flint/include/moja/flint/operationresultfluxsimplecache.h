#ifndef MOJA_FLINT_OPERATIONRESULTFLUXSIMPLECACHE_H_
#define MOJA_FLINT_OPERATIONRESULTFLUXSIMPLECACHE_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/imodule.h"
#include "moja/flint/ioperationresultflux.h"
#include "moja/flint/ioperation.h"

namespace moja {
namespace flint {

class OperationResultFluxSimpleCache : public IOperationResultFlux {
public:
	OperationResultFluxSimpleCache() = default;
	OperationResultFluxSimpleCache(OperationTransferType transferType, const ModuleMetaData* metaData, int source, int sink, double value);
	virtual ~OperationResultFluxSimpleCache() override;

	virtual int source() const override;
	virtual int sink() const override;
	virtual double value() const override;

	virtual OperationTransferType transferType() const override;
	virtual const ModuleMetaData* metaData() const override;

private:
	OperationTransferType _transferType;
	const ModuleMetaData* _metaData;
	int _source;
	int _sink;
	double _value;
};

inline OperationResultFluxSimpleCache::OperationResultFluxSimpleCache(OperationTransferType transferType, const ModuleMetaData* metaData, int source, int sink, double value)
	: _transferType(transferType), _metaData(metaData), _source(source), _sink(sink), _value(value) {}

inline OperationResultFluxSimpleCache::~OperationResultFluxSimpleCache() {}

inline int OperationResultFluxSimpleCache::source() const { return _source; }

inline int OperationResultFluxSimpleCache::sink() const { return _sink; }

inline double OperationResultFluxSimpleCache::value() const { return _value; }

inline OperationTransferType OperationResultFluxSimpleCache::transferType() const { return _transferType; }

inline const ModuleMetaData* OperationResultFluxSimpleCache::metaData() const { return _metaData; }

typedef std::vector<OperationResultFluxSimpleCache > OperationResultFluxSimpleCacheNewVector;

}
} // moja::flint

#endif // MOJA_FLINT_OPERATIONRESULTFLUXSIMPLECACHE_H_