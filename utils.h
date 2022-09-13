#pragma once

#include <Windows.h>
#include <string>

#include "structs.h"
#include "xorstr.h"

#define M_PI 3.1415926
#define RELATIVE_ADDR(addr, size) ((PBYTE)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))

#define ReadPtr(base, offset) (*(ULONGLONG*)((base + offset)))
#define ReadDWORD(base, offset) (*(PDWORD)(((PBYTE)base + offset)))
#define ReadBYTE(base, offset) (*(((PBYTE)base + offset)))
    
namespace utils {
	BOOLEAN Initialize();

	bool get_bone_matrix(uint64_t actor, unsigned int index, FVector* Out);
	
	template <size_t N>
	uint8_t* find_signature(const std::string_view module, const char(&signature)[ N ]);
	
	FVector2D w2s(FVector Location, FMinimalViewInfo info);
	bool get_aim_angles(FRotator cam_rotation, FVector cam_location, uint64_t aactor, int bone, FRotator* out);
	bool LoadStyle();
	const wchar_t* get_name(uint64_t object);

	static float sqrtf_custom(float n)
	{
		static union { int i; float f; } u;

		u.i = 0x5F375A86 - (*(int*)&n >> 1);

		return (int(3) - n * u.f * u.f) * n * u.f * 0.5f;
	}

	PBYTE FindPattern(LPCSTR pattern, LPCSTR mask);
	
	namespace detail
	{
		extern "C" void* _spoofer_stub();

		template <typename Ret, typename... Args>
		static inline auto shellcode_stub_helper(
			const void* shell,
			Args... args
			) -> Ret
		{
			auto fn = (Ret(*)(Args...))(shell);
			return fn(args...);
		}

		template <std::size_t Argc, typename>
		struct argument_remapper
		{
			// At least 5 params
			template<
				typename Ret,
				typename First,
				typename Second,
				typename Third,
				typename Fourth,
				typename... Pack
			>
				static auto do_call(
					const void* shell,
					void* shell_param,
					First first,
					Second second,
					Third third,
					Fourth fourth,
					Pack... pack
					) -> Ret
			{
				return shellcode_stub_helper<
					Ret,
					First,
					Second,
					Third,
					Fourth,
					void*,
					void*,
					Pack...
				>(
					shell,
					first,
					second,
					third,
					fourth,
					shell_param,
					nullptr,
					pack...
					);
			}
		};

		template <std::size_t Argc>
		struct argument_remapper<Argc, std::enable_if_t<Argc <= 4>>
		{
			// 4 or less params
			template<
				typename Ret,
				typename First = void*,
				typename Second = void*,
				typename Third = void*,
				typename Fourth = void*
			>
				static auto do_call(
					const void* shell,
					void* shell_param,
					First first = First{},
					Second second = Second{},
					Third third = Third{},
					Fourth fourth = Fourth{}
					) -> Ret
			{
				return shellcode_stub_helper<
					Ret,
					First,
					Second,
					Third,
					Fourth,
					void*,
					void*
				>(
					shell,
					first,
					second,
					third,
					fourth,
					shell_param,
					nullptr
					);
			}
		};
	}


	template <typename Ret, typename... Args>
	static inline auto spoof_call(
		const void* trampoline,
		Ret(*fn)(Args...),
		Args... args
		) -> Ret
	{
		struct shell_params
		{
			const void* trampoline;
			void* function;
			void* rbx;
		};

		shell_params p{ trampoline, reinterpret_cast<void*>(fn) };
		using mapper = detail::argument_remapper<sizeof...(Args), void>;
		return mapper::template do_call<Ret, Args...>((const void*)&detail::_spoofer_stub, &p, args...);
	}
}
