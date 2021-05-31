#include "moja/flint/ioperation.h"
#include "moja/flint/ipool.h"

#include <moja/flint/modulebase.h>
#include <moja/flint/operationmanagersimple.h>
#include <moja/flint/timing.h>

#include <moja/dynamic.h>

#include <benchmark/benchmark.h>

using namespace moja;
using namespace flint;

template <class T>
struct benchmark_module {
   benchmark_module(const std::string& name) : config({{"use_kahan", false}}), manager(timing, config) {
      timing.setStartDate(DateTime(2000, 1, 1));
      timing.setStartDate(DateTime(2020, 12, 31));
      timing.init();
      auto& metadata = module.metaData();
      metadata.setDefaults();
      metadata.moduleName = name;
   }
   Timing timing;
   ModuleBase module;
   DynamicObject config;
   T manager;
};

template <class T>
void do_proportional(benchmark::State& state, T& bench_module) {
   auto& manager = bench_module.manager;
   auto& module = bench_module.module;

   std::vector<std::tuple<std::string, double, const IPool*>> data = {
       {"A", 100.0, nullptr}, {"B", 50.0, nullptr}, {"C", 50.0, nullptr}, {"D", 50.0, nullptr}};
   for (auto& [name, value, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto& [a_name, a_value, a] = data[0];
   const auto& [b_name, b_value, b] = data[1];
   const auto& [c_name, c_value, c] = data[2];
   const auto& [d_name, d_value, d] = data[3];

   for (auto _ : state) {
      auto operation = manager.createProportionalOperation(module);
      operation->addTransfer(a, b, .10);
      operation->addTransfer(b, c, .10);
      operation->addTransfer(c, d, .10);
      operation->addTransfer(d, a, .10);
      operation->submitOperation();
      manager.applyOperations();
   }
}

template <class T>
void do_proportional_children(benchmark::State& state, T& bench_module) {
   auto& manager = bench_module.manager;
   auto& module = bench_module.module;

   auto* pool_A  = manager.addPool("A"  , "", "", 1.0, 1, std::optional<double>{}, nullptr);
   auto* pool_Aa = manager.addPool("A.a", "", "", 1.0, 2, 50.0, pool_A);
   auto* pool_Ab = manager.addPool("A.b", "", "", 1.0, 3, 50.0, pool_A);
   auto* pool_B  = manager.addPool("B"  , "", "", 1.0, 4, 100.0, nullptr);
   auto* pool_C  = manager.addPool("C"  , "", "", 1.0, 5, 100.0, nullptr);

   manager.initialisePools();

   for (auto _ : state) {
      auto operation = manager.createProportionalOperation(module);
      operation->addTransfer(pool_Aa, pool_B, .10);
      operation->addTransfer(pool_Ab, pool_B, .10);
      operation->addTransfer(pool_B, pool_C, .10);
      operation->addTransfer(pool_C, pool_Aa, .05);
      operation->addTransfer(pool_C, pool_Ab, .05);
      operation->submitOperation();
      manager.applyOperations();
   }
}

template <class T>
void do_stock(benchmark::State& state, T& bench_module) {
   auto& manager = bench_module.manager;
   auto& module = bench_module.module;

   std::vector<std::tuple<std::string, double, const IPool*>> data = {
       {"A", 100.0, nullptr}, {"B", 50.0, nullptr}, {"C", 50.0, nullptr}, {"D", 50.0, nullptr}};
   for (auto& [name, value, pool] : data) {
      pool = manager.addPool(name, "", "", 1.0, 1, value, nullptr);
   }
   manager.initialisePools();

   const auto& [a_name, a_value, a] = data[0];
   const auto& [b_name, b_value, b] = data[1];
   const auto& [c_name, c_value, c] = data[2];
   const auto& [d_name, d_value, d] = data[3];

   for (auto _ : state) {
      auto operation1 = manager.createStockOperation(module);
      operation1->addTransfer(a, b, 45);
      operation1->addTransfer(b, c, 45);
      operation1->addTransfer(c, d, 45);
      operation1->addTransfer(d, a, 45);
      operation1->submitOperation();
      auto operation2 = manager.createStockOperation(module);
      operation2->addTransfer(a, b, 45);
      operation2->addTransfer(b, c, 45);
      operation2->addTransfer(c, d, 45);
      operation2->addTransfer(d, a, 45);
      operation2->submitOperation();
      manager.applyOperations();
      manager.clearAllOperationResults();
   }
}

template <class T>
void do_stock_children(benchmark::State& state, T& bench_module) {
   auto& manager = bench_module.manager;
   auto& module = bench_module.module;

   auto* pool_A = manager.addPool("A", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   auto* pool_Aa = manager.addPool("A.a", "", "", 1.0, 2, 50.0, pool_A);
   auto* pool_Ab = manager.addPool("A.b", "", "", 1.0, 3, 50.0, pool_A);
   auto* pool_B = manager.addPool("B", "", "", 1.0, 4, 100.0, nullptr);
   auto* pool_C = manager.addPool("C", "", "", 1.0, 5, 100.0, nullptr);

   manager.initialisePools();

   for (auto _ : state) {
      auto operation = manager.createStockOperation(module);
      operation->addTransfer(pool_Aa, pool_B, 10.0);
      operation->addTransfer(pool_Ab, pool_B, 10.0);
      operation->addTransfer(pool_B, pool_C, 10.0);
      operation->addTransfer(pool_C, pool_Aa, 5.0);
      operation->addTransfer(pool_C, pool_Ab, 5.0);
      operation->submitOperation();
      manager.applyOperations();
   }
}

static void BM_ProportionTransfer(benchmark::State& state) {
   benchmark_module<OperationManagerSimple> benchmark_module("Simple");
   do_proportional(state, benchmark_module);
}

BENCHMARK(BM_ProportionTransfer);

static void BM_ProportionTransferChildren(benchmark::State& state) {
   benchmark_module<OperationManagerSimple> benchmark_module("Simple");
   do_proportional_children(state, benchmark_module);
}

BENCHMARK(BM_ProportionTransferChildren);

static void BM_StockTransfer(benchmark::State& state) {
   benchmark_module<OperationManagerSimple> benchmark_module("Simple");
   do_stock(state, benchmark_module);
}

BENCHMARK(BM_StockTransfer);

static void BM_StockTransferChildren(benchmark::State& state) {
   benchmark_module<OperationManagerSimple> benchmark_module("Simple");
   do_stock_children(state, benchmark_module);
}

BENCHMARK(BM_StockTransferChildren);

BENCHMARK_MAIN();
