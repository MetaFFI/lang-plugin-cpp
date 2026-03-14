
find_or_install_package(nlohmann_json)
find_or_install_package(Boost COMPONENTS filesystem)

if(NOT TARGET nlohmann_json::nlohmann_json AND NOT DEFINED nlohmann_json_INCLUDE_DIRS)
	message(FATAL_ERROR
		"[cpp_idl_plugin] nlohmann_json not found.\n"
		"  Windows: winget install nlohmann.json  (or use vcpkg / conan)\n"
		"  Linux:   sudo apt install nlohmann-json3-dev\n"
	)
endif()

# ---------------------------------------------------------------------------
# Locate libclang (LLVM stable C API) — same logic as sdk/idl_compiler/cpp/
# ---------------------------------------------------------------------------

if(NOT DEFINED LLVM_INSTALL_DIR)
	if(DEFINED ENV{LLVM_DIR})
		set(LLVM_INSTALL_DIR "$ENV{LLVM_DIR}")
	elseif(WIN32)
		set(LLVM_INSTALL_DIR "C:/Program Files/LLVM")
	else()
		find_program(LLVM_CONFIG NAMES llvm-config llvm-config-18 llvm-config-17 llvm-config-16)
		if(LLVM_CONFIG)
			execute_process(
				COMMAND "${LLVM_CONFIG}" --prefix
				OUTPUT_VARIABLE LLVM_INSTALL_DIR
				OUTPUT_STRIP_TRAILING_WHITESPACE
			)
		else()
			set(LLVM_INSTALL_DIR "/usr")
		endif()
	endif()
endif()

message(STATUS "[cpp_idl_plugin] Looking for libclang in: ${LLVM_INSTALL_DIR}")

set(_LIBCLANG_INCLUDE_HINTS "${LLVM_INSTALL_DIR}/include")
set(_LIBCLANG_LIB_HINTS     "${LLVM_INSTALL_DIR}/lib")
if(UNIX AND NOT APPLE)
	foreach(_ver 20 19 18 17 16 15 14)
		list(APPEND _LIBCLANG_INCLUDE_HINTS "/usr/lib/llvm-${_ver}/include")
		list(APPEND _LIBCLANG_LIB_HINTS
			"/usr/lib/llvm-${_ver}/lib"
			"/usr/lib/x86_64-linux-gnu"
			"/lib/x86_64-linux-gnu"
		)
	endforeach()
endif()

find_path(LIBCLANG_INCLUDE_DIR
	NAMES "clang-c/Index.h"
	HINTS ${_LIBCLANG_INCLUDE_HINTS}
)

find_library(LIBCLANG_LIB
	NAMES libclang clang libclang-18 libclang.so.18 libclang-18.so.18
	HINTS ${_LIBCLANG_LIB_HINTS}
)

if(NOT LIBCLANG_INCLUDE_DIR)
	message(FATAL_ERROR
		"[cpp_idl_plugin] libclang header 'clang-c/Index.h' not found.\n"
		"  Windows: winget install LLVM.LLVM\n"
		"  Linux:   sudo apt install libclang-dev  (or libclang-18-dev)\n"
		"  Or set LLVM_INSTALL_DIR / LLVM_DIR to your LLVM installation."
	)
endif()

if(NOT LIBCLANG_LIB)
	message(FATAL_ERROR
		"[cpp_idl_plugin] libclang library not found.\n"
		"  Windows: winget install LLVM.LLVM\n"
		"  Linux:   sudo apt install libclang-dev  (or libclang-18-dev)\n"
		"  Or set LLVM_INSTALL_DIR / LLVM_DIR to your LLVM installation."
	)
endif()

message(STATUS "[cpp_idl_plugin] libclang include: ${LIBCLANG_INCLUDE_DIR}")
message(STATUS "[cpp_idl_plugin] libclang lib:     ${LIBCLANG_LIB}")

# On Windows: also locate libclang.dll so we can copy it next to the plugin
if(WIN32)
	find_file(LIBCLANG_DLL
		NAMES libclang.dll
		HINTS "${LLVM_INSTALL_DIR}/bin"
	)
endif()

# ---------------------------------------------------------------------------
# SDK IDL compiler C++ sources
# ---------------------------------------------------------------------------

set(sdk_cpp_idl_srcs
	${metaffi_sdk_root}/idl_compiler/cpp/type_mapper.cpp
	${metaffi_sdk_root}/idl_compiler/cpp/entity_path.cpp
	${metaffi_sdk_root}/idl_compiler/cpp/extractor.cpp
	${metaffi_sdk_root}/idl_compiler/cpp/idl_generator.cpp
)

# Collect plugin source file (cpp_idl_plugin.cpp)
collect_c_cpp_files(${CMAKE_CURRENT_LIST_DIR} metaffi.idl.cpp_plugin)

set(idl_plugin_include
	${sdk_include_dir}
	${metaffi_sdk_root}
	${metaffi_sdk_root}/idl_compiler/cpp
	${LIBCLANG_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS}
)

# Resolve nlohmann_json include dir for target_include_directories
if(NOT TARGET nlohmann_json::nlohmann_json)
	set(_nlohmann_include ${nlohmann_json_INCLUDE_DIRS})
else()
	set(_nlohmann_include "")
endif()

# Build shared library: metaffi.idl.cpp
c_cpp_shared_lib(metaffi.idl.cpp
	"${metaffi.idl.cpp_plugin_src};${sdk_cpp_idl_srcs};${sdk_src}"
	"${idl_plugin_include};${_nlohmann_include}"
	"${LIBCLANG_LIB};Boost::filesystem"
	"./cpp")

if(TARGET nlohmann_json::nlohmann_json)
	target_link_libraries(metaffi.idl.cpp PRIVATE nlohmann_json::nlohmann_json)
endif()

# On Windows: copy libclang.dll next to the plugin
if(WIN32 AND LIBCLANG_DLL)
	add_custom_command(TARGET metaffi.idl.cpp POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${LIBCLANG_DLL}"
			"$ENV{METAFFI_HOME}/cpp/libclang.dll"
		COMMENT "[cpp_idl_plugin] Copying libclang.dll to METAFFI_HOME/cpp/"
	)
endif()

# On Linux: copy libclang.so next to the plugin (matches the Windows copy above)
if(UNIX AND NOT APPLE)
	add_custom_command(TARGET metaffi.idl.cpp POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${LIBCLANG_LIB}"
			"$ENV{METAFFI_HOME}/cpp/libclang.so"
		COMMENT "[cpp_idl_plugin] Copying libclang.so to METAFFI_HOME/cpp/"
	)
endif()

set(metaffi.idl.cpp metaffi.idl.cpp PARENT_SCOPE)
