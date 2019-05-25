#ifndef MOJA_FLINT_TESTMODULE1_H_
#define MOJA_FLINT_TESTMODULE1_H_

#include "moja/flint/ioperationresult.h"
#include "moja/flint/ipool.h"
#include "moja/flint/modulebase.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

class FLINT_API TestModule1 : public ModuleBase {
  public:
   // _pool1(nullptr), _pool2(nullptr), _pool3(nullptr), _variable1(nullptr), _variable2(nullptr), _variable3(nullptr)
   TestModule1() = default;
   virtual ~TestModule1() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onLocalDomainInit() override;
   void onTimingInit() override;
   void onTimingStep() override;

  private:
   // Pools
   const flint::IPool* _pool1;
   const flint::IPool* _pool2;
   const flint::IPool* _pool3;

   // Variables
   const flint::IVariable* _variable1;
   const flint::IVariable* _variable2;
   const flint::IVariable* _variable3;

   // Settings
   double ratio_1;
   double ratio_2;
   double ratio_3;
};

}  // namespace flint
}  // namespace moja
#endif  // MOJA_FLINT_TESTMODULE1_H_