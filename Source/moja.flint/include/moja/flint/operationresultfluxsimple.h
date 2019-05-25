#ifndef MOJA_FLINT_OPERATIONRESULTFLUXSIMPLE_H_
#define MOJA_FLINT_OPERATIONRESULTFLUXSIMPLE_H_

#include "moja/flint/ioperationresultflux.h"

namespace moja {
namespace flint {

class OperationResultFluxSimple : public IOperationResultFlux {
  public:
   OperationResultFluxSimple() = default;
   OperationResultFluxSimple(OperationTransferType transferType, const ModuleMetaData* metaData, int source, int sink,
                             double value);

   ~OperationResultFluxSimple() override;

   int source() const override;
   int sink() const override;
   double value() const override;

   OperationTransferType transferType() const override;
   const ModuleMetaData* metaData() const override;

  private:
   OperationTransferType _transferType;
   const ModuleMetaData* _metaData;
   int _source;
   int _sink;
   double _value;
};

inline OperationResultFluxSimple::OperationResultFluxSimple(OperationTransferType transferType,
                                                            const ModuleMetaData* metaData, int source, int sink,
                                                            double value)
    : _transferType(transferType), _metaData(metaData), _source(source), _sink(sink), _value(value) {}

inline OperationResultFluxSimple::~OperationResultFluxSimple() {}

inline int OperationResultFluxSimple::source() const { return _source; }

inline int OperationResultFluxSimple::sink() const { return _sink; }

inline double OperationResultFluxSimple::value() const { return _value; }

inline OperationTransferType OperationResultFluxSimple::transferType() const { return _transferType; }

inline const ModuleMetaData* OperationResultFluxSimple::metaData() const { return _metaData; }

typedef std::vector<OperationResultFluxSimple> OperationResultFluxSimpleNewVector;

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OPERATIONRESULTFLUXSIMPLE_H_