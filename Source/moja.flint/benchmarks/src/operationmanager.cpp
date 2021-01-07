#include "moja/flint/ioperation.h"
#include "moja/flint/operationmanagersimplecache.h"
#include "moja/flint/operationmanagercohort.h"
#include "moja/flint/operationmanagerublas.h"

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
      timing.setStartDate(DateTime(2001, 1, 1));
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
      pool = manager.addPool(name, value);
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
void do_stock(benchmark::State& state, T& bench_module) {
   auto& manager = bench_module.manager;
   auto& module = bench_module.module;

   std::vector<std::tuple<std::string, double, const IPool*>> data = {
       {"A", 100.0, nullptr}, {"B", 50.0, nullptr}, {"C", 50.0, nullptr}, {"D", 50.0, nullptr}};
   for (auto& [name, value, pool] : data) {
      pool = manager.addPool(name, value);
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

static void BM_SimpleProportionTransfer(benchmark::State& state) {
   benchmark_module<OperationManagerSimple> benchmark_module("Simple");
   do_proportional(state, benchmark_module);
}

BENCHMARK(BM_SimpleProportionTransfer)->Threads(1);

static void BM_CohortProportionTransfer(benchmark::State& state) {
   benchmark_module<OperationManagerCohort> benchmark_module("Cohort");
   do_proportional(state, benchmark_module);
}

BENCHMARK(BM_CohortProportionTransfer)->Threads(1);

void BM_SimpleCacheProportionTransfer(benchmark::State& state) {
   benchmark_module<OperationManagerSimpleCache> benchmark_module("Simple Cached");
   do_proportional(state, benchmark_module);
}

BENCHMARK(BM_SimpleCacheProportionTransfer)->Threads(1);

void BM_UblasProportionTransfer(benchmark::State& state) {
   benchmark_module<OperationManagerUblas> benchmark_module("Ublas");
   do_proportional(state, benchmark_module);
}

BENCHMARK(BM_UblasProportionTransfer)->Threads(1);

static void BM_SimpleStockTransfer(benchmark::State& state) {
   benchmark_module<OperationManagerSimple> benchmark_module("Simple");
   do_stock(state, benchmark_module);
}

BENCHMARK(BM_SimpleStockTransfer)->Threads(1);

static void BM_CohortStockTransfer(benchmark::State& state) {
   benchmark_module<OperationManagerCohort> benchmark_module("Simple Pooled");
   do_stock(state, benchmark_module);
}

BENCHMARK(BM_CohortStockTransfer)->Threads(1);

void BM_SimpleCacheStockTransfer(benchmark::State& state) {
   benchmark_module<OperationManagerSimpleCache> benchmark_module("Simple Cached");
   do_stock(state, benchmark_module);
}

BENCHMARK(BM_SimpleCacheStockTransfer)->Threads(1);

void BM_UblasStockTransfer(benchmark::State& state) {
   benchmark_module<OperationManagerUblas> benchmark_module("Ublas");
   do_stock(state, benchmark_module);
}

BENCHMARK(BM_UblasStockTransfer)->Threads(1);

BENCHMARK_MAIN();
