#ifndef MOJA_FLINT_OUTPUTERSTREAMFLUX_H_
#define MOJA_FLINT_OUTPUTERSTREAMFLUX_H_

#include "moja/flint/modulebase.h"

#include <Poco/FileStream.h>
#include <Poco/TeeStream.h>

#include <string>

namespace moja {
namespace flint {

class FLINT_API OutputerStreamFlux : public ModuleBase {
  public:
   explicit OutputerStreamFlux() : ModuleBase(), _outputToScreen(false), _outputInfoHeader(false) {}
   virtual ~OutputerStreamFlux() {}

   void outputOnNotification(const std::string& notification, std::ostream& stream) const;
   void outputShutdown(std::ostream& stream) const;
   void outputHeader(std::ostream& stream) const;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onSystemInit() override;
   void onSystemShutdown() override;
   void onTimingPostInit() override;
   void onTimingEndStep() override;
   void onPostDisturbanceEvent() override; 

  protected:
   std::string _fileName;
   bool _outputToScreen;
   bool _outputInfoHeader;

   Poco::FileOutputStream _streamFile;
   Poco::TeeOutputStream _output;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OUTPUTERSTREAMFLUX_H_