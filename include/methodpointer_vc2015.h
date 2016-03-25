#pragma once

#include "vtable.h"

namespace virtualwrangler {

namespace internal {

	template <size_t Size, typename C, typename R, typename... Args>
	struct Helper;

	// Single inheritance
	template <typename C, typename R, typename... Args>
	struct Helper<8, C, R, Args...>
	{
		static VFPtr GetMethodPointer(R(C::*method)(Args...) const)
		{
			// in this case, the function is either a non-virtual, or a single-inheritance virtual call thunk
			size_t index = VirtualMethodIndex(method);
			if (index == (size_t)-1)
				return *(VFPtr*)&method; // function is not-virtual

										 //get the function from the vtable
			return GetVTable<C>().vtbl[index];
		}
		static VFPtr GetMethodPointer(R(C::*method)(Args...))
		{
			// in this case, the function is either a non-virtual, or a single-inheritance virtual call thunk
			size_t index = VirtualMethodIndex(method);
			if (index == (size_t)-1)
				return *(VFPtr*)&method; // function is not-virtual

			//get the function from the vtable
			return GetVTable<C>().vtbl[index];
		}
	};

	// Multiple inheritance
	template <typename C, typename R, typename... Args>
	struct Helper<16, C, R, Args...>
	{
		static VFPtr GetMethodPointer(R(C::*method)(Args...) const)
		{
			// TODO
		}
		static VFPtr GetMethodPointer(R(C::*method)(Args...))
		{
			// TODO
		}
	};

	// Virtual inheritance
	//...

}

template <typename C, typename R, typename... Args>
MethodPointer<C, R(Args...) const>::MethodPointer(R(C::*method)(Args...) const)
{
	ptr = internal::Helper<sizeof(method), C, R, Args...>::GetMethodPointer(method);
}
template <typename C, typename R, typename... Args>
R MethodPointer<C, R(Args...) const>::call(const C *pThis, Args... args) const
{
	R(C::*method)(Args...) const;
	*(VFPtr*)&method = ptr;
	return (pThis->*method)(args...);
}

template <typename C, typename R, typename... Args>
MethodPointer<C, R(Args...)>::MethodPointer(R(C::*method)(Args...))
{
	ptr = internal::Helper<sizeof(method), C, R, Args...>::GetMethodPointer(method);
}
template <typename C, typename R, typename... Args>
R MethodPointer<C, R(Args...)>::call(C *pThis, Args... args)
{
	R(C::*method)(Args...);
	*(VFPtr*)&method = ptr;
	return (pThis->*method)(args...);
}

template <typename C, typename R, typename... Args>
inline MethodPointer<C, R(Args...) const> GetMethodPointer(R(C::*method)(Args...) const)
{
	return MethodPointer<C, R(Args...) const>(method);
}
template <typename C, typename R, typename... Args>
inline MethodPointer<C, R(Args...)> GetMethodPointer(R(C::*method)(Args...))
{
	return MethodPointer<C, R(Args...)>(method);
}

}
