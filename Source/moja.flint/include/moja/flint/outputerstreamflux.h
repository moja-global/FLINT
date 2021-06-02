#pragma once

#include "moja/flint/modulebase.h"

#include <Poco/FileStream.h>
#include <Poco/TeeStream.h>

#include <string>

namespace moja::flint {

class FLINT_API OutputerStreamFlux : public ModuleBase {
  public:
   enum class name_format { short_name = 0, long_name };

   explicit OutputerStreamFlux()
       : ModuleBase(),
         output_to_screen_(false),
         output_info_header_(false),
         cohort_show_parent_name_(false),
         cohort_name_format_(name_format::short_name) {}

   virtual ~OutputerStreamFlux() {}

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onSystemInit() override;
   void onSystemShutdown() override;
   void onTimingPostInit() override;
   void onTimingEndStep() override;
   void onPostDisturbanceEvent() override;



  private:
   static constexpr auto DL_CHR = ",";
   static constexpr auto STOCK_PRECISION = 15;

   void outputOnNotification(const std::string& notification, std::ostream& stream) const;
   void outputShutdown(std::ostream& stream) const;
   void outputHeader(std::ostream& stream) const;
   std::string get_pool_name(const IPool* pool) const;

   std::string file_name_;
   bool output_to_screen_;
   bool output_info_header_;
   bool cohort_show_parent_name_;
   name_format cohort_name_format_;

   Poco::FileOutputStream _streamFile;
   Poco::TeeOutputStream _output;
};

}  // namespace moja::flint
