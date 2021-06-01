#include "operationmanagertestutils.h"

#include <moja/flint/ioperation.h>
#include <moja/flint/ioperationresult.h>
#include <moja/flint/ipool.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

#include <memory>

struct PoolInitValueAndResult {
   std::string name;
   double value;
   double result;

   const moja::flint::IPool* pool;
};

constexpr auto pct_tolerance = 0.0000001;

void test_NoPoolIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   manager.initialisePools();

   int count = 0;
   for (auto& pool : manager.poolCollection()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 0);
   BOOST_CHECK_EQUAL(manager.poolCollection().size(), 0);
}

void test_SinglePoolIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   manager.addPool("A", "", "", 1.0, 1, 100, nullptr);
   manager.initialisePools();

   int count = 0;
   for (auto& pool : manager.poolCollection()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 1);
   BOOST_CHECK_EQUAL(manager.poolCollection().size(), 1);
}

void test_MultiplePoolIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<std::tuple<std::string, double>> data = {{"A", 100.0}, {"B", 100.0}, {"C", 100.0}, {"D", 100.0}};
   for (auto& [name, init_value] : data) {
      manager.addPool(name, "", "", 1.0, 1, init_value, nullptr);
   }
   manager.initialisePools();

   int count = 0;
   for (auto& pool : manager.poolCollection()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 4);
   BOOST_CHECK_EQUAL(manager.poolCollection().size(), 4);
}

void test_NoResultIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<std::tuple<std::string, double>> data = {{"A", 100.0}, {"B", 100.0}, {"C", 100.0},
                                                        {"D", 100.0}, {"E", 100.0}, {"F", 100.0}};
   for (auto& [name, value] : data) {
      manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   int count = 0;
   for (auto p : manager.operationResultsPending()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 0);
   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 0);
}

void test_SingleResultIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   const auto* A = manager.addPool("A", "", "", 1.0, 1, 100.0, nullptr);
   const auto* B = manager.addPool("B", "", "", 1.0, 1, 100.0, nullptr);
   const auto* C = manager.addPool("C", "", "", 1.0, 1, 100.0, nullptr);
   const auto* D = manager.addPool("D", "", "", 1.0, 1, 100.0, nullptr);
   const auto* E = manager.addPool("E", "", "", 1.0, 1, 100.0, nullptr);

   manager.initialisePools();

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
}

void test_MultipleResultIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   const auto* A = manager.addPool("A", "", "", 1.0, 1, 100.0, nullptr);
   const auto* B = manager.addPool("B", "", "", 1.0, 1, 100.0, nullptr);
   const auto* C = manager.addPool("C", "", "", 1.0, 1, 100.0, nullptr);
   const auto* D = manager.addPool("D", "", "", 1.0, 1, 100.0, nullptr);
   const auto* E = manager.addPool("E", "", "", 1.0, 1, 100.0, nullptr);

   manager.initialisePools();

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
}

void test_NoResultFluxIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   const auto* A = manager.addPool("A", "", "", 1.0, 1, 100.0, nullptr);
   const auto* B = manager.addPool("B", "", "", 1.0, 1, 100.0, nullptr);

   manager.initialisePools();

   auto op1 = manager.createProportionalOperation(module);
   op1->addTransfer(A, B, 0.0);
   op1->submitOperation();

   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 1);
}

// the ResultFluxIterator - or the OperationResult doesn't guarantee that transfers from the same source and to the same
// sink will be combined. This means that checking the count won't be consistent across tests that have multiple
// transfers added to same source/sink combinations.

void test_SingleResultFluxIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   const auto* A = manager.addPool("A", "", "", 1.0, 1, 100.0, nullptr);
   const auto* B = manager.addPool("B", "", "", 1.0, 1, 100.0, nullptr);

   manager.initialisePools();

   auto op1 = manager.createProportionalOperation(module);
   op1->addTransfer(A, B, .13);
   op1->submitOperation();

   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 1);

   auto operationResult = *(manager.operationResultsPending().begin());

   int count = 0;
   for (auto* p : operationResult->operationResultFluxCollection()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 1);
}

void test_MultipleResultFluxIteration(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   const auto* A = manager.addPool("A", "", "", 1.0, 1, 100.0, nullptr);
   const auto* B = manager.addPool("B", "", "", 1.0, 1, 100.0, nullptr);
   const auto* C = manager.addPool("C", "", "", 1.0, 1, 100.0, nullptr);
   const auto* D = manager.addPool("D", "", "", 1.0, 1, 100.0, nullptr);
   const auto* E = manager.addPool("E", "", "", 1.0, 1, 100.0, nullptr);

   manager.initialisePools();

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
   for (auto* p : operationResult->operationResultFluxCollection()) {
      count++;
   }

   BOOST_CHECK_EQUAL(count, 6);
}

void test_SingleProportionTransfer(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 90.00, nullptr}, {"B", 50.0, 60.00, nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;

   auto op1 = manager.createProportionalOperation(module);

   op1->addTransfer(A, B, .10);
   op1->submitOperation();

   manager.applyOperations();

   for (auto& [name, value, result, pool] : data) {
      BOOST_CHECK_CLOSE(pool->value(), result, pct_tolerance);
   }
   data.clear();
}

void test_SingleStockTransfer(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 55.00, nullptr}, {"B", 0.0, 45.00, nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, 45);
   op1->submitOperation();

   manager.applyOperations();

   for (auto& [name, value, result, pool] : data) {
      BOOST_CHECK_CLOSE(pool->value(), result, pct_tolerance);
   }
}

void test_DoubleProportionalTransfer(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {
       {"A", 100.0, 0.0, nullptr}, {"B", 0.0, 50.0, nullptr}, {"C", 0.0, 50.0, nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;
   const auto* C = data[2].pool;

   auto op1 = manager.createProportionalOperation(module);

   op1->addTransfer(A, B, .50);
   op1->addTransfer(A, C, .50);
   op1->submitOperation();

   manager.applyOperations();

   for (auto& [name, value, result, pool] : data) {
      BOOST_CHECK_CLOSE(pool->value(), result, pct_tolerance);
   }
}

void test_DoubleStockTransfer(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {
       {"A", 100.0, 20.0, nullptr}, {"B", 0.0, 45.0, nullptr}, {"C", 0.0, 35.0, nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;
   const auto* C = data[2].pool;

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, 45);
   op1->addTransfer(A, C, 35);
   op1->submitOperation();

   manager.applyOperations();

   for (auto& [name, value, result, pool] : data) {
      BOOST_CHECK_CLOSE(pool->value(), result, pct_tolerance);
   }
}

void test_DoubleStockAndApplyTransfer(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {
       {"A", 100.0, 9.99, nullptr}, {"B", 0.0, 50.00, nullptr}, {"C", 0.0, 40.00, nullptr}, {"D", 0.0, 0.010, nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;
   const auto* C = data[2].pool;
   const auto* D = data[3].pool;

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

   for (auto& [name, value, result, pool] : data) {
      BOOST_CHECK_CLOSE(pool->value(), result, pct_tolerance);
   }
}

void test_TwoOperationsStockAndProportional(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 0.0, nullptr},
                                               {"B", 0.0, 50.0, nullptr},
                                               {"C", 0.0, 45.0, nullptr},
                                               {"D", 150.0, 150.0, nullptr},
                                               {"E", 0.0, 5.0, nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;
   const auto* C = data[2].pool;
   const auto* D = data[3].pool;
   const auto* E = data[4].pool;

   auto op1 = manager.createProportionalOperation(module);
   auto op2 = manager.createStockOperation(module);

   op1->addTransfer(A, B, .50);
   op1->addTransfer(A, C, .45);
   op2->addTransfer(A, E, 5.0);
   op1->submitOperation();
   op2->submitOperation();

   manager.applyOperations();

   for (auto& [name, value, result, pool] : data) {
      BOOST_CHECK_CLOSE(pool->value(), result, pct_tolerance);
   }
}

void test_Kahan_summation_issues_Proportion(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   // These values taken from a CBM move in spinup (SoftwoodStemSnag -> MediumSoil) in a Dist event
   std::vector<PoolInitValueAndResult> data = {{"A", 21.440093961169907, 0.0, nullptr},
                                               {"B", 0.0, 10.7200469805849535, nullptr},
                                               {"C", 0.0, 10.7200469805849535, nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;
   const auto* C = data[2].pool;

   auto op1 = manager.createProportionalOperation(module);

   op1->addTransfer(A, B, .50);
   op1->addTransfer(A, C, .50);
   op1->submitOperation();

   manager.applyOperations();

   for (auto& [name, value, result, pool] : data) {
      BOOST_CHECK_CLOSE(pool->value(), result, pct_tolerance);
   }
}

void test_Kahan_summation_issues_Proportion_with_SpinUp(moja::flint::IOperationManager& manager,
                                                        moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   // These values taken from a CBM move in spinup (SoftwoodStemSnag -> MediumSoil) in a Dist event
   std::vector<PoolInitValueAndResult> data = {{"A", 21.440093961169907, 0.0, nullptr},
                                               {"B", 0.0, 10.7200469805849535, nullptr},
                                               {"C", 0.0, 10.7200469805849535, nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;
   const auto* C = data[2].pool;

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

   BOOST_CHECK_CLOSE(A->value(), 0.0, pct_tolerance);
}

void test_PerformanceTestProportionalSLEEK(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

#if defined(_DEBUG)
   constexpr int test_loops = 1;
   constexpr int test_steps = (20 * 12);
   constexpr int test_mods = 10;
   constexpr double test_val1 = 0.01;
   constexpr double test_val2 = 0.005;

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 100.0, 66.66666691607, nullptr}, {"A2", 100.0, 133.33333308393, nullptr},
       {"A3", 100.0, 66.66666691607, nullptr}, {"A4", 100.0, 133.33333308393, nullptr},
       {"B1", 100.0, 66.66666691607, nullptr}, {"B2", 100.0, 133.33333308393, nullptr},
       {"B3", 100.0, 66.66666691607, nullptr}, {"B4", 100.0, 133.33333308393, nullptr},
       {"C1", 100.0, 66.66666691607, nullptr}, {"C2", 100.0, 133.33333308393, nullptr},
       {"C3", 100.0, 66.66666691607, nullptr}, {"C4", 100.0, 133.33333308393, nullptr},
       {"D1", 150.0, 100.0000003741, nullptr}, {"D2", 150.0, 199.99999962590, nullptr},
       {"D3", 150.0, 100.0000003741, nullptr}, {"D4", 150.0, 199.99999962590, nullptr},
       {"E1", 100.0, 66.66666691607, nullptr}, {"E2", 100.0, 133.33333308393, nullptr},
       {"E3", 100.0, 66.66666691607, nullptr}, {"E4", 100.0, 133.33333308393, nullptr},
       {"F1", 100.0, 66.66666691607, nullptr}, {"F2", 100.0, 133.33333308393, nullptr},
       {"F3", 100.0, 66.66666691607, nullptr}, {"F4", 100.0, 133.33333308393, nullptr},
       {"G1", 100.0, 66.66666691607, nullptr}, {"G2", 100.0, 133.33333308393, nullptr},
       {"G3", 100.0, 66.66666691607, nullptr}, {"G4", 100.0, 133.33333308393, nullptr},
       {"H1", 100.0, 66.66666691607, nullptr}, {"H2", 100.0, 133.33333308393, nullptr},
       {"H3", 100.0, 66.66666691607, nullptr}, {"H4", 100.0, 133.33333308393, nullptr},
       {"I1", 100.0, 66.66666691607, nullptr}, {"I2", 100.0, 133.33333308393, nullptr},
       {"I3", 100.0, 66.66666691607, nullptr}, {"I4", 100.0, 133.33333308393, nullptr},
       {"J1", 100.0, 66.66666691607, nullptr}, {"J2", 100.0, 133.33333308393, nullptr},
       {"J3", 100.0, 66.66666691607, nullptr}, {"J4", 100.0, 133.33333308393, nullptr}};
#else
   constexpr int test_loops = 100;
   constexpr int test_steps = (50 * 12);
   constexpr int test_mods = 10;
   constexpr double test_val1 = 0.01;
   constexpr double test_val2 = 0.005;

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 100.0, 66.6666666667, nullptr}, {"A2", 100.0, 133.333333333, nullptr},
       {"A3", 100.0, 66.6666666667, nullptr}, {"A4", 100.0, 133.333333333, nullptr},
       {"B1", 100.0, 66.6666666667, nullptr}, {"B2", 100.0, 133.333333333, nullptr},
       {"B3", 100.0, 66.6666666667, nullptr}, {"B4", 100.0, 133.333333333, nullptr},
       {"C1", 100.0, 66.6666666667, nullptr}, {"C2", 100.0, 133.333333333, nullptr},
       {"C3", 100.0, 66.6666666667, nullptr}, {"C4", 100.0, 133.333333333, nullptr},
       {"D1", 150.0, 100.000000000, nullptr}, {"D2", 150.0, 200.000000000, nullptr},
       {"D3", 150.0, 100.000000000, nullptr}, {"D4", 150.0, 200.000000000, nullptr},
       {"E1", 100.0, 66.6666666667, nullptr}, {"E2", 100.0, 133.333333333, nullptr},
       {"E3", 100.0, 66.6666666667, nullptr}, {"E4", 100.0, 133.333333333, nullptr},
       {"F1", 100.0, 66.6666666667, nullptr}, {"F2", 100.0, 133.333333333, nullptr},
       {"F3", 100.0, 66.6666666667, nullptr}, {"F4", 100.0, 133.333333333, nullptr},
       {"G1", 100.0, 66.6666666667, nullptr}, {"G2", 100.0, 133.333333333, nullptr},
       {"G3", 100.0, 66.6666666667, nullptr}, {"G4", 100.0, 133.333333333, nullptr},
       {"H1", 100.0, 66.6666666667, nullptr}, {"H2", 100.0, 133.333333333, nullptr},
       {"H3", 100.0, 66.6666666667, nullptr}, {"H4", 100.0, 133.333333333, nullptr},
       {"I1", 100.0, 66.6666666667, nullptr}, {"I2", 100.0, 133.333333333, nullptr},
       {"I3", 100.0, 66.6666666667, nullptr}, {"I4", 100.0, 133.333333333, nullptr},
       {"J1", 100.0, 66.6666666667, nullptr}, {"J2", 100.0, 133.333333333, nullptr},
       {"J3", 100.0, 66.6666666667, nullptr}, {"J4", 100.0, 133.333333333, nullptr}};
#endif

   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   for (auto x = 0; x < test_loops; x++) {
      for (auto step = 0; step < test_steps; step++) {
         for (auto m = 0; m < test_mods; m++) {
            auto op1 = manager.createProportionalOperation(module);
            for (auto dataIdx = 0; dataIdx < data.size(); dataIdx += 2) {
               const auto test = (m % 2) == 0;
               const auto* src = data[test ? dataIdx : dataIdx + 1].pool;
               const auto* snk = data[test ? dataIdx + 1 : dataIdx].pool;
               op1->addTransfer(src, snk, test ? test_val1 : test_val2);
            }
            op1->submitOperation();
         }
         manager.applyOperations();
      }
      manager.clearAllOperationResults();
   }

   for (auto& [name, value, result, pool] : data) {
      BOOST_CHECK_CLOSE(pool->value(), result, pct_tolerance);
   }
}

void test_PerformanceTestStockSLEEK(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

#if defined(_DEBUG)
   constexpr int test_loops = 1;
   constexpr int test_steps = (20 * 12);
   constexpr int test_mods = 10;
   constexpr double test_val1 = 0.002;
   constexpr double test_val2 = 0.001;

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 10000000.0, 9999998.8, nullptr}, {"A2", 15000000.0, 15000001.2, nullptr},
       {"A3", 10000000.0, 9999998.8, nullptr}, {"A4", 15000000.0, 15000001.2, nullptr},
       {"B1", 10000000.0, 9999998.8, nullptr}, {"B2", 15000000.0, 15000001.2, nullptr},
       {"B3", 10000000.0, 9999998.8, nullptr}, {"B4", 15000000.0, 15000001.2, nullptr},
       {"C1", 10000000.0, 9999998.8, nullptr}, {"C2", 15000000.0, 15000001.2, nullptr},
       {"C3", 10000000.0, 9999998.8, nullptr}, {"C4", 15000000.0, 15000001.2, nullptr},
       {"D1", 10000000.0, 9999998.8, nullptr}, {"D2", 15000000.0, 15000001.2, nullptr},
       {"D3", 10000000.0, 9999998.8, nullptr}, {"D4", 15000000.0, 15000001.2, nullptr},
       {"E1", 10000000.0, 9999998.8, nullptr}, {"E2", 15000000.0, 15000001.2, nullptr},
       {"E3", 10000000.0, 9999998.8, nullptr}, {"E4", 15000000.0, 15000001.2, nullptr},
       {"F1", 10000000.0, 9999998.8, nullptr}, {"F2", 15000000.0, 15000001.2, nullptr},
       {"F3", 10000000.0, 9999998.8, nullptr}, {"F4", 15000000.0, 15000001.2, nullptr},
       {"G1", 10000000.0, 9999998.8, nullptr}, {"G2", 15000000.0, 15000001.2, nullptr},
       {"G3", 10000000.0, 9999998.8, nullptr}, {"G4", 15000000.0, 15000001.2, nullptr},
       {"H1", 10000000.0, 9999998.8, nullptr}, {"H2", 15000000.0, 15000001.2, nullptr},
       {"H3", 10000000.0, 9999998.8, nullptr}, {"H4", 15000000.0, 15000001.2, nullptr},
       {"I1", 10000000.0, 9999998.8, nullptr}, {"I2", 15000000.0, 15000001.2, nullptr},
       {"I3", 10000000.0, 9999998.8, nullptr}, {"I4", 15000000.0, 15000001.2, nullptr},
       {"J1", 10000000.0, 9999998.8, nullptr}, {"J2", 15000000.0, 15000001.2, nullptr},
       {"J3", 10000000.0, 9999998.8, nullptr}, {"J4", 15000000.0, 15000001.2, nullptr}};
#else
#if 1  // 100 loops
   constexpr int test_loops = 100;
   constexpr int test_steps = (50 * 12);
   constexpr int test_mods = 10;
   constexpr double test_val1 = 0.002;
   constexpr double test_val2 = 0.001;

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 10000000.0, 9999700.0, nullptr}, {"A2", 15000000.0, 15000300.0, nullptr},
       {"A3", 10000000.0, 9999700.0, nullptr}, {"A4", 15000000.0, 15000300.0, nullptr},
       {"B1", 10000000.0, 9999700.0, nullptr}, {"B2", 15000000.0, 15000300.0, nullptr},
       {"B3", 10000000.0, 9999700.0, nullptr}, {"B4", 15000000.0, 15000300.0, nullptr},
       {"C1", 10000000.0, 9999700.0, nullptr}, {"C2", 15000000.0, 15000300.0, nullptr},
       {"C3", 10000000.0, 9999700.0, nullptr}, {"C4", 15000000.0, 15000300.0, nullptr},
       {"D1", 10000000.0, 9999700.0, nullptr}, {"D2", 15000000.0, 15000300.0, nullptr},
       {"D3", 10000000.0, 9999700.0, nullptr}, {"D4", 15000000.0, 15000300.0, nullptr},
       {"E1", 10000000.0, 9999700.0, nullptr}, {"E2", 15000000.0, 15000300.0, nullptr},
       {"E3", 10000000.0, 9999700.0, nullptr}, {"E4", 15000000.0, 15000300.0, nullptr},
       {"F1", 10000000.0, 9999700.0, nullptr}, {"F2", 15000000.0, 15000300.0, nullptr},
       {"F3", 10000000.0, 9999700.0, nullptr}, {"F4", 15000000.0, 15000300.0, nullptr},
       {"G1", 10000000.0, 9999700.0, nullptr}, {"G2", 15000000.0, 15000300.0, nullptr},
       {"G3", 10000000.0, 9999700.0, nullptr}, {"G4", 15000000.0, 15000300.0, nullptr},
       {"H1", 10000000.0, 9999700.0, nullptr}, {"H2", 15000000.0, 15000300.0, nullptr},
       {"H3", 10000000.0, 9999700.0, nullptr}, {"H4", 15000000.0, 15000300.0, nullptr},
       {"I1", 10000000.0, 9999700.0, nullptr}, {"I2", 15000000.0, 15000300.0, nullptr},
       {"I3", 10000000.0, 9999700.0, nullptr}, {"I4", 15000000.0, 15000300.0, nullptr},
       {"J1", 10000000.0, 9999700.0, nullptr}, {"J2", 15000000.0, 15000300.0, nullptr},
       {"J3", 10000000.0, 9999700.0, nullptr}, {"J4", 15000000.0, 15000300.0, nullptr}};
#else  // 10000 loops
   constexpr int test_loops = 10000;
   constexpr int test_steps = (50 * 12);
   constexpr int test_mods = 10;
   constexpr double test_val1 = 0.002;
   constexpr double test_val2 = 0.001;

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 10000000.0, 9970000.0, nullptr}, {"A2", 15000000.0, 15030000.0, nullptr},
       {"A3", 10000000.0, 9970000.0, nullptr}, {"A4", 15000000.0, 15030000.0, nullptr},
       {"B1", 10000000.0, 9970000.0, nullptr}, {"B2", 15000000.0, 15030000.0, nullptr},
       {"B3", 10000000.0, 9970000.0, nullptr}, {"B4", 15000000.0, 15030000.0, nullptr},
       {"C1", 10000000.0, 9970000.0, nullptr}, {"C2", 15000000.0, 15030000.0, nullptr},
       {"C3", 10000000.0, 9970000.0, nullptr}, {"C4", 15000000.0, 15030000.0, nullptr},
       {"D1", 10000000.0, 9970000.0, nullptr}, {"D2", 15000000.0, 15030000.0, nullptr},
       {"D3", 10000000.0, 9970000.0, nullptr}, {"D4", 15000000.0, 15030000.0, nullptr},
       {"E1", 10000000.0, 9970000.0, nullptr}, {"E2", 15000000.0, 15030000.0, nullptr},
       {"E3", 10000000.0, 9970000.0, nullptr}, {"E4", 15000000.0, 15030000.0, nullptr},
       {"F1", 10000000.0, 9970000.0, nullptr}, {"F2", 15000000.0, 15030000.0, nullptr},
       {"F3", 10000000.0, 9970000.0, nullptr}, {"F4", 15000000.0, 15030000.0, nullptr},
       {"G1", 10000000.0, 9970000.0, nullptr}, {"G2", 15000000.0, 15030000.0, nullptr},
       {"G3", 10000000.0, 9970000.0, nullptr}, {"G4", 15000000.0, 15030000.0, nullptr},
       {"H1", 10000000.0, 9970000.0, nullptr}, {"H2", 15000000.0, 15030000.0, nullptr},
       {"H3", 10000000.0, 9970000.0, nullptr}, {"H4", 15000000.0, 15030000.0, nullptr},
       {"I1", 10000000.0, 9970000.0, nullptr}, {"I2", 15000000.0, 15030000.0, nullptr},
       {"I3", 10000000.0, 9970000.0, nullptr}, {"I4", 15000000.0, 15030000.0, nullptr},
       {"J1", 10000000.0, 9970000.0, nullptr}, {"J2", 15000000.0, 15030000.0, nullptr},
       {"J3", 10000000.0, 9970000.0, nullptr}, {"J4", 15000000.0, 15030000.0, nullptr}};
#endif
#endif
   for (auto& pool : data) {
      pool.pool = manager.addPool(pool.name, "", "", 1.0, 1, pool.value, nullptr);
   }
   manager.initialisePools();

   for (auto x = 0; x < test_loops; x++) {
      for (auto step = 0; step < test_steps; step++) {
         for (auto m = 0; m < test_mods; m++) {
            auto op1 = manager.createStockOperation(module);
            for (auto dataIdx = 0; dataIdx < data.size(); dataIdx += 2) {
               const auto test = (step % 2) == 0;
               const auto* src = data[test ? dataIdx : dataIdx + 1].pool;
               const auto* snk = data[test ? dataIdx + 1 : dataIdx].pool;
               op1->addTransfer(src, snk, test ? test_val1 : test_val2);
            }
            op1->submitOperation();
         }
         manager.applyOperations();
      }
      manager.clearAllOperationResults();
   }

   for (auto& [name, value, result, pool] : data) {
      BOOST_CHECK_CLOSE(pool->value(), result, pct_tolerance);
   }
}

void test_PerformanceTestCBM(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

#if defined(_DEBUG)
   constexpr int test_loops = 1;
   constexpr int test_steps = (20 * 12);
   constexpr int test_mods = 10;
   constexpr double test_val1 = .01;
   constexpr double test_val2 = .005;

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 100.0, 66.8896325746, nullptr}, {"A2", 100.0, 133.110367425, nullptr},
       {"A3", 100.0, 66.8896325746, nullptr}, {"A4", 100.0, 133.110367425, nullptr},
       {"B1", 100.0, 66.8896325746, nullptr}, {"B2", 100.0, 133.110367425, nullptr},
       {"B3", 100.0, 66.8896325746, nullptr}, {"B4", 100.0, 133.110367425, nullptr},
       {"C1", 100.0, 66.8896325746, nullptr}, {"C2", 100.0, 133.110367425, nullptr},
       {"C3", 100.0, 66.8896325746, nullptr}, {"C4", 100.0, 133.110367425, nullptr},
       {"D1", 150.0, 100.334448862, nullptr}, {"D2", 150.0, 199.665551138, nullptr},
       {"D3", 150.0, 100.334448862, nullptr}, {"D4", 150.0, 199.665551138, nullptr},
       {"E1", 100.0, 66.8896325746, nullptr}, {"E2", 100.0, 133.110367425, nullptr},
       {"E3", 100.0, 66.8896325746, nullptr}, {"E4", 100.0, 133.110367425, nullptr},
       {"F1", 100.0, 66.8896325746, nullptr}, {"F2", 100.0, 133.110367425, nullptr},
       {"F3", 100.0, 66.8896325746, nullptr}, {"F4", 100.0, 133.110367425, nullptr},
       {"G1", 100.0, 66.8896325746, nullptr}, {"G2", 100.0, 133.110367425, nullptr},
       {"G3", 100.0, 66.8896325746, nullptr}, {"G4", 100.0, 133.110367425, nullptr},
       {"H1", 100.0, 66.8896325746, nullptr}, {"H2", 100.0, 133.110367425, nullptr},
       {"H3", 100.0, 66.8896325746, nullptr}, {"H4", 100.0, 133.110367425, nullptr},
       {"I1", 100.0, 66.8896325746, nullptr}, {"I2", 100.0, 133.110367425, nullptr},
       {"I3", 100.0, 66.8896325746, nullptr}, {"I4", 100.0, 133.110367425, nullptr},
       {"J1", 100.0, 66.8896325746, nullptr}, {"J2", 100.0, 133.110367425, nullptr},
       {"J3", 100.0, 66.8896325746, nullptr}, {"J4", 100.0, 133.110367425, nullptr}};
#else
   constexpr int test_loops = 100;
   constexpr int test_steps = (50 * 12);
   constexpr int test_mods = 10;
   constexpr double test_val1 = .01;
   constexpr double test_val2 = .005;

   std::vector<PoolInitValueAndResult> data = {
       {"A1", 100.0, 66.889632107, nullptr},  {"A2", 100.0, 133.110367893, nullptr},
       {"A3", 100.0, 66.889632107, nullptr},  {"A4", 100.0, 133.110367893, nullptr},
       {"B1", 100.0, 66.889632107, nullptr},  {"B2", 100.0, 133.110367893, nullptr},
       {"B3", 100.0, 66.889632107, nullptr},  {"B4", 100.0, 133.110367893, nullptr},
       {"C1", 100.0, 66.889632107, nullptr},  {"C2", 100.0, 133.110367893, nullptr},
       {"C3", 100.0, 66.889632107, nullptr},  {"C4", 100.0, 133.110367893, nullptr},
       {"D1", 150.0, 100.334448161, nullptr}, {"D2", 150.0, 199.665551839, nullptr},
       {"D3", 150.0, 100.334448161, nullptr}, {"D4", 150.0, 199.665551839, nullptr},
       {"E1", 100.0, 66.889632107, nullptr},  {"E2", 100.0, 133.110367893, nullptr},
       {"E3", 100.0, 66.889632107, nullptr},  {"E4", 100.0, 133.110367893, nullptr},
       {"F1", 100.0, 66.889632107, nullptr},  {"F2", 100.0, 133.110367893, nullptr},
       {"F3", 100.0, 66.889632107, nullptr},  {"F4", 100.0, 133.110367893, nullptr},
       {"G1", 100.0, 66.889632107, nullptr},  {"G2", 100.0, 133.110367893, nullptr},
       {"G3", 100.0, 66.889632107, nullptr},  {"G4", 100.0, 133.110367893, nullptr},
       {"H1", 100.0, 66.889632107, nullptr},  {"H2", 100.0, 133.110367893, nullptr},
       {"H3", 100.0, 66.889632107, nullptr},  {"H4", 100.0, 133.110367893, nullptr},
       {"I1", 100.0, 66.889632107, nullptr},  {"I2", 100.0, 133.110367893, nullptr},
       {"I3", 100.0, 66.889632107, nullptr},  {"I4", 100.0, 133.110367893, nullptr},
       {"J1", 100.0, 66.889632107, nullptr},  {"J2", 100.0, 133.110367893, nullptr},
       {"J3", 100.0, 66.889632107, nullptr},  {"J4", 100.0, 133.110367893, nullptr}};
#endif

   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   auto start = moja::DateTime::now();
   for (auto x = 0; x < test_loops; x++) {
      for (auto step = 0; step < test_steps; step++) {
         for (auto m = 0; m < test_mods; m++) {
            auto op1 = manager.createProportionalOperation(module);

            for (auto dataIdx = 0; dataIdx < data.size(); dataIdx += 2) {
               const auto test = (m % 2) == 0;
               const auto* src = data[test ? dataIdx : dataIdx + 1].pool;
               const auto* snk = data[test ? dataIdx + 1 : dataIdx].pool;
               op1->addTransfer(src, snk, test ? test_val1 : test_val2);
            }
            op1->submitOperation();
            manager.applyOperations();
         }
      }
      manager.clearAllOperationResults();
   }

   for (auto& [name, value, result, pool] : data) {
      BOOST_CHECK_CLOSE(pool->value(), result, pct_tolerance);
   }
}

void SubmitOperationAddsToPendingQueue(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 55.00, nullptr}, {"B", 0.0, 45.00, nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, 45);
   op1->submitOperation();

   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 1);
}

void ClearLastAppliedOperationResults(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 55.00, nullptr}, {"B", 0.0, 45.00, nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, 45);
   op1->submitOperation();
   manager.applyOperations();
   manager.clearLastAppliedOperationResults();

   BOOST_CHECK_EQUAL(manager.operationResultsLastApplied().size(), 0);
}

void ApplyOperationsAppendsToCommittedQueue(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 1.00, nullptr}, {"B", 0.0, 99.00, nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;

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
}

void ApplyOperationsCorrectlyUpdatesPoolsForSimpleCase(moja::flint::IOperationManager& manager,
                                                       moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   const double p1InitialValue = 100.00;
   const double amountToMove = 40.0;
   std::vector<PoolInitValueAndResult> data = {{"A", p1InitialValue, (p1InitialValue - amountToMove), nullptr},
                                               {"B", 0.0, (0.0 + amountToMove), nullptr}};
   for (auto& p : data) {
      p.pool = manager.addPool(p.name, "", "", 1.0, 1, p.value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;
   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, amountToMove);
   op1->submitOperation();
   manager.applyOperations();

   for (auto& [name, value, result, pool] : data) {
      BOOST_CHECK_CLOSE(pool->value(), result, pct_tolerance);
   }
}

void ApplyOperationsCorrectlyUpdatesPoolsForComplexCase(moja::flint::IOperationManager& manager,
                                                        moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   const double p1InitialValue = 100.00;
   const double p2InitialValue = 50.00;
   const double p1AmountToMove = 75.0;
   const double p2AmountToMove = 25.0;

   std::vector<PoolInitValueAndResult> data = {
       {"A", p1InitialValue, (p1InitialValue - p1AmountToMove + p2AmountToMove),nullptr},
       {"B", p2InitialValue, (p2InitialValue - p2AmountToMove + p1AmountToMove), nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;
   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, p1AmountToMove);
   op1->addTransfer(B, A, p2AmountToMove);

   op1->submitOperation();
   manager.applyOperations();

   for (auto& [name, value, result, pool] : data) {
      BOOST_CHECK_CLOSE(pool->value(), result, pct_tolerance);
   }
}

void ApplyAndGetOperationsLastApplied(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   std::vector<PoolInitValueAndResult> data = {{"A", 100.0, 1.00, nullptr}, {"B", 0.0, 99.00, nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;

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
}

// --------------------------------------------------------------------------------------------

void ApplyOperationsClearsPendingQueue(moja::flint::IOperationManager& manager, moja::flint::IModule& module) {
   auto testName = boost::unit_test::framework::current_test_case().p_name;
   auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(
                             boost::unit_test::framework::current_test_case().p_parent_id))
                            .full_name();

   const double p1InitialValue = 100.00;
   const double p2InitialValue = 50.00;
   const double p1AmountToMove = 75.0;
   const double p2AmountToMove = 25.0;

   std::vector<PoolInitValueAndResult> data = {
       {"A", p1InitialValue, (p1InitialValue - p1AmountToMove + p2AmountToMove), nullptr},
       {"B", p2InitialValue, (p2InitialValue - p2AmountToMove + p1AmountToMove), nullptr}};
   for (auto& [name, value, result, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto* A = data[0].pool;
   const auto* B = data[1].pool;
   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, p1AmountToMove);
   op1->addTransfer(B, A, p2AmountToMove);

   op1->submitOperation();
   manager.applyOperations();

   BOOST_CHECK_EQUAL(manager.operationResultsPending().size(), 0);
}
