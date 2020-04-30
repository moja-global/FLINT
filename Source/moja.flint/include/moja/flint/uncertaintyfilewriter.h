#pragma once

#include "moja/flint/modulebase.h"
#include "moja/flint/recordaccumulatorwithmutex.h"

#include <string>

namespace moja::flint {
class UncertaintySimulationUnitData;

class FLINT_API UncertaintyFileWriter : public ModuleBase {
  public:
   enum class confidence_interval {
      eighty_percent,
      eighty_five_percent,
      ninety_percent,
      ninety_five_percent,
      ninety_nine_percent
   };

   explicit UncertaintyFileWriter(Poco::Mutex& file_mutex,
                                  std::shared_ptr<RecordAccumulatorWithMutex<Date2Row>> date_dimension,
                                  std::shared_ptr<RecordAccumulatorWithMutex<PoolInfoRow>> pool_info_dimension,
                                  std::shared_ptr<RecordAccumulatorWithMutex<ModuleInfoRow>> module_info_dimension)
       : ModuleBase(),
         file_mutex_(file_mutex),
         date_dimension_(date_dimension),
         pool_info_dimension_(pool_info_dimension),
         module_info_dimension_(module_info_dimension),
         output_to_screen_(false),
         output_info_header_(false),
         module_info_on_(false),
         confidence_interval_(confidence_interval::ninety_percent) {}

   virtual ~UncertaintyFileWriter() {}

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onSystemInit() override;
   void onLocalDomainInit() override;
   void onLocalDomainShutdown() override;

  private:
   std::string record_to_string_func(const UncertaintyFluxRow& rec, const std::string& dl, const IPool* src_pool,
                                            const IPool* dst_pool) const;
   static std::string record_to_string_func(const Date2Row& rec, const std::string& dl);
   static std::string record_to_string_func(const ModuleInfoRow& rec, const std::string& dl);
   void write_flux() const;
   static confidence_interval str_to_confidence_interval(const std::string& confidence_interval);
   static double confidence_interval_to_Z(confidence_interval confidence_interval);

   Poco::Mutex& file_mutex_;
   std::shared_ptr<RecordAccumulatorWithMutex<Date2Row>> date_dimension_;
   std::shared_ptr<RecordAccumulatorWithMutex<PoolInfoRow>> pool_info_dimension_;
   std::shared_ptr<RecordAccumulatorWithMutex<ModuleInfoRow>> module_info_dimension_;

   std::shared_ptr<UncertaintySimulationUnitData> simulation_unit_data_;

   std::string file_name_;
   bool output_to_screen_;
   bool output_info_header_;
   bool module_info_on_;
   confidence_interval confidence_interval_;
};

}  // namespace moja::flint
