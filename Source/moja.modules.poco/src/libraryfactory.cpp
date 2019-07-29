
#include <moja/flint/mojalibapi.h>

// Modules

// Transforms

#include "moja/modules/poco/nosqlcollectiontransform.h"
#include "moja/modules/poco/pocomongodbquerytransform.h"

// Flint Data

// Providers

#include "moja/modules/poco/providernosqlpocojson.h"
#include "moja/modules/poco/providernosqlpocomongodb.h"

// Other

using moja::flint::DataRepositoryProviderRegistration;
using moja::flint::FlintDataFactoryRegistration;
using moja::flint::FlintDataRegistration;
using moja::flint::IFlintData;
using moja::flint::IModule;
using moja::flint::ITransform;
using moja::flint::ModuleRegistration;
using moja::flint::TransformRegistration;

namespace moja {
namespace modules {
namespace poco {

extern "C" {

MOJA_LIB_API int getModuleRegistrations(ModuleRegistration* outModuleRegistrations) {
   int index = 0;
   return index;
}

MOJA_LIB_API int getTransformRegistrations(TransformRegistration* outTransformRegistrations) {
   int index = 0;
   // outTransformRegistrations[index++] = ModuleRegistration{ "WriteSpatialAWSS3",	[]() -> flint::IModule* { return
   // new base::WriteVariableGrid(std::make_shared<RasterWriterFactoryAWSS3>()); } };

   outTransformRegistrations[index++] = TransformRegistration{
       "PocoMongoDBQueryTransform", []() -> flint::ITransform* { return new PocoMongoDBQueryTransform(); }};
   outTransformRegistrations[index++] = TransformRegistration{
       "NoSQLCollectionTransform", []() -> flint::ITransform* { return new NoSQLCollectionTransform(); }};
   return index;
}

MOJA_LIB_API int getFlintDataRegistrations(FlintDataRegistration* outFlintDataRegistrations) {
   auto index = 0;
   return index;
}

MOJA_LIB_API int getFlintDataFactoryRegistrations(FlintDataFactoryRegistration* outFlintDataFactoryRegistrations) {
   auto index = 0;
   return index;
}

MOJA_LIB_API int getDataRepositoryProviderRegistrations(
    moja::flint::DataRepositoryProviderRegistration* outDataRepositoryProviderRegistration) {
   auto index = 0;
   outDataRepositoryProviderRegistration[index++] = DataRepositoryProviderRegistration{
       "PocoJSON", static_cast<int>(datarepository::ProviderTypes::Relational),
       [](const DynamicObject& settings) -> std::shared_ptr<datarepository::IProviderInterface> {
          return std::make_shared<ProviderNoSQLPocoJSON>(settings);
       }};
   outDataRepositoryProviderRegistration[index++] = DataRepositoryProviderRegistration{
       "PocoMongoDB", static_cast<int>(datarepository::ProviderTypes::Relational),
       [](const DynamicObject& settings) -> std::shared_ptr<datarepository::IProviderInterface> {
          return std::make_shared<ProviderNoSQLPocoMongoDB>(settings);
       }};
   return index;
}
}

}  // namespace poco
}  // namespace modules
}  // namespace moja
