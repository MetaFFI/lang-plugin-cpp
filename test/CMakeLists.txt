# C++ host tests: C++ executable calling cpp_guest_module and c_guest_module via xllr.cpp

find_or_install_package(Boost COMPONENTS filesystem)
find_or_install_package(doctest)

set(cpp_host_test_src
	${CMAKE_CURRENT_LIST_DIR}/test_main.cpp
	${CMAKE_CURRENT_LIST_DIR}/cpp_test_env.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_core_cpp.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_core_c.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_primitives.cpp
	${CMAKE_CURRENT_LIST_DIR}/test_arrays.cpp
)

set(cpp_host_test_includes
	${sdk_include_dir}
	${metaffi_sdk_root}/api/cpp/include
	${CMAKE_CURRENT_LIST_DIR}
	${doctest_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

c_cpp_exe(cpp_host_test
	"${cpp_host_test_src}"
	"${cpp_host_test_includes}"
	"doctest::doctest;Boost::filesystem;metaffi.api.cpp"
	"$ENV{METAFFI_HOME}"
)

target_compile_definitions(cpp_host_test PRIVATE
	CPP_GUEST_LIB_NAME="${CMAKE_SHARED_LIBRARY_PREFIX}cpp_guest_module${CMAKE_SHARED_LIBRARY_SUFFIX}"
	C_GUEST_LIB_NAME="${CMAKE_SHARED_LIBRARY_PREFIX}c_guest_module${CMAKE_SHARED_LIBRARY_SUFFIX}"
	DOCTEST_CONFIG_NO_WINDOWS_SEH
)

add_dependencies(cpp_host_test cpp_guest_module c_guest_module)

add_test(NAME cpp_host_test
	COMMAND $ENV{METAFFI_HOME}/cpp_host_test${CMAKE_EXECUTABLE_SUFFIX})

if(WIN32)
	set(_cpp_host_test_path "$ENV{METAFFI_HOME}/cpp;$ENV{PATH}")
	string(REPLACE ";" "\\;" _cpp_host_test_path "${_cpp_host_test_path}")
	set_tests_properties(cpp_host_test PROPERTIES
		ENVIRONMENT "PATH=${_cpp_host_test_path}")
elseif(APPLE)
	set_tests_properties(cpp_host_test PROPERTIES
		ENVIRONMENT "DYLD_LIBRARY_PATH=$ENV{METAFFI_HOME}/cpp:$ENV{DYLD_LIBRARY_PATH}")
else()
	set_tests_properties(cpp_host_test PROPERTIES
		ENVIRONMENT "LD_LIBRARY_PATH=$ENV{METAFFI_HOME}/cpp:$ENV{LD_LIBRARY_PATH}")
endif()

set(cpp_host_test cpp_host_test PARENT_SCOPE)
