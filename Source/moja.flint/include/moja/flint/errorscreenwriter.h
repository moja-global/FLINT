#ifndef MOJA_FLINT_ERRORSCREENWRITER_H_
#define MOJA_FLINT_ERRORSCREENWRITER_H_

#include "moja/flint/modulebase.h"

namespace moja {
namespace flint {
class SpatialLocationInfo;
}
}  // namespace moja

namespace moja {
namespace flint {

class ErrorScreenWriter : public flint::ModuleBase {
  public:
   ErrorScreenWriter() {}

   virtual ~ErrorScreenWriter() = default;
   void subscribe(NotificationCenter& notificationCenter) override;

   flint::ModuleTypes moduleType() override { return flint::ModuleTypes::Aggregator; };

   void onLocalDomainInit() override;
   void onError(std::string msg) override;

  private:
   // -- flintdata objects data
   std::shared_ptr<const flint::SpatialLocationInfo> _spatiallocationinfo;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_ERRORSCREENWRITER_H_