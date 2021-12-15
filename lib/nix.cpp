#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <istream>

#include <IsExecutable/IsExecutable.h>

#include "platform_specific.hpp"

// https://serializethoughts.com/2019/06/29/elf-pic-pie
// https://formats.kaitai.io/elf/

namespace IsExecutable{

enum struct HeaderType: uint32_t {
	null = 0,
	interp = 3
};

void is_executable_platform_specific_nix(std::ifstream &f, const size_t sz, IsExecutable &res){
	if(sz <= 5){
		return;
	}

	uint16_t sig = 0U;
	f.read(reinterpret_cast<char *>(&sig), sizeof(sig));
	switch(sig){
		case 0x457F:// 0x7F_"E":
		break;
		case 0x2123:// "#!"
			res.cls = decltype(res.cls)::IX_FC_script;
			return;
		break;
		default:
			return;
	}

	f.read(reinterpret_cast<char *>(&sig), sizeof(sig));
	if(sig != 0x464C){// "LF"
		return;
	}

	uint8_t byteness = 0U;
	f.read(reinterpret_cast<char *>(&byteness), sizeof(byteness));
	if(byteness >= 2 or byteness == 0){
		return;
	}

	res.is_64 = byteness - 1;
	byteness = static_cast<uint8_t>(1U << (1U + byteness));
#ifdef WITH_DEBUGGING
	std::cerr << "byteness " << std::dec << static_cast<uint16_t>(byteness) << std::endl;
#endif

	if(sz <= byteness * 3 + 44){
		return;
	}

	uint8_t endianness = 0U;
	f.read(reinterpret_cast<char *>(&endianness), sizeof(endianness));
	if(endianness > 2 or endianness == 0){
		return;
	}
	endianness = endianness - 1;
#ifdef WITH_DEBUGGING
	std::cerr << "endianness " << std::dec << static_cast<uint16_t>(endianness) << std::endl;
#endif

	uint16_t machine = 0;
	f.read(reinterpret_cast<char *>(&machine), sizeof(machine));
#ifdef WITH_DEBUGGING
	std::cerr << "machine " << std::hex << machine << std::endl;
#endif

	f.seekg(16, std::ios_base::beg);
	uint16_t e_typ = 0U;
	f.read(reinterpret_cast<char *>(&e_typ), sizeof(e_typ));
#ifdef WITH_DEBUGGING
	std::cerr << "e_typ " << std::hex << e_typ << std::endl;
#endif
	f.seekg(2 + 4, std::ios_base::cur);

	uint64_t entry_point = 0;
	f.read(reinterpret_cast<char *>(&entry_point), byteness);
#ifdef WITH_DEBUGGING
	std::cerr << "entry_point " << std::hex << entry_point << std::endl;
#endif

	uint64_t program_header_offset = 0;
	f.read(reinterpret_cast<char *>(&program_header_offset), byteness);
#ifdef WITH_DEBUGGING
	std::cerr << "program_header_offset " << std::hex << program_header_offset << std::endl;
#endif

	f.seekg(byteness + 4 + 2, std::ios_base::cur);

	uint16_t program_header_size = 0;
	f.read(reinterpret_cast<char *>(&program_header_size), sizeof(program_header_size));
#ifdef WITH_DEBUGGING
	std::cerr << "program_header_size " << std::dec << program_header_size << std::endl;
#endif

	uint16_t program_header_count = 0;
	f.read(reinterpret_cast<char *>(&program_header_count), sizeof(program_header_count));
#ifdef WITH_DEBUGGING
	std::cerr << "program_header_count " << std::dec << program_header_count << std::endl;
#endif

	if(sz < program_header_offset + program_header_count * program_header_size){
		return;
	}

	res.cls = decltype(res.cls)::IX_FC_library;
	switch(e_typ){
		case 2:
			res.cls = decltype(res.cls)::IX_FC_executable;
		break;
		case 3:
			res.is_pic = true;
			if(entry_point) {
				for(decltype(program_header_count) i = 0; i < program_header_count; ++i){
					const uint64_t offst = program_header_offset + program_header_size * i;
					f.seekg(offst, std::ios_base::beg);

					HeaderType typ = HeaderType::null;
					f.read(reinterpret_cast<char *>(&typ), sizeof(typ));
#ifdef WITH_DEBUGGING
					std::cerr << i << " " << "typ " << std::hex << static_cast<int>(typ) << std::endl;
#endif
					if(HeaderType::interp == typ) {
#ifdef WITH_DEBUGGING
						std::cerr << "Interp" << std::endl;
#endif
						res.cls = decltype(res.cls)::IX_FC_executable;
						return;
					}
				}
			}
		break;
	}
}

Backend nixBackend{
	.checkBinary=is_executable_platform_specific_nix,
	.script_extensions={"sh", "bash", "zsh"}
};

};
