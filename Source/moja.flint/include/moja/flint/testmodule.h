#pragma once

#include "moja/flint/ipool.h"
#include "moja/flint/modulebase.h"

namespace moja::flint {

class FLINT_API TestModule : public ModuleBase {
  public:
   TestModule() = default;
   virtual ~TestModule() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onLocalDomainInit() override;
   void onTimingInit() override;
   void onTimingStep() override;

  private:
   // Pools
   const IPool* _pool1;
   const IPool* _pool2;
   const IPool* _pool3;

   // Variables
   const IVariable* _variable1;
   const IVariable* _variable2;
   const IVariable* _variable3;

   // Settings
   double ratio_1;
   double ratio_2;
   double ratio_3;

   std::string variable_1;
   std::string variable_2;
   std::string variable_3;

   std::string pool_1;
   std::string pool_2;
   std::string pool_3;
};

}  // namespace moja::flint
