//
// ProviderManager.cpp
//

#include "moja/datarepository/providermanager.h"
#include "moja/environment.h"
#include "moja/sharedlibrary.h"
#include "moja/string.h"
#include "moja/directoryiterator.h"

#include "moja/datarepository/providersleekspatialraster.h"
#include "moja/datarepository/providersleekspatialvector.h"
#include "moja/datarepository/providersleekrelationalsqlite.h"
#include "moja/datarepository/providersleeknosqlmongodb.h"

#include <sstream>
#include <memory>

namespace moja {
namespace datarepository {

Int32 ProviderManager::CurrentLoadOrder = 1;

ProviderInfoBase::ProviderInfoBase() {
	LoadOrder = ProviderManager::CurrentLoadOrder++;
	ProviderNumber = 1;
	InitializeProvider = nullptr;
	DestroyProvider = nullptr;
};

ExternalProviderInfo::ExternalProviderInfo() : Handle(nullptr) {}

InternalProviderInfo::InternalProviderInfo() {}

ProviderManager::~ProviderManager() {
	// NOTE: It may not be safe to unload providers by this point (static deinitialization), as other
	//       libraries may have already been unloaded, which means we can't safely call clean up methods
}

void ProviderManager::AddProvider(ProviderType providerType, const std::string& InProviderName) {
	// Do we already know about this provider?  If not, we'll create information for this provider now.
	if (_providers.find(InProviderName) == _providers.end()) {

		std::unique_ptr<ProviderInfoBase> providerInfo;
		switch (providerType) {
		case ProviderType::External: {
			auto provider = std::make_unique<ExternalProviderInfo>();
			std::map<std::string, std::string> ProviderPathMap = FindProviderPaths(InProviderName);
			if (ProviderPathMap.size() == 1) {
				// Add this provider to the set of providers that we know about
				provider->OriginalFilename = (*std::begin(ProviderPathMap)).second;
				provider->Filename = provider->OriginalFilename;
				providerInfo = std::move(provider);
			}
			break;
		}
		case ProviderType::Internal: {
			// Add this provider to the set of providers that we know about
			providerInfo = std::make_unique<InternalProviderInfo>();
			break;
		}
		}
		// Update hash table
		_providers[InProviderName] = std::move(providerInfo);
	}
}

std::map<std::string, std::string> ProviderManager::FindProviderPaths(const std::string& NamePattern) const {
	return FindProviderPathsInDirectory(moja::Environment::startProcessFolder(), NamePattern, true);
}

std::map<std::string, std::string> ProviderManager::FindProviderPathsExact(const std::string& NamePattern) const {
	return FindProviderPathsInDirectory(moja::Environment::startProcessFolder(), NamePattern, false);
}

std::map<std::string, std::string> ProviderManager::FindProviderPathsInDirectory(const std::string& directoryName, const std::string&  NamePattern, bool useSuffix) const {

	//const char* suf = useSuffix ? "suffex=T] " : "suffex=F] ";
	//std::cout << "[FindProviderPathsInDirectory 1," << suf << NamePattern << std::endl;
	std::map<std::string, std::string> OutProviderPaths;

	DirectoryIterator dirIterator(directoryName);
	DirectoryIterator end;

	std::string prefix = SharedLibrary::prefix();
	std::string suffix = useSuffix ? SharedLibrary::suffix() : "";
	//std::cout << "[FindProviderPathsInDirectory 1a," << prefix << NamePattern << suffix << std::endl;

	// Parse all the matching provider names
	while (dirIterator != end) {
		auto providerPath = Path(dirIterator->path());
		//std::cout << "[FindProviderPathsInDirectory 2]" << dirIterator->path() << std::endl;
		auto file = providerPath.getFileName();
		if (icompare(file, 0, prefix.size(), prefix) == 0
			&& icompare(file, file.size() - suffix.size(), suffix.size(), suffix) == 0) {
			auto it = file.begin() + prefix.size();
			auto end = file.end() - suffix.size();
			std::string providerName;
			while (it != end) providerName += *it++;

			bool match = NamePattern == providerName;
			//std::cout << "[FindProviderPathsInDirectory 3]" << NamePattern << " ** " << providerName << " ** " << providerPath.toString() << " ** " << match << std::endl;

			if (NamePattern.size() == 0 || NamePattern == providerName)
				OutProviderPaths[providerName] = providerPath.toString();
		}
		++dirIterator;
	}
	//		std::cout << "[FindProviderPathsInDirectory 4:finished]" << OutProviderPaths.size() << std::endl;
	return OutProviderPaths;
}

IProviderInterface* CreateInternalProvider(const char* providerName) {
	//std::cout << "[InitializeProvider]: " << "Internal, " << providerName << std::endl;
	std::string _providerNames(providerName);
	if (_providerNames == "ProviderSLEEKSpatialRaster")
		return new ProviderSLEEKSpatialRaster();
	else if (_providerNames == "ProviderSLEEKSpatialVector")
		return new ProviderSLEEKSpatialVector();
	else if (_providerNames == "ProviderSLEEKRelationalSQLite")
		return new ProviderSLEEKRelationalSQLite();
	else if (_providerNames == "ProviderSLEEKNoSQLMongoDB")
		return new ProviderSLEEKNoSQLMongoDB();

	throw LibraryLoadException("Unknown internal provider");
}

IProviderInterface::Ptr ProviderManager::GetProvider(const std::string& providerName) {
	// Do we even know about this provider?
	const auto providerInfoIt = _providers.find(providerName);
	if (providerInfoIt == std::end(_providers))
		return nullptr;

	std::shared_ptr<ProviderInfoBase> providerInfo = (*providerInfoIt).second;
	if (providerInfo->InitializeProvider == nullptr)
		throw LibraryLoadException("Failed to initialise the provider");

	std::shared_ptr<IProviderInterface> providerInterface;
	if (providerInfo->GetProviderType() == ProviderType::Internal) {
		providerInterface = std::shared_ptr<IProviderInterface>(providerInfo->InitializeProvider(providerName.c_str()));
	}
	else {
		providerInterface = std::shared_ptr<IProviderInterface>(providerInfo->InitializeProvider(providerName.c_str()), providerInfo->DestroyProvider);
	}
	if (providerInterface) {
		auto provider = std::static_pointer_cast<IProviderInterface>(providerInterface);
		// TODO: resolve when landUnitController gets passed to the providerWrapper
		//auto landUnitDataHandle = std::make_unique<LandUnitDataWrapper>(&landUnitController, provider.get());

		// TODO: work this part out for Providers - dont need wrapper stuff
		//auto landUnitDataHandle = std::make_unique<LandUnitDataWrapper>(provider.get());
		//providerInfo->ProviderList.push_back(providerInterface);
		//ProviderMetaData metaData;
		//metaData.providerInfoId = providerInfo->LoadOrder;
		//metaData.providerId = providerInfo->ProviderNumber++;
		//metaData.providerName = providerName;
		//metaData.providerType = (int)(providerInfo->GetProviderType());
		//providerInterface->StartupProvider(std::move(landUnitDataHandle), metaData);
	}
	else
		throw LibraryLoadException("Failed to initialise the provider");

	return providerInterface;
}

bool ProviderManager::LoadInternalProvider(const std::string& providerName) {
	// Update our set of known providers, in case we don't already know about this provider
	AddProvider(ProviderType::Internal, providerName);
	std::shared_ptr<ProviderInfoBase> providerInfo = _providers[providerName];

	if (providerInfo->InitializeProvider == nullptr) {
		if (providerInfo->GetProviderType() != ProviderType::Internal)
			throw LibraryLoadException("Attempt to load provider already loaded as different type");

		auto internalProviderInfo = std::static_pointer_cast<InternalProviderInfo>(providerInfo);
		internalProviderInfo->InitializeProvider = CreateInternalProvider;
		internalProviderInfo->DestroyProvider = nullptr;
	}
	return true;
}

bool ProviderManager::LoadExternalProvider(const std::string& providerName) {

	// Update our set of known providers, in case we don't already know about this provider
	AddProvider(ProviderType::External, providerName);

	// Grab the provider info.  This has the file name of the provider, as well as other info.
	std::shared_ptr<ProviderInfoBase> providerInfo = _providers[providerName];
	//std::cout << (providerInfo==nullptr?std::string("null"):std::string("not null")) <<std::endl;

	if (providerInfo->InitializeProvider == nullptr) {
		if (providerInfo->GetProviderType() != ProviderType::External)
			throw LibraryLoadException("Attempt to load provider already loaded as different type");

		auto externalProviderInfo = std::static_pointer_cast<ExternalProviderInfo>(_providers[providerName]);
		//std::cout << externalProviderInfo->Filename <<std::endl;

		// Determine which file to load for this provider.
		auto ProviderFileToLoad = File(externalProviderInfo->Filename);

		// Clear the handle and set it again below if the provider is successfully loaded
		externalProviderInfo->Handle = nullptr;

		// Skip this check if file manager has not yet been initialized
		if (ProviderFileToLoad.exists()) {
			if (CheckProviderCompatibility(ProviderFileToLoad.path())) {
				externalProviderInfo->Handle = std::make_unique<moja::SharedLibrary>(ProviderFileToLoad.path());
				externalProviderInfo->InitializeProvider = (InitializeProviderFunctionPtr)externalProviderInfo->Handle->getSymbol("InitializeProvider");
				externalProviderInfo->DestroyProvider = (DestroyProviderFunctionPtr)externalProviderInfo->Handle->getSymbol("DestroyProvider");
			}
			else {
				throw LibraryLoadException("Provider not compatible");
			}
		}
		else {
			//std::cout << externalProviderInfo->Filename << std::endl;
			//std::cout << ProviderFileToLoad.path() << std::endl;
			throw LibraryLoadException("Provider not found");
		}
	}
	return true;
}

bool ProviderManager::CheckProviderCompatibility(const std::string& filename) {
	//Int32 ProviderApiVersion = MOJA_VERSION;
	//if (ProviderApiVersion != MOJA_API_VERSION) {
	//	return false;
	//}
	return true;
}

}} // namespace moja::datarepository
