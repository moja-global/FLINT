#ifndef MOJA_FLINT_OPERATIONRESULTFLUXEIGENFULL_H_
#define MOJA_FLINT_OPERATIONRESULTFLUXEIGENFULL_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/imodule.h"
#include "moja/flint/ioperationresultflux.h"

namespace moja {
namespace flint {

class OperationResultFluxEigenFull : public IOperationResultFlux {
  public:
   friend class OperationResultFluxIteratorEigenFull;

   OperationResultFluxEigenFull() = default;
   OperationResultFluxEigenFull(OperationTransferType transferType, const ModuleMetaData* metaData, int source,
                                int sink, double value);
   virtual ~OperationResultFluxEigenFull() override;

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

inline OperationResultFluxEigenFull::OperationResultFluxEigenFull(OperationTransferType transferType,
                                                                  const ModuleMetaData* metaData, int source, int sink,
                                                                  double value)
    : _transferType(transferType), _metaData(metaData), _source(source), _sink(sink), _value(value) {}

inline OperationResultFluxEigenFull::~OperationResultFluxEigenFull() {}

inline int OperationResultFluxEigenFull::source() const { return _source; }

inline int OperationResultFluxEigenFull::sink() const { return _sink; }

inline double OperationResultFluxEigenFull::value() const { return _value; }

inline OperationTransferType OperationResultFluxEigenFull::transferType() const { return _transferType; }

inline const ModuleMetaData* OperationResultFluxEigenFull::metaData() const { return _metaData; }

typedef std::vector<OperationResultFluxEigenFull> OperationResultFluxEigenNewVector;

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OPERATIONRESULTFLUXEIGENFULL_H_