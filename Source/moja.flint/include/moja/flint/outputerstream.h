#ifndef MOJA_FLINT_OUTPUTERSTREAM_H_
#define MOJA_FLINT_OUTPUTERSTREAM_H_

#include "moja/flint/modulebase.h"

#include <Poco/FileStream.h>
#include <Poco/TeeStream.h>

#include <string>

namespace moja {
namespace flint {

class FLINT_API OutputerStream : public ModuleBase {
  public:
   OutputerStream()
       : ModuleBase(),
         _outputToScreen(false),
         _outputAnnually(false),
         _outputInfoHeader(false),
         _outputOnOutputStep(true),
         _outputOnTimingEndStep(false),
         _outputOnPostDisturbanceEvent(false) {}
   virtual ~OutputerStream() {}

   virtual void configure(const DynamicObject& config) override;
   virtual void subscribe(NotificationCenter& notificationCenter) override;

   virtual void outputHeader(std::ostream& stream) const;
   virtual void outputInit(std::ostream& stream);
   virtual void outputEndStep(const std::string& notification, std::ostream& stream);
   virtual void outputShutdown(std::ostream& stream);

   void onSystemInit() override;
   void onSystemShutdown() override;
   void onLocalDomainInit() override;
   void onTimingPostInit() override;
   void onOutputStep() override;
   void onTimingEndStep() override;
   void onPostDisturbanceEvent() override;

  protected:
   std::string _fileName;
   bool _outputToScreen;
   bool _outputAnnually;
   bool _outputInfoHeader;
   bool _outputOnOutputStep;
   bool _outputOnTimingEndStep;
   bool _outputOnPostDisturbanceEvent;

   std::vector<std::tuple<std::string, std::string, std::string, IVariable*>>
       _variables;  // including sub property if it has one

   Poco::FileOutputStream _streamFile;
   Poco::TeeOutputStream _output;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OUTPUTERSTREAM_H_