#ifndef MOJA_FLINT_CONFIGURATION_CONFIGURATION_H_
#define MOJA_FLINT_CONFIGURATION_CONFIGURATION_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/ivariable.h"
#include "moja/flint/configuration/itiming.h"

#include "moja/datetime.h"
#include "moja/dynamic.h"


#include <vector>

namespace moja {
namespace flint {
namespace configuration {

enum class LibraryType;
enum class LocalDomainType;
enum class LocalDomainIterationType;
class LocalDomain;
class Module;
class Provider;
class Library;
class ExternalPool;
class Pool;
class Variable;
class ExternalVariable;
class FlintDataVariable;
class Transform;
class Spinup;
class OperationManager;

class CONFIGURATION_API Configuration {
public:
	Configuration(moja::DateTime startDate, moja::DateTime endDate);
	virtual ~Configuration() {}

	void Copy(const Configuration& configuration);

	inline moja::DateTime startDate() const { return _startDate; }
	inline moja::DateTime endDate() const { return _endDate; }
	inline const LocalDomain* localDomain() const { return _localDomain.get(); }
	inline LocalDomain* localDomain() { return _localDomain.get(); }

	void setLocalDomain(
		LocalDomainType type,
		LocalDomainIterationType iterationType,
		bool doLogging,
		int numThreads,
		const std::string& sequencerLibrary,
		const std::string& sequencer,
		const std::string& simulateLandUnit,
		const std::string& landUnitBuildSuccess,
		DynamicObject settings = moja::DynamicObject(),
        TimeStepping stepping = TimeStepping::Monthly);

	void addLibrary(const std::string& path, const std::string& library, const std::string& name, LibraryType type);
	void addLibrary(const std::string& name, LibraryType type);

	void addProvider(const std::string& name, const std::string& library, const std::string& providerType, moja::DynamicObject settings = moja::DynamicObject());

	void addPool(const std::string& name, double initValue = 0.0);
	void addPool(const std::string& name, const std::string& description, const std::string& units, double scale, int order, double initValue = 0.0);

	void addVariable(const std::string& name, moja::DynamicVar value = moja::DynamicVar());
	void removeVariable(const std::string& name);
	void replaceVariable(const std::string& name, moja::DynamicVar value = moja::DynamicVar());

    void addExternalPool(const std::string& name, const std::string& description, const std::string& units, double scale, int order,
                         const std::string& libraryName, const std::string transformTypeName, DynamicObject settings);
    
    void addExternalVariable(const std::string& name,
		const std::string& libraryName,
		const std::string transformTypeName,
		DynamicObject settings);

	void removeExternalVariable(const std::string& name);

	void addFlintDataVariable(const std::string& name,
		const std::string& libraryName,
		const std::string flintDataTypeName,
		DynamicObject settings);

	void removeFlintDataVariable(const std::string& name);

	void addModule(const std::string& libraryName, const std::string& name, int order, bool isProxy, DynamicObject settings = moja::DynamicObject());
	void removeModule(std::string libraryName, std::string moduleName);

#if 0
	void addVariable2(const std::string& name, moja::DynamicVar value);
	void removeVariable2(const std::string& name);
	void replaceVariable2(const std::string& name, moja::DynamicVar value);
	std::shared_ptr<ExternalVariable> addExternalVariable2(const std::string& name, const std::string& libraryName, const std::string transformTypeName, DynamicObject settings);
	void removeExternalVariable2(const std::string& name);
	std::shared_ptr<FlintDataVariable> addFlintDataVariable2(const std::string& name, const std::string& libraryName, const std::string flintDataTypeName, DynamicObject settings);
	void removeFlintDataVariable2(const std::string& name);
#endif
	void setSpinup(
		bool enabled = false, 
		const std::string& sequencerLibrary = std::string(), 
		const std::string& sequencerName = std::string(), 
		const std::string& simulateLandUnit = std::string(), 
		const std::string& landUnitBuildSuccess = std::string(), 
		DynamicObject settings = moja::DynamicObject());

	std::vector<const Library*> libraries() const;
	std::vector<const Provider*> providers() const;
	std::vector<const Pool*> pools() const;
    std::vector<const ExternalPool*> externalPools() const;
	std::vector<const Variable*> variables() const;
	std::vector<const ExternalVariable*> externalVariables() const;
	std::vector<const FlintDataVariable*> flintDataVariables() const;
	//std::vector<const IVariable*> variables2() const;
	std::vector<const Module*> modules() const;
	Spinup* spinup() const;

private:
	moja::DateTime _startDate;
	moja::DateTime _endDate;
	std::shared_ptr<LocalDomain> _localDomain;
	std::vector<std::shared_ptr<Library>> _libraries;
	std::vector<std::shared_ptr<Provider>> _providers;
	std::vector<std::shared_ptr<Pool>> _pools;
    std::vector<std::shared_ptr<ExternalPool>> _externalPools;

	std::vector<std::shared_ptr<IVariable>> _variables2;

	std::vector<std::shared_ptr<Variable>> _variables;
	std::vector<std::shared_ptr<ExternalVariable>> _externalVariables;
	std::vector<std::shared_ptr<FlintDataVariable>> _flintDataVariables;
	std::vector<std::shared_ptr<Module>> _modules;
	std::shared_ptr<Spinup> _spinup;

	template<class T>
	inline std::vector<const T*> copy(const std::vector<std::shared_ptr<T>>& vec) const {
		std::vector<const T*> result;
		for (const auto item : vec) {
			result.push_back(item.get());
		}

		return result;
	}
};

inline std::vector<const Library*> Configuration::libraries() const {
	return copy(_libraries);
}

inline std::vector<const Provider*> Configuration::providers() const {
	return copy(_providers);
}

inline std::vector<const Pool*> Configuration::pools() const {
	return copy(_pools);
}

inline std::vector<const ExternalPool*> Configuration::externalPools() const {
    return copy(_externalPools);
}

inline std::vector<const Variable*> Configuration::variables() const {
	return copy(_variables);
}

inline std::vector<const ExternalVariable*> Configuration::externalVariables() const {
	return copy(_externalVariables);
}

inline std::vector<const FlintDataVariable*> Configuration::flintDataVariables() const {
	return copy(_flintDataVariables);
}

//inline std::vector<const IVariable*> Configuration::variables2() const {
//	return copy(_variables2);
//}
//

inline std::vector<const Module*> Configuration::modules() const {
	return copy(_modules);
}

inline Spinup* Configuration::spinup() const {
	return _spinup.get();
}

}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_CONFIGURATION_H_
