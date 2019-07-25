#include "operationmanagertestutils.h"

#include "flinttests.h"

#include <moja/flint/ipool.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <memory>

// --------------------------------------------------------------------------------------------

struct PoolInitValueAndResult {
   std::string name;
   double value;
   double result;

   const mf::IPool* poolHandle;
};

// --------------------------------------------------------------------------------------------
void test_NoPoolIteration(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {};
   manager.initialisePools();

   int count = 0;
   for (auto& pool : manager.poolCollection()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 0);
   BOOST_CHECK_EQUAL(manager.poolCollection().size(), 0);

   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_SinglePoolIteration(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 90.00}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   int count = 0;
   for (auto& pool : manager.poolCollection()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 1);
   BOOST_CHECK_EQUAL(manager.poolCollection().size(), 1);

   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_MultiplePoolIteration(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {
       {"A", 100.0, 90.00}, {"B", 100.0, 90.00}, {"C", 100.0, 90.00}, {"D", 100.0, 90.00}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   int count = 0;
   for (auto& pool : manager.poolCollection()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 4);
   BOOST_CHECK_EQUAL(manager.poolCollection().size(), 4);

   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_NoResultIteration(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 0.0}, {"B", 100.0, 0.0}, {"C", 100.0, 0.0},
                                               {"D", 100.0, 0.0}, {"E", 100.0, 0.0}, {"F", 100.0, 0.0}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   int count = 0;
   for (auto p : manager.operationResultsPending()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 0);
   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 0);

   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_SingleResultIteration(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 0.0}, {"B", 100.0, 0.0}, {"C", 100.0, 0.0},
                                               {"D", 100.0, 0.0}, {"E", 100.0, 0.0}, {"F", 100.0, 0.0}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   int index = 0;
   auto A = data[index++].poolHandle;
   auto B = data[index++].poolHandle;
   auto C = data[index++].poolHandle;
   auto D = data[index++].poolHandle;
   auto E = data[index++].poolHandle;
   auto F = data[index++].poolHandle;

   auto op1 = manager.createProportionalOperation(module);
   op1->addTransfer(A, B, .13);
   op1->addTransfer(A, C, .13);
   op1->addTransfer(D, B, .13);
   op1->addTransfer(D, C, .13);
   op1->addTransfer(E, B, .13);
   op1->addTransfer(E, C, .13);
   op1->submitOperation();

   int count = 0;
   for (auto p : manager.operationResultsPending()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 1);
   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 1);

   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_MultipleResultIteration(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 0.0}, {"B", 100.0, 0.0}, {"C", 100.0, 0.0},
                                               {"D", 100.0, 0.0}, {"E", 100.0, 0.0}, {"F", 100.0, 0.0}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   int index = 0;
   auto A = data[index++].poolHandle;
   auto B = data[index++].poolHandle;
   auto C = data[index++].poolHandle;
   auto D = data[index++].poolHandle;
   auto E = data[index++].poolHandle;
   auto F = data[index++].poolHandle;

   auto op1 = manager.createProportionalOperation(module);
   op1->addTransfer(A, B, .13);
   op1->addTransfer(A, C, .13);
   op1->submitOperation();

   auto op2 = manager.createProportionalOperation(module);
   op2->addTransfer(D, B, .13);
   op2->addTransfer(D, C, .13);
   op2->submitOperation();

   auto op3 = manager.createProportionalOperation(module);
   op3->addTransfer(E, B, .13);
   op3->addTransfer(E, C, .13);
   op3->submitOperation();

   int count = 0;
   for (auto p : manager.operationResultsPending()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 3);
   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 3);

   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_NoResultFluxIteration(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 0.0}, {"B", 100.0, 0.0}, {"C", 100.0, 0.0},
                                               {"D", 100.0, 0.0}, {"E", 100.0, 0.0}, {"F", 100.0, 0.0}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   int index = 0;
   auto A = data[index++].poolHandle;
   auto B = data[index++].poolHandle;
   auto C = data[index++].poolHandle;
   auto D = data[index++].poolHandle;
   auto E = data[index++].poolHandle;
   auto F = data[index++].poolHandle;

   auto op1 = manager.createProportionalOperation(module);
   op1->addTransfer(A, B, 0.0);
   op1->submitOperation();

   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 1);

   // UBLAS will remove the zero result flux in the product, other implementations won't

   // if (manager.operationResultsPending().size() == 0)
   //	return;

   // auto operationResult = *(manager.operationResultsPending().begin());

   // std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') <<
   // testName << ": fluxes" << std::endl;

   // int count = 0;
   // for (auto p : operationResult->operationResultFluxCollection()) {
   //	std::cout << "Src: "<< p->source() << ", Snk: " << p->sink() << ", Val: " << p->value() << std::endl;
   //	count++;
   //}

   // BOOST_CHECK_EQUAL(count, 0);

   data.clear();
}

// --------------------------------------------------------------------------------------------
// the ResultFluxIterator - or the OperationResult doesn't guarantee that transfers from the same source and to the same
// sink will be combined. This means that checking the count won't be consistent across tests that have multiple
// transfers added to same source/sink combinations.

void test_SingleResultFluxIteration(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 0.0}, {"B", 100.0, 0.0}, {"C", 100.0, 0.0},
                                               {"D", 100.0, 0.0}, {"E", 100.0, 0.0}, {"F", 100.0, 0.0}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   int index = 0;
   auto A = data[index++].poolHandle;
   auto B = data[index++].poolHandle;
   auto C = data[index++].poolHandle;
   auto D = data[index++].poolHandle;
   auto E = data[index++].poolHandle;
   auto F = data[index++].poolHandle;

   auto op1 = manager.createProportionalOperation(module);
   op1->addTransfer(A, B, .13);
   op1->submitOperation();

   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 1);

   auto operationResult = *(manager.operationResultsPending().begin());

   int count = 0;
   for (auto p : operationResult->operationResultFluxCollection()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 1);

   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_MultipleResultFluxIteration(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 0.0}, {"B", 100.0, 0.0}, {"C", 100.0, 0.0},
                                               {"D", 100.0, 0.0}, {"E", 100.0, 0.0}, {"F", 100.0, 0.0}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   int index = 0;
   auto A = data[index++].poolHandle;
   auto B = data[index++].poolHandle;
   auto C = data[index++].poolHandle;
   auto D = data[index++].poolHandle;
   auto E = data[index++].poolHandle;
   auto F = data[index++].poolHandle;

   auto op1 = manager.createProportionalOperation(module);
   op1->addTransfer(A, B, .13);
   op1->addTransfer(A, C, .13);
   op1->addTransfer(D, B, .13);
   op1->addTransfer(D, C, .13);
   op1->addTransfer(E, B, .13);
   op1->addTransfer(E, C, .13);
   op1->submitOperation();

   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 1);

   auto operationResult = *(manager.operationResultsPending().begin());

   int count = 0;
   for (auto p : operationResult->operationResultFluxCollection()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 6);

   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_SingleProportionTransfer(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 90.00}, {"B", 50.0, 60.00}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;

   auto op1 = manager.createProportionalOperation(module);

   op1->addTransfer(A, B, .10);
   op1->submitOperation();

   manager.applyOperations();

   for (auto& p : data) {
      BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.000000001);
   }
   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_SingleStockTransfer(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 55.00}, {"B", 0.0, 45.00}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   // std::cout << testSuiteName << ": " << testName << ": Pool start values" << std::endl;
   // for (auto pool : manager.poolCollection()) {
   //	std::cout << pool->name() << " = " << std::setprecision(12) << pool->value() << std::endl;
   //}
   // std::cout << std::endl;

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, 45);
   op1->submitOperation();

   manager.applyOperations();

   // std::cout << testSuiteName << ": " << testName << ": Pool ending values" << std::endl;
   // for (auto pool : manager.poolCollection()) {
   //	std::cout << pool->name() << " = " << std::setprecision(12) << pool->value() << std::endl;
   //}
   // std::cout << std::endl;

   for (auto& p : data) {
      BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.000000001);
   }
   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_DoubleProportionalTransfer(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 0.0}, {"B", 0.0, 50.0}, {"C", 0.0, 50.0}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;
   auto C = data[2].poolHandle;

   auto op1 = manager.createProportionalOperation(module);

   op1->addTransfer(A, B, .50);
   op1->addTransfer(A, C, .50);
   op1->submitOperation();

   manager.applyOperations();

   for (auto& p : data) {
      BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.000000001);
   }
   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_DoubleStockTransfer(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 20.0}, {"B", 0.0, 45.0}, {"C", 0.0, 35.0}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;
   auto C = data[2].poolHandle;

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, 45);
   op1->addTransfer(A, C, 35);
   op1->submitOperation();

   manager.applyOperations();

   for (auto& p : data) {
      BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.000000001);
   }
   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_DoubleStockAndApplyTransfer(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {
       {"A", 100.0, 9.99}, {"B", 0.0, 50.00}, {"C", 0.0, 40.00}, {"D", 0.0, 0.010}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;
   auto C = data[2].poolHandle;
   auto D = data[3].poolHandle;

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, 45);
   op1->addTransfer(A, C, 35);
   op1->addTransfer(A, D, 0.001);
   op1->addTransfer(A, D, 0.001);
   op1->addTransfer(A, D, 0.001);
   op1->addTransfer(A, D, 0.001);
   op1->addTransfer(A, D, 0.001);
   op1->addTransfer(A, D, 0.001);
   op1->addTransfer(A, D, 0.001);
   op1->addTransfer(A, D, 0.001);
   op1->addTransfer(A, D, 0.001);
   op1->addTransfer(A, D, 0.001);
   op1->submitOperation();

   manager.applyOperations();

   auto op2 = manager.createStockOperation(module);

   op2->addTransfer(A, B, 5);
   op2->addTransfer(A, C, 5);
   op2->submitOperation();

   manager.applyOperations();

   for (auto& p : data) {
      BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.000000001);
   }
   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_TwoOperationsStockAndProportional(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {
       {"A", 100.0, 0.0}, {"B", 0.0, 50.0}, {"C", 0.0, 45.0}, {"D", 150.0, 150.0}, {"E", 0.0, 5.0}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;
   auto C = data[2].poolHandle;
   auto D = data[3].poolHandle;
   auto E = data[4].poolHandle;

   auto op1 = manager.createProportionalOperation(module);
   auto op2 = manager.createStockOperation(module);

   op1->addTransfer(A, B, .50);
   op1->addTransfer(A, C, .45);
   op2->addTransfer(A, E, 5.0);
   op1->submitOperation();
   op2->submitOperation();

   manager.applyOperations();

   for (auto& p : data) {
      BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.000000001);
   }
   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_Kahan_summation_issues_Proportion(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   // These values taken from a CBM move in spinup (SoftwoodStemSnag -> MediumSoil) in a Dist event
   //

   std::vector<PoolInitValueAndResult> data = {
       {"A", 21.440093961169907, 0.0}, {"B", 0.0, 10.7200469805849535}, {"C", 0.0, 10.7200469805849535}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;
   auto C = data[2].poolHandle;

   auto op1 = manager.createProportionalOperation(module);

   op1->addTransfer(A, B, .50);
   op1->addTransfer(A, C, .50);
   op1->submitOperation();

   manager.applyOperations();

   for (auto& p : data) {
      BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.000000001);
   }
   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_Kahan_summation_issues_Proportion_with_SpinUp(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   // These values taken from a CBM move in spinup (SoftwoodStemSnag -> MediumSoil) in a Dist event
   //

   std::vector<PoolInitValueAndResult> data = {
       {"A", 21.440093961169907, 0.0}, {"B", 0.0, 10.7200469805849535}, {"C", 0.0, 10.7200469805849535}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;
   auto C = data[2].poolHandle;

#if defined(_DEBUG)
   for (int i = 0; i < 1600; i++) {
#else
   for (int i = 0; i < 160000; i++) {
#endif
      auto op_spinup_1 = manager.createStockOperation(module);

      op_spinup_1->addTransfer(A, B, 1.11199966);
      op_spinup_1->submitOperation();
      manager.applyOperations();

      auto op_spinup_2 = manager.createStockOperation(module);

      op_spinup_2->addTransfer(B, A, 1.11199966);
      op_spinup_2->submitOperation();
      manager.applyOperations();

      auto op_spinup_3 = manager.createStockOperation(module);

      op_spinup_3->addTransfer(C, A, 33.34767543 + i / 2);
      op_spinup_3->submitOperation();
      manager.applyOperations();

      auto op_spinup_4 = manager.createProportionalOperation(module);

      op_spinup_4->addTransfer(A, B, 0.4514292);
      op_spinup_4->submitOperation();
      manager.applyOperations();
   }

   auto op1 = manager.createProportionalOperation(module);
   op1->addTransfer(A, B, 1.00);
   op1->submitOperation();
   manager.applyOperations();

   auto op2 = manager.createStockOperation(module);
   op2->addTransfer(C, A, 21.440093961169907);
   op2->submitOperation();
   manager.applyOperations();

   auto op3 = manager.createProportionalOperation(module);

   op3->addTransfer(A, B, .50);
   op3->addTransfer(A, C, .50);
   op3->submitOperation();
   manager.applyOperations();

   BOOST_CHECK_EQUAL(A->value(), 0.0);

   // for (auto& p : data) {
   //	BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.000000001);
   //}
   data.clear();
}

// --------------------------------------------------------------------------------------------
void test_PerformanceTestProportionalSLEEK(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

#if defined(_DEBUG)
#define TEST_LOOPS 1
#define TEST_STEPS (20 * 12)
#define TEST_MODS 10
#define TEST_VAL1 0.01
#define TEST_VAL2 0.005

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 100.0, 66.66666691607},  {"A2", 100.0, 133.33333308393}, {"A3", 100.0, 66.66666691607},
       {"A4", 100.0, 133.33333308393}, {"B1", 100.0, 66.66666691607},  {"B2", 100.0, 133.33333308393},
       {"B3", 100.0, 66.66666691607},  {"B4", 100.0, 133.33333308393}, {"C1", 100.0, 66.66666691607},
       {"C2", 100.0, 133.33333308393}, {"C3", 100.0, 66.66666691607},  {"C4", 100.0, 133.33333308393},
       {"D1", 150.0, 100.00000037410}, {"D2", 150.0, 199.99999962590}, {"D3", 150.0, 100.00000037410},
       {"D4", 150.0, 199.99999962590}, {"E1", 100.0, 66.66666691607},  {"E2", 100.0, 133.33333308393},
       {"E3", 100.0, 66.66666691607},  {"E4", 100.0, 133.33333308393}, {"F1", 100.0, 66.66666691607},
       {"F2", 100.0, 133.33333308393}, {"F3", 100.0, 66.66666691607},  {"F4", 100.0, 133.33333308393},
       {"G1", 100.0, 66.66666691607},  {"G2", 100.0, 133.33333308393}, {"G3", 100.0, 66.66666691607},
       {"G4", 100.0, 133.33333308393}, {"H1", 100.0, 66.66666691607},  {"H2", 100.0, 133.33333308393},
       {"H3", 100.0, 66.66666691607},  {"H4", 100.0, 133.33333308393}, {"I1", 100.0, 66.66666691607},
       {"I2", 100.0, 133.33333308393}, {"I3", 100.0, 66.66666691607},  {"I4", 100.0, 133.33333308393},
       {"J1", 100.0, 66.66666691607},  {"J2", 100.0, 133.33333308393}, {"J3", 100.0, 66.66666691607},
       {"J4", 100.0, 133.33333308393}};
#else
#define TEST_LOOPS 100
#define TEST_STEPS (50 * 12)
#define TEST_MODS 10
#define TEST_VAL1 0.01
#define TEST_VAL2 0.005

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 100.0, 66.6666666667},  {"A2", 100.0, 133.333333333},  {"A3", 100.0, 66.6666666667},
       {"A4", 100.0, 133.333333333},  {"B1", 100.0, 66.6666666667},  {"B2", 100.0, 133.333333333},
       {"B3", 100.0, 66.6666666667},  {"B4", 100.0, 133.333333333},  {"C1", 100.0, 66.6666666667},
       {"C2", 100.0, 133.333333333},  {"C3", 100.0, 66.6666666667},  {"C4", 100.0, 133.333333333},
       {"D1", 150.0, 100.0000000000}, {"D2", 150.0, 200.0000000000}, {"D3", 150.0, 100.0000000000},
       {"D4", 150.0, 200.0000000000}, {"E1", 100.0, 66.6666666667},  {"E2", 100.0, 133.333333333},
       {"E3", 100.0, 66.6666666667},  {"E4", 100.0, 133.333333333},  {"F1", 100.0, 66.6666666667},
       {"F2", 100.0, 133.333333333},  {"F3", 100.0, 66.6666666667},  {"F4", 100.0, 133.333333333},
       {"G1", 100.0, 66.6666666667},  {"G2", 100.0, 133.333333333},  {"G3", 100.0, 66.6666666667},
       {"G4", 100.0, 133.333333333},  {"H1", 100.0, 66.6666666667},  {"H2", 100.0, 133.333333333},
       {"H3", 100.0, 66.6666666667},  {"H4", 100.0, 133.333333333},  {"I1", 100.0, 66.6666666667},
       {"I2", 100.0, 133.333333333},  {"I3", 100.0, 66.6666666667},  {"I4", 100.0, 133.333333333},
       {"J1", 100.0, 66.6666666667},  {"J2", 100.0, 133.333333333},  {"J3", 100.0, 66.6666666667},
       {"J4", 100.0, 133.333333333}};
#endif

   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto operations = 0;
   auto applys = 0;
   auto transfers = 0;
   auto start = moja::DateTime::now();
   for (auto x = 0; x < TEST_LOOPS; x++) {
      for (auto step = 0; step < TEST_STEPS; step++) {
         for (auto m = 0; m < TEST_MODS; m++) {
            auto op1 = manager.createProportionalOperation(module);
            operations++;

            for (auto dataIdx = 0; dataIdx < data.size(); dataIdx += 2) {
               auto test = (m % 2) == 0;
               auto src = data[test ? dataIdx : dataIdx + 1].poolHandle;
               auto snk = data[test ? dataIdx + 1 : dataIdx].poolHandle;
               op1->addTransfer(src, snk, test ? TEST_VAL1 : TEST_VAL2);
               transfers++;
            }
            op1->submitOperation();
         }

         // std::cout << "OperationManagerSimple: TwoOperationsStockAndProportional: Pending:" << std::endl;
         // for (auto pending : manager.operationResultsPending()) {
         //	for (auto flux : pending->operationResultFluxCollection()) {
         //		std::cout << "type: " << OperationTransferTypeToString(flux->transferType()) << ", src: " <<
         //flux->source() << ", snk: " << flux->sink() << ", val: " << flux->value() << std::endl;
         //	}
         //}
         // std::cout << std::endl;

         manager.applyOperations();
         applys++;
      }
      manager.clearAllOperationResults();
   }
   auto finish = moja::DateTime::now();

   auto length = finish - start;
   std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ')
             << testName << ": Milliseconds : " << length.totalMilliseconds() << std::endl;
   // std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(40) << std::setfill(' ') <<
   // testName << ": operations   : " << operations << std::endl; std::cout << std::setw(40) << std::setfill(' ') <<
   // testSuiteName << ": " << std::setw(40) << std::setfill(' ') << testName << ": applys       : " << applys <<
   // std::endl; std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(40) <<
   // std::setfill(' ') << testName << ": transfers    : " << transfers << std::endl;

   // std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') <<
   // testName << ": Pool ending values:" << std::endl; for (auto pool : manager.poolCollection()) { 	std::cout <<
   //pool->name() << " = " << std::setprecision(12) << pool->value() << std::endl;
   //}
   // std::cout << std::endl;

   for (auto& p : data) {
      BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.000000001);
   }
   data.clear();

#undef TEST_LOOPS
#undef TEST_STEPS
#undef TEST_MODS
#undef TEST_VAL1
#undef TEST_VAL2
}

// --------------------------------------------------------------------------------------------
void test_PerformanceTestStockSLEEK(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

#if defined(_DEBUG)
#define TEST_LOOPS 1
#define TEST_STEPS (20 * 12)
#define TEST_MODS 10
#define TEST_VAL1 0.002
#define TEST_VAL2 0.001

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 10000000.0, 9999998.800000},  {"A2", 15000000.0, 15000001.200000}, {"A3", 10000000.0, 9999998.800000},
       {"A4", 15000000.0, 15000001.200000}, {"B1", 10000000.0, 9999998.800000},  {"B2", 15000000.0, 15000001.200000},
       {"B3", 10000000.0, 9999998.800000},  {"B4", 15000000.0, 15000001.200000}, {"C1", 10000000.0, 9999998.800000},
       {"C2", 15000000.0, 15000001.200000}, {"C3", 10000000.0, 9999998.800000},  {"C4", 15000000.0, 15000001.200000},
       {"D1", 10000000.0, 9999998.800000},  {"D2", 15000000.0, 15000001.200000}, {"D3", 10000000.0, 9999998.800000},
       {"D4", 15000000.0, 15000001.200000}, {"E1", 10000000.0, 9999998.800000},  {"E2", 15000000.0, 15000001.200000},
       {"E3", 10000000.0, 9999998.800000},  {"E4", 15000000.0, 15000001.200000}, {"F1", 10000000.0, 9999998.800000},
       {"F2", 15000000.0, 15000001.200000}, {"F3", 10000000.0, 9999998.800000},  {"F4", 15000000.0, 15000001.200000},
       {"G1", 10000000.0, 9999998.800000},  {"G2", 15000000.0, 15000001.200000}, {"G3", 10000000.0, 9999998.800000},
       {"G4", 15000000.0, 15000001.200000}, {"H1", 10000000.0, 9999998.800000},  {"H2", 15000000.0, 15000001.200000},
       {"H3", 10000000.0, 9999998.800000},  {"H4", 15000000.0, 15000001.200000}, {"I1", 10000000.0, 9999998.800000},
       {"I2", 15000000.0, 15000001.200000}, {"I3", 10000000.0, 9999998.800000},  {"I4", 15000000.0, 15000001.200000},
       {"J1", 10000000.0, 9999998.800000},  {"J2", 15000000.0, 15000001.200000}, {"J3", 10000000.0, 9999998.800000},
       {"J4", 15000000.0, 15000001.200000}};
#else
#if 1  // 100 loops
#define TEST_LOOPS 100
#define TEST_STEPS (50 * 12)
#define TEST_MODS 10
#define TEST_VAL1 0.002
#define TEST_VAL2 0.001

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 10000000.0, 9999700.000000000}, {"A2", 15000000.0, 15000300.000000000},
       {"A3", 10000000.0, 9999700.000000000}, {"A4", 15000000.0, 15000300.000000000},
       {"B1", 10000000.0, 9999700.000000000}, {"B2", 15000000.0, 15000300.000000000},
       {"B3", 10000000.0, 9999700.000000000}, {"B4", 15000000.0, 15000300.000000000},
       {"C1", 10000000.0, 9999700.000000000}, {"C2", 15000000.0, 15000300.000000000},
       {"C3", 10000000.0, 9999700.000000000}, {"C4", 15000000.0, 15000300.000000000},
       {"D1", 10000000.0, 9999700.000000000}, {"D2", 15000000.0, 15000300.000000000},
       {"D3", 10000000.0, 9999700.000000000}, {"D4", 15000000.0, 15000300.000000000},
       {"E1", 10000000.0, 9999700.000000000}, {"E2", 15000000.0, 15000300.000000000},
       {"E3", 10000000.0, 9999700.000000000}, {"E4", 15000000.0, 15000300.000000000},
       {"F1", 10000000.0, 9999700.000000000}, {"F2", 15000000.0, 15000300.000000000},
       {"F3", 10000000.0, 9999700.000000000}, {"F4", 15000000.0, 15000300.000000000},
       {"G1", 10000000.0, 9999700.000000000}, {"G2", 15000000.0, 15000300.000000000},
       {"G3", 10000000.0, 9999700.000000000}, {"G4", 15000000.0, 15000300.000000000},
       {"H1", 10000000.0, 9999700.000000000}, {"H2", 15000000.0, 15000300.000000000},
       {"H3", 10000000.0, 9999700.000000000}, {"H4", 15000000.0, 15000300.000000000},
       {"I1", 10000000.0, 9999700.000000000}, {"I2", 15000000.0, 15000300.000000000},
       {"I3", 10000000.0, 9999700.000000000}, {"I4", 15000000.0, 15000300.000000000},
       {"J1", 10000000.0, 9999700.000000000}, {"J2", 15000000.0, 15000300.000000000},
       {"J3", 10000000.0, 9999700.000000000}, {"J4", 15000000.0, 15000300.000000000}};
#else  // 10000 loops
#define TEST_LOOPS 10000
#define TEST_STEPS (50 * 12)
#define TEST_MODS 10
#define TEST_VAL1 0.002
#define TEST_VAL2 0.001

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 10000000.0, 9970000.000000000}, {"A2", 15000000.0, 15030000.000000000},
       {"A3", 10000000.0, 9970000.000000000}, {"A4", 15000000.0, 15030000.000000000},
       {"B1", 10000000.0, 9970000.000000000}, {"B2", 15000000.0, 15030000.000000000},
       {"B3", 10000000.0, 9970000.000000000}, {"B4", 15000000.0, 15030000.000000000},
       {"C1", 10000000.0, 9970000.000000000}, {"C2", 15000000.0, 15030000.000000000},
       {"C3", 10000000.0, 9970000.000000000}, {"C4", 15000000.0, 15030000.000000000},
       {"D1", 10000000.0, 9970000.000000000}, {"D2", 15000000.0, 15030000.000000000},
       {"D3", 10000000.0, 9970000.000000000}, {"D4", 15000000.0, 15030000.000000000},
       {"E1", 10000000.0, 9970000.000000000}, {"E2", 15000000.0, 15030000.000000000},
       {"E3", 10000000.0, 9970000.000000000}, {"E4", 15000000.0, 15030000.000000000},
       {"F1", 10000000.0, 9970000.000000000}, {"F2", 15000000.0, 15030000.000000000},
       {"F3", 10000000.0, 9970000.000000000}, {"F4", 15000000.0, 15030000.000000000},
       {"G1", 10000000.0, 9970000.000000000}, {"G2", 15000000.0, 15030000.000000000},
       {"G3", 10000000.0, 9970000.000000000}, {"G4", 15000000.0, 15030000.000000000},
       {"H1", 10000000.0, 9970000.000000000}, {"H2", 15000000.0, 15030000.000000000},
       {"H3", 10000000.0, 9970000.000000000}, {"H4", 15000000.0, 15030000.000000000},
       {"I1", 10000000.0, 9970000.000000000}, {"I2", 15000000.0, 15030000.000000000},
       {"I3", 10000000.0, 9970000.000000000}, {"I4", 15000000.0, 15030000.000000000},
       {"J1", 10000000.0, 9970000.000000000}, {"J2", 15000000.0, 15030000.000000000},
       {"J3", 10000000.0, 9970000.000000000}, {"J4", 15000000.0, 15030000.000000000}};
#endif
#endif
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto operations = 0;
   auto applys = 0;
   auto transfers = 0;
   auto start = moja::DateTime::now();
   for (auto x = 0; x < TEST_LOOPS; x++) {
      for (auto step = 0; step < TEST_STEPS; step++) {
         for (auto m = 0; m < TEST_MODS; m++) {
            auto op1 = manager.createStockOperation(module);
            operations++;

            for (auto dataIdx = 0; dataIdx < data.size(); dataIdx += 2) {
               auto test = (step % 2) == 0;
               auto src = data[test ? dataIdx : dataIdx + 1].poolHandle;
               auto snk = data[test ? dataIdx + 1 : dataIdx].poolHandle;
               op1->addTransfer(src, snk, test ? TEST_VAL1 : TEST_VAL2);
               transfers++;
            }
            op1->submitOperation();
         }

         // std::cout << testSuiteName << ": " << testName << ": Pending:" << std::endl;
         // for (auto pending : manager.operationResultsPending()) {
         //	for (auto flux : pending->operationResultFluxCollection()) {
         //		std::cout << "type: " << OperationTransferTypeToString(flux->transferType()) << ", src: " <<
         //flux->source() << ", snk: " << flux->sink() << ", val: " << flux->value() << std::endl;
         //	}
         //}
         // std::cout << std::endl;

         manager.applyOperations();
         applys++;
      }
      manager.clearAllOperationResults();
   }
   auto finish = moja::DateTime::now();

   auto length = finish - start;
   std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ')
             << testName << ": Milliseconds : " << length.totalMilliseconds() << std::endl;
   // std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(40) << std::setfill(' ') <<
   // testName << ": operations   : " << operations << std::endl; std::cout << std::setw(40) << std::setfill(' ') <<
   // testSuiteName << ": " << std::setw(40) << std::setfill(' ') << testName << ": applys       : " << applys <<
   // std::endl; std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(40) <<
   // std::setfill(' ') << testName << ": transfers    : " << transfers << std::endl;

   // std::cout << testSuiteName << ": " << testName << ": Pool ending values" << std::endl;
   // for (auto pool : manager.poolCollection()) {
   //	std::cout << pool->name() << " = " << std::setprecision(12) << pool->value() << std::endl;
   //}
   // std::cout << std::endl;

   // std::cout << testSuiteName << ": " << testName << ": Pool CHECK values" << std::endl;
   for (auto& p : data) {
      BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.0000000001);
      // std::cout << "Pool: " << std::setprecision(15) << p.poolHandle->value() << ", Result: " <<
      // std::setprecision(15) << p.result << ", diff: " << std::setprecision(15) << abs(p.poolHandle->value() -
      // p.result) << std::endl;
   }
   data.clear();

#undef TEST_LOOPS
#undef TEST_STEPS
#undef TEST_MODS
#undef TEST_VAL1
#undef TEST_VAL2
}

// --------------------------------------------------------------------------------------------
void test_PerformanceTestCBM(mf::IOperationManager& manager, mf::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

#if defined(_DEBUG)
#define TEST_LOOPS 1
#define TEST_STEPS (20 * 12)
#define TEST_MODS 10
#define TEST_VAL1 0.002
#define TEST_VAL2 0.001

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 100.0, 66.8896325746}, {"A2", 100.0, 133.110367425}, {"A3", 100.0, 66.8896325746},
       {"A4", 100.0, 133.110367425}, {"B1", 100.0, 66.8896325746}, {"B2", 100.0, 133.110367425},
       {"B3", 100.0, 66.8896325746}, {"B4", 100.0, 133.110367425}, {"C1", 100.0, 66.8896325746},
       {"C2", 100.0, 133.110367425}, {"C3", 100.0, 66.8896325746}, {"C4", 100.0, 133.110367425},
       {"D1", 150.0, 100.334448862}, {"D2", 150.0, 199.665551138}, {"D3", 150.0, 100.334448862},
       {"D4", 150.0, 199.665551138}, {"E1", 100.0, 66.8896325746}, {"E2", 100.0, 133.110367425},
       {"E3", 100.0, 66.8896325746}, {"E4", 100.0, 133.110367425}, {"F1", 100.0, 66.8896325746},
       {"F2", 100.0, 133.110367425}, {"F3", 100.0, 66.8896325746}, {"F4", 100.0, 133.110367425},
       {"G1", 100.0, 66.8896325746}, {"G2", 100.0, 133.110367425}, {"G3", 100.0, 66.8896325746},
       {"G4", 100.0, 133.110367425}, {"H1", 100.0, 66.8896325746}, {"H2", 100.0, 133.110367425},
       {"H3", 100.0, 66.8896325746}, {"H4", 100.0, 133.110367425}, {"I1", 100.0, 66.8896325746},
       {"I2", 100.0, 133.110367425}, {"I3", 100.0, 66.8896325746}, {"I4", 100.0, 133.110367425},
       {"J1", 100.0, 66.8896325746}, {"J2", 100.0, 133.110367425}, {"J3", 100.0, 66.8896325746},
       {"J4", 100.0, 133.110367425}};
#else
#define TEST_LOOPS 100
#define TEST_STEPS (50 * 12)
#define TEST_MODS 10
#define TEST_VAL1 0.002
#define TEST_VAL2 0.001

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 100.0, 66.889632107},  {"A2", 100.0, 133.110367893}, {"A3", 100.0, 66.889632107},
       {"A4", 100.0, 133.110367893}, {"B1", 100.0, 66.889632107},  {"B2", 100.0, 133.110367893},
       {"B3", 100.0, 66.889632107},  {"B4", 100.0, 133.110367893}, {"C1", 100.0, 66.889632107},
       {"C2", 100.0, 133.110367893}, {"C3", 100.0, 66.889632107},  {"C4", 100.0, 133.110367893},
       {"D1", 150.0, 100.334448161}, {"D2", 150.0, 199.665551839}, {"D3", 150.0, 100.334448161},
       {"D4", 150.0, 199.665551839}, {"E1", 100.0, 66.889632107},  {"E2", 100.0, 133.110367893},
       {"E3", 100.0, 66.889632107},  {"E4", 100.0, 133.110367893}, {"F1", 100.0, 66.889632107},
       {"F2", 100.0, 133.110367893}, {"F3", 100.0, 66.889632107},  {"F4", 100.0, 133.110367893},
       {"G1", 100.0, 66.889632107},  {"G2", 100.0, 133.110367893}, {"G3", 100.0, 66.889632107},
       {"G4", 100.0, 133.110367893}, {"H1", 100.0, 66.889632107},  {"H2", 100.0, 133.110367893},
       {"H3", 100.0, 66.889632107},  {"H4", 100.0, 133.110367893}, {"I1", 100.0, 66.889632107},
       {"I2", 100.0, 133.110367893}, {"I3", 100.0, 66.889632107},  {"I4", 100.0, 133.110367893},
       {"J1", 100.0, 66.889632107},  {"J2", 100.0, 133.110367893}, {"J3", 100.0, 66.889632107},
       {"J4", 100.0, 133.110367893}};
#endif

   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto operations = 0;
   auto applys = 0;
   auto transfers = 0;
   auto start = moja::DateTime::now();
   for (auto x = 0; x < TEST_LOOPS; x++) {
      for (auto step = 0; step < TEST_STEPS; step++) {
         for (auto m = 0; m < TEST_MODS; m++) {
            auto op1 = manager.createProportionalOperation(module);
            operations++;

            for (auto dataIdx = 0; dataIdx < data.size(); dataIdx += 2) {
               auto test = (m % 2) == 0;
               auto src = data[test ? dataIdx : dataIdx + 1].poolHandle;
               auto snk = data[test ? dataIdx + 1 : dataIdx].poolHandle;
               op1->addTransfer(src, snk, test ? .01 : .005);
               transfers++;
            }
            op1->submitOperation();
            manager.applyOperations();
            applys++;
         }

         // std::cout << "OperationManagerSimple: TwoOperationsStockAndProportional: Pending:" << std::endl;
         // for (auto pending : manager.operationResultsPending()) {
         //	for (auto flux : pending->operationResultFluxCollection()) {
         //		std::cout << "type: " << OperationTransferTypeToString(flux->transferType()) << ", src: " <<
         //flux->source() << ", snk: " << flux->sink() << ", val: " << flux->value() << std::endl;
         //	}
         //}
         // std::cout << std::endl;
      }
      manager.clearAllOperationResults();
   }
   auto finish = moja::DateTime::now();

   auto length = finish - start;
   std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ')
             << testName << ": Milliseconds : " << length.totalMilliseconds() << std::endl;
   // std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(40) << std::setfill(' ') <<
   // testName << ": operations   : " << operations << std::endl; std::cout << std::setw(40) << std::setfill(' ') <<
   // testSuiteName << ": " << std::setw(40) << std::setfill(' ') << testName << ": applys       : " << applys <<
   // std::endl; std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(40) <<
   // std::setfill(' ') << testName << ": transfers    : " << transfers << std::endl;

   // std::cout << testSuiteName << ": " << testName << ": Pool ending values" << std::endl;
   // for (auto pool : manager.poolCollection()) {
   //	std::cout << pool->name() << " = " << std::setprecision(12) << pool->value() << std::endl;
   //}
   // std::cout << std::endl;

   for (auto& p : data) {
      BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.000000001);
   }
   data.clear();

#undef TEST_LOOPS
#undef TEST_STEPS
#undef TEST_MODS
#undef TEST_VAL1
#undef TEST_VAL2
}

// --------------------------------------------------------------------------------------------

void SubmitOperationAddsToPendingQueue(mf::IOperationManager& manager, mf::IModule& module) {
#if 0
	auto opMocks = createOperation();
	controller.submitOperation(opMocks.operation);
	std::vector<mf::IOperationResult*> allPending;
	for (auto op : controller.operationManager()->operationResultsPending()) {
		allPending.push_back(op);
	}
	BOOST_CHECK_EQUAL(allPending.size(), 1);
#endif

   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 55.00}, {"B", 0.0, 45.00}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, 45);
   op1->submitOperation();

   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 1);

   data.clear();
}

// --------------------------------------------------------------------------------------------

void ClearLastAppliedOperationResults(mf::IOperationManager& manager, mf::IModule& module) {
#if 0
	BOOST_FIXTURE_TEST_CASE(flint_LandUnitController_ClearLastAppliedOperationResults, LandUnitControllerOperationTestsFixture) {
		auto opMocks = createOperation();
		opMocks.addTransfer(10.0);
		MOCK_EXPECT(opMocks.resultIterator->opBool).returns(false);
		controller.submitOperation(opMocks.operation);
		controller.applyOperations();
		controller.clearLastAppliedOperationResults();
		std::vector<std::shared_ptr<moja::flint::IOperationResult>> lastApplied;
		for (auto op : controller.operationManager()->operationResultsLastApplied()) {
			lastApplied.push_back(op);
		}
		BOOST_CHECK_EQUAL(lastApplied.size(), 0);
	}
#endif

   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 55.00}, {"B", 0.0, 45.00}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, 45);
   op1->submitOperation();
   manager.applyOperations();
   manager.clearLastAppliedOperationResults();

   BOOST_CHECK_EQUAL(manager.operationResultsLastApplied().size(), 0);

   data.clear();
}

// --------------------------------------------------------------------------------------------

void ApplyOperationsAppendsToCommittedQueue(mf::IOperationManager& manager, mf::IModule& module) {
#if 0
	BOOST_FIXTURE_TEST_CASE(flint_LandUnitController_ApplyOperationsAppendsToCommittedQueue, LandUnitControllerOperationTestsFixture) {
		// Operation 1
		auto op1Mocks = createOperation();
		double firstAmount = 10.0;
		op1Mocks.addTransfer(firstAmount);
		MOCK_EXPECT(op1Mocks.resultIterator->opBool).returns(false);
		controller.submitOperation(op1Mocks.operation);
		controller.applyOperations();

		// Operation 2
		auto op2Mocks = createOperation();
		double secondAmount = 25.0;
		double thirdAmount = 5.0;
		double fourthAmount = 1.0;
		op2Mocks.addTransfer(secondAmount);
		op2Mocks.addTransfer(thirdAmount);
		op2Mocks.addTransfer(fourthAmount);
		MOCK_EXPECT(op2Mocks.resultIterator->opBool).returns(false);
		controller.submitOperation(op2Mocks.operation);
		controller.applyOperations();

		std::vector<std::shared_ptr<moja::flint::IOperationResult>> committedOperations;
		for (auto op : controller.operationManager()->operationResultsCommitted()) {
			committedOperations.push_back(op);
		}

		BOOST_CHECK_EQUAL(committedOperations.size(), 2);
	}
#endif
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 1.00}, {"B", 0.0, 99.00}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, 45);
   op1->submitOperation();
   manager.applyOperations();

   auto op2 = manager.createStockOperation(module);
   op2->addTransfer(A, B, 35);
   op2->addTransfer(A, B, 4);
   op2->addTransfer(A, B, 5);
   op2->submitOperation();
   manager.applyOperations();

   manager.clearLastAppliedOperationResults();

   BOOST_CHECK_EQUAL(manager.operationResultsCommitted().size(), 2);

   data.clear();
}

void ApplyOperationsCorrectlyUpdatesPoolsForSimpleCase(mf::IOperationManager& manager, mf::IModule& module) {
#if 0
	auto opMocks = createOperation();
	double amountToMove = 40.0;
	opMocks.addTransfer(amountToMove);
	MOCK_EXPECT(opMocks.resultIterator->opBool).returns(false);
	controller.submitOperation(opMocks.operation);
	controller.applyOperations();

	auto p1 = controller.operationManager()->getPool("p1");
	auto p2 = controller.operationManager()->getPool("p2");
	BOOST_CHECK_EQUAL(p1->value(), p1InitialValue - amountToMove);
	BOOST_CHECK_EQUAL(p2->value(), p2InitialValue + amountToMove);
#endif

   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   double p1InitialValue = 100.00;
   double amountToMove = 40.0;
   std::vector<PoolInitValueAndResult> data = {{"A", p1InitialValue, (p1InitialValue - amountToMove)},
                                               {"B", 0.0, (0.0 + amountToMove)}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;
   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, amountToMove);
   op1->submitOperation();
   manager.applyOperations();

   for (auto& p : data) {
      BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.000000001);
   }
   data.clear();
}

void ApplyOperationsCorrectlyUpdatesPoolsForComplexCase(mf::IOperationManager& manager, mf::IModule& module) {
#if 0
	BOOST_FIXTURE_TEST_CASE(flint_LandUnitController_ApplyOperationsCorrectlyUpdatesPoolsForComplexCase, LandUnitControllerOperationTestsFixture) {
		auto opMocks = createOperation();
		double p1AmountToMove = 75.0;
		double p2AmountToMove = 25.0;
		opMocks.addTransfer(p1AmountToMove);
		opMocks.addTransfer(p2AmountToMove, true);
		MOCK_EXPECT(opMocks.resultIterator->opBool).returns(false);
		controller.submitOperation(opMocks.operation);
		controller.applyOperations();

		double p1Expected = p1InitialValue - p1AmountToMove + p2AmountToMove;
		double p2Expected = p2InitialValue + p1AmountToMove - p2AmountToMove;

		auto p1 = controller.operationManager()->getPool("p1");
		auto p2 = controller.operationManager()->getPool("p2");
		BOOST_CHECK_EQUAL(p1->value(), p1Expected);
		BOOST_CHECK_EQUAL(p2->value(), p2Expected);
#endif

   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   double p1InitialValue = 100.00;
   double p2InitialValue = 50.00;
   double p1AmountToMove = 75.0;
   double p2AmountToMove = 25.0;

   std::vector<PoolInitValueAndResult> data = {
       {"A", p1InitialValue, (p1InitialValue - p1AmountToMove + p2AmountToMove)},
       {"B", p2InitialValue, (p2InitialValue - p2AmountToMove + p1AmountToMove)}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;
   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, p1AmountToMove);
   op1->addTransfer(B, A, p2AmountToMove);

   op1->submitOperation();
   manager.applyOperations();

   for (auto& p : data) {
      BOOST_CHECK(abs(p.poolHandle->value() - p.result) < 0.000000001);
   }
   data.clear();
}

// --------------------------------------------------------------------------------------------

void ApplyAndGetOperationsLastApplied(mf::IOperationManager& manager, mf::IModule& module) {
#if 0
	BOOST_FIXTURE_TEST_CASE(flint_LandUnitController_ApplyAndGetOperationsLastApplied, LandUnitControllerOperationTestsFixture) {
		// Operation 1
		auto op1Mocks = createOperation();
		double firstAmount = 10.0;
		op1Mocks.addTransfer(firstAmount);
		MOCK_EXPECT(op1Mocks.resultIterator->opBool).returns(false);
		controller.submitOperation(op1Mocks.operation);
		controller.applyOperations();

		// Operation 2
		auto op2Mocks = createOperation();
		double secondAmount = 25.0;
		double thirdAmount = 5.0;
		double fourthAmount = 1.0;
		op2Mocks.addTransfer(secondAmount);
		op2Mocks.addTransfer(thirdAmount);
		op2Mocks.addTransfer(fourthAmount);
		MOCK_EXPECT(op2Mocks.resultIterator->opBool).returns(false);
		controller.submitOperation(op2Mocks.operation);
		controller.applyOperations();

		// Check that the last applied operation was operation 1 & 2 - lastApplied doesn't get cleared until TransactionManagerEndOfStepModule->onTimingPostStep 
		std::vector<std::shared_ptr<moja::flint::IOperationResult>> lastApplied;
		for (auto op : controller.operationManager()->operationResultsLastApplied()) {
			lastApplied.push_back(op);
		}
		BOOST_CHECK_EQUAL(lastApplied.size(), 2);
	}
#endif

   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 1.00}, {"B", 0.0, 99.00}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, 45);
   op1->submitOperation();
   manager.applyOperations();

   auto op2 = manager.createStockOperation(module);
   op2->addTransfer(A, B, 35);
   op2->addTransfer(A, B, 4);
   op2->addTransfer(A, B, 5);
   op2->submitOperation();
   manager.applyOperations();

   // Check that the last applied operation was operation 1 & 2 - lastApplied doesn't get cleared until
   // TransactionManagerEndOfStepModule->onTimingPostStep
   BOOST_CHECK_EQUAL(manager.operationResultsLastApplied().size(), 2);

   data.clear();
}

// --------------------------------------------------------------------------------------------

void ApplyOperationsClearsPendingQueue(mf::IOperationManager& manager, mf::IModule& module) {
#if 0
	BOOST_FIXTURE_TEST_CASE(flint_LandUnitController_ApplyOperationsClearsPendingQueue, LandUnitControllerOperationTestsFixture) {
		auto opMocks = createOperation();
		opMocks.addTransfer(10.0);
		MOCK_EXPECT(opMocks.resultIterator->opBool).returns(false);
		controller.submitOperation(opMocks.operation);
		controller.applyOperations();	// does the clear in here
		std::vector<mf::IOperationResult*> allPending;
		for (auto op : controller.operationManager()->operationResultsPending()) {
			allPending.push_back(op);
		}
		BOOST_CHECK_EQUAL(allPending.size(), 0);
	}
#endif

   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   double p1InitialValue = 100.00;
   double p2InitialValue = 50.00;
   double p1AmountToMove = 75.0;
   double p2AmountToMove = 25.0;

   std::vector<PoolInitValueAndResult> data = {
       {"A", p1InitialValue, (p1InitialValue - p1AmountToMove + p2AmountToMove)},
       {"B", p2InitialValue, (p2InitialValue - p2AmountToMove + p1AmountToMove)}};
   for (auto& p : data) {
      p.poolHandle = manager.addPool(p.name, p.value);
   }
   manager.initialisePools();

   auto A = data[0].poolHandle;
   auto B = data[1].poolHandle;
   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, p1AmountToMove);
   op1->addTransfer(B, A, p2AmountToMove);

   op1->submitOperation();
   manager.applyOperations();

   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 0);

   data.clear();
}
