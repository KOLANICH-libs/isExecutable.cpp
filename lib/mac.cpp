#include <cstddef>
#include <cstdint>
#include <istream>

#include <IsExecutable/IsExecutable.h>

#include "enum_utils.hpp"
#include "platform_specific.hpp"

namespace IsExecutable{
//https://formats.kaitai.io/mach_o/

enum struct FileType: uint32_t{
	none = 0,
	object = 0x1,
	execute = 0x2,
	dylib = 0x6,
	dylinker = 0x7,
	bundle = 0x8,
};

enum struct Flags: uint32_t{
	none = 0,
	pie = 0x200000,
};

void is_executable_platform_specific_mac(std::ifstream &f, const size_t sz, IsExecutable &res){
	if(sz <= 4){
		return;
	}

	uint32_t sig = 0U;
	f.read(reinterpret_cast<char *>(&sig), sizeof(sig));
	switch(sig){
		case 0xFEEDFACE:
			res.is_64 = false;
		break;
		case 0xFEEDFACF:
			res.is_64 = true;
		break;
		default:
			return;
	}

	f.seekg(4 * 2, std::ios_base::cur);

	FileType fileType;
	f.read(reinterpret_cast<char *>(&fileType), sizeof(fileType));

	switch(fileType){
		case FileType::object:
			
		break;
		case FileType::execute:
			res.cls = decltype(res.cls)::IX_FC_executable;
			f.seekg(4 * 2, std::ios_base::cur);

			Flags flags;
			f.read(reinterpret_cast<char *>(&flags), sizeof(flags));
			res.is_pic = check_flag(decltype(flags)::pie, flags);
		break;
		case FileType::dylib:
		case FileType::dylinker:
			res.cls = decltype(res.cls)::IX_FC_library;
			res.is_pic = true;
		break;
		case FileType::bundle:
			
		break;
		default:
			return;
	}
}

Backend macBackend{
	.checkBinary=is_executable_platform_specific_mac,
	.script_extensions=nixBackend.script_extensions
};

};
