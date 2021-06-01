#pragma once

#include "moja/flint/modulebase.h"

namespace moja::flint {

class SpatialLocationInfo;

class ErrorScreenWriter : public ModuleBase {
  public:
   ErrorScreenWriter() {}

   virtual ~ErrorScreenWriter() = default;
   void subscribe(NotificationCenter& notificationCenter) override;

   ModuleTypes moduleType() override { return ModuleTypes::Aggregator; }

   void onLocalDomainInit() override;
   void onError(std::string msg) override;

  private:
   // -- flintdata objects data
   std::shared_ptr<const SpatialLocationInfo> _spatiallocationinfo;
};

}  // namespace moja::flint
