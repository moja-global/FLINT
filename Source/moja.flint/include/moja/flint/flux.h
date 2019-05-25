#ifndef MOJA_FLINT_FLUX_H_
#define MOJA_FLINT_FLUX_H_

#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ioperationresult.h"
#include "moja/flint/ioperationresultflux.h"
#include "moja/flint/ipool.h"

namespace moja {
namespace flint {

enum class FluxSource { DISTURBANCE, ANNUAL_PROCESS, COMBINED };

class Flux {
  public:
   Flux(std::vector<std::string> sourcePools, std::vector<std::string> destPools,
        FluxSource fluxSource = FluxSource::COMBINED, bool subtract = false)
       : _sourcePoolNames(sourcePools), _destPoolNames(destPools), _fluxSource(fluxSource), _subtract(subtract) {}

   void init(ILandUnitDataWrapper* landUnitData) {
      for (const auto& poolName : _sourcePoolNames) {
         _sourcePools.push_back(landUnitData->getPool(poolName)->idx());
      }

      for (const auto& poolName : _destPoolNames) {
         _destPools.push_back(landUnitData->getPool(poolName)->idx());
      }
   }

   double calculate(std::shared_ptr<IOperationResult> operationResult) {
      if (!isMatchingFluxSource(operationResult)) {
         return 0.0;
      }

      double sum = 0.0;
      for (auto it : operationResult->operationResultFluxCollection()) {
         if (isIncluded(*it)) {
            sum += it->value();
         }
      }

      return _subtract ? -sum : sum;
   }

  private:
   std::vector<std::string> _sourcePoolNames;
   std::vector<int> _sourcePools;

   std::vector<std::string> _destPoolNames;
   std::vector<int> _destPools;

   FluxSource _fluxSource;
   bool _subtract;

   bool isMatchingFluxSource(std::shared_ptr<IOperationResult> operationResult) {
      auto isDisturbance = isDisturbanceFlux(operationResult);
      switch (_fluxSource) {
         case FluxSource::DISTURBANCE:
            return isDisturbance;
         case FluxSource::ANNUAL_PROCESS:
            return !isDisturbance;
         default:
            return true;
      }
   }

   bool isDisturbanceFlux(std::shared_ptr<IOperationResult> flux) {
      if (!flux->hasDataPackage()) {
         return false;
      }

      auto& disturbanceData = flux->dataPackage().extract<const DynamicObject>();
      for (const auto& disturbanceField : {"disturbance", "disturbance_type_code", "pre_disturbance_age_class"}) {
         if (!disturbanceData.contains(disturbanceField)) {
            return false;
         }
      }

      return true;
   }

   bool isIncluded(const IOperationResultFlux& fluxRecord) {
      auto srcIx = fluxRecord.source();
      auto dstIx = fluxRecord.sink();
      if (srcIx == dstIx) {
         return false;  // don't process diagonal - flux to & from same pool is ignored
      }

      if (std::find(_sourcePools.begin(), _sourcePools.end(), srcIx) == _sourcePools.end()) {
         return false;
      }

      if (std::find(_destPools.begin(), _destPools.end(), dstIx) == _destPools.end()) {
         return false;
      }

      return true;
   }
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_FLUX_H_