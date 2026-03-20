find_or_install_package(doctest)

# --- Phase 1: Generate C++ host code from xllr.test IDL at build time ---
set(idl_path "${metaffi_sdk_root}/test_modules/guest_modules/test/xllr.test.idl.json")
set(gen_dir  "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(gen_hpp  "${gen_dir}/host_MetaFFIHost.hpp")
set(gen_cpp  "${gen_dir}/host_MetaFFIHost.cpp")

file(MAKE_DIRECTORY "${gen_dir}")

add_custom_command(
    OUTPUT ${gen_hpp} ${gen_cpp}
    COMMAND metaffi -c --idl "${idl_path}" -h cpp
    WORKING_DIRECTORY ${gen_dir}
    DEPENDS ${idl_path} metaffi.compiler.cpp xllr metaffi
    COMMENT "cpp compiler test: generating C++ host stubs from xllr.test IDL"
)

add_custom_target(cpp_compiler_generate_host DEPENDS ${gen_hpp} ${gen_cpp})

# --- Phase 2: Build doctest executable ---
# Target is named cpp_e2e_compiler_test to avoid collision with sdk/compiler/cpp/cpp_compiler_test
set(cpp_e2e_compiler_test_src
    ${CMAKE_CURRENT_LIST_DIR}/test_main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/e2e_entity_test.cpp
    ${gen_cpp}
)

set(cpp_e2e_compiler_test_includes
    ${gen_dir}
    ${sdk_include_dir}
    ${metaffi_sdk_root}/api/cpp/include
    ${CMAKE_CURRENT_LIST_DIR}
    ${doctest_INCLUDE_DIRS}
)

c_cpp_exe(cpp_e2e_compiler_test
    "${cpp_e2e_compiler_test_src}"
    "${cpp_e2e_compiler_test_includes}"
    "doctest::doctest"
    "$ENV{METAFFI_HOME}"
)

# Header-only API: force-include metaffi.h so all TUs (including generated code) get inline implementations
if(MSVC)
    target_compile_options(cpp_e2e_compiler_test PRIVATE "/FImetaffi/metaffi.h")
else()
    target_compile_options(cpp_e2e_compiler_test PRIVATE -include metaffi/metaffi.h)
endif()

target_compile_definitions(cpp_e2e_compiler_test PRIVATE DOCTEST_CONFIG_NO_WINDOWS_SEH)
add_dependencies(cpp_e2e_compiler_test cpp_compiler_generate_host xllr.test metaffi.compiler.cpp)

# --- Phase 3: CTest entry ---
add_test(NAME cpp_e2e_compiler_test
    COMMAND $ENV{METAFFI_HOME}/cpp_e2e_compiler_test${CMAKE_EXECUTABLE_SUFFIX})

if(WIN32)
    set(_path "$ENV{METAFFI_HOME}/cpp;$ENV{METAFFI_HOME}/test;$ENV{PATH}")
    string(REPLACE ";" "\\;" _path "${_path}")
    set_tests_properties(cpp_e2e_compiler_test PROPERTIES ENVIRONMENT "PATH=${_path}")
elseif(APPLE)
    set_tests_properties(cpp_e2e_compiler_test PROPERTIES
        ENVIRONMENT "DYLD_LIBRARY_PATH=$ENV{METAFFI_HOME}/cpp:$ENV{METAFFI_HOME}/test:$ENV{DYLD_LIBRARY_PATH}")
else()
    set_tests_properties(cpp_e2e_compiler_test PROPERTIES
        ENVIRONMENT "LD_LIBRARY_PATH=$ENV{METAFFI_HOME}/cpp:$ENV{METAFFI_HOME}/test:$ENV{LD_LIBRARY_PATH}")
endif()

set(cpp_e2e_compiler_test cpp_e2e_compiler_test PARENT_SCOPE)
