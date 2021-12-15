#pragma once

#ifdef _MSC_VER
	#define ISEXECUTABLE_EXPORT_API __declspec(dllexport)
	#define ISEXECUTABLE_IMPORT_API __declspec(dllimport)
#else
	#ifdef _WIN32
		#ifdef __cplusplus
			#define ISEXECUTABLE_EXPORT_API [[gnu::dllexport]]
			#define ISEXECUTABLE_IMPORT_API [[gnu::dllimport]]
		#else
			#define ISEXECUTABLE_EXPORT_API __attribute__((dllexport))
			#define ISEXECUTABLE_IMPORT_API __attribute__((dllimport))
		#endif
	#else
		#ifdef __cplusplus
			#define ISEXECUTABLE_EXPORT_API [[gnu::visibility("default")]]
		#else
			#define ISEXECUTABLE_EXPORT_API __attribute__((visibility("default")))
		#endif
		#define ISEXECUTABLE_IMPORT_API
	#endif
#endif

#ifdef ISEXECUTABLE_EXPORTS
	#define ISEXECUTABLE_API ISEXECUTABLE_EXPORT_API
#else
	#define ISEXECUTABLE_API ISEXECUTABLE_IMPORT_API
#endif
