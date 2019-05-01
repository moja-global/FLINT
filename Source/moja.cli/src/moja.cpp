#include "moja/flint/configuration/json2configurationprovider.h"
#include "moja/flint/configuration/localdomain.h"
#include "moja/flint/configuration/variable.h"
#include "moja/flint/configuration/externalvariable.h"
#include "moja/flint/configuration/spinup.h"
#include "moja/flint/threadedaspatiallocaldomaincontroller.h"
#include "moja/flint/spatialtiledlocaldomaincontroller.h"
#include "moja/flint/ilocaldomaincontroller.h"
#include "moja/flint/moduleproxybase.h"
#include "moja/flint/ioperationmanager.h"

#include "moja/signals.h"
#include "moja/exception.h"
#include "moja/logging.h"

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

#include <Poco/File.h>

#include <memory>
#include <string>
#include <fstream>

static constexpr const char* CLI_VERSION_STRING = "flint cli version 1.0.0";

namespace opt = boost::program_options;
namespace mf = moja::flint;
namespace conf = mf::configuration;

using mf::configuration::LocalDomainType;
using mf::ILocalDomainController;

bool checkFilePath(const std::string& filePath) {
    Poco::File file(filePath);
    if (!file.exists()) {
        std::cerr << "File not found: " << filePath;
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {

	opt::options_description general_opt("General options");
	general_opt.add_options()
		("help,h", "produce a help message")
		("help-section", opt::value<std::string>(), "produce a help message for a named section")
		("version,v", "output the version number")
		;

	// Declare a group of options for commandline arguments only
	opt::options_description cmdLineOptions("Commandline only options");
	cmdLineOptions.add_options()
		("logging_config", opt::value<std::string>(), "path to Moja logging config file")
		("config_file", opt::value<std::string>(), "path to Moja run config file")
		("provider_file", opt::value<std::string>(), "path to Moja data provider config file");			

	// Declare a group of options that will be allowed
	// both on the command line and in a config file

	// In the external configuration files
	// list the config in following format:
	// config=C:\Test\config_1.json
	// config=C:\Test\config-2.json

	// list provider config in following format:
	// config_provider=C:\Test\config_provider_1.json
	// config_provider=C:\Test\config_provider_2.json

	opt::options_description configFileOptions("Configuration file options");
	configFileOptions.add_options()
		("config", opt::value<std::vector<std::string>>()->multitoken()->zero_tokens()->composing(),
			"path to Moja project config files")
		("config_provider", opt::value<std::vector<std::string>>()->multitoken()->zero_tokens()->composing(),
			"path to Moja project config files for data providers");			

	opt::variables_map args;

	opt::options_description all_options;
	all_options.add(general_opt).add(cmdLineOptions).add(configFileOptions);

	// Declare an options description instance which will be shown
	// to the user
	opt::options_description visible("Allowed options");
	visible.add(general_opt).add(cmdLineOptions).add(configFileOptions);

	try {
		opt::store(opt::parse_command_line(argc, argv, all_options), args);
	}
	catch (opt::error const& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	opt::notify(args);


	if (args.count("help")) {
		std::cout << visible;
		return EXIT_SUCCESS;
	}
	if (args.count("help-section")) {
		const std::string& help_option = args["help-section"].as<std::string>();

		if (help_option == "all") {
			std::cout << visible;
		}
		else if (help_option == "files") {
			std::cout << cmdLineOptions;
		}
		else if (help_option == "configs") {
			std::cout << configFileOptions;
		}
		else {
			std::cout << "Unknown section '" << help_option << "' in the --help-section option\n";
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}
	if (args.count("version")) {
		std::cout << CLI_VERSION_STRING << "\n";
		return EXIT_SUCCESS;
	}

	if (args.count("config_file")) {
		//when run config file is passed in commandline
		auto runConfig = args["config_file"].as<std::string>();
        if (!checkFilePath(runConfig)) {
            return EXIT_FAILURE;
        }
				
		try {			
			std::ifstream ifss(runConfig);
			opt::store(opt::parse_config_file(ifss, all_options), args);		
		} catch (opt::error const& e) {
			std::cerr << e.what() << std::endl;
			return EXIT_FAILURE;
		}	
	}

	if (args.count("provider_file")) {
		//when provider config file is passed in commandline
		auto providerConfig = args["provider_file"].as<std::string>();
        if (!checkFilePath(providerConfig)) {
            return EXIT_FAILURE;
        }

		try {
			std::ifstream ifss(providerConfig);
			opt::store(opt::parse_config_file(ifss, all_options), args);
		}
		catch (opt::error const& e) {
			std::cerr << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}

	if (args.count("logging_config")) {
		auto loggingConf = args["logging_config"].as<std::string>();
        if (!checkFilePath(loggingConf)) {
            return EXIT_FAILURE;
        }

		moja::Logging::setConfigurationFile(loggingConf);
	}

	if (!args.count("config")) {
		std::cout << CLI_VERSION_STRING << std::endl;
		std::cout << std::endl;
		std::cout << visible;
		return EXIT_SUCCESS;
	}

	auto configPath = args["config"].as<std::vector<std::string>>();
    for (const auto& filePath : configPath) {
        if (!checkFilePath(filePath)) {
            return EXIT_FAILURE;
        }
    }

    auto configProviderPath = args["config_provider"].as<std::vector<std::string>>();
    for (const auto& filePath : configProviderPath) {
        if (!checkFilePath(filePath)) {
            return EXIT_FAILURE;
        }
    }

    MOJA_LOG_INFO << "Config has files: " << configPath.size();

#if defined(_DEBUG)
	//// DEBUG LOGGING
	const std::string s =
		"[Sinks.console]\n"
		"Destination=Console\n"
		"Asynchronous = false\n"
		"AutoFlush = true\n"
		"Format = \"<%TimeStamp%> (%Severity%) - %Message%\"\n";
	//"Filter = \"%Severity% >= info\"" << std::endl;

	moja::Logging::setConfigurationText(s);
#endif

	try {
		MOJA_LOG_INFO << "Using Logging Configuration: " << moja::Logging::configurationFilename();
		MOJA_LOG_INFO << "Using configurations: ";
		for (auto configFilePath : configPath) {
			MOJA_LOG_INFO << configFilePath;
		}
		MOJA_LOG_INFO << "Using provider configurations: ";
		for (auto configFilePath : configProviderPath) {
			MOJA_LOG_INFO << configFilePath;
		}

		conf::JSON2ConfigurationProvider jsonConfigProvider{configPath, configProviderPath};
		auto config = jsonConfigProvider.createConfiguration();

		MOJA_LOG_INFO << "Using operation manager: " << config->localDomain()->operationManagerObject()->name();

		if (config->spinup()->enabled()) {
			const auto spinup = config->spinup();
			auto spinupSequencer = spinup->sequencerName();
			MOJA_LOG_INFO << "spinup ON, sequencer - " << spinupSequencer;
			MOJA_LOG_DEBUG << "\tModules:\t";
			for (auto spinupModule : spinup->modules())
				MOJA_LOG_DEBUG << spinupModule->name() << "(" << spinupModule->order() << "), ";
			MOJA_LOG_DEBUG;
			MOJA_LOG_DEBUG << "\tVariables:\t";
			for (auto variable : spinup->variables())
				MOJA_LOG_DEBUG << variable->name() << ", ";
			MOJA_LOG_DEBUG;
			MOJA_LOG_DEBUG << "\tExtVariables:\t";
			for (auto externalVariable : spinup->externalVariables())
				MOJA_LOG_DEBUG << externalVariable->name() << ", ";
			MOJA_LOG_DEBUG;
		}

		std::unique_ptr<ILocalDomainController> ldc;
		switch (config->localDomain()->type()) {
		case LocalDomainType::SpatiallyReferencedSQL: {
			ldc = std::make_unique<mf::ThreadedAspatialLocalDomainController>();
			break;
		}
		case LocalDomainType::SpatialTiled: {
			if (config->localDomain()->numThreads() > 0	)
				ldc = std::make_unique<mf::SpatialTiledLocalDomainController>(true, true, false);
			else
				ldc = std::make_unique<mf::SpatialTiledLocalDomainController>(false, false, false);
			break;
		}
		case LocalDomainType::Point: {
			ldc = std::make_unique<mf::LocalDomainControllerBase>();
			break;
		}
		case LocalDomainType::SpatiallyReferencedNoSQL: 
		case LocalDomainType::ThreadedSpatiallyReferencedNoSQL:
		default:
			ldc = nullptr;
			MOJA_LOG_INFO << "Unknown LocalDomain Type in config";
			return EXIT_FAILURE;
		}

		ldc->configure(*config);
		MOJA_LOG_INFO << "modules loaded: ";
		for (auto module : ldc->modules()) {
			MOJA_LOG_INFO << "\tlibrary: " << module.first.first << ", module name: " << module.first.second;
			if (module.second->moduleType() == moja::flint::ModuleTypes::Proxy) {
				const auto m = dynamic_cast<const moja::flint::ModuleProxyBase*>(module.second);
				auto ml = m->listModules();
				for (auto moduleproxy : ml) {
					MOJA_LOG_INFO << "\t\tProxy - library: " << std::get<0>(moduleproxy)
                                  << ", module name: " << std::get<1>(moduleproxy)
                                  << ", variable: " << std::get<2>(moduleproxy);
				}
			}
		}

		MOJA_LOG_INFO << "Operation manager: " << ldc->landUnitController().operationManager()->name()
                      << ", Version: " << ldc->landUnitController().operationManager()->version()
                      << ", Config: " << ldc->landUnitController().operationManager()->config();

        ldc->_notificationCenter.postNotification(moja::signals::SystemInit);
        ldc->startup();
        ldc->run();
        ldc->shutdown();
        ldc->_notificationCenter.postNotification(moja::signals::SystemShutdown);
    }
    catch (const moja::Exception& e) {
		MOJA_LOG_FATAL << e.displayText();
		return EXIT_FAILURE;
	} catch (const boost::exception& e) {
        MOJA_LOG_FATAL << boost::diagnostic_information(e);
		return EXIT_FAILURE;
    } catch (const Poco::Exception& e) {
        MOJA_LOG_FATAL << e.message();
        return EXIT_FAILURE;
	} catch (const std::exception& e) {
        MOJA_LOG_FATAL << e.what();
		return EXIT_FAILURE;
	} catch (...) {
        MOJA_LOG_FATAL << "Unknown exception";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
