file(GLOB_RECURSE SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")


add_executable(IsExecutable "${SRCFILES}")
target_include_directories(IsExecutable PUBLIC "${Include_dir}")
set_target_properties(IsExecutable PROPERTIES OUTPUT_NAME "IsExecutable")
harden(IsExecutable)

target_link_libraries(IsExecutable PRIVATE libIsExecutable HydrArgs::libHydrArgs HydrArgs_discoverer::HydrArgs_discoverer mio::mio)

if(WITH_HYDRARGS)
	target_compile_definitions(IsExecutable PRIVATE -DWITH_HYDRARGS=1)
endif()

cpack_add_component(bin
	DISPLAY_NAME "binary"
	DESCRIPTION "The CLI tool scanning a binary for certain bytes"
	DEPENDS "library" "library_sdk"
)

install(TARGETS "IsExecutable"
	RUNTIME
	COMPONENT "bin"
)

set("CPACK_COMPONENT_BIN_DESCRIPTION" "${PROJECT_DESCRIPTION}")
set("CPACK_DEBIAN_BIN_PACKAGE_NAME" "IsExecutable")
set("CPACK_RPM_BIN_PACKAGE_NAME" "IsExecutable")

set("CPACK_DEBIAN_BIN_PACKAGE_DEPENDS" "${CPACK_DEBIAN_LIBRARY_PACKAGE_NAME} (>= ${CPACK_PACKAGE_VERSION})")

if(WITH_HYDRARGS)
	set("CPACK_DEBIAN_BIN_PACKAGE_DEPENDS" "${CPACK_DEBIAN_BIN_PACKAGE_DEPENDS}, libhydrargs_discoverer")
endif()

set("CPACK_RPM_BIN_PACKAGE_DEPENDS" "${CPACK_RPM_LIBRARY_PACKAGE_NAME}")


list(APPEND CPACK_COMPONENTS_ALL "bin")  # strangely, not populated automatically correctly

pass_through_cpack_vars()
