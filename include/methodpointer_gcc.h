#pragma once

#include "vtable.h"

namespace virtualwrangler {

namespace internal {

    template <typename C, typename R, typename... Args>
    struct GCCPtrToMember
    {
        GCCPtrToMember(R(C::*method)(Args...) const) : constMethod(method) {}
        GCCPtrToMember(R(C::*method)(Args...)) : method(method) {}
        GCCPtrToMember(VFPtr ptr, size_t vptr_offset = 0) : ptr_or_offset((size_t)ptr), vtable_offset(vptr_offset) {}

        union
        {
            R(C::*method)();
            R(C::*constMethod)() const;
            struct
            {
                size_t ptr_or_offset;
                size_t vtable_offset;
            };
        };
    };

    template <typename C, typename R, typename... Args>
    VFPtr GetMethodPointerImpl(R(C::*method)(Args...) const)
    {
        GCCPtrToMember<C, R, Args...> t(method);
        if ((t.ptr_or_offset & 1) == 0) // if the '1' bit is clear, it's not virtual
            return (VFPtr)t.ptr_or_offset;
        // 'ptr_or_offset' is an offset into the vtable
        size_t index = (t.ptr_or_offset - 1) / sizeof(VFPtr); 
        return GetVTable<C>().vtbl[index];
    }
    template <typename C, typename R, typename... Args>
    VFPtr GetMethodPointerImpl(R(C::*method)(Args...))
    {
        GCCPtrToMember<C, R, Args...> t(method);
        if ((t.ptr_or_offset & 1) == 0) // if the '1' bit is clear, it's not virtual
            return (VFPtr)t.ptr_or_offset;
        // 'ptr_or_offset' is an offset into the vtable
        size_t index = (t.ptr_or_offset - 1) / sizeof(VFPtr); 
        return GetVTable<C>().vtbl[index];
    }

}

template <typename C, typename R, typename... Args>
MethodPointer<C, R(Args...) const>::MethodPointer(R(C::*method)(Args...) const)
{
    ptr = internal::GetMethodPointerImpl(method);
}
template <typename C, typename R, typename... Args>
R MethodPointer<C, R(Args...) const>::call(const C *pThis, Args... args) const
{
    internal::GCCPtrToMember<C, R, Args...> t(ptr);
    return (pThis->*t.constMethod)(args...);
}

template <typename C, typename R, typename... Args>
MethodPointer<C, R(Args...)>::MethodPointer(R(C::*method)(Args...))
{
    ptr = internal::GetMethodPointerImpl(method);
}
template <typename C, typename R, typename... Args>
R MethodPointer<C, R(Args...)>::call(C *pThis, Args... args)
{
    internal::GCCPtrToMember<C, R, Args...> t(ptr);
    return (pThis->*t.method)(args...);
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
