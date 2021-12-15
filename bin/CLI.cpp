#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <IsExecutable/IsExecutable.h>

#ifdef WITH_HYDRARGS
#include <HydrArgs/HydrArgs.hpp>
#endif

const std::string usage{" <file to check>"};
const std::string programName{"IsExecutable"};
const std::string description{"allows you to get info about a file executability."};

int main(int argc, const char **argv) {
#ifdef WITH_HYDRARGS
	HydrArgs::SArg<HydrArgs::ArgType::flag> jsonFlag{'j', "json", "Dump as JSON", 0, "", false};
	HydrArgs::SArg<HydrArgs::ArgType::flag> humanFlag{'h', "human", "human-friendly output", 0, "", false};
	std::vector<HydrArgs::Arg *> dashedSpec {&jsonFlag, &humanFlag};

	HydrArgs::SArg<HydrArgs::ArgType::string> fileArg{'f', "file", "file to check", 1, "path to file", "", ""};
	std::vector<HydrArgs::Arg *> positionalSpec{&fileArg};

	std::unique_ptr<HydrArgs::IArgsParser> ap {HydrArgs::Backend::argsParserFactory(programName, description, usage, dashedSpec, positionalSpec)};
	auto status = (*ap)({argv[0], {&argv[1], static_cast<size_t>(argc - 1)}});

	if(status.parsingStatus){
		return status.parsingStatus.returnCode;
	}

	auto &file_path_str = fileArg.value;
	bool brief = !humanFlag.value;
#else
	bool brief = true;
	auto printHelp = [&]() -> int {
		if(argc == 1){
			std::cout << argv[0];
		} else {
			std::cout << programName;
		}
		std::cout << usage << std::endl;
		std::cout << description << std::endl;
		return 1;
	};

	if(argc <= 1){
		return printHelp();
	}

	const std::string_view file_path_str{argv[1]};

	if(file_path_str == "--help"){
		return printHelp();
	}
#endif

	const std::filesystem::path file_path(file_path_str);

	auto res = IsExecutable::isExecutable(file_path);

#ifdef WITH_HYDRARGS
	if(jsonFlag.value){
		IsExecutable::toJSON(res, std::cout);
		std::cout << std::endl;
		return 0;
	}
#endif

	char briefMsg[9];

	//IsExecutable::toHuman(res, std::cout, brief);
	
	to_human(res, briefMsg, sizeof(briefMsg) / sizeof(briefMsg[0]), brief);
	std::cout << briefMsg;
	
	std::cout << std::endl;
	return 0;
}
