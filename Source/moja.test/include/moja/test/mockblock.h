#ifndef MOJA_TEST_MOCKBLOCK_H_
#define MOJA_TEST_MOCKBLOCK_H_

#include <turtle/mock.hpp>
#include "moja/flint/configuration/configblock.h"
#include "moja/test/mocktile.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockBlock, moja::flint::configuration::ConfigBlock) {
	MockBlock() : moja::flint::configuration::ConfigBlock(MockTile(), 0, 0, 1, 1) {}
	MOCK_METHOD(toString, 0, std::string())
	MOCK_METHOD(left, 0, double())
	MOCK_METHOD(top, 0, double())
	MOCK_METHOD(xPixelSize, 0, double())
	MOCK_METHOD(yPixelSize, 0, double())
	MOCK_METHOD(xPixels, 0, int())
	MOCK_METHOD(yPixels, 0, int())
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKBLOCK_H_