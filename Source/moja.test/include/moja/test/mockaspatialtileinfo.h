#ifndef MOJA_TEST_MOCKASPATIALTILEINFO_H_
#define MOJA_TEST_MOCKASPATIALTILEINFO_H_

#include <turtle/mock.hpp>
#include "moja/datarepository/aspatialtileinfo.h"
#include "moja/test/mockaspatialtileinfocollection.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockAspatialTileInfo, moja::datarepository::AspatialTileInfo) {
	MockAspatialTileInfo() : moja::datarepository::AspatialTileInfo(
		MockAspatialTileInfoCollection(), 1) {}

	MOCK_METHOD(id, 0, int())
	MOCK_METHOD(size, 0, int())
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKASPATIALTILEINFO_H_