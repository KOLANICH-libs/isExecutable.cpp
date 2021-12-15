find_package(GTest)
add_executable(tests "${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp")
target_include_directories(tests PRIVATE "${Include_dir}" "${Private_include_dir}")
target_link_libraries(tests GTest::GTest GTest::Main "${BACKEND_TARGET_NAME}")

harden(tests)
add_sanitizers(tests)
gtest_discover_tests(
	tests
	DISCOVERY_MODE PRE_TEST
)
