#pragma once

#include "vtable.h"

namespace virtualwrangler {

template <typename C, typename Signature>
struct MethodPointer;

template <typename C, typename R, typename... Args>
struct MethodPointer<C, R(Args...) const>
{
	MethodPointer() {}
	MethodPointer(R(C::*method)(Args...) const);

	R call(const C *pThis, Args... args) const;

private:
	VFPtr ptr = nullptr;
};
template <typename C, typename R, typename... Args>
struct MethodPointer<C, R(Args...)>
{
	MethodPointer() {}
	MethodPointer(R(C::*method)(Args...));

	R call(C *pThis, Args... args);

private:
	VFPtr ptr = nullptr;
};

template <typename C, typename R, typename... Args>
inline MethodPointer<C, R(Args...) const> GetMethodPointer(R(C::*method)(Args...) const);
template <typename C, typename R, typename... Args>
inline MethodPointer<C, R(Args...)> GetMethodPointer(R(C::*method)(Args...));

}

#if _MSC_VER >= 1900 // vs 2015
#	include "methodpointer_vc2015.h"
#else
#	include "methodpointer_gcc.h"
#endif
