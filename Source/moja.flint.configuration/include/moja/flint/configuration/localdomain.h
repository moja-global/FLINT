#ifndef MOJA_FLINT_CONFIGURATION_LOCALDOMAIN_H_
#define MOJA_FLINT_CONFIGURATION_LOCALDOMAIN_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/iterationbase.h"
#include "moja/flint/configuration/itiming.h"
#include "moja/flint/configuration/landscape.h"
#include "moja/flint/configuration/operationmanager.h"
#include "moja/datarepository/providerspatialrastertiled.h"

#include <moja/dynamic.h>

#include <string>

namespace moja {
namespace flint {
namespace configuration {

enum class LocalDomainType {
   SpatialTiled,
   SpatiallyReferencedSQL,
   SpatiallyReferencedNoSQL,
   ThreadedSpatiallyReferencedNoSQL,
   Point
};

class CONFIGURATION_API LocalDomain {
  public:
   LocalDomain(LocalDomainType type, LocalDomainIterationType iterationType, bool doLogging, int numThreads,
               const std::string& sequencerLibrary, const std::string& sequencer, const std::string& simulateLandUnit,
               const std::string& landUnitBuildSuccess, DynamicObject settings,
               TimeStepping timeStepping = TimeStepping::Monthly);

   virtual ~LocalDomain() {}

   virtual LocalDomainType type() const { return _type; }
   virtual LocalDomainIterationType iterationType() const { return _iterationType; }
   virtual bool doLogging() const { return _doLogging; }
   virtual int numThreads() const { return _numThreads; }
   virtual const std::string& sequencerLibrary() const { return _sequencerLibrary; }
   virtual const std::string& sequencer() const { return _sequencer; }
   virtual const std::string& simulateLandUnit() const { return _simulateLandUnit; }
   virtual double simulateLandUnitArea(const CellIdx& cell) { return ProviderSpatialRasterTiled::area(cell); }
   virtual const std::string& landUnitBuildSuccess() const { return _landUnitBuildSuccess; }
   virtual const DynamicObject& settings() const { return _settings; }
   virtual DynamicObject& settings() { return _settings; }
   virtual TimeStepping timeStepping() const { return _timeStepping; }

   virtual void set_iterationType(LocalDomainIterationType value) { _iterationType = value; }
   virtual void set_doLogging(bool value) { _doLogging = value; }
   virtual void set_settings(DynamicObject& value) { _settings = value; }

   virtual const Landscape* landscapeObject() const { return _landscapeObject.get(); }
   virtual Landscape* landscapeObject() { return _landscapeObject.get(); }

   void setLandscapeObject(const std::string& providerName, LocalDomainIterationType iterationType);

   virtual const OperationManager* operationManagerObject() const { return &_operationManager; }
   virtual OperationManager* operationManagerObject() { return &_operationManager; }

   static std::string localDomainTypeToStr(LocalDomainType type);

  private:
   LocalDomainType _type;
   LocalDomainIterationType _iterationType;
   bool _doLogging;
   int _numThreads;
   std::string _sequencerLibrary;
   std::string _sequencer;
   std::string _simulateLandUnit;
   std::string _landUnitBuildSuccess;
   TimeStepping _timeStepping;

   DynamicObject _settings;

   std::shared_ptr<Landscape> _landscapeObject;
   OperationManager _operationManager;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_LOCALDOMAIN_H_
