#include <idl_compiler/idl_plugin_interface.h>
#include <idl_compiler/cpp/extractor.h>
#include <idl_compiler/cpp/idl_generator.h>
#include <runtime/xllr_capi_loader.h>
#include <utils/logger.hpp>

#include <string>
#include <cstring>
#include <stdexcept>
#include <fstream>

static auto LOG = metaffi::get_logger("cpp.idl");

// ===========================================================================
// CppIDLPlugin
//
// Parses a C/C++ header file (.h / .hpp) using libclang (CppExtractor) and
// generates MetaFFI IDL JSON from the extracted metadata (CppIDLGenerator).
//
// Input (parse_idl):
//   source_code / source_length   — optional: raw header content to write to
//                                    file_or_path before parsing; ignored if empty.
//   file_or_path / file_or_path_length — path to the header file to parse.
// ===========================================================================

class CppIDLPlugin : public idl_plugin_interface
{
public:
	void init() override
	{
		METAFFI_INFO(LOG, "CppIDLPlugin initialized");
	}

	void parse_idl(const char* source_code,   uint32_t source_length,
	               const char* file_or_path,  uint32_t file_or_path_length,
	               char** out_idl_def_json,   uint32_t* out_idl_def_json_length,
	               char** out_err,            uint32_t* out_err_len) override
	{
		try
		{
			// Validate file path
			if(!file_or_path || file_or_path_length == 0)
			{
				throw std::invalid_argument("file_or_path is required");
			}

			std::string header_path(file_or_path, file_or_path_length);

			// If source code is provided, write it to the given path before parsing
			if(source_code && source_length > 0)
			{
				std::ofstream ofs(header_path, std::ios::binary);
				if(!ofs)
				{
					throw std::runtime_error("Failed to open '" + header_path + "' for writing");
				}
				ofs.write(source_code, source_length);
			}

			// Extract C++ interface using libclang
			CppExtractor extractor(header_path);
			CppModuleInfo info = extractor.extract();

			// Generate MetaFFI IDL JSON
			CppIDLGenerator generator(info);
			nlohmann::json idl_json = generator.generate();
			std::string idl_str    = idl_json.dump(2);

			// Return result via xllr_alloc_string (caller frees via xllr_free_string)
			*out_idl_def_json        = xllr_alloc_string(idl_str.c_str(), idl_str.size());
			*out_idl_def_json_length = static_cast<uint32_t>(idl_str.size());
			*out_err                 = nullptr;
			*out_err_len             = 0;

			METAFFI_INFO(LOG, "parse_idl: generated IDL for '{}'", header_path);
		}
		catch(const std::exception& e)
		{
			const char* msg     = e.what();
			const auto  msg_len = std::strlen(msg);
			*out_err     = xllr_alloc_string(msg, msg_len);
			*out_err_len = static_cast<uint32_t>(msg_len);
			*out_idl_def_json        = nullptr;
			*out_idl_def_json_length = 0;
		}
		catch(...)
		{
			const char* msg     = "Unknown error in CppIDLPlugin::parse_idl";
			const auto  msg_len = std::strlen(msg);
			*out_err     = xllr_alloc_string(msg, msg_len);
			*out_err_len = static_cast<uint32_t>(msg_len);
			*out_idl_def_json        = nullptr;
			*out_idl_def_json_length = 0;
		}
	}
};

// ===========================================================================
// Flat extern "C" entry points (required by MetaFFI plugin loader)
// ===========================================================================

static CppIDLPlugin s_plugin;
static bool         s_initialized = false;

extern "C"
{

void init_plugin()
{
	if(!s_initialized)
	{
		s_plugin.init();
		s_initialized = true;
	}
}

void parse_idl(const char* source_code,   uint32_t source_code_length,
               const char* file_or_path,  uint32_t file_or_path_length,
               char** out_idl_def_json,   uint32_t* out_idl_def_json_length,
               char** out_err,            uint32_t* out_err_len)
{
	if(!s_initialized)
	{
		s_plugin.init();
		s_initialized = true;
	}

	s_plugin.parse_idl(source_code,   source_code_length,
	                   file_or_path,  file_or_path_length,
	                   out_idl_def_json, out_idl_def_json_length,
	                   out_err,       out_err_len);
}

} // extern "C"
