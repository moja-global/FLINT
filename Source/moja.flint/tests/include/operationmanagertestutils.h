#include <moja/flint/ioperationmanager.h>
#include <moja/flint/landunitcontroller.h>
#include <moja/flint/modulebase.h>

#include <moja/test/mockoperation.h>
#include <moja/test/mockoperationmanager.h>
#include <moja/test/mockoperationresult.h>
#include <moja/test/mockoperationresultcollection.h>
#include <moja/test/mockoperationresultflux.h>
#include <moja/test/mockoperationresultfluxcollection.h>
#include <moja/test/mockoperationresultfluxiterator.h>

#include <mock.hpp>

// --------------------------------------------------------------------------------------------

struct UtilsClass {
   
   double p1InitialValue = 100.0;
   double p2InitialValue = 50.0;

   moja::flint::ModuleMetaData metadata;
   moja::flint::IOperationManager& _operationManager;

   struct MockOperationBundle {
      std::shared_ptr<moja::test::MockOperationManager> operationManager;
      std::shared_ptr<moja::test::MockOperation> operation;
      std::shared_ptr<moja::test::MockOperationResult> result;
      std::shared_ptr<moja::test::MockOperationResultFlux> resultFlux;
      std::shared_ptr<moja::test::MockOperationResultFluxIterator> resultFluxIterator;
      std::shared_ptr<moja::test::MockOperationResultFluxCollection> resultFluxCollection;

      // Set up a transfer of <amount> from p1 to p2 (or p2 to p1, if reverse = true).
      void addTransfer(double amount, bool reverse = false) {
         MOCK_EXPECT(resultFlux->source).once().returns(reverse ? 1 : 0);
         MOCK_EXPECT(resultFlux->sink).once().returns(reverse ? 0 : 1);
         MOCK_EXPECT(resultFlux->value).once().returns(amount);
      }
   };

   UtilsClass(moja::flint::IOperationManager& operationManager) : _operationManager(operationManager) {
      // Set up a pool structure that looks like this in an operation matrix:
      // From |To:    p1    p2
      // p1   |
      // p2   |

      // controller.operationManager()->addPool("p1", p1InitialValue);
      // controller.operationManager()->addPool("p2", p2InitialValue);
      // controller.initialiseData();

      metadata.setDefaults();
      metadata.moduleName = "UtilsClass";
   }

   MockOperationBundle createOperation() {
      auto operationManager = std::make_shared<moja::test::MockOperationManager>();
      auto operation = std::make_shared<moja::test::MockOperation>(&metadata);
      auto operationResult = std::make_shared<moja::test::MockOperationResult>();
      auto operationResultFlux = std::make_shared<moja::test::MockOperationResultFlux>();
      auto operationResultFluxIterator = std::make_shared<moja::test::MockOperationResultFluxIterator>();
      auto operationResultFluxCollection =
          std::make_shared<moja::test::MockOperationResultFluxCollection>(operationResultFluxIterator);

      MOCK_EXPECT(operation->computeOperation).returns(operationResult);
      MOCK_EXPECT(operationResult->operationResultFluxCollection).returns(*(operationResultFluxCollection.get()));

      return MockOperationBundle{operationManager,
                                 operation,
                                 operationResult,
                                 operationResultFlux,
                                 operationResultFluxIterator,
                                 operationResultFluxCollection};
   }
};

// --------------------------------------------------------------------------------------------

void test_NoPoolIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_SinglePoolIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_MultiplePoolIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module);

void test_NoResultIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_SingleResultIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_MultipleResultIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module);

void test_NoResultFluxIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_SingleResultFluxIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_MultipleResultFluxIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module);

void test_SingleProportionTransfer(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_SingleStockTransfer(moja::flint::IOperationManager& manager, moja::flint::IModule& module);

void test_DoubleProportionalTransfer(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_DoubleStockTransfer(moja::flint::IOperationManager& manager, moja::flint::IModule& module);

void test_DoubleStockAndApplyTransfer(moja::flint::IOperationManager& manager, moja::flint::IModule& module);

void test_TwoOperationsStockAndProportional(moja::flint::IOperationManager& manager, moja::flint::IModule& module);

void test_Kahan_summation_issues_Proportion(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_Kahan_summation_issues_Proportion_with_SpinUp(moja::flint::IOperationManager& manager,
                                                        moja::flint::IModule& module);

void test_PerformanceTestProportionalSLEEK(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_PerformanceTestStockSLEEK(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_PerformanceTestCBM(moja::flint::IOperationManager& manager, moja::flint::IModule& module);

void SubmitOperationAddsToPendingQueue(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void ClearLastAppliedOperationResults(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void ApplyOperationsAppendsToCommittedQueue(moja::flint::IOperationManager& manager, moja::flint::IModule& module);

void ApplyOperationsCorrectlyUpdatesPoolsForSimpleCase(moja::flint::IOperationManager& manager,
                                                       moja::flint::IModule& module);
void ApplyOperationsCorrectlyUpdatesPoolsForComplexCase(moja::flint::IOperationManager& manager,
                                                        moja::flint::IModule& module);

void ApplyAndGetOperationsLastApplied(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void ApplyOperationsClearsPendingQueue(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
