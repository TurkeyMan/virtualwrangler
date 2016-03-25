#pragma once

#include <cstddef>

#if _MSC_VER >= 1900
#error
#   define use_constexpr constexpr
#else
#   define use_constexpr
#endif

namespace virtualwrangler {

typedef void(*VFPtr)();

struct VTable
{
	const VFPtr *vtbl;
	size_t length;
};

#if 0
#	define PrepareVTable(C) // Note: this is defined for each platform
#endif

template <typename C, typename R, typename... Args>
inline use_constexpr size_t VirtualMethodIndex(R(C::*method)(Args...) const);
template <typename C, typename R, typename... Args>
inline use_constexpr size_t VirtualMethodIndex(R(C::*method)(Args...));

template <typename C, typename R, typename... Args>
inline use_constexpr bool IsMethodVirtual(R(C::*method)(Args...) const);
template <typename C, typename R, typename... Args>
inline use_constexpr bool IsMethodVirtual(R(C::*method)(Args...));

template <typename C>
inline VTable GetVTable();

} // namespace virtualwrangler

#if _MSC_VER >= 1900 // vs 2015
#	include "vtable_vc2015.h"
#else
#	include "vtable_gcc.h"
#endif
