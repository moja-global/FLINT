#ifndef MOJA_FLINT_SIMULATIONUNITDATABASE_H_
#define MOJA_FLINT_SIMULATIONUNITDATABASE_H_

#include "moja/flint/iflintdata.h"
#include "moja/flint/record.h"
#include "moja/flint/recordaccumulator.h"

#include <moja/dynamic.h>

namespace moja {
namespace flint {

class SimulationUnitDataBase : public flint::IFlintData {
  public:
   SimulationUnitDataBase()
       : _localDomainId(-1),
         _landUnitArea(0.0),
         _locationId(-1),
         _processUnitCount(0),
         _luCountProcessingUnit(0),
         _luCountLocalDomain(0) {}

   virtual ~SimulationUnitDataBase() = default;
   SimulationUnitDataBase(const SimulationUnitDataBase&) = delete;
   SimulationUnitDataBase& operator=(const SimulationUnitDataBase&) = delete;

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  datarepository::DataRepository& dataRepository) override {}

   DynamicVar getProperty(const std::string& key) const override;

   // Common data
   int _localDomainId;
   double _landUnitArea;

   // Collection record ids
   Int64 _locationId;

   // Collections shared within a thread
   RecordAccumulator<FluxRow> _fluxResults;

   // Error log members
   int _logErrorCount;
   RecordAccumulator2<ErrorRow, ErrorRecord> _errorLog;

   // -- Run Stats
   int _processUnitCount;
   Int64 _luCountProcessingUnit;
   Int64 _luCountLocalDomain;

   DateTime _startSystemTime;
   DateTime _endSystemTime;
   DateTime _startLocalDomainTime;
   DateTime _endLocalDomainTime;
   DateTime _startProcessingUnitTime;
   DateTime _endProcessingUnitTime;
};

inline DynamicVar SimulationUnitDataBase::getProperty(const std::string& key) const { return DynamicVar(); }

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_SIMULATIONUNITDATABASE_H_