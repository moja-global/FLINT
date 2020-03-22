#ifndef MOJA_FLINT_IFLINTDATA_H_
#define MOJA_FLINT_IFLINTDATA_H_

#include "moja/flint/_flint_exports.h"

#include <moja/dynamic.h>

#include <functional>

namespace moja {
namespace datarepository {
class DataRepository;
}
namespace flint {
class ILandUnitController;

class FLINT_API IFlintData {
  public:
   typedef std::shared_ptr<IFlintData> (*FactoryFunc)(const std::string&, int, const std::string&,
                                                      const DynamicObject&);

   virtual ~IFlintData() = default;

   virtual void registerEventFactoryFunc(FactoryFunc func) {}
   virtual void registerEventFactoryFunc(
       std::function<std::shared_ptr<IFlintData>(const std::string&, int, const std::string&, const DynamicObject&)>
           func) {}

   virtual void configure(DynamicObject config, const ILandUnitController& landUnitController,
                          datarepository::DataRepository& dataRepository) = 0;

   virtual DynamicVar getProperty(const std::string& key) const { return DynamicVar(); }
   virtual void setProperty(const DynamicObject& query, const std::string& key, DynamicVar value) {}
   virtual DynamicObject exportObject() const { return DynamicObject(); }

   virtual void controllerChanged(const ILandUnitController& controller){};

   std::string libraryName;
   std::string typeName;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_IFLINTDATA_H_
