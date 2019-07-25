#ifndef MOJA_TEST_MOCKTILE_H_
#define MOJA_TEST_MOCKTILE_H_

#include <moja/flint/configuration/configtile.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockTile, moja::flint::configuration::ConfigTile) {
   MOCK_METHOD(toString, 0, std::string());
   MOCK_METHOD(cellAreaInHa, 1, double(double));
   MOCK_METHOD(xIndex, 0, int());
   MOCK_METHOD(yIndex, 0, int());
   MOCK_METHOD(xSize, 0, double());
   MOCK_METHOD(ySize, 0, double());
   MOCK_METHOD(xPixels, 0, int());
   MOCK_METHOD(yPixels, 0, int());
   MOCK_METHOD(xPixelSize, 0, double());
   MOCK_METHOD(yPixelSize, 0, double());
   MOCK_METHOD(xOrigin, 0, double());
   MOCK_METHOD(yOrigin, 0, double());
};
}  // namespace test
}  // namespace moja

#endif  // MOJA_TEST_MOCKTILE_H_