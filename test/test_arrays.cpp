#include <doctest/doctest.h>

#include "cpp_test_env.h"

#include <cstdint>
#include <numeric>
#include <vector>

// ============================================================
// C++ guest array tests
// ============================================================

TEST_CASE("cpp - make_1d_int64_array")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity(
		"callable=xcall_make_1d_int64_array",
		{},
		{metaffi_int64_packed_array_type});
	auto [arr] = f.call<std::vector<int64_t>>();

	CHECK(arr.size() == 5u);

	int64_t sum = 0;
	for (auto v : arr)
	{
		sum += v;
	}
	CHECK(sum == 15);
}

TEST_CASE("cpp - sum_1d_int64_array")
{
	auto& env = cpp_test_env();
	auto f = env.cpp_module.load_entity(
		"callable=xcall_sum_1d_int64_array",
		{metaffi_int64_packed_array_type},
		{metaffi_int64_type});

	std::vector<int64_t> input = {1, 2, 3, 4, 5};
	auto [result] = f.call<int64_t>(input);
	CHECK(result == 15);
}

// ============================================================
// C guest array tests
// ============================================================

TEST_CASE("c - make_1d_int64_array")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity(
		"callable=xcall_c_make_1d_int64_array",
		{},
		{metaffi_int64_packed_array_type});
	auto [arr] = f.call<std::vector<int64_t>>();

	CHECK(arr.size() == 5u);

	int64_t sum = 0;
	for (auto v : arr)
	{
		sum += v;
	}
	CHECK(sum == 15);
}

TEST_CASE("c - sum_1d_int64_array")
{
	auto& env = cpp_test_env();
	auto f = env.c_module.load_entity(
		"callable=xcall_c_sum_1d_int64_array",
		{metaffi_int64_packed_array_type},
		{metaffi_int64_type});

	std::vector<int64_t> input = {1, 2, 3, 4, 5};
	auto [result] = f.call<int64_t>(input);
	CHECK(result == 15);
}
