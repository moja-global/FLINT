#pragma once

#include "moja/flint/ioperationtransfer.h"

namespace moja::flint {

class OperationTransferCohort : public IOperationTransfer {
  public:
   OperationTransferCohort() = default;
   OperationTransferCohort(OperationTransferType type, int source, int sink, double value,
                           const ModuleMetaData* metaData)
       : type_(type), source_(source), sink_(sink), value_(value), meta_data_(metaData) {}
   ~OperationTransferCohort() override;

   int source() const override;
   int sink() const override;
   double value() const override;
   OperationTransferType transferType() const override;
   const ModuleMetaData* metaData() const override;

  private:
   OperationTransferType type_;
   int source_;
   int sink_;
   double value_;
   const ModuleMetaData* meta_data_;
};

inline OperationTransferCohort::~OperationTransferCohort() {}

inline int OperationTransferCohort::source() const { return source_; }

inline int OperationTransferCohort::sink() const { return sink_; }

inline double OperationTransferCohort::value() const { return value_; }

inline OperationTransferType OperationTransferCohort::transferType() const { return type_; }

inline const ModuleMetaData* OperationTransferCohort::metaData() const { return meta_data_; }

}  // namespace moja::flint
