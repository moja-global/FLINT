#include "moja/flint/uncertaintyfilewriter.h"

#include "moja/flint/aggregatorfilewriter.h"
#include "moja/flint/ioperationresult.h"
#include "moja/flint/ipool.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/uncertaintysimulationdata.h"
#include "moja/logging.h"

#include <moja/datetime.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/TeeStream.h>

#include <fmt/format.h>

#include <iomanip>  // std::setprecision
#include <iostream>
#include <numeric>

namespace moja::flint {

constexpr auto DL_CHR = ",";
constexpr auto STOCK_PRECISION = 15;

void UncertaintyFileWriter::configure(const DynamicObject& config) {
   file_name_ = config.contains("output_filename") ? config["output_filename"].extract<std::string>()
                                                   : "OutputerUncertainty.txt";
   output_to_screen_ = config.contains("output_to_screen") ? bool(config["output_to_screen"]) : true;
   output_info_header_ = config.contains("output_info_header") ? bool(config["output_info_header"]) : true;
   confidence_interval_ = config.contains("confidence_interval")
                              ? str_to_confidence_interval(config["confidence_interval"].extract<std::string>())
                              : confidence_interval::ninety_percent;
}

void UncertaintyFileWriter::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &UncertaintyFileWriter::onSystemInit, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &UncertaintyFileWriter::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainShutdown, &UncertaintyFileWriter::onLocalDomainShutdown, *this);
}

double mean(const std::vector<double>& data) { return std::accumulate(data.begin(), data.end(), 0.0) / data.size(); }

double variance(const std::vector<double>& data) {
   const double x_bar = mean(data);
   const double sq_sum = std::inner_product(data.begin(), data.end(), data.begin(), 0.0);
   return sq_sum / data.size() - x_bar * x_bar;
}

double st_dev(const std::vector<double>& data) { return std::sqrt(variance(data)); }

static std::string vec_to_string_func(const std::vector<double>& vec) {
   std::ostringstream out;
   out << "[";
   if (!vec.empty()) {
      std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<double>(out, ","));
      out << vec.back();
   }
   out << "]";
   return out.str();
}

std::string UncertaintyFileWriter::record_to_string_func(const UncertaintyFluxRow& rec, const std::string& dl,
                                                         const IPool* src_pool,
                                                         const IPool* dst_pool) const {
   const auto& fluxes = rec.get<7>();
   auto ave = mean(fluxes);
   auto stdev = st_dev(fluxes);
   auto Z = confidence_interval_to_Z(confidence_interval_);
   auto margin_of_error = Z * stdev / sqrt(fluxes.size());
   return fmt::format("{1}{0}{2}{0}{3}{0}{4}{0}{5}{0}{6}{0}{7}{0}{8}", dl, rec.get<1>(), src_pool->name(),
                      dst_pool->name(), ave, stdev, margin_of_error, ave - margin_of_error, ave + margin_of_error);
}

std::string UncertaintyFileWriter::record_to_string_func(const Date2Row& rec, const std::string& dl) {
   return fmt::format("{}", rec.get<1>());
}

std::string UncertaintyFileWriter::record_to_string_func(const ModuleInfoRow& rec, const std::string& dl) {
   return (fmt::format("{1}{0}{2}{0}{3}{0}{4}{0}{5}{0}{6}{0}", dl, rec.get<0>(), rec.get<1>(), rec.get<2>(),
                       rec.get<3>(), rec.get<4>(), rec.get<5>()));
}

void UncertaintyFileWriter::write_flux() const {
   try {
      const auto persistables = simulation_unit_data_->flux_results.getPersistableCollection();
      MOJA_LOG_DEBUG << simulation_unit_data_->local_domain_id << ":File writer for Uncertainty aggregation)";

      Poco::ScopedLockWithUnlock<Poco::Mutex> lock(file_mutex_);

      Poco::FileOutputStream streamFile(file_name_, std::ios::ate);
      Poco::TeeOutputStream output(streamFile);
      if (output_to_screen_) output.addStream(std::cout);

      for (auto& rec : persistables) {
         auto src_pool = _landUnitData->getPool(rec.get<5>());
         auto dst_pool = _landUnitData->getPool(rec.get<6>());
         auto str = record_to_string_func(rec, DL_CHR, src_pool, dst_pool);

         std::string date_str;
         Int64 date_rec_id = rec.get<2>();
         auto stored_date_record = date_dimension_->searchId(date_rec_id);
         if (stored_date_record != nullptr) {
            auto date_rec = stored_date_record->asPersistable();
            date_str = record_to_string_func(date_rec, DL_CHR);
         }
         output << date_str << DL_CHR;

         std::string module_info_str;
         auto module_info_rec_id = rec.get<3>();
         if (module_info_on_) {
            auto stored_module_info_record = module_info_dimension_->searchId(module_info_rec_id);
            if (stored_module_info_record != nullptr) {
               auto module_info_rec = stored_module_info_record->asPersistable();
               module_info_str = record_to_string_func(module_info_rec, DL_CHR);
            }
            output << module_info_str << DL_CHR;
         }
         output << str << std::endl;
      }
      streamFile.close();
   } catch (Poco::FileException& exc) {
      MOJA_LOG_DEBUG << simulation_unit_data_->local_domain_id << ":FileException - " << exc.displayText();
      throw;
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << simulation_unit_data_->local_domain_id << ":AssertionViolationException - "
                     << exc.displayText();
      throw;
   } catch (const std::exception& e) {
      MOJA_LOG_DEBUG << simulation_unit_data_->local_domain_id << ":Unknown exception: - " << e.what();
      throw;
   } catch (...) {
      MOJA_LOG_DEBUG << simulation_unit_data_->local_domain_id << ":Unknown Exception";
      throw;
   }
}

void UncertaintyFileWriter::write_stock() const {
   const auto persistables = simulation_unit_data_->stock_results.getTupleCollection();
   for (auto& rec : persistables) {
      //_id, date_id, location_id, pool_id, values, item_count

      const auto date_rec_id = std::get<1>(rec);
      const auto pool = _landUnitData->getPool(std::get<3>(rec));
      const auto& values = std::get<4>(rec);
      auto count = std::get<5>(rec);
   }
}

UncertaintyFileWriter::confidence_interval UncertaintyFileWriter::str_to_confidence_interval(
    const std::string& confidence_interval) {
   if (confidence_interval == "eighty_percent") return confidence_interval::eighty_percent;
   if (confidence_interval == "eighty_five_percent") return confidence_interval::eighty_five_percent;
   if (confidence_interval == "ninety_percent") return confidence_interval::ninety_percent;
   if (confidence_interval == "ninety_five_percent") return confidence_interval::ninety_five_percent;
   if (confidence_interval == "ninety_nine_percent") return confidence_interval::ninety_nine_percent;
   return confidence_interval::ninety_percent;
}

double UncertaintyFileWriter::confidence_interval_to_Z(confidence_interval confidence_interval) {
   switch (confidence_interval) {
      case confidence_interval::eighty_percent:
         return 1.282;
      case confidence_interval::eighty_five_percent:
         return 1.440;
      case confidence_interval::ninety_percent:
         return 1.645;
      case confidence_interval::ninety_five_percent:
         return 1.960;
      case confidence_interval::ninety_nine_percent:
         return 2.576;
      default:;
         return 1.645;
   }
}

void UncertaintyFileWriter::onSystemInit() {
   try {
      // create file here and append flux data later, this will work in threaded mode as well
      // file created on onSystemInit, shared Mutex around file access used to append to it.
      Poco::ScopedLockWithUnlock<Poco::Mutex> lock(file_mutex_);
      Poco::File outputFile(file_name_);
      if (outputFile.exists()) outputFile.remove();
      outputFile.createFile();
      if (output_info_header_) {
         Poco::FileOutputStream streamFile(file_name_);
         Poco::TeeOutputStream output(streamFile);
         if (output_to_screen_) output.addStream(std::cout);

         output << "year" << DL_CHR;

         if (module_info_on_) {
            output << "module_id" << DL_CHR << "library_type" << DL_CHR << "library_info_id" << DL_CHR << "module_type"
                   << DL_CHR << "module_id" << DL_CHR << "module_name" << DL_CHR << "disturbance_type" << DL_CHR;
         }

         output << "localdomain_id" << DL_CHR << "src pool" << DL_CHR << "sink_pool" << DL_CHR << "mean" << DL_CHR
                << "S.D." << DL_CHR << "margin of error" << DL_CHR << "90% limit low" << DL_CHR << "90% limit high"
                << std::endl;
         streamFile.close();
      }
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << -1 << ":AssertionViolationException - " << exc.displayText();
      throw;
   } catch (const std::exception& e) {
      MOJA_LOG_DEBUG << -1 << ":Unknown exception: - " << e.what();
      throw;
   } catch (...) {
      MOJA_LOG_DEBUG << -1 << ":Unknown exception";
      throw;
   }
}

void UncertaintyFileWriter::onLocalDomainInit() {
   simulation_unit_data_ = std::static_pointer_cast<UncertaintySimulationUnitData>(
       _landUnitData->getVariable("uncertaintySimulationUnitData")->value().extract<std::shared_ptr<IFlintData>>());
}

void UncertaintyFileWriter::onLocalDomainShutdown() {
   write_flux();
   write_stock();
}

}  // namespace moja::flint
