#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <string_view>
#include <sstream>
#include <filesystem>
#include <type_traits>
#include <algorithm>
#include <array>
#include <span>

#include <IsExecutable/IsExecutable.h>

#include "enum_utils.hpp"
#include "platform_specific.hpp"
#include "svstream.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(_MSC_VER)
#define WINDOWS
#endif

//#include <mio/mmap.hpp>
//mio::ummap_source m(fileArg.value);

namespace IsExecutable{

constexpr std::array<Backend *, 4> systems{
	nullptr,
	&nixBackend,
	&macBackend,
	&winBackend,
};

constexpr std::array<const char *, 8> Systems_strings{
	"unknown",
	"sysv",
	"mac",
	"windows",
};

constexpr std::array<const char, 8> Systems_brief{
	'?',
	'n',
	'm',
	'w',
};

constexpr std::array<const char *, 8> FileClass_strings{
	"error",
	"ordinary",
	"library",
	"executable",
	"script",
	"unknown",
	"unknown",
	"unknown"
};

using SystemUT = std::underlying_type_t<enum System>;

constexpr Backend& getBackend(enum System sI){
	return *systems[static_cast<SystemUT>(sI)];
}

void is_executable_platform_specific(const std::filesystem::path &p, std::ifstream &f, const size_t sz, IsExecutable &res, const enum System sI){
	auto backend = getBackend(sI);
	
	backend.checkBinary(f, sz, res);
	
	if(res.cls == decltype(res.cls)::IX_FC_ordinary){
		std::string ext{p.extension()};
		std::string_view extv{ext};
		std::span<char> exts{ext};
		if(!ext.empty()){
			if(ext[0] == '.'){
				extv = extv.substr(1, extv.size());
				std::transform(begin(exts), end(exts), begin(exts), [](char c) { return std::tolower(c); });
#ifdef WITH_DEBUGGING
				std::cerr << extv << std::endl;
#endif
				if(backend.script_extensions.find(std::string{extv}) != end(backend.script_extensions)){
					res.cls = decltype(res.cls)::IX_FC_script;
				}
			}
		}
	}
};

void is_executable_platform_specific_dispatch(const std::filesystem::path &p, std::ifstream &f, const size_t sz, IsExecutable &res){
	for(SystemUT sI = 1; sI < systems.size(); ++sI){
		f.seekg(0, std::ios_base::beg);
		#ifdef WITH_DEBUGGING
		std::cerr << "sI: " << (uint16_t) sI << " (" << Systems_strings[sI] << ") "; 
		#endif
		is_executable_platform_specific(p, f, sz, res, static_cast<enum System>(sI));
		#ifdef WITH_DEBUGGING
		toHumanVerbose(res, std::cerr);
		std::cerr << std::endl;
		#endif
		if(res.cls != decltype(res.cls)::IX_FC_error && res.cls != decltype(res.cls)::IX_FC_ordinary){
			#ifdef WITH_DEBUGGING
			std::cerr << "success" << std::endl;
			#endif
			res.sys = static_cast<enum System>(sI);
			return;
		}
		#ifdef WITH_DEBUGGING
		std::cerr << "fail" << std::endl;
		#endif
	}
};

IsExecutable isExecutable(const std::filesystem::path &p) {
	IsExecutable res;
	if(std::filesystem::exists(p)){
		res.cls = decltype(res.cls)::IX_FC_ordinary;
		auto st = std::filesystem::status(p);
		auto tp = st.type();
		auto perms = st.permissions();
		auto suitableType = (tp == std::filesystem::file_type::regular) || (tp == std::filesystem::file_type::symlink);
		res.is_executable = (perms & (std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec)) != std::filesystem::perms::none;
		
		const size_t sz = std::filesystem::file_size(p);
		std::ifstream f(p);

		if(suitableType){
			is_executable_platform_specific_dispatch(p, f, sz, res);
		}
	}
	return res;
}

constexpr size_t HUMAN_FORMAT_GROUPS_COUNT = 5;
constexpr size_t HUMAN_FORMAT_SPACES_COUNT = HUMAN_FORMAT_GROUPS_COUNT - 1;

constexpr size_t MAX_SIZE_VERBOSE_FORMAT_OS = 7; // windows
constexpr size_t MAX_SIZE_VERBOSE_FORMAT_CLASS = 10; // executable

constexpr size_t MAX_SIZE_VERBOSE_FORMAT = HUMAN_FORMAT_SPACES_COUNT + MAX_SIZE_VERBOSE_FORMAT_OS + MAX_SIZE_VERBOSE_FORMAT_CLASS + sizeof("64") + sizeof("-bit") + sizeof("PIC") + sizeof("noexec") + 1 /*terminator*/;
void toHumanVerbose(const IsExecutable info, std::ostream &s){
	s << Systems_strings[static_cast<uint8_t>(info.sys)] << ' ' << (info.is_64?"64":"32") << "-bit" << ' ' << FileClass_strings[static_cast<uint8_t>(info.cls)] << ' ' << (info.is_pic ? "PIC" : "PDC") << ' ' << (info.is_executable ? "exec" : "noexec"); // literal strings for numbers in order not to change stream number format
}

constexpr size_t MAX_SIZE_BRIEF_FORMAT = HUMAN_FORMAT_SPACES_COUNT + HUMAN_FORMAT_GROUPS_COUNT + 1 /*terminator*/;
void toHumanBrief(const IsExecutable info, std::ostream &s){
	s << Systems_brief[static_cast<uint8_t>(info.sys)] << ' ' << (32 << info.is_64) / 10 << ' ' << FileClass_strings[static_cast<uint8_t>(info.cls)][0] << ' ' << (info.is_pic ? 'I' : 'D') << ' ' << (info.is_executable ? 'x' : '-');
}

void toHuman(const IsExecutable info, std::ostream &s, const bool brief){
	if(brief){
		toHumanBrief(info, s);
	} else {
		toHumanVerbose(info, s);
	}
}

void toJSON(const IsExecutable info, std::ostream &s){
	std::ostream oS(s.rdbuf()); // https://stackoverflow.com/questions/48676734/how-to-automatically-set-stream-mode-back-to-default/48680600#48680600
	oS << "{" << std::endl;
	oS << "\tsys: \"" << Systems_strings[static_cast<uint8_t>(info.sys)] << "\"," << std::endl;
	oS << "\tbitness: " << (32 << info.is_64) << "," << std::endl;
	oS << "\tcls: \"" << FileClass_strings[static_cast<uint8_t>(info.cls)] << "\"," << std::endl;
	oS << "\tis_pic: \"" << (info.is_pic ? "true" : "false") << "\"," << std::endl;
	oS << "\tis_executable: " << (info.is_executable ? "true" : "false") << std::endl;
	oS << "}" << std::endl;
}

};

is_executable_t is_executable(const char *p){
	return IsExecutable::isExecutable(std::filesystem::path{p});
}

size_t to_human(const is_executable_t info, char *out, size_t size, const bool brief){
	if(brief){
		if(size < IsExecutable::MAX_SIZE_BRIEF_FORMAT){
			return IsExecutable::MAX_SIZE_BRIEF_FORMAT;
		}
	} else {
		if(size < IsExecutable::MAX_SIZE_BRIEF_FORMAT){
			return IsExecutable::MAX_SIZE_VERBOSE_FORMAT;
		}
		if(size < IsExecutable::MAX_SIZE_VERBOSE_FORMAT){
			std::stringstream sS;
			toHumanVerbose(info, sS);

			if(sS.tellg() < size){
				auto sV = sS.rdbuf()->view();
				std::copy(begin(sV), end(sV), out);
				return 0;
			} else {
				return sS.tellg();
			}
		} else {
			std::string_view v{out, size};
			zh::sviewstream sS{v};
			toHumanVerbose(info, sS);
			return 0;
		}
	}
}

size_t toJSON(const is_executable_t info, const char *out, size_t size){
	std::string_view v{out, size};
	zh::sviewstream sS{v};
	toJSON(info, sS);
	return 0;
}
