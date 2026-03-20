#include "cpp_test_env.h"

#include <utils/env_utils.h>

#include <filesystem>
#include <stdexcept>
#include <string>

// DLL names are injected as compile-time macros from CMakeLists.txt
#ifndef CPP_GUEST_LIB_NAME
#	define CPP_GUEST_LIB_NAME "cpp_guest_module.dll"
#endif
#ifndef C_GUEST_LIB_NAME
#	define C_GUEST_LIB_NAME "c_guest_module.dll"
#endif
#ifndef CPP_RUNTIME_NAME
#	define CPP_RUNTIME_NAME "cpp"
#endif

namespace
{

std::string require_env(const char* name)
{
	std::string value = get_env_var(name);
	if (value.empty())
	{
		throw std::runtime_error(std::string("Environment variable not set: ") + name);
	}
	return value;
}

std::string resolve_cpp_module_path()
{
	std::string root = require_env("METAFFI_SOURCE_ROOT");
	std::filesystem::path path(root);
	path /= "sdk";
	path /= "test_modules";
	path /= "guest_modules";
	path /= "cpp";
	path /= "test_bin";
	path /= CPP_GUEST_LIB_NAME;

	std::string path_str = path.string();
	if (!std::filesystem::exists(path_str))
	{
		throw std::runtime_error(
			"C++ guest module not found: " + path_str +
			". Build cpp_guest_module target first.");
	}
	return path_str;
}

std::string resolve_c_module_path()
{
	std::string root = require_env("METAFFI_SOURCE_ROOT");
	std::filesystem::path path(root);
	path /= "sdk";
	path /= "test_modules";
	path /= "guest_modules";
	path /= "c";
	path /= "test_bin";
	path /= C_GUEST_LIB_NAME;

	std::string path_str = path.string();
	if (!std::filesystem::exists(path_str))
	{
		throw std::runtime_error(
			"C guest module not found: " + path_str +
			". Build c_guest_module target first.");
	}
	return path_str;
}

} // namespace

CppTestEnv::CppTestEnv()
	: runtime(CPP_RUNTIME_NAME)
	, cpp_module(runtime.runtime_plugin(), resolve_cpp_module_path())
	, c_module(runtime.runtime_plugin(), resolve_c_module_path())
{
	runtime.load_runtime_plugin();
}

CppTestEnv::~CppTestEnv()
{
	runtime.release_runtime_plugin();
}

CppTestEnv& cpp_test_env()
{
	static CppTestEnv env;
	return env;
}
