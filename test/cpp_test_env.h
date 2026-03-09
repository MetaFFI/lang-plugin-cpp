#pragma once

#include <metaffi/api/metaffi_api.h>

struct CppTestEnv
{
	metaffi::api::MetaFFIRuntime runtime;
	metaffi::api::MetaFFIModule  cpp_module;
	metaffi::api::MetaFFIModule  c_module;

	CppTestEnv();
	~CppTestEnv();
};

CppTestEnv& cpp_test_env();
