#pragma once

#include "moja/flint/ioperationresult.h"
#include "moja/flint/operationresultfluxsimple.h"
#include "moja/flint/timing.h"

#include <moja/dynamic.h>

namespace moja::flint {

class IOperation;

class OperationResultCohort : public IOperationResult {
   friend class OperationManagerCohort;
   friend class StockOperationCohort;
   friend class ProportionalOperationCohort;

   inline static uint32_t cnt = 0;

  public:
   OperationResultCohort::OperationResultCohort(IOperation& operation);
   OperationResultCohort() : transfer_type_(), meta_data_(nullptr), has_data_package_(false) {
      fluxes_.reserve(40);
      // std::cout << "OperationResultCohort c_tor " << cnt<< std::endl;
      // cnt++;
   }

   virtual ~OperationResultCohort() = default;
   OperationResultCohort(const OperationResultCohort&) = delete;

   virtual OperationResultFluxCollection operationResultFluxCollection() override;

   virtual OperationTransferType transferType() const override;
   virtual const ModuleMetaData* metaData() const override;
   virtual const DynamicVar& dataPackage() const override;
   virtual bool hasDataPackage() const override;

   virtual const Timing& timingWhenApplied() const override { return timing_when_applied_; }
   virtual void setTimingWhenApplied(const Timing& timing) override { timing_when_applied_ = timing; }

   std::vector<OperationResultFluxSimple>& fluxes() { return fluxes_; }

  private:
   void addFlux(int source, int sink, double value);

  protected:
   OperationTransferType transfer_type_;
   const ModuleMetaData* meta_data_;
   DynamicVar data_package_;
   bool has_data_package_;
   std::vector<OperationResultFluxSimple> fluxes_;

   Timing timing_when_applied_;
};

inline OperationTransferType OperationResultCohort::transferType() const { return transfer_type_; }

inline const ModuleMetaData* OperationResultCohort::metaData() const { return meta_data_; }

inline const DynamicVar& OperationResultCohort::dataPackage() const { return data_package_; }

inline bool OperationResultCohort::hasDataPackage() const { return has_data_package_; }

}  // namespace moja::flint
