#include "moja/flint/sumpoolstransform.h"

#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ioperationmanager.h"
#include "moja/flint/ipool.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

void SumPoolsTransform::configure(DynamicObject config, const ILandUnitController& landUnitController,
                                  datarepository::DataRepository& dataRepository) {
   _landUnitController = &landUnitController;

   // Look for a list of pools to use
   if (config.contains("pools")) {
      auto opMan = _landUnitController->operationManager();
      auto pools = config["pools"];
      if (pools.isVector()) {
         for (const auto& pool : pools) {
            const auto& poolName = pool.extract<const std::string>();
            poolNames.push_back(poolName);
            poolHandles.push_back(opMan->getPool(poolName));
         }
      }
   }
}

void SumPoolsTransform::controllerChanged(const ILandUnitController& controller) {
   _landUnitController = &controller;
   poolHandles.clear();
   auto opMan = _landUnitController->operationManager();
   for (auto& poolName : poolNames) {
      poolHandles.push_back(opMan->getPool(poolName));
   }
};

const moja::DynamicVar& SumPoolsTransform::value() const {
   double sum = 0.0;
   for (auto& pool : poolHandles) {
      sum += pool->value();
   }
   _result = sum;
   return _result;
}

}  // namespace flint
}  // namespace moja
