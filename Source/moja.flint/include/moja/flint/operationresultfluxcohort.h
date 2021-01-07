#pragma once

#include "moja/flint/ioperationresultflux.h"

namespace moja::flint {

class OperationResultFluxCohort : public IOperationResultFlux {
  public:
   OperationResultFluxCohort() = default;
   OperationResultFluxCohort(OperationTransferType transferType, const ModuleMetaData* metaData, int source,
                                   int sink, double value);

   ~OperationResultFluxCohort() override;

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

inline OperationResultFluxCohort::OperationResultFluxCohort(OperationTransferType transferType,
                                                                        const ModuleMetaData* metaData, int source,
                                                                        int sink, double value)
    : _transferType(transferType), _metaData(metaData), _source(source), _sink(sink), _value(value) {}

inline OperationResultFluxCohort::~OperationResultFluxCohort() {}

inline int OperationResultFluxCohort::source() const { return _source; }

inline int OperationResultFluxCohort::sink() const { return _sink; }

inline double OperationResultFluxCohort::value() const { return _value; }

inline OperationTransferType OperationResultFluxCohort::transferType() const { return _transferType; }

inline const ModuleMetaData* OperationResultFluxCohort::metaData() const { return _metaData; }

typedef std::vector<OperationResultFluxCohort> OperationResultFluxCohortNewVector;

}  // namespace moja::flint
