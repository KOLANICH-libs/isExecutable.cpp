#include <IsExecutable/IsExecutable.h>

#include <unordered_set>

namespace IsExecutable{
	typedef void (PlatformSpecificFunctionT)(std::ifstream &f, const size_t sz, IsExecutable &res);
	
	struct Backend{
		PlatformSpecificFunctionT *checkBinary;
		std::unordered_set<std::string> script_extensions;
	};

	extern Backend nixBackend, winBackend, macBackend;
};
