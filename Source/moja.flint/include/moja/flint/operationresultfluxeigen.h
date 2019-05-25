#ifndef MOJA_FLINT_OPERATIONRESULTFLUXEIGEN_H_
#define MOJA_FLINT_OPERATIONRESULTFLUXEIGEN_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/imodule.h"
#include "moja/flint/ioperationresultflux.h"

namespace moja {
namespace flint {

class OperationResultFluxEigen : public IOperationResultFlux {
  public:
   friend class OperationResultFluxIteratorEigen;

   OperationResultFluxEigen() = default;
   OperationResultFluxEigen(OperationTransferType transferType, const ModuleMetaData* metaData, int source, int sink,
                            double value);
   virtual ~OperationResultFluxEigen() override;

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

inline OperationResultFluxEigen::OperationResultFluxEigen(OperationTransferType transferType,
                                                          const ModuleMetaData* metaData, int source, int sink,
                                                          double value)
    : _transferType(transferType), _metaData(metaData), _source(source), _sink(sink), _value(value) {}

inline OperationResultFluxEigen::~OperationResultFluxEigen() {}

inline int OperationResultFluxEigen::source() const { return _source; }

inline int OperationResultFluxEigen::sink() const { return _sink; }

inline double OperationResultFluxEigen::value() const { return _value; }

inline OperationTransferType OperationResultFluxEigen::transferType() const { return _transferType; }

inline const ModuleMetaData* OperationResultFluxEigen::metaData() const { return _metaData; }

typedef std::vector<OperationResultFluxEigen> OperationResultFluxEigenNewVector;

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OPERATIONRESULTFLUXEIGEN_H_