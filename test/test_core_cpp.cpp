#include <doctest/doctest.h>

#include "cpp_test_env.h"

#include <cstdint>
#include <string>

TEST_CASE("cpp - no_op")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_no_op", {}, {});
	CHECK_NOTHROW(f.call<>());
}

TEST_CASE("cpp - hello_world")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_hello_world", {}, {metaffi_string8_type});
	auto [msg] = f.call<std::string>();
	CHECK(msg == "Hello World from C++");
}

TEST_CASE("cpp - returns_an_error")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity("callable=xcall_returns_an_error", {}, {});
	CHECK_THROWS(f.call<>());
}

TEST_CASE("cpp - div_integers")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity(
		"callable=xcall_div_integers",
		{metaffi_int64_type, metaffi_int64_type},
		{metaffi_float64_type});
	auto [v] = f.call<double>(int64_t(10), int64_t(2));
	CHECK(v == doctest::Approx(5.0));
}

TEST_CASE("cpp - counter")
{
	auto& env = cpp_test_env();

	auto set = env.cpp_module.load_entity(
		"callable=xcall_set_counter",
		{metaffi_int64_type},
		{});

	auto get = env.cpp_module.load_entity(
		"callable=xcall_get_counter",
		{},
		{metaffi_int64_type});

	auto inc = env.cpp_module.load_entity(
		"callable=xcall_inc_counter",
		{metaffi_int64_type},
		{metaffi_int64_type});

	// Reset counter to 0
	CHECK_NOTHROW(set.call<>(int64_t(0)));

	auto [v0] = get.call<int64_t>();
	CHECK(v0 == 0);

	auto [v1] = inc.call<int64_t>(int64_t(3));
	CHECK(v1 == 3);
}
