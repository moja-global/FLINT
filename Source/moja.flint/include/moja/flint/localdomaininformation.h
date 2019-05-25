#ifndef MOJA_FLINT_LOCALDOMAININFORMATION_H_
#define MOJA_FLINT_LOCALDOMAININFORMATION_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/iflintdata.h"

#include <moja/dynamic.h>

namespace moja {
namespace flint {

class FLINT_API LocalDomainInformation : public flint::IFlintData {
  public:
   LocalDomainInformation() = default;
   virtual ~LocalDomainInformation() = default;
   LocalDomainInformation(const LocalDomainInformation&) = delete;
   LocalDomainInformation& operator=(const LocalDomainInformation&) = delete;

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  moja::datarepository::DataRepository& dataRepository) override {
      controllerName = "";
      controllerVersion = "";
      numberOfThreads = 0;
      isThreadSystem = false;
      isMaster = false;
      isThread = false;

      if (config.contains("controllerName")) {
         controllerName = config["controllerName"].extract<const std::string>();
      }
      if (config.contains("controllerVersion")) {
         controllerVersion = config["controllerVersion"].extract<const std::string>();
      }
      if (config.contains("numberOfThreads")) {
         numberOfThreads = config["numberOfThreads"];
      }
      if (config.contains("isThreadSystem")) {
         isThreadSystem = config["isThreadSystem"];
      }
      if (config.contains("isMaster")) {
         isMaster = config["isMaster"];
      }
      if (config.contains("isThread")) {
         isThread = config["isThread"];
      }
   }

   DynamicObject exportObject() const override {
      DynamicObject object;
      object["controllerName"] = controllerName;
      object["controllerVersion"] = controllerVersion;
      object["numberOfThreads"] = numberOfThreads;
      object["isThreadSystem"] = isThreadSystem;
      object["isMaster"] = isMaster;
      object["isThread"] = isThread;
      return object;
   };

   std::string controllerName;
   std::string controllerVersion;
   int numberOfThreads;
   bool isThreadSystem;
   bool isMaster;
   bool isThread;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_LOCALDOMAININFORMATION_H_