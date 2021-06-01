#include "moja/flint/testmodule.h"

#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ioperation.h"

#include <moja/notificationcenter.h>
#include <moja/signals.h>

namespace moja::flint {

void TestModule::configure(const DynamicObject& config) {
   ratio_1 = 0.50;
   ratio_2 = 0.50;
   ratio_3 = 0.50;

   variable_1 = "variable 1";
   variable_2 = "variable 2";
   variable_3 = "variable 3";

   pool_1 = "Pool 1";
   pool_2 = "Pool 2";
   pool_3 = "Pool 3";

   if (config.contains("ratio_1")) {
      ratio_1 = config["ratio_1"];
   }
   if (config.contains("ratio_2")) {
      ratio_3 = config["ratio_2"];
   }
   if (config.contains("ratio_3")) {
      ratio_3 = config["ratio_3"];
   }

   if (config.contains("variable_1")) {
      variable_1 = config["variable_1"].extract<const std::string>();
   }
   if (config.contains("variable_2")) {
      variable_2 = config["variable_2"].extract<const std::string>();
   }
   if (config.contains("variable_3")) {
      variable_3 = config["variable_3"].extract<const std::string>();
   }

   if (config.contains("pool_1")) {
      pool_1 = config["pool_1"].extract<const std::string>();
   }
   if (config.contains("pool_2")) {
      pool_2 = config["pool_2"].extract<const std::string>();
   }
   if (config.contains("pool_3")) {
      pool_3 = config["pool_3"].extract<const std::string>();
   }
}

void TestModule::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::LocalDomainInit, &TestModule::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::TimingInit, &TestModule::onTimingInit, *this);
   notificationCenter.subscribe(signals::TimingStep, &TestModule::onTimingStep, *this);
}

void TestModule::onLocalDomainInit() {
   // Pools
   _pool1 = _landUnitData->getPool(pool_1);
   _pool2 = _landUnitData->getPool(pool_2);
   _pool3 = _landUnitData->getPool(pool_3);

   // Variables
   _variable1 = _landUnitData->getVariable(variable_1);
   _variable2 = _landUnitData->getVariable(variable_2);
   _variable3 = _landUnitData->getVariable(variable_3);
}

void TestModule::onTimingInit() {}

void TestModule::onTimingStep() {
   auto operation = _landUnitData->createProportionalOperation();
   operation->addTransfer(_pool1, _pool2, ratio_1)
       ->addTransfer(_pool2, _pool3, ratio_2)
       ->addTransfer(_pool3, _pool1, ratio_3);

   _landUnitData->submitOperation(operation);
}

}  // namespace moja::flint
