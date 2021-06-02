#include "moja/flint/outputerstreamflux.h"

#include "moja/flint/ioperationresult.h"
#include "moja/flint/ioperationresultflux.h"
#include "moja/flint/ipool.h"
#include "moja/flint/itiming.h"
#include "moja/flint/operationdatapackage.h"

#include <moja/datetime.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <boost/algorithm/string.hpp>

#include <fmt/format.h>

#include <filesystem>
#include <iomanip>  // std::setprecision
#include <iostream>

namespace moja::flint {

void OutputerStreamFlux::configure(const DynamicObject& config) {
   file_name_ =
       config.contains("output_filename") ? config["output_filename"].extract<std::string>() : "OutputerStreamFlux.txt";
   output_to_screen_ = config.contains("output_to_screen") ? static_cast<bool>(config["output_to_screen"]) : true;
   output_info_header_ =
       config.contains("output_info_header") ? static_cast<bool>(config["output_info_header"]) : false;
   cohort_show_parent_name_ =
       config.contains("cohort_show_parent_name") ? bool(config["cohort_show_parent_name"]) : true;
   cohort_name_format_ = name_format::short_name;
   if (config.contains("cohort_name_format")) {
      std::string format = config["cohort_name_format"];
      if (boost::iequals(format, "long")) {
         cohort_name_format_ = name_format::long_name;
      }
   }
}

void OutputerStreamFlux::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &OutputerStreamFlux::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &OutputerStreamFlux::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::TimingPostInit, &OutputerStreamFlux::onTimingPostInit, *this);
   notificationCenter.subscribe(signals::TimingEndStep, &OutputerStreamFlux::onTimingEndStep, *this);
   notificationCenter.subscribe(signals::PostDisturbanceEvent, &OutputerStreamFlux::onPostDisturbanceEvent, *this);
}

void OutputerStreamFlux::outputHeader(std::ostream& stream) const {
   if (output_info_header_) {
      DateTime start = DateTime::now();

      stream << "OutputerStreamFlux" << std::endl;
      stream << "==========================================================================" << std::endl;
      stream << "Started:" << start << std::endl;
      stream << "==========================================================================" << std::endl;
   }
   stream << "step" << DL_CHR << "step_date" << DL_CHR << "module_name" << DL_CHR << "disturbance_type";
   if (cohort_show_parent_name_) stream << DL_CHR << "source_parent_pool";
   stream << DL_CHR << "source_pool";
   if (cohort_show_parent_name_) stream << DL_CHR << "sink_parent_pool";
   stream << DL_CHR << "sink_pool" << DL_CHR << "value" << std::endl;
}

std::string OutputerStreamFlux::get_pool_name(const IPool* pool) const {
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

void OutputerStreamFlux::outputOnNotification(const std::string& notification, std::ostream& stream) const {
   const auto& timing = _landUnitData->timing();
   for (auto operation_result : _landUnitData->getOperationLastAppliedIterator()) {
      const auto* meta_data = operation_result->metaData();
      for (auto flux : operation_result->operationResultFluxCollection()) {
         const auto src_ix = flux->source();
         const auto dst_ix = flux->sink();
         if (src_ix == dst_ix)  // don't process diagonal
            continue;

         const auto flux_value = flux->value();
         const auto src_pool = _landUnitData->getPool(src_ix);
         const auto dst_pool = _landUnitData->getPool(dst_ix);

         std::string flux_type_info = "Unclassified";
         if (operation_result->hasDataPackage()) {
            const auto data_packet = operation_result->dataPackage().extract<std::shared_ptr<OperationDataPackage>>();
            flux_type_info = data_packet->to_string();
         }

         stream << timing->step() << DL_CHR << timing->curStartDate() << DL_CHR << meta_data->moduleName << DL_CHR
                << flux_type_info;
         if (cohort_show_parent_name_) {
            stream << DL_CHR << get_pool_name(src_pool->parent());
         }
         stream << DL_CHR << get_pool_name(src_pool);
         if (cohort_show_parent_name_) {
            stream << DL_CHR << get_pool_name(dst_pool->parent());
         }
         stream << DL_CHR << get_pool_name(dst_pool) << DL_CHR << std::setprecision(STOCK_PRECISION) << flux_value
                << std::endl;
      }
   }
}

void OutputerStreamFlux::outputShutdown(std::ostream& stream) const {
   if (output_info_header_) {
      DateTime finish = DateTime::now();
      stream << "==========================================================================" << std::endl;
      stream << "Finished:" << finish << std::endl;
      stream << "==========================================================================" << std::endl;
   }
}

void OutputerStreamFlux::onSystemInit() {
   namespace fs = std::filesystem;

   const auto output_file = fs::path(file_name_);
   if (fs::exists(output_file)) fs::remove(output_file);

   _streamFile.open(file_name_, std::ios::out);
   _output.addStream(_streamFile);
   if (output_to_screen_) _output.addStream(std::cout);
   outputHeader(_output);
}

void OutputerStreamFlux::onSystemShutdown() {
   outputShutdown(_output);
   _streamFile.close();
}

void OutputerStreamFlux::onTimingPostInit() { outputOnNotification("onTimingPostInit", _output); }

void OutputerStreamFlux::onTimingEndStep() { outputOnNotification("onTimingEndStep", _output); }

void OutputerStreamFlux::onPostDisturbanceEvent() { outputOnNotification("onPostDisturbanceEvent", _output); }

}  // namespace moja::flint
