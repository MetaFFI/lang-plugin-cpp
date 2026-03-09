#include <doctest/doctest.h>

#include "cpp_test_env.h"

#include <cstdint>
#include <string>

// ============================================================
// C++ guest primitives
// ============================================================

TEST_CASE("cpp - return_int8")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_return_int8", {}, {metaffi_int8_type});
	auto [v] = f.call<int8_t>();
	CHECK(v == static_cast<int8_t>(42));
}

TEST_CASE("cpp - return_int16")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_return_int16", {}, {metaffi_int16_type});
	auto [v] = f.call<int16_t>();
	CHECK(v == static_cast<int16_t>(42));
}

TEST_CASE("cpp - return_int32")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_return_int32", {}, {metaffi_int32_type});
	auto [v] = f.call<int32_t>();
	CHECK(v == static_cast<int32_t>(42));
}

TEST_CASE("cpp - return_int64")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_return_int64", {}, {metaffi_int64_type});
	auto [v] = f.call<int64_t>();
	CHECK(v == int64_t(42));
}

TEST_CASE("cpp - return_uint8")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_return_uint8", {}, {metaffi_uint8_type});
	auto [v] = f.call<uint8_t>();
	CHECK(v == static_cast<uint8_t>(42));
}

TEST_CASE("cpp - return_uint16")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_return_uint16", {}, {metaffi_uint16_type});
	auto [v] = f.call<uint16_t>();
	CHECK(v == static_cast<uint16_t>(42));
}

TEST_CASE("cpp - return_uint32")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_return_uint32", {}, {metaffi_uint32_type});
	auto [v] = f.call<uint32_t>();
	CHECK(v == static_cast<uint32_t>(42));
}

TEST_CASE("cpp - return_uint64")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_return_uint64", {}, {metaffi_uint64_type});
	auto [v] = f.call<uint64_t>();
	CHECK(v == static_cast<uint64_t>(42));
}

TEST_CASE("cpp - return_float32")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_return_float32", {}, {metaffi_float32_type});
	auto [v] = f.call<float>();
	CHECK(v == doctest::Approx(3.14f));
}

TEST_CASE("cpp - return_float64")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_return_float64", {}, {metaffi_float64_type});
	auto [v] = f.call<double>();
	CHECK(v == doctest::Approx(3.14));
}

TEST_CASE("cpp - return_bool")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_return_bool", {}, {metaffi_bool_type});
	auto [v] = f.call<bool>();
	CHECK(v == true);
}

TEST_CASE("cpp - return_string")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_return_string", {}, {metaffi_string8_type});
	auto [v] = f.call<std::string>();
	CHECK(v == "hello");
}

TEST_CASE("cpp - accept_int8")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_accept_int8", {metaffi_int8_type}, {});
	CHECK_NOTHROW(f.call<>(int8_t(42)));
}

TEST_CASE("cpp - accept_int16")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_accept_int16", {metaffi_int16_type}, {});
	CHECK_NOTHROW(f.call<>(int16_t(42)));
}

TEST_CASE("cpp - accept_int32")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_accept_int32", {metaffi_int32_type}, {});
	CHECK_NOTHROW(f.call<>(int32_t(42)));
}

TEST_CASE("cpp - accept_int64")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_accept_int64", {metaffi_int64_type}, {});
	CHECK_NOTHROW(f.call<>(int64_t(42)));
}

TEST_CASE("cpp - accept_uint8")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_accept_uint8", {metaffi_uint8_type}, {});
	CHECK_NOTHROW(f.call<>(uint8_t(42)));
}

TEST_CASE("cpp - accept_uint16")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_accept_uint16", {metaffi_uint16_type}, {});
	CHECK_NOTHROW(f.call<>(uint16_t(42)));
}

TEST_CASE("cpp - accept_uint32")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_accept_uint32", {metaffi_uint32_type}, {});
	CHECK_NOTHROW(f.call<>(uint32_t(42)));
}

TEST_CASE("cpp - accept_uint64")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_accept_uint64", {metaffi_uint64_type}, {});
	CHECK_NOTHROW(f.call<>(uint64_t(42)));
}

TEST_CASE("cpp - accept_float32")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_accept_float32", {metaffi_float32_type}, {});
	CHECK_NOTHROW(f.call<>(float(1.5f)));
}

TEST_CASE("cpp - accept_float64")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_accept_float64", {metaffi_float64_type}, {});
	CHECK_NOTHROW(f.call<>(double(1.5)));
}

TEST_CASE("cpp - accept_bool")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_accept_bool", {metaffi_bool_type}, {});
	CHECK_NOTHROW(f.call<>(true));
}

TEST_CASE("cpp - accept_string")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_accept_string", {metaffi_string8_type}, {});
	CHECK_NOTHROW(f.call<>(std::string("hello")));
}

TEST_CASE("cpp - echo_int64")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity(
		"callable=xcall_echo_int64",
		{metaffi_int64_type},
		{metaffi_int64_type});
	auto [v] = f.call<int64_t>(int64_t(99));
	CHECK(v == int64_t(99));
}

TEST_CASE("cpp - echo_float64")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity(
		"callable=xcall_echo_float64",
		{metaffi_float64_type},
		{metaffi_float64_type});
	auto [v] = f.call<double>(double(2.71));
	CHECK(v == doctest::Approx(2.71));
}

TEST_CASE("cpp - echo_bool true")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity(
		"callable=xcall_echo_bool",
		{metaffi_bool_type},
		{metaffi_bool_type});
	auto [v] = f.call<bool>(true);
	CHECK(v == true);
}

TEST_CASE("cpp - echo_bool false")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity(
		"callable=xcall_echo_bool",
		{metaffi_bool_type},
		{metaffi_bool_type});
	auto [v] = f.call<bool>(false);
	CHECK(v == false);
}

TEST_CASE("cpp - echo_string")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity(
		"callable=xcall_echo_string",
		{metaffi_string8_type},
		{metaffi_string8_type});
	auto [v] = f.call<std::string>(std::string("world"));
	CHECK(v == "world");
}

// ============================================================
// C guest primitives
// ============================================================

TEST_CASE("c - return_int8")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_return_int8", {}, {metaffi_int8_type});
	auto [v] = f.call<int8_t>();
	CHECK(v == static_cast<int8_t>(42));
}

TEST_CASE("c - return_int16")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_return_int16", {}, {metaffi_int16_type});
	auto [v] = f.call<int16_t>();
	CHECK(v == static_cast<int16_t>(42));
}

TEST_CASE("c - return_int32")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_return_int32", {}, {metaffi_int32_type});
	auto [v] = f.call<int32_t>();
	CHECK(v == static_cast<int32_t>(42));
}

TEST_CASE("c - return_int64")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_return_int64", {}, {metaffi_int64_type});
	auto [v] = f.call<int64_t>();
	CHECK(v == int64_t(42));
}

TEST_CASE("c - return_uint8")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_return_uint8", {}, {metaffi_uint8_type});
	auto [v] = f.call<uint8_t>();
	CHECK(v == static_cast<uint8_t>(42));
}

TEST_CASE("c - return_uint16")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_return_uint16", {}, {metaffi_uint16_type});
	auto [v] = f.call<uint16_t>();
	CHECK(v == static_cast<uint16_t>(42));
}

TEST_CASE("c - return_uint32")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_return_uint32", {}, {metaffi_uint32_type});
	auto [v] = f.call<uint32_t>();
	CHECK(v == static_cast<uint32_t>(42));
}

TEST_CASE("c - return_uint64")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_return_uint64", {}, {metaffi_uint64_type});
	auto [v] = f.call<uint64_t>();
	CHECK(v == static_cast<uint64_t>(42));
}

TEST_CASE("c - return_float32")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_return_float32", {}, {metaffi_float32_type});
	auto [v] = f.call<float>();
	CHECK(v == doctest::Approx(3.14f));
}

TEST_CASE("c - return_float64")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_return_float64", {}, {metaffi_float64_type});
	auto [v] = f.call<double>();
	CHECK(v == doctest::Approx(3.14));
}

TEST_CASE("c - return_bool")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_return_bool", {}, {metaffi_bool_type});
	auto [v] = f.call<bool>();
	CHECK(v != false);
}

TEST_CASE("c - return_string")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_return_string", {}, {metaffi_string8_type});
	auto [v] = f.call<std::string>();
	CHECK(v == "hello");
}

TEST_CASE("c - accept_int8")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_accept_int8", {metaffi_int8_type}, {});
	CHECK_NOTHROW(f.call<>(int8_t(42)));
}

TEST_CASE("c - accept_int16")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_accept_int16", {metaffi_int16_type}, {});
	CHECK_NOTHROW(f.call<>(int16_t(42)));
}

TEST_CASE("c - accept_int32")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_accept_int32", {metaffi_int32_type}, {});
	CHECK_NOTHROW(f.call<>(int32_t(42)));
}

TEST_CASE("c - accept_int64")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_accept_int64", {metaffi_int64_type}, {});
	CHECK_NOTHROW(f.call<>(int64_t(42)));
}

TEST_CASE("c - accept_uint8")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_accept_uint8", {metaffi_uint8_type}, {});
	CHECK_NOTHROW(f.call<>(uint8_t(42)));
}

TEST_CASE("c - accept_uint16")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_accept_uint16", {metaffi_uint16_type}, {});
	CHECK_NOTHROW(f.call<>(uint16_t(42)));
}

TEST_CASE("c - accept_uint32")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_accept_uint32", {metaffi_uint32_type}, {});
	CHECK_NOTHROW(f.call<>(uint32_t(42)));
}

TEST_CASE("c - accept_uint64")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_accept_uint64", {metaffi_uint64_type}, {});
	CHECK_NOTHROW(f.call<>(uint64_t(42)));
}

TEST_CASE("c - accept_float32")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_accept_float32", {metaffi_float32_type}, {});
	CHECK_NOTHROW(f.call<>(float(1.5f)));
}

TEST_CASE("c - accept_float64")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_accept_float64", {metaffi_float64_type}, {});
	CHECK_NOTHROW(f.call<>(double(1.5)));
}

TEST_CASE("c - accept_bool")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_accept_bool", {metaffi_bool_type}, {});
	CHECK_NOTHROW(f.call<>(true));
}

TEST_CASE("c - accept_string")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_accept_string", {metaffi_string8_type}, {});
	CHECK_NOTHROW(f.call<>(std::string("hello")));
}

TEST_CASE("c - echo_int64")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity(
		"callable=xcall_c_echo_int64",
		{metaffi_int64_type},
		{metaffi_int64_type});
	auto [v] = f.call<int64_t>(int64_t(99));
	CHECK(v == int64_t(99));
}

TEST_CASE("c - echo_float64")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity(
		"callable=xcall_c_echo_float64",
		{metaffi_float64_type},
		{metaffi_float64_type});
	auto [v] = f.call<double>(double(2.71));
	CHECK(v == doctest::Approx(2.71));
}

TEST_CASE("c - echo_bool true")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity(
		"callable=xcall_c_echo_bool",
		{metaffi_bool_type},
		{metaffi_bool_type});
	auto [v] = f.call<bool>(true);
	CHECK(v == true);
}

TEST_CASE("c - echo_bool false")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity(
		"callable=xcall_c_echo_bool",
		{metaffi_bool_type},
		{metaffi_bool_type});
	auto [v] = f.call<bool>(false);
	CHECK(v == false);
}

TEST_CASE("c - echo_string")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity(
		"callable=xcall_c_echo_string",
		{metaffi_string8_type},
		{metaffi_string8_type});
	auto [v] = f.call<std::string>(std::string("world"));
	CHECK(v == "world");
}
