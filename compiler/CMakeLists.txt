
find_or_install_package(nlohmann_json)
find_or_install_package(Boost COMPONENTS filesystem)

if(NOT TARGET nlohmann_json::nlohmann_json AND NOT DEFINED nlohmann_json_INCLUDE_DIRS)
	message(FATAL_ERROR
		"[cpp_compiler_plugin] nlohmann_json not found.\n"
		"  Windows: winget install nlohmann.json  (or use vcpkg / conan)\n"
		"  Linux:   sudo apt install nlohmann-json3-dev\n"
	)
endif()

# ---------------------------------------------------------------------------
# Locate MetaFFI IDL C++ headers (required by CppHostCompiler)
# ---------------------------------------------------------------------------

find_path(METAFFI_IDL_INCLUDE_DIR
	NAMES "metaffi/idl/idl_definition.hpp"
	HINTS
		"$ENV{METAFFI_HOME}/sdk/idl_entities/cpp/include"
		"${CMAKE_SOURCE_DIR}/sdk/idl_entities/cpp/include"
	NO_DEFAULT_PATH
)

if(NOT METAFFI_IDL_INCLUDE_DIR)
	message(FATAL_ERROR
		"[cpp_compiler_plugin] MetaFFI IDL C++ headers not found.\n"
		"  Expected: sdk/idl_entities/cpp/include/metaffi/idl/idl_definition.hpp\n"
		"  Ensure METAFFI_HOME is set and the sdk build has run first."
	)
endif()

# ---------------------------------------------------------------------------
# SDK host + guest compiler sources
# ---------------------------------------------------------------------------

set(sdk_cpp_host_srcs
	${metaffi_sdk_root}/compiler/cpp/host/host_compiler.cpp
	${metaffi_sdk_root}/compiler/cpp/host/code_generator.cpp
	${metaffi_sdk_root}/compiler/cpp/host/type_mapper.cpp
	${metaffi_sdk_root}/compiler/cpp/host/entity_path_converter.cpp
)

set(sdk_cpp_guest_srcs
	${metaffi_sdk_root}/compiler/cpp/guest/guest_compiler.cpp
	${metaffi_sdk_root}/compiler/cpp/guest/xcall_generator.cpp
)

set(compiler_plugin_include
	${sdk_include_dir}
	${metaffi_sdk_root}
	${metaffi_sdk_root}/compiler/cpp
	${metaffi_sdk_root}/compiler/cpp/host
	${metaffi_sdk_root}/compiler/cpp/guest
	${METAFFI_IDL_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS}
)

# Resolve nlohmann_json include dir for target_include_directories
if(NOT TARGET nlohmann_json::nlohmann_json)
	set(_nlohmann_include ${nlohmann_json_INCLUDE_DIRS})
else()
	set(_nlohmann_include "")
endif()

# Build shared library: metaffi.compiler.cpp
c_cpp_shared_lib(metaffi.compiler.cpp
	"${CMAKE_CURRENT_LIST_DIR}/cpp_compiler_entry.cpp;${sdk_cpp_host_srcs};${sdk_cpp_guest_srcs};${sdk_src}"
	"${compiler_plugin_include};${_nlohmann_include}"
	"Boost::filesystem"
	"./cpp")

if(TARGET nlohmann_json::nlohmann_json)
	target_link_libraries(metaffi.compiler.cpp PRIVATE nlohmann_json::nlohmann_json)
endif()

set(metaffi.compiler.cpp metaffi.compiler.cpp PARENT_SCOPE)

add_subdirectory(test)
set(cpp_e2e_compiler_test ${cpp_e2e_compiler_test} PARENT_SCOPE)
