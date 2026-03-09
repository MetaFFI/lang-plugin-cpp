#include <runtime/runtime_plugin_api.h>
#include <runtime_manager/cpp/runtime_manager.h>
#include <runtime_manager/cpp/module.h>
#include <runtime_manager/cpp/entity.h>
#include <runtime/metaffi_primitives.h>
#include <runtime/cdt.h>
#include <runtime/xcall.h>
#include <utils/logger.hpp>

#include <memory>
#include <cstring>
#include <stdexcept>

static auto LOG = metaffi::get_logger("cpp.runtime");

// ===========================================================================
// Internal helpers
// ===========================================================================

namespace
{

// Allocate an error string via malloc (caller is responsible for freeing).
void set_err(char** err, const char* msg)
{
	if(!err) return;
	const std::size_t len = std::strlen(msg);
	char* buf = static_cast<char*>(malloc(len + 1));
	if(buf) std::memcpy(buf, msg, len + 1);
	*err = buf;
}

// ---------------------------------------------------------------------------
// Context hierarchy: all holder types inherit from CppHolderBase so that
// free_xcall() can call delete with the correct virtual destructor regardless
// of which xcall variant (load_entity / make_callable) created it.
// ---------------------------------------------------------------------------

struct CppHolderBase
{
	virtual ~CppHolderBase() = default;
};

// Context owned by xcalls returned from load_entity().
// Keeps Module and Entity alive for the lifetime of the xcall.
struct CppXCallHolder : CppHolderBase
{
	std::shared_ptr<Module> module;
	std::shared_ptr<Entity> entity;

	CppXCallHolder(std::shared_ptr<Module> m, std::shared_ptr<Entity> e)
		: module(std::move(m)), entity(std::move(e)) {}
};

// Context owned by xcalls returned from make_callable().
// inner_xcall is BORROWED — this holder does NOT own it.
struct CppCallableHolder : CppHolderBase
{
	xcall* inner_xcall = nullptr;

	explicit CppCallableHolder(xcall* x) : inner_xcall(x) {}
	~CppCallableHolder() override = default; // deliberately does NOT free inner_xcall
};

// ---------------------------------------------------------------------------
// Singleton C++ runtime manager
// ---------------------------------------------------------------------------

cpp_runtime_manager& get_cpp_manager()
{
	static cpp_runtime_manager manager;
	return manager;
}

// ---------------------------------------------------------------------------
// Static dispatch helpers for make_callable xcalls.
// Each function receives a CppCallableHolder* as ctx and calls through the
// inner xcall using the appropriate cdts slice convention.
// ---------------------------------------------------------------------------

void cpp_callable_dispatch_params_ret(void* ctx, cdts params_ret[2], char** err)
{
	auto* holder = static_cast<CppCallableHolder*>(ctx);
	(*holder->inner_xcall)(params_ret, err);
}

void cpp_callable_dispatch_params_no_ret(void* ctx, cdts params_ret[2], char** err)
{
	auto* holder = static_cast<CppCallableHolder*>(ctx);
	(*holder->inner_xcall)(&params_ret[0], err);
}

void cpp_callable_dispatch_no_params_ret(void* ctx, cdts params_ret[2], char** err)
{
	auto* holder = static_cast<CppCallableHolder*>(ctx);
	(*holder->inner_xcall)(&params_ret[1], err);
}

void cpp_callable_dispatch_no_params_no_ret(void* ctx, char** err)
{
	auto* holder = static_cast<CppCallableHolder*>(ctx);
	(*holder->inner_xcall)(err);
}

} // anonymous namespace

// ===========================================================================
// runtime_plugin_api.h — required C exports
// ===========================================================================

// ---------------------------------------------------------------------------
// Runtime lifecycle — no-ops for C++ (no external interpreter to start/stop).
// ---------------------------------------------------------------------------

void load_runtime(char** err)
{
	(void)err;
	METAFFI_INFO(LOG, "load_runtime: no-op for C++ plugin");
}

void free_runtime(char** err)
{
	(void)err;
	METAFFI_INFO(LOG, "free_runtime: no-op for C++ plugin");
}

// ---------------------------------------------------------------------------
// load_entity — resolve a C/C++ symbol from a shared library.
//
// entity_path formats (handled by sdk/runtime_manager/cpp/module.h):
//   callable=symbol_name            → CppFreeFunction (extern "C" wrapper)
//   global=symbol_name,getter=true  → CppGlobalGetter  (variable pointer)
//   global=symbol_name,setter=true  → CppGlobalSetter  (variable pointer)
//
// The returned xcall stores:
//   pxcall_and_context[0] = function pointer (resolved symbol address)
//   pxcall_and_context[1] = CppXCallHolder* (keeps Module + Entity alive)
//
// XLLR dispatches by calling: [0]([1], params_ret, err)
// C++ MetaFFI entrypoints must therefore have signature:
//   void entrypoint(void* ctx, cdts* params_ret, char** err)
// ---------------------------------------------------------------------------

xcall* load_entity(const char* module_path, const char* entity_path,
                   metaffi_type_info* params_types, int8_t params_count,
                   metaffi_type_info* retvals_types, int8_t retval_count,
                   char** err)
{
	(void)params_types;
	(void)params_count;
	(void)retvals_types;
	(void)retval_count;

	if(!module_path || module_path[0] == '\0')
	{
		set_err(err, "load_entity: module_path cannot be null or empty");
		return nullptr;
	}
	if(!entity_path || entity_path[0] == '\0')
	{
		set_err(err, "load_entity: entity_path cannot be null or empty");
		return nullptr;
	}

	METAFFI_INFO(LOG, "load_entity: module='{}' entity='{}'", module_path, entity_path);

	try
	{
		std::shared_ptr<Module> module = get_cpp_manager().load_module(module_path);
		std::shared_ptr<Entity> entity = module->load_entity(entity_path);

		void* func_ptr = entity->get_function_pointer();
		if(!func_ptr)
		{
			set_err(err, "load_entity: symbol resolved to null — use extern \"C\" callable= entity paths");
			return nullptr;
		}

		auto* holder = new CppXCallHolder(std::move(module), std::move(entity));
		return new xcall(func_ptr, holder);
	}
	catch(const std::exception& e)
	{
		METAFFI_ERROR(LOG, "load_entity failed: {}", e.what());
		set_err(err, e.what());
		return nullptr;
	}
}

// ---------------------------------------------------------------------------
// make_callable — wrap a C++ callback (cdt_metaffi_callable*) as an xcall.
//
// make_callable_context is a cdt_metaffi_callable* whose val member points
// to an xcall* that wraps the C++ function to be used as a callback.
//
// The returned xcall stores:
//   pxcall_and_context[0] = static cpp_callable_dispatch_* function
//   pxcall_and_context[1] = CppCallableHolder* (non-owning ref to inner xcall)
// ---------------------------------------------------------------------------

xcall* make_callable(void* make_callable_context,
                     metaffi_type_info* params_types, int8_t params_count,
                     metaffi_type_info* retvals_types, int8_t retval_count,
                     char** err)
{
	try
	{
		if(!make_callable_context)
		{
			set_err(err, "make_callable: context is null");
			return nullptr;
		}

		auto* callable  = static_cast<cdt_metaffi_callable*>(make_callable_context);
		auto* inner     = static_cast<xcall*>(callable->val);

		if(!inner || !inner->pxcall_and_context[0])
		{
			set_err(err, "make_callable: inner xcall is null or has null function pointer");
			return nullptr;
		}

		const bool has_params  = params_count  > 0;
		const bool has_retvals = retval_count > 0;

		void* dispatcher =
			 has_params &&  has_retvals ? (void*)cpp_callable_dispatch_params_ret
			: !has_params &&  has_retvals ? (void*)cpp_callable_dispatch_no_params_ret
			:  has_params && !has_retvals ? (void*)cpp_callable_dispatch_params_no_ret
			:                               (void*)cpp_callable_dispatch_no_params_no_ret;

		auto* holder = new CppCallableHolder(inner);
		return new xcall(dispatcher, holder);
	}
	catch(const std::exception& e)
	{
		set_err(err, e.what());
		return nullptr;
	}
}

// ---------------------------------------------------------------------------
// free_xcall — release the xcall and its associated context.
//
// Both CppXCallHolder and CppCallableHolder derive from CppHolderBase, so a
// single delete via the base pointer handles both cases correctly.
// CppCallableHolder's destructor does NOT free the inner xcall since it is
// borrowed from the caller.
// ---------------------------------------------------------------------------

void free_xcall(xcall* pxcall, char** err)
{
	if(!pxcall) return;
	try
	{
		delete static_cast<CppHolderBase*>(pxcall->pxcall_and_context[1]);
		pxcall->pxcall_and_context[1] = nullptr;
		pxcall->pxcall_and_context[0] = nullptr;
		delete pxcall;
	}
	catch(const std::exception& e)
	{
		set_err(err, e.what());
	}
}

// ===========================================================================
// xcall dispatcher exports (required by runtime_plugin_api.h).
//
// XLLR dispatches calls by invoking pxcall_and_context[0] directly, so these
// functions are NOT called by XLLR itself. They are exported for use when
// other languages hold an xcall* and need to dispatch through it (e.g., a
// C host calling a foreign callback via xcall::operator()).
//
// Convention: context IS the xcall* returned by load_entity, so these
// functions simply cast and call through — identical to the Go plugin.
// ===========================================================================

void xcall_params_ret(void* context, cdts params_ret[2], char** out_err)
{
	if(!context) { set_err(out_err, "xcall_params_ret: null context"); return; }
	auto* pxcall = static_cast<xcall*>(context);
	(*pxcall)(params_ret, out_err);
}

void xcall_params_no_ret(void* context, cdts params_ret[2], char** out_err)
{
	if(!context) { set_err(out_err, "xcall_params_no_ret: null context"); return; }
	auto* pxcall = static_cast<xcall*>(context);
	(*pxcall)(&params_ret[0], out_err);
}

void xcall_no_params_ret(void* context, cdts params_ret[2], char** out_err)
{
	if(!context) { set_err(out_err, "xcall_no_params_ret: null context"); return; }
	auto* pxcall = static_cast<xcall*>(context);
	(*pxcall)(&params_ret[1], out_err);
}

void xcall_no_params_no_ret(void* context, char** out_err)
{
	if(!context) { set_err(out_err, "xcall_no_params_no_ret: null context"); return; }
	auto* pxcall = static_cast<xcall*>(context);
	(*pxcall)(out_err);
}
