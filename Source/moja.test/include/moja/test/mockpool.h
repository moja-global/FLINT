#ifndef MOJA_TEST_MOCKPOOL_H_
#define MOJA_TEST_MOCKPOOL_H_

#include <moja/flint/ipool.h>
#include <moja/flint/poolmetadata.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockPool, flint::IPool) {
   std::string mockName;
   std::string mockDescription;
   std::string mockUnits;
   double mockScale;
   int mockOrder;
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

   const flint::PoolMetaData& metadata() override { return mockMetadata; };

   MOCK_METHOD(idx, 0, int())
   MOCK_METHOD(value, 0, double())
   MOCK_METHOD(initValue, 0, double())
   MOCK_METHOD(set_value, 1, void(double))
   MOCK_METHOD(init, 0, void())
};

}  // namespace test
}  // namespace moja

#endif  // MOJA_TEST_MOCKPOOL_H_