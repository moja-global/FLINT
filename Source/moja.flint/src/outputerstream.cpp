#include "moja/flint/outputerstream.h"

#include "moja/flint/flintdatavariable.h"
#include "moja/flint/iflintdata.h"
#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ipool.h"
#include "moja/flint/landunitcontroller.h"

#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <Poco/File.h>

#include <boost/format.hpp>

#include <chrono>
#include <iomanip>  // std::setprecision
#include <iostream>

//#define DL_CHR "\t"
#define DL_CHR ","
#define STOCK_PRECISION 15

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

std::string escape_json2(const std::string& s) {
   std::ostringstream o;
   for (auto c = s.cbegin(); c != s.cend(); ++c) {
      if (*c == '"' || *c == '\\' || ('\x00' <= *c && *c <= '\x1f'))
         // o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(*c);
         o << "\\";
      o << *c;
   }
   return o.str();
}

// --------------------------------------------------------------------------------------------

void outputDynamicToStream(std::ostream& fout, const DynamicVar& object) {
   if (object.isStruct()) {
      fout << "\"[struct]\"";
   } else if (object.isVector()) {
      fout << "\"[vector]\"";
   } else if (object.isBoolean()) {
      bool val = object;
      fout << (val ? "true" : "false");
   } else if (object.isEmpty()) {
      fout << "\"[null]\"";
   } else if (object.isString()) {
      fout << "\"" << escape_json2(object.extract<const std::string>()) << "\"";
   } else if (object.isInteger()) {
      int val = object;
      fout << val;
   } else if (object.isNumeric()) {
      auto val = object.extract<double>();
      fout << val;
   } else if (object.isSigned()) {
      fout << "\"[Signed]\"";
   } else {
      if (object.type() == typeid(DateTime)) {
         DateTime dt = object.extract<DateTime>();
         std::string simpleDateStr = (boost::format("\"%1%/%2%/%3%\"") % dt.year() % dt.month() % dt.day()).str();
         fout << simpleDateStr;
      } else if (object.type() == typeid(Int16)) {
         fout << object.extract<const Int16>();
      } else if (object.type() == typeid(std::shared_ptr<IFlintData>)) {
         fout << "\"[IFlintData\"]";
      } else
         fout << "\"[Unknown\"]";
   }
}

// --------------------------------------------------------------------------------------------

void OutputerStream::configure(const DynamicObject& config) {
   _fileName =
       config.contains("output_filename") ? config["output_filename"].extract<std::string>() : "OutputerStreamFlux.txt";
   _outputToScreen = config.contains("output_to_screen") ? bool(config["output_to_screen"]) : true;
   _outputInfoHeader = config.contains("output_info_header") ? bool(config["output_info_header"]) : true;
   _outputAnnually = config.contains("output_annual_only") ? bool(config["output_annual_only"]) : false;
   _outputOnOutputStep = config.contains("output_on_outputstep") ? bool(config["output_on_outputstep"]) : true;
   _outputOnTimingEndStep =
       config.contains("output_on_timingendstep") ? bool(config["output_on_timingendstep"]) : false;
   _outputOnPostDisturbanceEvent =
       config.contains("output_on_postdisturbanceevent") ? bool(config["output_on_postdisturbanceevent"]) : false;

   if (config.contains("variables")) {
      // assume we have a vector of vairables to include in output
      const auto& items = config["variables"].extract<DynamicVector>();
      for (auto& item : items) {
         auto var = item.extract<const std::string>();
         auto propertySeparator = var.find_first_of('.');
         if (propertySeparator != std::string::npos) {
            auto prop = var.substr(propertySeparator + 1, var.length());
            auto varname = var.substr(0, propertySeparator);
            _variables.push_back(std::make_tuple(var, varname, prop, nullptr));
         } else {
            _variables.push_back(std::make_tuple(var, var, "", nullptr));
         }
      }
   }
};

// --------------------------------------------------------------------------------------------

void OutputerStream::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &OutputerStream::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &OutputerStream::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &OutputerStream::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::TimingPostInit, &OutputerStream::onTimingPostInit, *this);
   notificationCenter.subscribe(signals::OutputStep, &OutputerStream::onOutputStep, *this);
   notificationCenter.subscribe(signals::TimingEndStep, &OutputerStream::onTimingEndStep, *this);
   notificationCenter.subscribe(signals::PostDisturbanceEvent, &OutputerStream::onPostDisturbanceEvent, *this);
};

// --------------------------------------------------------------------------------------------

void OutputerStream::outputHeader(std::ostream& stream) const {
   using namespace std::chrono;
   if (_outputInfoHeader) {
      auto t1 = moja::localtime(moja::systemtime_now());

      stream << "OutputerStream" << std::endl;
      stream << "==========================================================================" << std::endl;
      stream << "Started:" << moja::put_time(&t1, "%c %Z") << std::endl;
      stream << "==========================================================================" << std::endl;
   }
   if (_landUnitData->uncertainty().enabled()) {
      stream << "iteration" << DL_CHR;
   }
   stream << "notification" << DL_CHR << "step" << DL_CHR << "stepDate" << DL_CHR << "fracOfStep" DL_CHR
          << "stepLenInYears" << DL_CHR;
   auto pools = _landUnitData->poolCollection();
   for (auto& it : pools) {
      stream << it->name() << DL_CHR;
   }
   for (auto var : _variables) {
      stream << std::get<0>(var) << DL_CHR;
   }
   stream << std::endl;
}

// --------------------------------------------------------------------------------------------

void OutputerStream::outputInit(std::ostream& stream) {
   const auto& timingL = *_landUnitData->timing();

   if (_landUnitData->uncertainty().enabled()) {
      stream << _landUnitData->uncertainty().iteration() << DL_CHR;
   }
   stream << "onTimingPostInit" << DL_CHR << timingL.step() << DL_CHR << timingL.curEndDate().addMicroseconds(-1)
          << DL_CHR << timingL.fractionOfStep() << DL_CHR << timingL.stepLengthInYears() << DL_CHR;
   stream << std::setprecision(STOCK_PRECISION);

   auto pools = _landUnitData->poolCollection();
   for (auto& it : pools) {
      stream << it->value() << DL_CHR;
   }
   for (auto var : _variables) {
      auto varPtr = std::get<3>(var);
      auto varName = std::get<1>(var);
      auto varProp = std::get<2>(var);

      if (varPtr == nullptr) {
         stream << "(missing variable)";
      } else if (varProp != "") {
         auto varValue = varPtr->value();
         if (varValue.type() == typeid(std::shared_ptr<IFlintData>)) {
            auto flintDataVariable = varPtr->value().extract<std::shared_ptr<IFlintData>>();
            auto propValue = flintDataVariable->getProperty(varProp);
            outputDynamicToStream(stream, propValue);
         } else {
            if (varValue.isStruct()) {
               auto varStruct = varValue.extract<const DynamicObject>();
               auto varStructProp = varStruct.contains(varProp) ? varStruct[varProp] : DynamicVar();
               outputDynamicToStream(stream, varStructProp);
            } else
               outputDynamicToStream(stream, varValue);
         }
      } else {
         // TODO: extend this to do property of array/struct objects
         outputDynamicToStream(stream, varPtr->value());
      }
      stream << DL_CHR;
   }
   stream << std::endl;
}

// --------------------------------------------------------------------------------------------

void OutputerStream::outputEndStep(const std::string& notification, std::ostream& stream) {
   const auto& timingL = *_landUnitData->timing();
   const auto& uncertainty = _landUnitData->uncertainty();

   if (uncertainty.enabled()) {
      stream << uncertainty.iteration() << DL_CHR;
   }
   stream << notification << DL_CHR << timingL.step() << DL_CHR << timingL.curEndDate().addMicroseconds(-1) << DL_CHR
          << timingL.fractionOfStep() << DL_CHR << timingL.stepLengthInYears() << DL_CHR;
   stream << std::setprecision(STOCK_PRECISION);
   auto pools = _landUnitData->poolCollection();
   for (auto& it : pools) {
      stream << it->value() << DL_CHR;
   }
   for (auto var : _variables) {
      auto varPtr = std::get<3>(var);
      auto varName = std::get<1>(var);
      auto varProp = std::get<2>(var);

      if (varPtr == nullptr) {
         stream << "(missing variable)";
      } else if (varProp != "") {
         auto varValue = varPtr->value();
         if (varValue.type() == typeid(std::shared_ptr<IFlintData>)) {
            auto flintDataVariable = varPtr->value().extract<std::shared_ptr<IFlintData>>();
            auto propValue = flintDataVariable->getProperty(varProp);
            outputDynamicToStream(stream, propValue);
         } else {
            if (varValue.isStruct()) {
               auto varStruct = varValue.extract<const DynamicObject>();
               auto varStructProp = varStruct.contains(varProp) ? varStruct[varProp] : DynamicVar();
               outputDynamicToStream(stream, varStructProp);
            } else
               outputDynamicToStream(stream, varValue);
         }
      } else {
         // TODO: extend this to do property of array/struct objects
         outputDynamicToStream(stream, varPtr->value());
      }
      stream << DL_CHR;
   }
   stream << std::endl;
}

// --------------------------------------------------------------------------------------------

void OutputerStream::outputShutdown(std::ostream& stream) {
   using namespace std::chrono;

   if (_outputInfoHeader) {
      auto t1 = localtime(moja::systemtime_now());
      stream << "==========================================================================" << std::endl;
      stream << "Finished:" << moja::put_time(&t1, "%c %Z") << std::endl;
      stream << "==========================================================================" << std::endl;
   }
}

// --------------------------------------------------------------------------------------------

void OutputerStream::onSystemInit() {
   Poco::File outputFile(_fileName);
   if (outputFile.exists()) outputFile.remove();
   outputFile.createFile();

   _streamFile.open(_fileName, std::ios::out);
   _output.addStream(_streamFile);
   if (_outputToScreen) _output.addStream(std::cout);
   outputHeader(_output);
}

// --------------------------------------------------------------------------------------------

void OutputerStream::onSystemShutdown() { outputShutdown(_output); }

// --------------------------------------------------------------------------------------------

void OutputerStream::onLocalDomainInit() {
   for (auto& var : _variables) {
      auto varName = std::get<1>(var);
      if (_landUnitData->hasVariable(varName))
         std::get<3>(var) = _landUnitData->getVariable(varName);
      else
         std::get<3>(var) = nullptr;
   }
}

// --------------------------------------------------------------------------------------------

void OutputerStream::onTimingPostInit() { outputInit(_output); }

// --------------------------------------------------------------------------------------------

void OutputerStream::onOutputStep() {
   if (_outputOnOutputStep) {
      const auto& timingL = *_landUnitData->timing();

      if (!_outputAnnually || timingL.curStartDate().month() == 12) outputEndStep("onOutputStep", _output);
   }
}

// --------------------------------------------------------------------------------------------

void OutputerStream::onTimingEndStep() {
   if (_outputOnTimingEndStep) outputEndStep("onTimingEndStep", _output);
}

// --------------------------------------------------------------------------------------------

void OutputerStream::onPostDisturbanceEvent() {
   if (_outputOnPostDisturbanceEvent) outputEndStep("onPostDisturbanceEvent", _output);
}

}  // namespace flint
}  // namespace moja
