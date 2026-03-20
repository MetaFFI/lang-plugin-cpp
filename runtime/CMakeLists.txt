
find_or_install_package(Boost COMPONENTS filesystem)

# Collect plugin source files (cpp_api.cpp)
collect_c_cpp_files(${CMAKE_CURRENT_LIST_DIR} xllr.cpp_plugin)

# Collect SDK C++ runtime_manager sources
collect_c_cpp_files("${metaffi_sdk_root}/runtime_manager/cpp" sdk_cpp_runtime_manager)

set(sdk_cpp_src
	${sdk_src}
	${sdk_cpp_runtime_manager_src}
)

set(sdk_cpp_include
	${sdk_include_dir}
	${metaffi_sdk_root}
	${Boost_INCLUDE_DIRS}
)

# Build shared library: xllr.cpp.<abi_suffix> (e.g. xllr.cpp.msvc14d)
c_cpp_shared_lib(xllr.cpp.${CPP_ABI_SUFFIX}
	"${xllr.cpp_plugin_src};${sdk_cpp_src}"
	"${sdk_cpp_include}"
	"Boost::filesystem"
	"./cpp")

# Copy header-only API into the cpp plugin directory
# Also copy the ABI-versioned DLL/SO as the plain "xllr.cpp" name so the
# MetaFFI runtime can find it as "xllr.cpp.dll" / "xllr.cpp.so".
add_custom_command(TARGET xllr.cpp.${CPP_ABI_SUFFIX} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory $ENV{METAFFI_HOME}/cpp/api/metaffi
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${metaffi_sdk_root}/api/cpp/include/metaffi/metaffi.h
		$ENV{METAFFI_HOME}/cpp/api/metaffi/metaffi.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:xllr.cpp.${CPP_ABI_SUFFIX}>
		$ENV{METAFFI_HOME}/cpp/$<TARGET_FILE_NAME:xllr.cpp.${CPP_ABI_SUFFIX}>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:xllr.cpp.${CPP_ABI_SUFFIX}>
		$ENV{METAFFI_HOME}/cpp/xllr.cpp$<TARGET_FILE_SUFFIX:xllr.cpp.${CPP_ABI_SUFFIX}>
)

set(xllr.cpp xllr.cpp.${CPP_ABI_SUFFIX} PARENT_SCOPE)
