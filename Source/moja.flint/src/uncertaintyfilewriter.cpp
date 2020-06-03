#include "moja/flint/uncertaintyfilewriter.h"

#include "moja/flint/aggregatorfilewriter.h"
#include "moja/flint/ioperationresult.h"
#include "moja/flint/ipool.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/uncertaintysimulationdata.h"
#include "moja/flint/uncertaintyvariable.h"
#include "moja/logging.h"

#include <moja/datetime.h>
#include <moja/floatcmp.h>
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
   flux_file_name_ = config.contains("flux_file_name") ? config["flux_file_name"].extract<std::string>()
                                                       : "OutputerUncertaintyFlux.csv";
   write_stock_ = false;
   if (config.contains("stock_file_name")) {
      stock_file_name_ = config["stock_file_name"].extract<std::string>();
      write_stock_ = true;
   }

   output_to_screen_ = config.contains("output_to_screen") ? bool(config["output_to_screen"]) : true;
   output_info_header_ = config.contains("output_info_header") ? bool(config["output_info_header"]) : true;
   confidence_interval_ = config.contains("confidence_interval")
                              ? str_to_confidence_interval(config["confidence_interval"].extract<std::string>())
                              : confidence_interval::ninety_percent;
   include_raw_data_ = config.contains("include_raw_data") ? bool(config["include_raw_data"]) : true;
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

// Calculations match excel STDEVP.P function
// https://support.microsoft.com/en-us/office/stdev-p-function-6e917c05-31a0-496f-ade7-4f4e7462f285?ns=excel&version=90&syslcid=1033&uilcid=1033&appver=zxl900&helpid=xlmain11.chm60559&ui=en-us&rs=en-us&ad=us

double st_dev(const std::vector<double>& data) {
   double sum = std::accumulate(data.begin(), data.end(), 0.0);
   double mean = sum / data.size();

   std::vector<double> diff(data.size());
   // std::transform(v.begin(), data.end(), diff.begin(), std::bind2nd(std::minus<double>(), mean));
   std::transform(data.begin(), data.end(), diff.begin(), [mean](double x) { return x - mean; });
   double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
   double stdev = std::sqrt(sq_sum / data.size());

   return stdev;
}

// can produce incorrect result if variance is very small because of rounding errors
double st_dev_old(const std::vector<double>& data) {
   auto _variance = variance(data);

   if (FloatCmp::lessThan(_variance, 0.0)) {
      return 0.0;
   }
   return std::sqrt(_variance);
}

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

// TODO: look inot boost methods for this:
// we could provided options in modulke settings to select distribution used to calculate confidence intervals
// https://www.boost.org/doc/libs/1_59_0/libs/math/doc/html/math_toolkit/stat_tut/weg/cs_eg/chi_sq_intervals.html

// Current margin of error (confidence) matches closely to excel func CONFIDENCE.NORM
// https://support.microsoft.com/en-us/office/confidence-norm-function-7cec58a6-85bb-488d-91c3-63828d4fbfd4?ns=excel&version=90&syslcid=1033&uilcid=1033&appver=zxl900&helpid=xlmain11.chm60536&ui=en-us&rs=en-us&ad=us

std::string UncertaintyFileWriter::record_to_string_func(const UncertaintyFluxRow& rec, const std::string& dl,
                                                         const IPool* src_pool, const IPool* dst_pool) const {
   const auto& fluxes = rec.get<6>();
   auto ave = mean(fluxes);
   auto stdev = st_dev(fluxes);
   auto Z = confidence_interval_to_Z(confidence_interval_);
   auto margin_of_error = Z * stdev / sqrt(fluxes.size());
   if (include_raw_data_) {
      return fmt::format("{1}{0}{2}{0}{3}{0}{4}{0}{5}{0}{6}{0}{7}{0}{8}{0}{9}", dl, rec.get<1>(), src_pool->name(),
                         dst_pool->name(), ave, stdev, margin_of_error, ave - margin_of_error, ave + margin_of_error,
                         fmt::join(fluxes, dl));
   }
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
      const auto tuples = simulation_unit_data_->flux_results.getTupleCollection();
      MOJA_LOG_DEBUG << simulation_unit_data_->local_domain_id << ":File writer for Uncertainty flux aggregation)";

      Poco::ScopedLockWithUnlock<Poco::Mutex> lock(file_mutex_);

      Poco::FileOutputStream streamFile(flux_file_name_, std::ios::ate);
      Poco::TeeOutputStream output(streamFile);
      if (output_to_screen_) output.addStream(std::cout);

      for (auto& rec : tuples) {
         auto src_pool = _landUnitData->getPool(rec.get<4>());
         auto dst_pool = _landUnitData->getPool(rec.get<5>());
         auto str = record_to_string_func(rec, DL_CHR, src_pool, dst_pool);

         std::string date_str;
         auto date_rec_id = rec.get<1>();
         auto stored_date_record = date_dimension_->searchId(date_rec_id);
         if (stored_date_record != nullptr) {
            auto date_rec = stored_date_record->asPersistable();
            date_str = record_to_string_func(date_rec, DL_CHR);
         }
         output << date_str << DL_CHR;

         std::string module_info_str;
         auto module_info_rec_id = rec.get<2>();
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
   if (!write_stock_) return;
   try {
      const auto tuples = simulation_unit_data_->stock_results.getTupleCollection();
      MOJA_LOG_DEBUG << simulation_unit_data_->local_domain_id << ":File writer for Uncertainty stock aggregation)";

      Poco::ScopedLockWithUnlock<Poco::Mutex> lock(file_mutex_);

      Poco::FileOutputStream streamFile(stock_file_name_, std::ios::ate);
      Poco::TeeOutputStream output(streamFile);
      if (output_to_screen_) output.addStream(std::cout);

      for (auto& rec : tuples) {
         const auto date_rec_id = std::get<1>(rec);
         const auto location_id = std::get<2>(rec);
         const auto pool_id = std::get<3>(rec);
         const auto pool = _landUnitData->getPool(int(pool_id));
         const auto& values = std::get<4>(rec);
         const auto count = std::get<5>(rec);
         const auto ave = mean(values);
         const auto stdev = st_dev(values);
         const auto Z = confidence_interval_to_Z(confidence_interval_);
         const auto margin_of_error = Z * stdev / sqrt(values.size());
         std::string str;
         if (include_raw_data_) {
            str = fmt::format("{1}{0}{2}{0}{3}{0}{4}{0}{5}{0}{6}{0}{7}{0}{8}{0}{9}", DL_CHR, location_id, pool->name(),
                              ave, stdev, margin_of_error, ave - margin_of_error, ave + margin_of_error, count,
                              fmt::join(values, DL_CHR));
         } else {
            str = fmt::format("{1}{0}{2}{0}{3}{0}{4}{0}{5}{0}{6}{0}{7}{0}{8}", DL_CHR, location_id, pool->name(), ave,
                              stdev, margin_of_error, ave - margin_of_error, ave + margin_of_error, count);
         }
         std::string date_str;

         auto stored_date_record = date_dimension_->searchId(date_rec_id);
         if (stored_date_record != nullptr) {
            auto date_rec = stored_date_record->asPersistable();
            date_str = record_to_string_func(date_rec, DL_CHR);
         }
         output << date_str << DL_CHR;
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

UncertaintyFileWriter::confidence_interval UncertaintyFileWriter::str_to_confidence_interval(
    const std::string& confidence_interval) {
   if (confidence_interval == "eighty_percent") return confidence_interval::eighty_percent;
   if (confidence_interval == "eighty_five_percent") return confidence_interval::eighty_five_percent;
   if (confidence_interval == "ninety_percent") return confidence_interval::ninety_percent;
   if (confidence_interval == "ninety_five_percent") return confidence_interval::ninety_five_percent;
   if (confidence_interval == "ninety_nine_percent") return confidence_interval::ninety_nine_percent;
   return confidence_interval::ninety_percent;
}

std::string UncertaintyFileWriter::confidence_interval_to_str(confidence_interval confidence_interval) {
   switch (confidence_interval) {
      case confidence_interval::eighty_percent:
         return "80%";
      case confidence_interval::eighty_five_percent:
         return "85%";
      case confidence_interval::ninety_percent:
         return "90%";
      case confidence_interval::ninety_five_percent:
         return "95%";
      case confidence_interval::ninety_nine_percent:
         return "99%";
      default:
         return "";
   }
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

void UncertaintyFileWriter::write_flux_header() const {
   // create file here and append flux data later, this will work in threaded mode as well
   // file created on onSystemInit, shared Mutex around file access used to append to it.
   Poco::ScopedLockWithUnlock<Poco::Mutex> lock(file_mutex_);
   Poco::File outputFile(flux_file_name_);
   if (outputFile.exists()) outputFile.remove();
   outputFile.createFile();
   if (output_info_header_) {
      Poco::FileOutputStream streamFile(flux_file_name_);
      Poco::TeeOutputStream output(streamFile);
      if (output_to_screen_) output.addStream(std::cout);

      output << "year" << DL_CHR;

      if (module_info_on_) {
         output << "module_id" << DL_CHR << "library_type" << DL_CHR << "library_info_id" << DL_CHR << "module_type"
                << DL_CHR << "module_id" << DL_CHR << "module_name" << DL_CHR << "disturbance_type" << DL_CHR;
      }

      output << "localdomain_id" << DL_CHR << "src pool" << DL_CHR << "sink_pool" << DL_CHR << "mean" << DL_CHR
             << "S.D." << DL_CHR << "margin of error" << DL_CHR << confidence_interval_to_str(confidence_interval_)
             << " limit low" << DL_CHR << confidence_interval_to_str(confidence_interval_) << " limit high";
      if (include_raw_data_) {
         output << DL_CHR;
         for (int i = 0; i < _landUnitData->uncertainty().iterations(); ++i) {
            output << i << DL_CHR;
         }
      }
      output << std::endl;

      streamFile.close();
   }
}
void UncertaintyFileWriter::write_stock_header() const {
   if (!write_stock_) return;

   Poco::ScopedLockWithUnlock<Poco::Mutex> lock(file_mutex_);
   Poco::File outputFile(stock_file_name_);
   if (outputFile.exists()) outputFile.remove();
   outputFile.createFile();
   if (output_info_header_) {
      Poco::FileOutputStream streamFile(stock_file_name_);
      Poco::TeeOutputStream output(streamFile);
      if (output_to_screen_) output.addStream(std::cout);

      output << "year" << DL_CHR << "localdomain_id" << DL_CHR << "pool" << DL_CHR << "mean" << DL_CHR << "S.D."
             << DL_CHR << "margin of error" << DL_CHR << confidence_interval_to_str(confidence_interval_)
             << " limit low" << DL_CHR << confidence_interval_to_str(confidence_interval_) << " limit high" << DL_CHR
             << "count";
      if (include_raw_data_) {
         output << DL_CHR;
         for (int i = 0; i < _landUnitData->uncertainty().iterations(); ++i) {
            output << i << DL_CHR;
         }
      }
      output << std::endl;

      streamFile.close();
   }
}

void UncertaintyFileWriter::onSystemInit() {
   try {
      write_flux_header();
      write_stock_header();
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
