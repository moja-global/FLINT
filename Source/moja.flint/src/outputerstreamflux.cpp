#include "moja/flint/outputerstreamflux.h"

#include "moja/flint/ioperationresult.h"
#include "moja/flint/ioperationresultflux.h"
#include "moja/flint/ipool.h"
#include "moja/flint/itiming.h"

#include <moja/datetime.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <Poco/File.h>

#include <iomanip>  // std::setprecision
#include <iostream>

//#define DL_CHR "\t"
#define DL_CHR ","
#define STOCK_PRECISION 15

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
   stream << "step" << DL_CHR << "step date" << DL_CHR << "module name" << DL_CHR << "disturbance_type" << DL_CHR
          << "source pool" DL_CHR << "sink pool" << DL_CHR << "value" << std::endl;
}

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::outputInit(std::ostream& stream) const {
   const auto timingL = _landUnitData->timing();

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

         stream << timingL->step() << DL_CHR << timingL->curStartDate() << DL_CHR;
         stream << mdata->moduleName << DL_CHR;
         stream << srcPool->name() << DL_CHR << dstPool->name() << DL_CHR << std::setprecision(STOCK_PRECISION) << val
                << std::endl;
      }
   }
}

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::outputEndStep(std::ostream& stream) const {
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

         stream << timingL->step() << DL_CHR << timingL->curStartDate() << DL_CHR;
         stream << mdata->moduleName << DL_CHR;
         stream << srcPool->name() << DL_CHR << dstPool->name() << DL_CHR << std::setprecision(STOCK_PRECISION) << val
                << std::endl;
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
   Poco::File outputFile(_fileName);
   if (outputFile.exists()) outputFile.remove();
   outputFile.createFile();

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

void OutputerStreamFlux::onTimingPostInit() { outputInit(_output); }

// --------------------------------------------------------------------------------------------

void OutputerStreamFlux::onTimingEndStep() { outputEndStep(_output); }

}  // namespace flint
}  // namespace moja
