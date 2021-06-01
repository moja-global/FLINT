#pragma once

#include <moja/flint/ipool.h>
#include <moja/flint/poolmetadata.h>

#include <turtle/mock.hpp>

namespace moja::test {

MOCK_BASE_CLASS(MockPool, flint::IPool) {
   std::string mockName;
   std::string mockDescription;
   std::string mockUnits;
   double mockScale;
   int mockOrder;
   const IPool* mockParent;
   std::vector<const IPool*> mockChildren;
   flint::PoolMetaData mockMetadata;

   MockPool(const std::string& name, int idx) : mockMetadata(name, "", "", 1.0, idx) {
      mockName = name;
      mockDescription = "description";
      mockUnits = "units";
      mockScale = 1.0;
      mockOrder = idx;
   }

   MockPool(const std::string& name, const std::string& description, const std::string& units, double scale, int order)
       : mockMetadata(name, description, units, scale, order) {
      mockName = name;
      mockDescription = description;
      mockUnits = units;
      mockScale = scale;
      mockOrder = order;
   }

   const std::string& name() const override { return mockName; }

   const std::string& description() const override { return mockDescription; }

   const std::string& units() const override { return mockUnits; }

   double scale() const override { return mockScale; }

   int order() const override { return mockOrder; }

   const IPool* parent() const { return mockParent; }

   const std::vector<const IPool*>& children() const { return mockChildren; }

   const flint::PoolMetaData& metadata() const override { return mockMetadata; }

   void add_child(IPool * pool) { mockChildren.emplace_back(pool); }

   bool has_children() const { return !mockChildren.empty(); }
   bool is_child() const { return mockParent != nullptr; }

   MOCK_METHOD(idx, 0, int())
   MOCK_METHOD(value, 0, double())
   MOCK_METHOD(initValue, 0, std::optional<double>())
   MOCK_METHOD(set_value, 1, void(double))
   MOCK_METHOD(init, 0, void())
};

}  // namespace moja::test
