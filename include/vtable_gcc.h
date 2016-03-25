#pragma once


namespace virtualwrangler {

// internal muschief...
namespace internal {

	template <typename C>
	struct VTableHolder
	{
		VTableHolder(VTable _vtbl)
		{
			if (_vtbl.vtbl)
				vtbl = _vtbl;
		}
		static VTable vtbl;
	};
	template <typename C>
	VTable VTableHolder<C>::vtbl = VTable{ nullptr, 0 };

	template <size_t N>
	constexpr VTable MakeVTable(const VFPtr(&vtable)[N]) { return{ vtable, N }; }


    // TODO: ONOES! Formulate some sort of macro that will initialise VTableHolder...


} // namespace internal

template <typename C, typename R, typename... Args>
inline size_t VirtualMethodIndex(R(C::*method)(Args...) const)
{
	return (size_t&)method & 1 ? ((size_t&)method -1) / sizeof(VFPtr) : (size_t)-1;
}
template <typename C, typename R, typename... Args>
inline size_t VirtualMethodIndex(R(C::*method)(Args...))
{
	return (size_t&)method & 1 ? ((size_t&)method -1) / sizeof(VFPtr) : (size_t)-1;
}
template <typename C, typename R, typename... Args>
inline bool IsMethodVirtual(R(C::*method)(Args...) const)
{
	return VirtualMethodIndex(method) != (size_t)-1;
}
template <typename C, typename R, typename... Args>
inline bool IsMethodVirtual(R(C::*method)(Args...))
{
	return VirtualMethodIndex(method) != (size_t)-1;
}

template <typename C>
inline VTable GetVTable()
{
	return internal::VTableHolder<C>::vtbl;
}

} // namespace virtualwrangler
