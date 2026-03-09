#include <doctest/doctest.h>

#include "cpp_test_env.h"

#include <cstdint>
#include <string>

TEST_CASE("c - no_op")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_no_op", {}, {});
	CHECK_NOTHROW(f.call<>());
}

TEST_CASE("c - hello_world")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity("callable=xcall_c_hello_world", {}, {metaffi_string8_type});
	auto [msg] = f.call<std::string>();
	CHECK(msg == "Hello World from C");
}

TEST_CASE("c - returns_an_error")
{
	// c_guest_returns_an_error returns int (-1); we expose it as int32 return, not throw
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity(
		"callable=xcall_c_returns_an_error",
		{},
		{metaffi_int32_type});
	auto [v] = f.call<int32_t>();
	CHECK(v == -1);
}

TEST_CASE("c - div_integers")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity(
		"callable=xcall_c_div_integers",
		{metaffi_int64_type, metaffi_int64_type},
		{metaffi_float64_type});
	auto [v] = f.call<double>(int64_t(10), int64_t(2));
	CHECK(v == doctest::Approx(5.0));
}

TEST_CASE("c - counter")
{
	auto& env = cpp_test_env();

	auto set = env.c_module.load_entity(
		"callable=xcall_c_set_counter",
		{metaffi_int64_type},
		{});

	auto get = env.c_module.load_entity(
		"callable=xcall_c_get_counter",
		{},
		{metaffi_int64_type});

	auto inc = env.c_module.load_entity(
		"callable=xcall_c_inc_counter",
		{metaffi_int64_type},
		{metaffi_int64_type});

	// Reset counter to 0
	CHECK_NOTHROW(set.call<>(int64_t(0)));

	auto [v0] = get.call<int64_t>();
	CHECK(v0 == 0);

	auto [v1] = inc.call<int64_t>(int64_t(3));
	CHECK(v1 == 3);
}
