#ifndef MOJA_TEST_MOCKASPATIALTILEINFO_H_
#define MOJA_TEST_MOCKASPATIALTILEINFO_H_

#include "moja/test/mockaspatialtileinfocollection.h"

#include <moja/datarepository/aspatialtileinfo.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockAspatialTileInfo, moja::datarepository::AspatialTileInfo){
   MockAspatialTileInfo() : moja::datarepository::AspatialTileInfo(MockAspatialTileInfoCollection(), 1){}

   MOCK_METHOD(id, 0, int()) MOCK_METHOD(size, 0, int())
};

}
}  // namespace moja

#endif  // MOJA_TEST_MOCKASPATIALTILEINFO_H_