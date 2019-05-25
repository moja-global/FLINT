#include <moja/flint/ioperationmanager.h>
#include <moja/flint/landunitcontroller.h>

#include <moja/test/mockoperation.h>
#include <moja/test/mockoperationmanager.h>
#include <moja/test/mockoperationresult.h>
#include <moja/test/mockoperationresultflux.h>
#include <moja/test/mockoperationresultfluxcollection.h>
#include <moja/test/mockoperationresultfluxiterator.h>

namespace mf = moja::flint;
namespace conf = moja::flint::configuration;
namespace mocks = moja::test;

// --------------------------------------------------------------------------------------------

struct UtilsClass {
   // mf::LandUnitController controller;
   double p1InitialValue = 100.0;
   double p2InitialValue = 50.0;

   mf::ModuleMetaData metadata;
   mf::IOperationManager& _operationManager;

   struct MockOperationBundle {
      std::shared_ptr<mocks::MockOperationManager> operationManager;
      std::shared_ptr<mocks::MockOperation> operation;
      std::shared_ptr<mocks::MockOperationResult> result;
      std::shared_ptr<mocks::MockOperationResultFlux> resultFlux;
      // std::shared_ptr<mocks::MockOperationResultCollection> resultCollection;
      std::shared_ptr<mocks::MockOperationResultFluxIterator> resultFluxIterator;
      std::shared_ptr<mocks::MockOperationResultFluxCollection> resultFluxCollection;

      // Set up a transfer of <amount> from p1 to p2 (or p2 to p1, if reverse = true).
      void addTransfer(double amount, bool reverse = false) {
         // MOCK_EXPECT(resultFluxIterator
         MOCK_EXPECT(resultFlux->source).once().returns(reverse ? 1 : 0);
         MOCK_EXPECT(resultFlux->sink).once().returns(reverse ? 0 : 1);
         MOCK_EXPECT(resultFlux->value).once().returns(amount);

         // MOCK_EXPECT(resultIterator->opBool).once().returns(true);
         // MOCK_EXPECT(resultIterator->row).once().returns(reverse ? 1 : 0);
         // MOCK_EXPECT(resultIterator->col).once().returns(reverse ? 0 : 1);
         // MOCK_EXPECT(resultIterator->value).once().returns(amount);
      }
   };

   UtilsClass(mf::IOperationManager& operationManager) : _operationManager(operationManager) {
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
      auto operationManager = std::make_shared<mocks::MockOperationManager>();
      auto operation = std::make_shared<mocks::MockOperation>(&metadata);
      auto operationResult = std::make_shared<mocks::MockOperationResult>();
      auto operationResultFlux = std::make_shared<mocks::MockOperationResultFlux>();
      // auto operationResultCollection = std::make_shared<mocks::MockOperationResultCollection>();
      auto operationResultFluxIterator = std::make_shared<mocks::MockOperationResultFluxIterator>();
      auto operationResultFluxCollection =
          std::make_shared<mocks::MockOperationResultFluxCollection>(operationResultFluxIterator);

      MOCK_EXPECT(operation->computeOperation).returns(operationResult);

      // MOCK_EXPECT(operationManager->operationResultsPending).returns(operationResultCollection);
      // MOCK_EXPECT(operationResultCollection->beginNormal).returns(operationResultIterator);
      // MOCK_EXPECT(operationResultCollection->endNormal).returns(operationResultIterator);

      MOCK_EXPECT(operationResult->operationResultFluxCollection).returns(*(operationResultFluxCollection.get()));
      // MOCK_EXPECT(operationResultFluxCollection->beginNormal).returns(operationResultFluxIterator);
      // MOCK_EXPECT(operationResultFluxCollection->endNormal).returns(operationResultFluxIterator);

      // MOCK_EXPECT(operationResultIterator->dereference).returns(operationResult.get());

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

void test_Kahan_summation_issues_Proportion(mf::IOperationManager& manager, mf::IModule& module);
void test_Kahan_summation_issues_Proportion_with_SpinUp(mf::IOperationManager& manager, mf::IModule& module);

void test_PerformanceTestProportionalSLEEK(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_PerformanceTestStockSLEEK(moja::flint::IOperationManager& manager, moja::flint::IModule& module);
void test_PerformanceTestCBM(moja::flint::IOperationManager& manager, moja::flint::IModule& module);

void SubmitOperationAddsToPendingQueue(mf::IOperationManager& manager, mf::IModule& module);
void ClearLastAppliedOperationResults(mf::IOperationManager& manager, mf::IModule& module);
void ApplyOperationsAppendsToCommittedQueue(mf::IOperationManager& manager, mf::IModule& module);

void ApplyOperationsCorrectlyUpdatesPoolsForSimpleCase(mf::IOperationManager& manager, mf::IModule& module);
void ApplyOperationsCorrectlyUpdatesPoolsForComplexCase(mf::IOperationManager& manager, mf::IModule& module);

void ApplyAndGetOperationsLastApplied(mf::IOperationManager& manager, mf::IModule& module);
void ApplyOperationsClearsPendingQueue(mf::IOperationManager& manager, mf::IModule& module);
