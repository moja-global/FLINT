#include "moja/flint/outputerstreamflux.h"

#include "moja/flint/ioperationresult.h"
#include "moja/flint/ioperationresultflux.h"
#include "moja/flint/ipool.h"
#include "moja/flint/itiming.h"

// #include <moja/flint/operationresultsimple.h>
#include <moja/flint/operationdatapackage.h>
#include <moja/datetime.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>
#include <moja/filesystem.h>

#include <iomanip>  // std::setprecision
#include <iostream>

//#define DL_CHR "\t"
#define DL_CHR ","
#define STOCK_PRECISION 15

namespace fs = moja::filesystem;

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::configure(const DynamicObject& config) {
   _fileName =
       config.contains("output_filename") ? config["output_filename"].extract<std::string>() : "OutputerStreamFlux.txt";
   _outputToScreen = config.contains("output_to_screen") ? bool(config["output_to_screen"]) : true;
   _outputInfoHeader = config.contains("output_info_header") ? bool(config["output_info_header"]) : false;
}

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &OutputerStreamFlux::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &OutputerStreamFlux::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::TimingPostInit, &OutputerStreamFlux::onTimingPostInit, *this);
   notificationCenter.subscribe(signals::TimingEndStep, &OutputerStreamFlux::onTimingEndStep, *this);
   notificationCenter.subscribe(signals::PostDisturbanceEvent, &OutputerStreamFlux::onPostDisturbanceEvent, *this);
}

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::outputHeader(std::ostream& stream) const {
   if (_outputInfoHeader) {
      moja::DateTime start = DateTime::now();
      stream << "OutputerStreamFlux" << std::endl;
      stream << "==========================================================================" << std::endl;
      stream << "Started:" << start << std::endl;
      stream << "==========================================================================" << std::endl;
   }
   stream << 
      "step"               << DL_CHR << 
      "step_date"          << DL_CHR << 
      "module_name"        << DL_CHR << 
      "disturbance_type"   << DL_CHR << 
      "source_pool"        << DL_CHR << 
      "sink_pool"          << DL_CHR << 
      "value"              << std::endl;
}

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::outputOnNotification(const std::string& notification, std::ostream& stream) const {
   const auto& timingL = _landUnitData->timing();
   for (auto operationResult : _landUnitData->getOperationLastAppliedIterator()) {
      const auto mdata = operationResult->metaData();
      for (auto f : operationResult->operationResultFluxCollection()) {
         const auto srcIx = f->source();
         const auto dstIx = f->sink();
         if (srcIx == dstIx)  // don't process diagonal
            continue;

         const auto val = f->value();
         const auto srcPool = _landUnitData->getPool(srcIx);
         const auto dstPool = _landUnitData->getPool(dstIx);

         auto fluxTypeInfoRecordId = flint::FluxType::Unclassified;
         if (operationResult->hasDataPackage()) {
            auto dataPacket = operationResult->dataPackage().extract<std::shared_ptr<flint::OperationDataPackage>>();
            fluxTypeInfoRecordId = dataPacket->_fluxType;
         }

         stream << timingL->step()           << DL_CHR << 
         timingL->curStartDate()             << DL_CHR << 
         mdata->moduleName                   << DL_CHR << 
         int(fluxTypeInfoRecordId)           << DL_CHR << 
         srcPool->name()                     << DL_CHR << 
         dstPool->name()                     << DL_CHR << 
         std::setprecision(STOCK_PRECISION)  << val << std::endl;
      }
   }
}

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::outputShutdown(std::ostream& stream) const {
   if (_outputInfoHeader) {
      DateTime finish = DateTime::now();
      stream << "==========================================================================" << std::endl;
      stream << "Finished:" << finish << std::endl;
      stream << "==========================================================================" << std::endl;
   }
}

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::onSystemInit() {
   if (fs::exists(_fileName)) fs::remove(_fileName);
   _streamFile.open(_fileName, std::ios::out);
   _output.addStream(_streamFile);
   if (_outputToScreen) _output.addStream(std::cout);
   outputHeader(_output);
}

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::onSystemShutdown() {
   outputShutdown(_output);
   _streamFile.close();
}

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::onTimingPostInit() { outputOnNotification("onTimingPostInit", _output); }

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::onTimingEndStep() { outputOnNotification("onTimingEndStep", _output); }

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::onPostDisturbanceEvent() { outputOnNotification("onPostDisturbanceEvent", _output); }

}  // namespace flint
}  // namespace moja
