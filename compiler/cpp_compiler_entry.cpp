/**
 * MetaFFI C++ compiler plugin entry point.
 *
 * Exports the flat extern "C" entry points expected by the MetaFFI CLI
 * (language_plugin_interface_wrapper.cpp):
 *   init_plugin()       — one-time initialisation (no-op for C++)
 *   compile_to_guest()  — generate xcall wrapper .cpp from IDL JSON
 *                         (delegates to CppGuestCompiler).
 *   compile_from_host() — generate C++ host stubs that wrap foreign entities
 *                         via the MetaFFI C++ API (delegates to CppHostCompiler).
 */

#include <compiler/cpp/host/host_compiler.h>
#include <compiler/cpp/guest/guest_compiler.h>

#include <string>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

namespace
{

void set_error(const std::string& msg, char** out_err, uint32_t* out_err_len)
{
	if(!out_err || !out_err_len) return;

	const std::size_t len = msg.size();
	*out_err     = static_cast<char*>(malloc(len + 1));
	if(*out_err)
	{
		std::memcpy(*out_err, msg.c_str(), len + 1);
	}
	*out_err_len = static_cast<uint32_t>(len);
}

metaffi::compiler::cpp::CppHostCompiler& get_host_compiler()
{
	static metaffi::compiler::cpp::CppHostCompiler compiler;
	return compiler;
}

metaffi::compiler::cpp::CppGuestCompiler& get_guest_compiler()
{
	static metaffi::compiler::cpp::CppGuestCompiler compiler;
	return compiler;
}

// Parse "stem=<name>" from a guest_options string; return "guest_xcall" if not found.
std::string parse_stem(const char* opts, uint32_t opts_len)
{
	if (!opts || opts_len == 0) return "guest_xcall";
	const std::string s(opts, opts_len);
	const std::string prefix = "stem=";
	const auto pos = s.find(prefix);
	if (pos == std::string::npos) return "guest_xcall";
	return s.substr(pos + prefix.size());
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// Flat extern "C" entry points
// ---------------------------------------------------------------------------

extern "C"
{

void init_plugin()
{
	// Trigger static initialisation of CppHostCompiler — no-op otherwise.
	(void)get_host_compiler();
}

void compile_to_guest(const char* idl_def_json,   uint32_t idl_def_json_length,
                      const char* output_path,    uint32_t output_path_length,
                      const char* guest_options,  uint32_t guest_options_length,
                      char** out_err,             uint32_t* out_err_len)
{
	try
	{
		const std::string json_str(idl_def_json, idl_def_json_length);
		const std::string out_path(output_path,  output_path_length);
		const std::string stem = parse_stem(guest_options, guest_options_length);

		get_guest_compiler().compile(json_str, out_path, stem);

		if(out_err)     *out_err     = nullptr;
		if(out_err_len) *out_err_len = 0;
	}
	catch(const std::exception& e)
	{
		set_error(std::string("compile_to_guest: ") + e.what(), out_err, out_err_len);
	}
	catch(...)
	{
		set_error("compile_to_guest: unknown error", out_err, out_err_len);
	}
}

void compile_from_host(const char* idl_def_json,   uint32_t idl_def_json_length,
                       const char* output_path,    uint32_t output_path_length,
                       const char* host_options,   uint32_t host_options_length,
                       char** out_err,             uint32_t* out_err_len)
{
	try
	{
		const std::string json_str(idl_def_json, idl_def_json_length);
		const std::string out_path(output_path,  output_path_length);

		// host_options may carry the desired output file stem; default to "host"
		const std::string stem =
			(host_options && host_options_length > 0 && host_options[0] != '\0')
			? std::string(host_options, host_options_length)
			: "host";

		get_host_compiler().compile(json_str, out_path, stem);

		if(out_err)     *out_err     = nullptr;
		if(out_err_len) *out_err_len = 0;
	}
	catch(const std::exception& e)
	{
		set_error(std::string("compile_from_host: ") + e.what(), out_err, out_err_len);
	}
	catch(...)
	{
		set_error("compile_from_host: unknown error", out_err, out_err_len);
	}
}

} // extern "C"
