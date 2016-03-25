#pragma once

#include "methodpointer.h"

namespace virtualwrangler {

template <typename Signature>
struct Delegate;

template <typename R, typename... Args>
struct Delegate<R(Args...)>
{
	Delegate() {}

	template <typename I, typename C>
	Delegate(const I *_instance, MethodPointer<C, R(Args...) const> method)
		: func(reinterpret_cast<MethodPointer<Simple, R(Args...)>&>(method))
	{
		const C *p = static_cast<const C*>(_instance); // check that I is compatible with C
		instance = const_cast<Simple*>(reinterpret_cast<const Simple*>(p));
	}
	template <typename I, typename C>
	Delegate(I *_instance, MethodPointer<C, R(Args...)> method)
		: func(reinterpret_cast<MethodPointer<Simple, R(Args...)>&>(method))
	{
		C *p = static_cast<C*>(_instance); // check that I is compatible with C
		instance = reinterpret_cast<Simple*>(p);
	}

	template <typename I, typename C>
	Delegate(const I *instance, R(C::*method)(Args...) const) : Delegate(instance, GetMethodPointer(method)) {}
	template <typename I, typename C>
	Delegate(I *instance, R(C::*method)(Args...)) : Delegate(instance, GetMethodPointer(method)) {}

	R operator()(Args... args)
	{
		return func.call(instance, args...);
	}

private:
	struct Simple {};
	MethodPointer<Simple, R(Args...)> func;
	Simple *instance = nullptr;
};

template <typename I, typename C, typename R, typename... Args>
inline Delegate<R(Args...)> MakeDelegate(const I *instance, MethodPointer<C, R(Args...) const> method)
{
	return Delegate<R(Args...)>(instance, method);
}
template <typename I, typename C, typename R, typename... Args>
inline Delegate<R(Args...)> MakeDelegate(I *instance, MethodPointer<C, R(Args...)> method)
{
	return Delegate<R(Args...)>(instance, method);
}

template <typename I, typename C, typename R, typename... Args>
inline Delegate<R(Args...)> MakeDelegate(const I *instance, R(C::*method)(Args...) const)
{
	return MakeDelegate(instance, GetMethodPointer(method));
}
template <typename I, typename C, typename R, typename... Args>
inline Delegate<R(Args...)> MakeDelegate(I *instance, R(C::*method)(Args...))
{
	return MakeDelegate(instance, GetMethodPointer(method));
}

// TODO: call shim for static functions...

}
