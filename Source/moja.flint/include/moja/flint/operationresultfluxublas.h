#ifndef MOJA_FLINT_OPERATIONRESULTFLUXUBLAS_H_
#define MOJA_FLINT_OPERATIONRESULTFLUXUBLAS_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/imodule.h"
#include "moja/flint/ioperation.h"
#include "moja/flint/ioperationresultflux.h"

namespace moja {
namespace flint {

class OperationResultFluxUblas : public IOperationResultFlux {
  public:
   friend class OperationResultFluxIteratorUblas;

   OperationResultFluxUblas() = default;
   OperationResultFluxUblas(OperationTransferType transferType, const ModuleMetaData* metaData, int source, int sink,
                            double value);
   virtual ~OperationResultFluxUblas() override;

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

inline OperationResultFluxUblas::OperationResultFluxUblas(OperationTransferType transferType,
                                                          const ModuleMetaData* metaData, int source, int sink,
                                                          double value)
    : _transferType(transferType), _metaData(metaData), _source(source), _sink(sink), _value(value) {}

inline OperationResultFluxUblas::~OperationResultFluxUblas() {}

inline int OperationResultFluxUblas::source() const { return _source; }

inline int OperationResultFluxUblas::sink() const { return _sink; }

inline double OperationResultFluxUblas::value() const { return _value; }

inline OperationTransferType OperationResultFluxUblas::transferType() const { return _transferType; }

inline const ModuleMetaData* OperationResultFluxUblas::metaData() const { return _metaData; }

typedef std::vector<OperationResultFluxUblas> OperationResultFluxUblasNewVector;

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OPERATIONRESULTFLUXUBLAS_H_