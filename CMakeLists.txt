
# lang-plugin-cpp: MetaFFI C++ language plugin
# Provides runtime plugin (xllr.cpp), IDL plugin (metaffi.idl.cpp),
# and compiler plugin (metaffi.compiler.cpp).

if(NOT DEFINED sdk_src)
    message(FATAL_ERROR "sdk_src is not defined. Ensure sdk/CMakeLists.txt is added before lang-plugin-cpp.")
endif()

add_subdirectory(runtime)
add_subdirectory(idl)
add_subdirectory(compiler)
add_subdirectory(test)

add_custom_target(cpp)
add_dependencies(cpp xllr.cpp metaffi.idl.cpp metaffi.compiler.cpp cpp_host_test cpp_e2e_compiler_test)

set(cpp cpp PARENT_SCOPE)
