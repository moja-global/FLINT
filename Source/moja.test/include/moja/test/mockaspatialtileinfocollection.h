#ifndef MOJA_TEST_MOCKASPATIALTILEINFOCOLLECTION_H_
#define MOJA_TEST_MOCKASPATIALTILEINFOCOLLECTION_H_

#include <map>
#include <turtle/mock.hpp>
#include "moja/datarepository/aspatialtileinfocollection.h"
#include "moja/test/mockproviderrelationalinterface.h"
#include "moja/types.h"
#include "moja/dynamic.h"

namespace moja {
namespace test {


typedef std::map<moja::Int64, moja::DynamicVar> fetchLUBlock_Map;
typedef std::shared_ptr<fetchLUBlock_Map> fetchLUBlock_SP;

MOCK_BASE_CLASS(MockAspatialTileInfoCollection, moja::datarepository::AspatialTileInfoCollection) {
	MockAspatialTileInfoCollection() : moja::datarepository::AspatialTileInfoCollection(MockProviderRelational(), 1) {}

	MOCK_METHOD(tileCount, 0, int())
	MOCK_METHOD(luCount, 1, int(int))

	MOCK_METHOD(fetchLUBlock, 1, fetchLUBlock_SP(moja::Int64))
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKASPATIALTILEINFOCOLLECTION_H_