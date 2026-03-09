
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

# Build shared library: xllr.cpp
c_cpp_shared_lib(xllr.cpp
	"${xllr.cpp_plugin_src};${sdk_cpp_src}"
	"${sdk_cpp_include}"
	"Boost::filesystem"
	"./cpp")

# Copy metaffi.api.cpp DLL into the cpp plugin directory so the plugin is self-contained
add_custom_command(TARGET xllr.cpp POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:metaffi.api.cpp>
		$ENV{METAFFI_HOME}/cpp/$<TARGET_FILE_NAME:metaffi.api.cpp>
)
add_dependencies(xllr.cpp metaffi.api.cpp)

set(xllr.cpp xllr.cpp PARENT_SCOPE)
