#include "moja/flint/outputerstream.h"

#include "moja/flint/flintdatavariable.h"
#include "moja/flint/iflintdata.h"
#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ipool.h"
#include "moja/flint/landunitcontroller.h"

#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <boost/algorithm/string.hpp>

#include <fmt/format.h>

#include <filesystem>
#include <iomanip>  // std::setprecision
#include <iostream>

namespace moja::flint {

std::string escape_json2(const std::string& input_string) {
   std::ostringstream output_stream;
   for (auto c = input_string.cbegin(); c != input_string.cend(); ++c) {
      if (*c == '"' || *c == '\\' || ('\x00' <= *c && *c <= '\x1f')) output_stream << "\\";
      output_stream << *c;
   }
   return output_stream.str();
}

void outputDynamicToStream(std::ostream& output_stream, const DynamicVar& object) {
   if (object.isStruct()) {
      output_stream << "\"[struct]\"";
   } else if (object.isVector()) {
      output_stream << "\"[vector]\"";
   } else if (object.isBoolean()) {
      const bool val = object;
      output_stream << (val ? "true" : "false");
   } else if (object.isEmpty()) {
      output_stream << "\"[null]\"";
   } else if (object.isString()) {
      output_stream << "\"" << escape_json2(object.extract<const std::string>()) << "\"";
   } else if (object.isInteger()) {
      const int val = object;
      output_stream << val;
   } else if (object.isNumeric()) {
      const auto val = object.extract<double>();
      output_stream << val;
   } else if (object.isSigned()) {
      output_stream << "\"[Signed]\"";
   } else {
      if (object.type() == typeid(DateTime)) {
         const auto& dt = object.extract<DateTime>();
         output_stream << fmt::format("\"{}/{}/{}\"", dt.year(), dt.month(), dt.day());
      } else if (object.type() == typeid(Int16)) {
         output_stream << object.extract<const Int16>();
      } else if (object.type() == typeid(std::shared_ptr<IFlintData>)) {
         output_stream << "\"[IFlintData\"]";
      } else
         output_stream << "\"[Unknown\"]";
   }
}

void OutputerStream::configure(const DynamicObject& config) {
   file_name_ =
       config.contains("output_filename") ? config["output_filename"].extract<std::string>() : "OutputerStreamFlux.txt";
   output_to_screen_ = config.contains("output_to_screen") ? bool(config["output_to_screen"]) : true;
   output_info_header_ = config.contains("output_info_header") ? bool(config["output_info_header"]) : true;
   output_annually_ = config.contains("output_annual_only") ? bool(config["output_annual_only"]) : false;
   output_on_output_step_ = config.contains("output_on_outputstep") ? bool(config["output_on_outputstep"]) : true;
   output_on_timing_end_step_ =
       config.contains("output_on_timingendstep") ? bool(config["output_on_timingendstep"]) : false;
   output_on_post_disturbance_event_ =
       config.contains("output_on_postdisturbanceevent") ? bool(config["output_on_postdisturbanceevent"]) : false;
   cohort_aggregation_enabled_ =
       config.contains("cohort_aggregation_enabled") ? bool(config["cohort_aggregation_enabled"]) : true;
   cohort_aggregation_show_nested_values_ = config.contains("cohort_aggregation_show_nested_values")
                                                ? bool(config["cohort_aggregation_show_nested_values"])
                                                : true;
   if (!cohort_aggregation_enabled_) {
      cohort_aggregation_show_nested_values_ = true;
   }
   cohort_name_format_ = name_format::short_name;
   if (config.contains("cohort_name_format")) {
      std::string format = config["cohort_name_format"];
      if (boost::iequals(format, "long")) {
         cohort_name_format_ = name_format::long_name;
      }
   }

   if (config.contains("variables")) {
      // assume we have a vector of variables to include in output
      const auto& items = config["variables"].extract<DynamicVector>();
      for (auto& item : items) {
         const auto& var = item.extract<const std::string>();
         auto property_separator = var.find_first_of('.');
         if (property_separator != std::string::npos) {
            auto var_prop = var.substr(property_separator + 1, var.length());
            auto var_name = var.substr(0, property_separator);
            variables_.emplace_back(var, var_name, var_prop, nullptr);
         } else {
            variables_.emplace_back(var, var, "", nullptr);
         }
      }
   }
};

void OutputerStream::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &OutputerStream::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &OutputerStream::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &OutputerStream::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::TimingPostInit, &OutputerStream::onTimingPostInit, *this);
   notificationCenter.subscribe(signals::OutputStep, &OutputerStream::onOutputStep, *this);
   notificationCenter.subscribe(signals::TimingEndStep, &OutputerStream::onTimingEndStep, *this);
   notificationCenter.subscribe(signals::PostDisturbanceEvent, &OutputerStream::onPostDisturbanceEvent, *this);
}

std::string OutputerStream::get_pool_name(const IPool* pool) const {
   std::stringstream ss;
   if (pool != nullptr) {
      if (!pool->is_child() || cohort_name_format_ == name_format::short_name) {
         ss << pool->name();
      } else {
         auto* p = pool->parent();
         std::vector<std::string> names;
         while (p != nullptr) {
            names.emplace_back(p->name());
            p = p->parent();
         }
         std::reverse(names.begin(), names.end());
         ss << fmt::format("{}", fmt::join(names, ".")) << "." << pool->name();
      }
   }
   return ss.str();
}

void OutputerStream::outputHeader(std::ostream& stream) const {
   if (output_info_header_) {
      DateTime start = DateTime::now();

      stream << "OutputerStream" << std::endl;
      stream << "==========================================================================" << std::endl;
      stream << "Started:" << start << std::endl;
      stream << "==========================================================================" << std::endl;
   }
   stream << "notification" << DL_CHR << "step" << DL_CHR << "stepDate" << DL_CHR << "fracOfStep" << DL_CHR
          << "stepLenInYears" << DL_CHR;

   for (auto var : variables_) {
      stream << std::get<0>(var) << DL_CHR;
   }
   auto pools = _landUnitData->poolCollection();
   for (auto& it : pools) {
      stream << it->name() << DL_CHR;
   }
   stream << std::endl;
}

void OutputerStream::outputOnNotification(const std::string& notification, std::ostream& stream) {
   const auto timing = _landUnitData->timing();

   stream << notification << DL_CHR << timing->step() << DL_CHR << timing->curStartDate() << DL_CHR
          << timing->fractionOfStep() << DL_CHR << timing->stepLengthInYears() << DL_CHR
          << std::setprecision(STOCK_PRECISION);

   for (const auto& [var, var_name, var_prop, var_ptr] : variables_) {
      if (var_ptr == nullptr) {
         stream << "(missing variable)";
      } else if (!var_prop.empty()) {
         auto& var_value = var_ptr->value();
         if (var_value.type() == typeid(std::shared_ptr<IFlintData>)) {
            auto flint_data_variable = var_ptr->value().extract<std::shared_ptr<IFlintData>>();
            auto prop_value = flint_data_variable->getProperty(var_prop);
            outputDynamicToStream(stream, prop_value);
         } else {
            if (var_value.isStruct()) {
               auto var_struct = var_value.extract<const DynamicObject>();
               auto var_struct_prop = var_struct.contains(var_prop) ? var_struct[var_prop] : DynamicVar();
               outputDynamicToStream(stream, var_struct_prop);
            } else
               outputDynamicToStream(stream, var_value);
         }
      } else {
         // TODO: extend this to do property of array/struct objects
         outputDynamicToStream(stream, var_ptr->value());
      }
      stream << DL_CHR;
   }
   auto pools = _landUnitData->poolCollection();
   for (auto& pool : pools) {
      if ((pool->has_children() && cohort_aggregation_enabled_) || (!pool->has_children() && !pool->is_child()) ||
          (pool->is_child() && cohort_aggregation_show_nested_values_)) {
         stream << pool->value() << DL_CHR;
      }
   }
   stream << std::endl;
}

void OutputerStream::outputShutdown(std::ostream& stream) {
   if (output_info_header_) {
      stream << "==========================================================================" << std::endl;
      stream << "notification" << DL_CHR << "step" << DL_CHR << "stepDate" << DL_CHR << "fracOfStep" << DL_CHR
             << "stepLenInYears" << DL_CHR;

      for (auto var : variables_) {
         stream << std::get<0>(var) << DL_CHR;
      }
      auto pools = _landUnitData->poolCollection();
      for (auto& pool : pools) {
         stream << get_pool_name(pool.get()) << DL_CHR;
      }
      stream << std::endl << std::endl;

      DateTime finish = DateTime::now();
      stream << "==========================================================================" << std::endl;
      stream << "Finished:" << finish << std::endl;
      stream << "==========================================================================" << std::endl;
   }
}

void OutputerStream::onSystemInit() {
   namespace fs = std::filesystem;

   const auto output_file = fs::path(file_name_);
   if (fs::exists(output_file)) fs::remove(output_file);

   stream_file_.open(file_name_, std::ios::out);
   output_.addStream(stream_file_);
   if (output_to_screen_) output_.addStream(std::cout);
   outputHeader(output_);
}

void OutputerStream::onSystemShutdown() {
   outputShutdown(output_);
   stream_file_.close();
}

void OutputerStream::onLocalDomainInit() {
   for (auto& [var, var_name, var_prop, var_ptr] : variables_) {
      if (_landUnitData->hasVariable(var_name))
         var_ptr = _landUnitData->getVariable(var_name);
      else
         var_ptr = nullptr;
   }
}

void OutputerStream::onTimingPostInit() { outputOnNotification("onTimingPostInit", output_); }

void OutputerStream::onOutputStep() {
   if (output_on_output_step_) {
      const auto* timing = _landUnitData->timing();
      if (!output_annually_ || timing->curStartDate().month() == 12) {
         outputOnNotification("onOutputStep", output_);
      }
   }
}

void OutputerStream::onTimingEndStep() {
   if (output_on_timing_end_step_) {
      outputOnNotification("onTimingEndStep", output_);
   }
}

void OutputerStream::onPostDisturbanceEvent() {
   if (output_on_post_disturbance_event_) {
      outputOnNotification("onPostDisturbanceEvent", output_);
   }
}

}  // namespace moja::flint
