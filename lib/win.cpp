#include <IsExecutable/IsExecutable.h>  // for IsExecutable, FileClass::IX_F...
#include <stddef.h>                     // for size_t
#include <stdint.h>                     // for uint16_t, uint32_t, uint8_t
#include <stddef.h>                     // for size_t
#include <stdint.h>                     // for uint16_t, uint32_t, uint8_t
#include <algorithm>                    // for transform
#include <string>                       // for char_traits, string, char_tra...
#include <string_view>                  // for string_view, operator<<, oper...
#include <filesystem>                   // for path, file_size
#include <cctype>                       // for tolower
#include <iostream>                     // for ifstream, basic_istream::read
#include <iterator>                     // for begin, end
#include <cctype>                       // for tolower
#include <iostream>                     // for ifstream, basic_istream::read
#include <iterator>                     // for begin, end

#include "enum_utils.hpp"               // for check_flag
#include "platform_specific.hpp"        // for is_executable_platform_specif...

namespace IsExecutable{
// https://formats.kaitai.io/microsoft_pe/index.html0
// https://learn.microsoft.com/en-us/previous-versions/ms809762(v=msdn.10)
// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#characteristics
enum struct CoffCharacteristics: uint16_t{
	none = 0,
	exe = 0x0002,
	dll = 0x2000,
	f_32bit_machine = 0x0100,
};

enum struct DllCharacteristics: uint16_t{
	none = 0,
	dynamic_base = 0x40,
	high_entropy_va = 0x20,
	nx_compat = 0x100
};

enum PEFormat: uint16_t{
	none = 0,
	pe32 = 0x10b,
	pe32_plus = 0x20b
};


void is_executable_platform_specific_pe(std::ifstream &f, const size_t sz, IsExecutable &res){
	if(sz <= 2 + 0x3a + 4){
		return;
	}

	uint16_t mz_sig = 0U;
	f.read(reinterpret_cast<char *>(&mz_sig), sizeof(mz_sig));
#ifdef WITH_DEBUGGING
	std::cerr << "mz_sig " << std::hex << mz_sig << std::endl;
#endif
	if(mz_sig != 0x5a4d){
		return;
	}
	
	f.seekg(0x3a, std::ios_base::cur);
	uint32_t ofs_pe = 0;
	f.read(reinterpret_cast<char *>(&ofs_pe), sizeof(ofs_pe));
#ifdef WITH_DEBUGGING
	std::cerr << "ofs_pe " << std::hex << ofs_pe << std::endl;
#endif

	if(sz <= ofs_pe){
		return;
	}
	
	f.seekg(ofs_pe, std::ios_base::beg);
	uint32_t pe_signature = 0;
	f.read(reinterpret_cast<char *>(&pe_signature), sizeof(pe_signature));
#ifdef WITH_DEBUGGING
	std::cerr << "pe_signature " << std::hex << pe_signature << std::endl;
#endif
	
	if(pe_signature != 0x4550){
		return;
	}

	f.seekg(2 * 2 + 3 * 4, std::ios_base::cur);
	uint16_t size_of_optional_header = 0;
	f.read(reinterpret_cast<char *>(&size_of_optional_header), sizeof(size_of_optional_header));
#ifdef WITH_DEBUGGING
	std::cerr << "size_of_optional_header " << std::hex << size_of_optional_header << std::endl;
#endif
	
	auto characteristics = CoffCharacteristics::none;
	f.read(reinterpret_cast<char *>(&characteristics), sizeof(characteristics));
#ifdef WITH_DEBUGGING
	std::cerr << "characteristics " << std::hex << static_cast<uint16_t>(characteristics) << std::endl;
#endif

	if(sz < static_cast<size_t>(f.tellg()) + size_of_optional_header){
		return;
	}

	bool const is_executable_image = check_flag(decltype(characteristics)::exe, characteristics);// must be set for everything linkable!
	bool const is_dll = check_flag(decltype(characteristics)::dll, characteristics);

#ifdef WITH_DEBUGGING
	std::cerr << "is_exe " << is_executable_image << std::endl;
	std::cerr << "is_dll " << is_dll << std::endl;
#endif

	auto format = PEFormat::none;
	f.read(reinterpret_cast<char *>(&format), sizeof(format));
	#ifdef WITH_DEBUGGING
		std::cerr << "format " << static_cast<uint16_t>(format) << std::endl;
	#endif

		auto const byteness = static_cast<uint8_t>(format >> 6);
#ifdef WITH_DEBUGGING
	std::cerr << "byteness " << std::dec << static_cast<uint16_t>(byteness) << std::endl;
#endif

	if(byteness > 8){
		return;
	}

	res.is_64 = byteness / 8;

	
	if(is_executable_image){
		if(is_dll){
			res.cls = decltype(res.cls)::IX_FC_library;
		} else {
			res.cls = decltype(res.cls)::IX_FC_executable;
		}
	} else {
		return;
	}

	f.seekg(1 * 2 + 5 * 4 + 4 * (format == PEFormat::pe32), std::ios_base::cur);
	f.seekg(byteness + 2 * 7 + 4 * 6, std::ios_base::cur);
	
	auto dllCharacteristics = DllCharacteristics::none;
	f.read(reinterpret_cast<char *>(&dllCharacteristics), sizeof(dllCharacteristics));
	
#ifdef WITH_DEBUGGING
	std::cerr << "dllCharacteristics " << std::hex << static_cast<uint16_t>(dllCharacteristics) << std::endl;
#endif
	
	res.is_pic = check_flag(decltype(dllCharacteristics)::dynamic_base, dllCharacteristics);
	
	// 4 * byteness
	
}

Backend winBackend{
	.checkBinary=is_executable_platform_specific_pe,
	.script_extensions={"bat"}
};

};
