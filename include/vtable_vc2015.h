#pragma once

#define STRINGIFY(Token) #Token
#define MSVC_MANGLE_VTABLE(Class) STRINGIFY(??_7##Class##@@6B@)
#define MSVC_VTABLE(Class) (__pragma(warning(suppress: 4483)) __identifier(MSVC_MANGLE_VTABLE(Class)))

#define MSVC_DECLARE_VTABLE_HOLDER(C) ::virtualwrangler::internal::VTableHolder<C>(::virtualwrangler::internal::MakeVTable(MSVC_VTABLE(C)))

#define PrepareVTable(C) []() -> VTable { auto x = MSVC_DECLARE_VTABLE_HOLDER(C); return x.vtbl; }()

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

	// VS makes virtual calls with thunk functions, which we can detect by comparing function pointers to known thunks
	struct VSHelper
	{
		virtual void a() {}		virtual void b() {}		virtual void c() {}		virtual void d() {}
		virtual void e() {}		virtual void f() {}		virtual void g() {}		virtual void h() {}
		virtual void i() {}		virtual void j() {}		virtual void k() {}		virtual void l() {}
		virtual void m() {}		virtual void n() {}		virtual void o() {}		virtual void p() {}
		virtual void q() {}		virtual void r() {}		virtual void s() {}		virtual void t() {}
		virtual void u() {}		virtual void v() {}		virtual void w() {}		virtual void x() {}
		virtual void y() {}		virtual void z() {}
		virtual void a2() {}	virtual void b2() {}	virtual void c2() {}	virtual void d2() {}
		virtual void e2() {}	virtual void f2() {}	virtual void g2() {}	virtual void h2() {}
		virtual void i2() {}	virtual void j2() {}	virtual void k2() {}	virtual void l2() {}
		virtual void m2() {}	virtual void n2() {}	virtual void o2() {}	virtual void p2() {}
		virtual void q2() {}	virtual void r2() {}	virtual void s2() {}	virtual void t2() {}
		virtual void u2() {}	virtual void v2() {}	virtual void w2() {}	virtual void x2() {}
		virtual void y2() {}	virtual void z2() {}
		virtual void a3() {}	virtual void b3() {}	virtual void c3() {}	virtual void d3() {}
		virtual void e3() {}	virtual void f3() {}	virtual void g3() {}	virtual void h3() {}
		virtual void i3() {}	virtual void j3() {}	virtual void k3() {}	virtual void l3() {}
		virtual void m3() {}	virtual void n3() {}	virtual void o3() {}	virtual void p3() {}
		virtual void q3() {}	virtual void r3() {}	virtual void s3() {}	virtual void t3() {}
		virtual void u3() {}	virtual void v3() {}	virtual void w3() {}	virtual void x3() {}
		virtual void y3() {}	virtual void z3() {}
	};
	static constexpr void(VSHelper::* const thunks[])() = {
		&VSHelper::a,  &VSHelper::b,  &VSHelper::c, &VSHelper::d,
		&VSHelper::e,  &VSHelper::f,  &VSHelper::g, &VSHelper::h,
		&VSHelper::i,  &VSHelper::j,  &VSHelper::k, &VSHelper::l,
		&VSHelper::m,  &VSHelper::n,  &VSHelper::o, &VSHelper::p,
		&VSHelper::q,  &VSHelper::r,  &VSHelper::s, &VSHelper::t,
		&VSHelper::u,  &VSHelper::v,  &VSHelper::w, &VSHelper::x,
		&VSHelper::y,  &VSHelper::z,
		&VSHelper::a2, &VSHelper::b2, &VSHelper::c2, &VSHelper::d2,
		&VSHelper::e2, &VSHelper::f2, &VSHelper::g2, &VSHelper::h2,
		&VSHelper::i2, &VSHelper::j2, &VSHelper::k2, &VSHelper::l2,
		&VSHelper::m2, &VSHelper::n2, &VSHelper::o2, &VSHelper::p2,
		&VSHelper::q2, &VSHelper::r2, &VSHelper::s2, &VSHelper::t2,
		&VSHelper::u2, &VSHelper::v2, &VSHelper::w2, &VSHelper::x2,
		&VSHelper::y2, &VSHelper::z2,
		&VSHelper::a3, &VSHelper::b3, &VSHelper::c3, &VSHelper::d3,
		&VSHelper::e3, &VSHelper::f3, &VSHelper::g3, &VSHelper::h3,
		&VSHelper::i3, &VSHelper::j3, &VSHelper::k3, &VSHelper::l3,
		&VSHelper::m3, &VSHelper::n3, &VSHelper::o3, &VSHelper::p3,
		&VSHelper::q3, &VSHelper::r3, &VSHelper::s3, &VSHelper::t3,
		&VSHelper::u3, &VSHelper::v3, &VSHelper::w3, &VSHelper::x3,
		&VSHelper::y3, &VSHelper::z3,
	};
	static constexpr size_t MaxVirtuals = sizeof(thunks) / sizeof(thunks)[0];

	template <size_t N>
	constexpr VTable MakeVTable(const VFPtr(&vtable)[N]) { return{ vtable, N }; }
 

	// detect if a method is virtual
	template <typename C, typename R, typename... Args>
	constexpr size_t VirtualMethodIndexImpl(R(C::*method)(Args...) const, size_t offset = 0)
	{
		return offset < MaxVirtuals ? (method == reinterpret_cast<decltype(method)>(thunks[offset]) ? offset : VirtualMethodIndexImpl(method, offset + 1)) : (size_t)-1;
	}
	template <typename C, typename R, typename... Args>
	constexpr size_t VirtualMethodIndexImpl(R(C::*method)(Args...), size_t offset = 0)
	{
		return offset < MaxVirtuals ? (method == reinterpret_cast<decltype(method)>(thunks[offset]) ? offset : VirtualMethodIndexImpl(method, offset + 1)) : (size_t)-1;
	}

} // namespace internal

template <typename C, typename R, typename... Args>
inline constexpr size_t VirtualMethodIndex(R(C::*method)(Args...) const)
{
	return internal::VirtualMethodIndexImpl(method);
}
template <typename C, typename R, typename... Args>
inline constexpr size_t VirtualMethodIndex(R(C::*method)(Args...))
{
	return internal::VirtualMethodIndexImpl(method);
}
template <typename C, typename R, typename... Args>
inline constexpr bool IsMethodVirtual(R(C::*method)(Args...) const)
{
	return internal::VirtualMethodIndexImpl(method) != (size_t)-1;
}
template <typename C, typename R, typename... Args>
inline constexpr bool IsMethodVirtual(R(C::*method)(Args...))
{
	return internal::VirtualMethodIndexImpl(method) != (size_t)-1;
}

template <typename C>
inline VTable GetVTable()
{
	return internal::VTableHolder<C>::vtbl;
}

} // namespace virtualwrangler
