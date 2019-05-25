#include "moja/flint/configuration/configuration.h"

#include "moja/flint/configuration/configurationexceptions.h"
#include "moja/flint/configuration/externalpool.h"
#include "moja/flint/configuration/externalvariable.h"
#include "moja/flint/configuration/flintdatavariable.h"
#include "moja/flint/configuration/library.h"
#include "moja/flint/configuration/localdomain.h"
#include "moja/flint/configuration/module.h"
#include "moja/flint/configuration/pool.h"
#include "moja/flint/configuration/provider.h"
#include "moja/flint/configuration/spinup.h"
#include "moja/flint/configuration/variable.h"
#include "moja/logging.h"

namespace moja {
namespace flint {
namespace configuration {

Configuration::Configuration(moja::DateTime startDate, moja::DateTime endDate) {
   if (endDate <= startDate) {
      throw std::invalid_argument("endDate must be after startDate");
   }
   _startDate = startDate;
   _endDate = endDate;
}

void Configuration::Copy(const Configuration& src) {
   setLocalDomain(src._localDomain->type(), src._localDomain->iterationType(), src._localDomain->doLogging(),
                  src._localDomain->numThreads(), src._localDomain->sequencerLibrary(), src._localDomain->sequencer(),
                  src._localDomain->simulateLandUnit(), src._localDomain->landUnitBuildSuccess(),
                  src._localDomain->settings(), src._localDomain->timeStepping());

   _localDomain->operationManagerObject()->set_name(src._localDomain->operationManagerObject()->name());
   _localDomain->operationManagerObject()->set_settings(src._localDomain->operationManagerObject()->settings());

   if (src._localDomain->type() != LocalDomainType::Point) {
      _localDomain->setLandscapeObject(src._localDomain->landscapeObject()->providerName(),
                                       src._localDomain->iterationType());

      if (src.localDomain()->iterationType() == LocalDomainIterationType::ASpatialIndex) {
         auto iteration = _localDomain->landscapeObject()->iterationASpatialIndex();
         const auto iterationSrc = src.localDomain()->landscapeObject()->iterationASpatialIndex();
         iteration->set_maxTileSize(iterationSrc->maxTileSize());
         iteration->set_tileCacheSize(iterationSrc->tileCacheSize());
      }
      if (src.localDomain()->iterationType() == LocalDomainIterationType::ASpatialMongoIndex) {
         auto iteration = _localDomain->landscapeObject()->iterationASpatialMongoIndex();
         const auto iterationSrc = src.localDomain()->landscapeObject()->iterationASpatialMongoIndex();
         iteration->set_processAlldocuments(iterationSrc->processAlldocuments());
         for (auto val : iterationSrc->documentIds()) iteration->addDocumentId(val);
      } else if (src.localDomain()->iterationType() == LocalDomainIterationType::AreaOfInterest) {
         auto iteration = _localDomain->landscapeObject()->iterationAreaOfInterest();
         const auto iterationSrc = src.localDomain()->landscapeObject()->iterationAreaOfInterest();
      } else if (src.localDomain()->iterationType() == LocalDomainIterationType::LandscapeTiles) {
         auto iteration = _localDomain->landscapeObject()->iterationLandscapeTiles();
         const auto iterationSrc = src.localDomain()->landscapeObject()->iterationLandscapeTiles();

         iteration->set_tileSizeX(iterationSrc->tileSizeX());
         iteration->set_tileSizeY(iterationSrc->tileSizeY());
         iteration->set_xPixels(iterationSrc->xPixels());
         iteration->set_yPixels(iterationSrc->yPixels());

         for (auto& val : iterationSrc->landscapeTiles()) iteration->addConfigTile(val);
      } else if (src.localDomain()->iterationType() == LocalDomainIterationType::TileIndex) {
         auto iteration = _localDomain->landscapeObject()->iterationTileIndex();
         const auto iterationSrc = src.localDomain()->landscapeObject()->iterationTileIndex();
         for (auto& val : iterationSrc->tileIndexList()) iteration->addTileIndex(val);
      } else if (src.localDomain()->iterationType() == LocalDomainIterationType::BlockIndex) {
         auto iteration = _localDomain->landscapeObject()->iterationBlockIndex();
         const auto iterationSrc = src.localDomain()->landscapeObject()->iterationBlockIndex();
         for (auto& val : iterationSrc->blockIndexList()) iteration->addBlockIndex(val);
      } else if (src.localDomain()->iterationType() == LocalDomainIterationType::CellIndex) {
         auto iteration = _localDomain->landscapeObject()->iterationCellIndex();
         const auto iterationSrc = src.localDomain()->landscapeObject()->iterationCellIndex();
         for (auto& val : iterationSrc->cellIndexList()) iteration->addCellIndex(val);
      }
   }
   _startDate = src._startDate;
   _endDate = src._endDate;

   _libraries = src._libraries;
   _providers = src._providers;
   _pools = src._pools;
   _externalPools = src._externalPools;
   _variables = src._variables;

   _externalVariables = src._externalVariables;
   _flintDataVariables = src._flintDataVariables;
   _modules = src._modules;
   _spinup = src._spinup;
}

void Configuration::setLocalDomain(LocalDomainType type, LocalDomainIterationType iterationType, bool doLogging,
                                   int numThreads, const std::string& sequencerLibrary, const std::string& sequencer,
                                   const std::string& simulateLandUnit, const std::string& landUnitBuildSuccess,
                                   DynamicObject settings, TimeStepping stepping) {
   _localDomain = std::make_shared<LocalDomain>(type, iterationType, doLogging, numThreads, sequencerLibrary, sequencer,
                                                simulateLandUnit, landUnitBuildSuccess, settings, stepping);
}

void Configuration::addLibrary(const std::string& path, const std::string& library, const std::string& name,
                               LibraryType type) {
   MOJA_LOG_DEBUG << "details (" << name << "): path - " << path << ", filename -" << library;

   auto lib = std::make_shared<Library>(path, library, name, type);
   _libraries.push_back(lib);
}

void Configuration::addLibrary(const std::string& name, LibraryType type) {
   auto library = std::make_shared<Library>(name, type);
   _libraries.push_back(library);
}

void Configuration::addProvider(const std::string& name, const std::string& library, const std::string& providerType,
                                moja::DynamicObject settings) {
   auto provider = std::make_shared<Provider>(name, library, providerType, settings);
   _providers.push_back(provider);
}

void Configuration::addPool(const std::string& name, double initValue) {
   auto pool = std::make_shared<Pool>(name, initValue);
   _pools.push_back(pool);
}

void Configuration::addPool(const std::string& name, const std::string& description, const std::string& units,
                            double scale, int order, double initValue) {
   auto pool = std::make_shared<Pool>(name, description, units, scale, order, initValue);
   _pools.push_back(pool);
}

void Configuration::addVariable(const std::string& name, moja::DynamicVar value) {
   auto variable = std::make_shared<Variable>(name, value);
   _variables.push_back(variable);
}

void Configuration::removeVariable(const std::string& name) {
   for (auto it = _variables.begin(); it != _variables.end(); ++it) {
      if ((*it)->name() == name) {
         _variables.erase(it);
         break;
      }
   }
}

void Configuration::replaceVariable(const std::string& name, moja::DynamicVar value) {
   for (auto it = _variables.begin(); it != _variables.end(); ++it) {
      if ((*it)->name() == name) {
         _variables.erase(it);
         break;
      }
   }
   auto variable = std::make_shared<Variable>(name, value);
   _variables.push_back(variable);
}

void Configuration::addExternalVariable(const std::string& name, const std::string& libraryName,
                                        const std::string transformTypeName, DynamicObject settings) {
   auto transform = std::make_shared<Transform>(libraryName, transformTypeName, settings);
   auto variable = std::make_shared<ExternalVariable>(name, transform);
   _externalVariables.push_back(variable);
}

void Configuration::addExternalPool(const std::string& name, const std::string& description, const std::string& units,
                                    double scale, int order, const std::string& libraryName,
                                    const std::string transformTypeName, DynamicObject settings) {
   auto transform = std::make_shared<Transform>(libraryName, transformTypeName, settings);
   auto pool = std::make_shared<ExternalPool>(name, description, units, scale, order, transform);
   _externalPools.push_back(pool);
}

void Configuration::removeExternalVariable(const std::string& name) {
   for (auto it = _externalVariables.begin(); it != _externalVariables.end(); ++it) {
      if ((*it)->name() == name) {
         _externalVariables.erase(it);
         break;
      }
   }
}

void Configuration::addFlintDataVariable(const std::string& name, const std::string& libraryName,
                                         const std::string flintDataTypeName, DynamicObject settings) {
   auto flintData = std::make_shared<FlintData>(libraryName, flintDataTypeName, settings);
   auto variable = std::make_shared<FlintDataVariable>(name, flintData);
   _flintDataVariables.push_back(variable);
}

void Configuration::removeFlintDataVariable(const std::string& name) {
   for (auto it = _flintDataVariables.begin(); it != _flintDataVariables.end(); ++it) {
      if ((*it)->name() == name) {
         _flintDataVariables.erase(it);
         break;
      }
   }
}

#if 0
// --------------------------------------------------------------------------------------------

void Configuration::addVariable2(const std::string& name, moja::DynamicVar value) {
	auto variable = std::make_shared<Variable>(name, value);
	_variables2.push_back(variable);
}

void Configuration::removeVariable2(const std::string& name) {
	for (auto it = _variables2.begin(); it != _variables2.end(); ++it) {
		if ((*it)->name() == name) {
			_variables2.erase(it);
			break;
		}
	}
}

void Configuration::replaceVariable2(const std::string& name, moja::DynamicVar value) {
	for (auto it = _variables2.begin(); it != _variables2.end(); ++it) {
		if ((*it)->name() == name) {
			_variables2.erase(it);
			break;
		}
	}
	auto variable = std::make_shared<Variable>(name, value);
	_variables2.push_back(variable);
}

std::shared_ptr<ExternalVariable> Configuration::addExternalVariable2(const std::string& name, const std::string& libraryName, const std::string transformTypeName, DynamicObject settings) {
	auto transform = std::make_shared<Transform>(libraryName, transformTypeName, settings);
	auto variable = std::make_shared<ExternalVariable>(name, transform);
	_variables2.push_back(variable);
	return variable;
}

void Configuration::removeExternalVariable2(const std::string& name) {
	for (auto it = _variables2.begin(); it != _variables2.end(); ++it) {
		if ((*it)->name() == name) {
			_variables2.erase(it);
			break;
		}
	}
}

std::shared_ptr<FlintDataVariable> Configuration::addFlintDataVariable2(const std::string& name, const std::string& libraryName, const std::string flintDataTypeName, DynamicObject settings) {
	auto flintData = std::make_shared<FlintData>(libraryName, flintDataTypeName, settings);
	auto variable = std::make_shared<FlintDataVariable>(name, flintData);
	_variables2.push_back(variable);
	return variable;
}

void Configuration::removeFlintDataVariable2(const std::string& name) {
	for (auto it = _variables2.begin(); it != _variables2.end(); ++it) {
		if ((*it)->name() == name) {
			_variables2.erase(it);
			break;
		}
	}
}

// --------------------------------------------------------------------------------------------
#endif

void Configuration::addModule(const std::string& libraryName, const std::string& name, int order, bool isProxy,
                              DynamicObject settings) {
   // It is an error for two modules to have the same order - simulation
   // results could be inconsistent.
   auto sameOrder = std::find_if(_modules.begin(), _modules.end(),
                                 [order](std::shared_ptr<Module> other) { return other->order() == order; });

   if (sameOrder != _modules.end()) {
      throw ModuleOrderOverlapException() << Order(order) << ModuleNames({libraryName, name, (*sameOrder)->name()});
   }

   auto module = std::make_shared<Module>(libraryName, name, order, isProxy, settings);
   _modules.push_back(module);
   std::sort(_modules.begin(), _modules.end(),
             [](std::shared_ptr<Module> lhs, std::shared_ptr<Module> rhs) { return lhs->order() < rhs->order(); });
}

void Configuration::setSpinup(bool enabled, const std::string& sequencerLibrary, const std::string& sequencerName,
                              const std::string& simulateLandUnit, const std::string& landUnitBuildSuccess,
                              DynamicObject settings) {
   _spinup = std::make_shared<Spinup>(enabled, sequencerLibrary, sequencerName, simulateLandUnit, landUnitBuildSuccess,
                                      settings);
}

void Configuration::removeModule(std::string libraryName, std::string moduleName) {
   for (auto it = _modules.begin(); it != _modules.end(); it++) {
      if ((*it)->name() == moduleName) {
         _modules.erase(it);
         break;
      }
   }
}
}  // namespace configuration
}  // namespace flint
}  // namespace moja