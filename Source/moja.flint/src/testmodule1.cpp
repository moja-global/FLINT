#include "moja/flint/testmodule1.h"

#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ioperation.h"
#include "moja/flint/ivariable.h"

#include <moja/notificationcenter.h>
#include <moja/signals.h>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

void TestModule1::configure(const DynamicObject& config) {
   ratio_1 = 0.50;
   ratio_2 = 0.50;
   ratio_3 = 0.50;

   if (config.contains("ratio_1")) {
      ratio_1 = config["ratio_1"];
   }
   if (config.contains("ratio_2")) {
      ratio_3 = config["ratio_2"];
   }
   if (config.contains("ratio_3")) {
      ratio_3 = config["ratio_3"];
   }
}

// --------------------------------------------------------------------------------------------

void TestModule1::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::LocalDomainInit, &TestModule1::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::TimingInit, &TestModule1::onTimingInit, *this);
   notificationCenter.subscribe(signals::TimingStep, &TestModule1::onTimingStep, *this);
}

void TestModule1::onLocalDomainInit() {
   // Pools
   _pool1 = _landUnitData->getPool("Pool 1");
   _pool2 = _landUnitData->getPool("Pool 2");
   _pool3 = _landUnitData->getPool("Pool 3");

   // Variables
   _variable1 = _landUnitData->getVariable("variable 1");
   _variable2 = _landUnitData->getVariable("variable 2");
   _variable3 = _landUnitData->getVariable("variable 3");
}

void TestModule1::onTimingInit() {}

void TestModule1::onTimingStep() {
   auto operation = _landUnitData->createProportionalOperation();
   operation->addTransfer(_pool1, _pool2, ratio_1)
       ->addTransfer(_pool2, _pool3, ratio_2)
       ->addTransfer(_pool3, _pool1, ratio_3);

   _landUnitData->submitOperation(operation);
}

}  // namespace flint
}  // namespace moja
