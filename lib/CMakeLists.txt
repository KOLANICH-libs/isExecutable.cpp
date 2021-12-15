set("CPACK_COMPONENT_LIBRARY_DESCRIPTION" "libIsExecutable is a library for detecting whether a file a shared lib or an executable")

buildAndPackageLib(${PROJECT_NAME}
	TARGET_NAME_WITH_LIB_PREFIX
	COMPONENT "library"
	DESCRIPTION "${PROJECT_DESCRIPTION}"
	PUBLIC_INCLUDES ${Include_dir}
)

if(WITH_MIO)
	target_compile_definitions(lib${PROJECT_NAME} PRIVATE -DWITH_MIO=1)
	target_link_libraries(lib${PROJECT_NAME} PRIVATE mio::mio)
endif()

option(WITH_DEBUGGING "Enable debug output" OFF)
if(WITH_DEBUGGING)
	target_compile_definitions(lib${PROJECT_NAME} PRIVATE -DWITH_DEBUGGING=1)
endif()
