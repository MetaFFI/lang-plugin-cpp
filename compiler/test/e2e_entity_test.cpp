#include <doctest/doctest.h>
#include "host_MetaFFIHost.hpp"

#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <utils/safe_func.h>
#include <vector>
#include <tuple>

// ---------------------------------------------------------------------------
// Bind once before any tests run via static initialisation.
// ---------------------------------------------------------------------------
namespace {

std::string plugin_path() {
    char* home = metaffi_getenv_alloc("METAFFI_HOME");
    if (!home) throw std::runtime_error("METAFFI_HOME not set");
    std::string p = std::string(home) + "/test/";
    metaffi_free_env(home);
#ifdef _WIN32
    p += "xllr.test.dll";
#elif defined(__APPLE__)
    p += "libxllr.test.dylib";
#else
    p += "libxllr.test.so";
#endif
    return p;
}

// Binds the generated test_host namespace to the xllr.test plugin.
// Runs before any TEST_CASE due to static initialisation order.
bool g_bound = []() {
    try {
        test_host::bind(plugin_path(), "test");
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "[FATAL] bind() failed: %s\n", ex.what());
        std::fflush(stderr);
        std::abort();
    }
    return true;
}();

} // namespace

// ---------------------------------------------------------------------------
// Core / void
// ---------------------------------------------------------------------------
TEST_CASE("no_op")       { CHECK_NOTHROW(test_host::no_op()); }
TEST_CASE("print_hello") { CHECK_NOTHROW(test_host::print_hello()); }

// ---------------------------------------------------------------------------
// Primitive returns
// ---------------------------------------------------------------------------
TEST_CASE("return_int8")   { CHECK(test_host::return_int8()  == int8_t(42)); }
TEST_CASE("return_int16")  { CHECK(test_host::return_int16() == int16_t(1000)); }
TEST_CASE("return_int32")  { CHECK(test_host::return_int32() == int32_t(100000)); }
TEST_CASE("return_int64")  { CHECK(test_host::return_int64() == int64_t(9223372036854775807LL)); }
TEST_CASE("return_uint8")  { CHECK(test_host::return_uint8()  == uint8_t(255)); }
TEST_CASE("return_uint16") { CHECK(test_host::return_uint16() == uint16_t(65535)); }
TEST_CASE("return_uint32") { CHECK(test_host::return_uint32() == uint32_t(4294967295u)); }
TEST_CASE("return_uint64") { CHECK(test_host::return_uint64() == uint64_t(18446744073709551615ULL)); }
TEST_CASE("return_float32") { CHECK(std::abs(test_host::return_float32() - 3.14159f) < 1e-5f); }
TEST_CASE("return_float64") { CHECK(std::abs(test_host::return_float64() - 3.141592653589793) < 1e-10); }
TEST_CASE("return_bool_true")  { CHECK(test_host::return_bool_true()  == true); }
TEST_CASE("return_bool_false") { CHECK(test_host::return_bool_false() == false); }
TEST_CASE("return_string8") { CHECK(test_host::return_string8() == "Hello from test plugin"); }

// return_null: metaffi_null_type cannot be deserialized as void* by the cdts serializer.
// The code generator maps null→void* correctly, but the runtime rejects the type.
TEST_CASE("return_null") { CHECK(test_host::return_null() == nullptr); }

// ---------------------------------------------------------------------------
// Primitive accepts
// ---------------------------------------------------------------------------
TEST_CASE("accept_int8")    { CHECK_NOTHROW(test_host::accept_int8(42)); }
TEST_CASE("accept_int16")   { CHECK_NOTHROW(test_host::accept_int16(1000)); }
TEST_CASE("accept_int32")   { CHECK_NOTHROW(test_host::accept_int32(100000)); }
TEST_CASE("accept_int64")   { CHECK_NOTHROW(test_host::accept_int64(12345678901234LL)); }
TEST_CASE("accept_float32") { CHECK_NOTHROW(test_host::accept_float32(3.14f)); }
TEST_CASE("accept_float64") { CHECK_NOTHROW(test_host::accept_float64(3.14159)); }
TEST_CASE("accept_bool")    {
    CHECK_NOTHROW(test_host::accept_bool(true));
    CHECK_NOTHROW(test_host::accept_bool(false));
}
TEST_CASE("accept_string8") { CHECK_NOTHROW(test_host::accept_string8("test string")); }

// ---------------------------------------------------------------------------
// Echo
// ---------------------------------------------------------------------------
TEST_CASE("echo_int64") {
    CHECK(test_host::echo_int64(123LL)  == 123LL);
    CHECK(test_host::echo_int64(-456LL) == -456LL);
    CHECK(test_host::echo_int64(0LL)    == 0LL);
}
TEST_CASE("echo_float64") {
    CHECK(std::abs(test_host::echo_float64(3.14) - 3.14) < 1e-10);
}
TEST_CASE("echo_string8") {
    CHECK(test_host::echo_string8("hello") == "hello");
    CHECK(test_host::echo_string8("")      == "");
}
TEST_CASE("echo_bool") {
    CHECK(test_host::echo_bool(true)  == true);
    CHECK(test_host::echo_bool(false) == false);
}

// ---------------------------------------------------------------------------
// Arithmetic / concatenation
// ---------------------------------------------------------------------------
TEST_CASE("add_int64") {
    CHECK(test_host::add_int64(3LL, 4LL)   == 7LL);
    CHECK(test_host::add_int64(-5LL, 10LL) == 5LL);
}
TEST_CASE("add_float64") {
    CHECK(std::abs(test_host::add_float64(1.5, 2.5) - 4.0) < 1e-10);
}
TEST_CASE("concat_strings") {
    CHECK(test_host::concat_strings("Hello", " World") == "Hello World");
}

// ---------------------------------------------------------------------------
// Arrays
// ---------------------------------------------------------------------------
TEST_CASE("return_int64_array_1d") {
    auto arr = test_host::return_int64_array_1d();
    REQUIRE(arr.size() == 3u);
    CHECK(arr[0] == 1); CHECK(arr[1] == 2); CHECK(arr[2] == 3);
}
TEST_CASE("return_int64_array_2d") {
    auto arr = test_host::return_int64_array_2d();
    REQUIRE(arr.size() == 2u);
    REQUIRE(arr[0].size() == 2u);
    REQUIRE(arr[1].size() == 2u);
    CHECK(arr[0][0] == 1); CHECK(arr[0][1] == 2);
    CHECK(arr[1][0] == 3); CHECK(arr[1][1] == 4);
}
TEST_CASE("return_string_array") {
    auto arr = test_host::return_string_array();
    REQUIRE(arr.size() == 3u);
    CHECK(arr[0] == "one"); CHECK(arr[1] == "two"); CHECK(arr[2] == "three");
}
TEST_CASE("sum_int64_array") {
    std::vector<int64_t> v = {1, 2, 3, 4, 5};
    CHECK(test_host::sum_int64_array(v) == 15LL);
}
TEST_CASE("echo_int64_array") {
    std::vector<int64_t> in = {10, 20, 30};
    auto out = test_host::echo_int64_array(in);
    REQUIRE(out.size() == in.size());
    for (size_t i = 0; i < in.size(); ++i) CHECK(out[i] == in[i]);
}
TEST_CASE("join_strings") {
    std::vector<std::string> v = {"a", "b", "c"};
    CHECK(test_host::join_strings(v) == "a, b, c");
}

// ---------------------------------------------------------------------------
// TestHandle class
// NOTE: generated getter names come from the IDL field accessor "name" fields:
//   id.getter.name   = "get_id"   → get_id() const
//   data.getter.name = "get_data" → get_data() const
//   data.setter.name = "set_data" → set_data(v)
// ---------------------------------------------------------------------------
// TestHandle tests: the xllr.test plugin creates handles with runtime_id=0xDEADBEEF,
// but the cdts serializer expects the "test" runtime's actual ID. Skip until
// the test plugin's handle creation is fixed to use the correct runtime_id.
TEST_CASE("TestHandle - constructor and id") {
    test_host::TestHandle h;
    CHECK(h.get_id() >= 1LL);
}
TEST_CASE("TestHandle - data getter/setter") {
    test_host::TestHandle h;
    CHECK(h.get_data() == "test_data");
    h.set_data("new_value");
    CHECK(h.get_data() == "new_value");
}
TEST_CASE("TestHandle - append_to_data") {
    test_host::TestHandle h;
    h.set_data("base");
    h.append_to_data("_suffix");
    CHECK(h.get_data() == "base_suffix");
}

// ---------------------------------------------------------------------------
// Global g_name
// NOTE: getter name = "get_g_name", setter name = "set_g_name" (from IDL)
// ---------------------------------------------------------------------------
TEST_CASE("g_name global") {
    test_host::set_g_name("test_value");
    CHECK(test_host::get_g_name() == "test_value");
}

// ---------------------------------------------------------------------------
// Error handling
// ---------------------------------------------------------------------------
TEST_CASE("throw_error") {
    CHECK_THROWS(test_host::throw_error());
}
TEST_CASE("throw_with_message") {
    CHECK_THROWS(test_host::throw_with_message("Custom error message"));
}
TEST_CASE("error_if_negative - positive") { CHECK_NOTHROW(test_host::error_if_negative(42LL)); }
TEST_CASE("error_if_negative - zero")     { CHECK_NOTHROW(test_host::error_if_negative(0LL)); }
TEST_CASE("error_if_negative - negative") { CHECK_THROWS(test_host::error_if_negative(-1LL)); }

// ---------------------------------------------------------------------------
// Multiple return values
// ---------------------------------------------------------------------------
TEST_CASE("return_two_values") {
    auto [n, s] = test_host::return_two_values();
    CHECK(n == 42LL);
    CHECK(s == "answer");
}
TEST_CASE("return_three_values") {
    auto [a, b, c] = test_host::return_three_values();
    CHECK(a == 1LL);
    CHECK(std::abs(b - 2.5) < 1e-10);
    CHECK(c == true);
}
TEST_CASE("swap_values") {
    auto [s, n] = test_host::swap_values(123LL, "hello");
    CHECK(s == "hello");
    CHECK(n == 123LL);
}
