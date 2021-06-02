#pragma once

#include "moja/flint/modulebase.h"

#include <Poco/FileStream.h>
#include <Poco/TeeStream.h>

#include <string>

namespace moja::flint {

class FLINT_API OutputerStream : public ModuleBase {
  public:
   enum class name_format { short_name = 0, long_name };

   OutputerStream()
       : ModuleBase(),
         output_to_screen_(false),
         output_annually_(false),
         output_info_header_(false),
         output_on_output_step_(true),
         output_on_timing_end_step_(false),
         output_on_post_disturbance_event_(false),
         cohort_aggregation_enabled_(true),
         cohort_aggregation_show_nested_values_(true),
         cohort_name_format_(name_format::short_name) {}

   virtual ~OutputerStream() {}

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onSystemInit() override;
   void onSystemShutdown() override;
   void onLocalDomainInit() override;
   void onTimingPostInit() override;
   void onOutputStep() override;
   void onTimingEndStep() override;
   void onPostDisturbanceEvent() override;

  private:
   static constexpr auto DL_CHR = ",";
   static constexpr auto STOCK_PRECISION = 15;

   std::string get_pool_name(const IPool* pool) const;

   void outputHeader(std::ostream& stream) const;
   void outputOnNotification(const std::string& notification, std::ostream& stream);
   void outputShutdown(std::ostream& stream);

   std::string file_name_;
   bool output_to_screen_;
   bool output_annually_;
   bool output_info_header_;
   bool output_on_output_step_;
   bool output_on_timing_end_step_;
   bool output_on_post_disturbance_event_;
   bool cohort_aggregation_enabled_;
   bool cohort_aggregation_show_nested_values_;
   name_format cohort_name_format_;

   std::vector<std::tuple<std::string, std::string, std::string, IVariable*>>
       variables_;  // including sub property if it has one

   Poco::FileOutputStream stream_file_;
   Poco::TeeOutputStream output_;
};

}  // namespace moja::flint
