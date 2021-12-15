#pragma once
#include <cstddef>
#include <cstdint>

#include <fstream>
#include <iostream>
#include <istream>

#include <filesystem>

#include "api.h"

#ifdef __cplusplus
namespace IsExecutable{
#endif

#ifdef __cplusplus
	enum struct System: uint8_t {
#else
	enum System {
#endif
		IX_SYS_unknown = 0,
		IX_SYS_nix = 1, //< a SystemV ELF binary
		IX_SYS_mac = 2, //< a Mach-O binary
		IX_SYS_win = 3, //< a PE binary
	};
#ifdef __cplusplus
	using enum System;
#endif

#ifdef __cplusplus
	enum struct FileClass: uint8_t {
#else
	enum FileClass {
#endif
		IX_FC_error = 0, //< error occured during retrieval of the info
		IX_FC_ordinary = 1, //< file format not recognized as a known executable one
		IX_FC_library = 2, //< a shared library
		IX_FC_executable = 3, //< an executable
		IX_FC_script = 4, //< a script
	};
#ifdef __cplusplus
	using enum FileClass;
#endif

	struct [[gnu::packed]] IsExecutable{
		enum System sys: 2 = System::IX_SYS_unknown; //< OS
		enum FileClass cls: 3 = FileClass::IX_FC_error; //< file class
		bool is_executable: 1 = false; //< has execute rights in FS
		bool is_pic: 1 = false; //< is executable using position-independent code
		bool is_64: 1 = false; //< is executable 32-bit
	};

	ISEXECUTABLE_API IsExecutable isExecutable(const std::filesystem::path &p); //< Gets info about the file into the structure

	ISEXECUTABLE_API void toHuman(IsExecutable i, std::ostream &s, bool brief); //< Prints info about the file into from structure into a stream
	ISEXECUTABLE_API void toHumanVerbose(IsExecutable i, std::ostream &s); //< Prints info about the file into from structure into a stream. Uses verbose format.
	ISEXECUTABLE_API void toHumanBrief(IsExecutable i, std::ostream &s); //< Prints info about the file into from structure into a stream Uses 1-char format

	ISEXECUTABLE_API void toJSON(IsExecutable info, std::ostream &s); //< Prints info about the file into from structure into a stream as JSON.
#ifdef __cplusplus
};
#endif

#ifdef __cplusplus
	typedef struct IsExecutable::IsExecutable is_executable_t;
#else
	typedef struct IsExecutable is_executable_t;
#endif

extern "C" {
	ISEXECUTABLE_API is_executable_t is_executable(char *p); //< Gets info about the file into the structure
	ISEXECUTABLE_API size_t to_human(is_executable_t i, char *out, size_t size, bool brief); //< Prints info about the file into a string from structure into a stream
	ISEXECUTABLE_API size_t toJSON(is_executable_t info, char *out, size_t size); //< Prints info about the file into from structure into a string as JSON.
};
