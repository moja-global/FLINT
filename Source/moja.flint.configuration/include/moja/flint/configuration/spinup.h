#ifndef MOJA_FLINT_CONFIGURATION_SPINUP_H_
#define MOJA_FLINT_CONFIGURATION_SPINUP_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include <moja/dynamic.h>

#include <string>
#include <vector>

namespace moja {
namespace flint {
namespace configuration {
class Variable;
class ExternalVariable;
class FlintDataVariable;

class CONFIGURATION_API SpinupModule {
  public:
   SpinupModule(const std::string& libraryName, const std::string& name, int order, bool createNew,
                DynamicObject settings);
   virtual ~SpinupModule() {}

   virtual const std::string& libraryName() const { return _libraryName; }
   virtual const std::string& name() const { return _name; }
   virtual int order() const { return _order; }
   virtual bool createNew() const { return _createNew; }
   virtual const DynamicObject& settings() const { return _settings; }

  private:
   std::string _libraryName;
   std::string _name;
   int _order;
   bool _createNew;
   DynamicObject _settings;
};

class CONFIGURATION_API Spinup {
  public:
   Spinup(bool enabled, const std::string& sequencerLibrary, const std::string& sequencerName,
          const std::string& simulateLandUnit, const std::string& landUnitBuildSuccess, DynamicObject settings);
   virtual ~Spinup() {}

   void addVariable(const std::string& name, moja::DynamicVar value = moja::DynamicVar());

   void addExternalVariable(const std::string& name, const std::string& libraryName,
                            const std::string transformTypeName, DynamicObject settings);

   void addFlintDataVariable(const std::string& name, const std::string& libraryName,
                             const std::string flintDataTypeName, DynamicObject settings);

   void addSpinupModule(const std::string& libraryName, const std::string& name, int order, bool createNew = false,
                        DynamicObject settings = moja::DynamicObject());

   virtual const bool enabled() const { return _enabled; }
   virtual const std::string& sequencerLibrary() const { return _sequencerLibrary; }
   virtual const std::string& sequencerName() const { return _sequencerName; }
   virtual const std::string& simulateLandUnit() const { return _simulateLandUnit; }
   virtual const std::string& landUnitBuildSuccess() const { return _landUnitBuildSuccess; }
   virtual const DynamicObject& settings() const { return _settings; }

   std::vector<const Variable*> variables() const;
   std::vector<const ExternalVariable*> externalVariables() const;
   std::vector<const FlintDataVariable*> flintDataVariables() const;
   std::vector<const SpinupModule*> modules() const;

  private:
   bool _enabled;
   std::string _sequencerLibrary;
   std::string _sequencerName;
   std::string _simulateLandUnit;
   std::string _landUnitBuildSuccess;
   DynamicObject _settings;

   // List of variables to replace from main config list
   // This way transforms can load and cache and not been thrown away. i.e if the spinup
   // has it's own set of variables we loose the caching when we restart the main sim.
   std::vector<std::shared_ptr<Variable>> _variables;
   std::vector<std::shared_ptr<ExternalVariable>> _externalVariables;
   std::vector<std::shared_ptr<FlintDataVariable>> _flintDataVariables;
   std::vector<std::shared_ptr<SpinupModule>> _modules;

   template <class T>
   inline std::vector<const T*> copy(const std::vector<std::shared_ptr<T>>& vec) const {
      std::vector<const T*> result;
      for (const auto item : vec) {
         result.push_back(item.get());
      }

      return result;
   }
};

inline std::vector<const Variable*> Spinup::variables() const { return copy(_variables); }

inline std::vector<const ExternalVariable*> Spinup::externalVariables() const { return copy(_externalVariables); }

inline std::vector<const FlintDataVariable*> Spinup::flintDataVariables() const { return copy(_flintDataVariables); }

inline std::vector<const SpinupModule*> Spinup::modules() const { return copy(_modules); }

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_SPINUP_H_
