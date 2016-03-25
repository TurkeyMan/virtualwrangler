# Virtual Wrangler
A C++11 header-only tool library for dynamically interacting with virtual tables.

Here is a suite of helper functions and classes to assist with type-safe interaction with
class vtables, virtual functions, 'raw' method pointer handling, and a bonus delegate implementation.

Virtual tables are intensely implementation-specific and no language features exist to
interact with class virtual tables or raw method pointers.
Indeed, virtual tables are an implementation detail, and not even mentioned in the C++ spec.

## Disclaimer:
Here be dragons! Contained within are all manner of compiler-specific hacks and workarounds.
Your code will not be portable.

Naturally, I don't recommend you use this library under any circumstances! ;)

## How do I even?
Since you have agreed not to use this under any circumstances, I present a usage guide for
informational purposes only.

Sadly, there remain some requirements for hacks and workarounds in user code.  
The most egregious of such is a requirements to prototype support for each given class you
intend to operate on:
```C++
PrepareVTable(MyClass);
PrepareVTable(MyOtherClass);
```
This is due to various compiler-specific complexities relating to naming and accessing the
vtable symbols.

Hopefully it is possible to automate this step among some existing macro-fu already present.

From here it's relatively smooth sailing:
```C++
void main()
{
    PrepareVTable(MyClass);

    VTable t = GetVTable<MyClass>();
    printf("%p, %zd", t.vtbl, t.length);
}
```

'Theoretically', you could capture a vtable and update some entries with runtime defined overrides...

Another feature of the library is the ability to resolve raw method pointers.  
Pointer-to-member syntax is insufficient on its own to resolve raw method pointers, particulary for
virtual functions, which require a vtable lookup to find the pointer.

I present a helper for capturing a raw method pointer from class methods like so:
```C++
class C
{
    void f() { printf("C::f"); }
    virtual int g(int arg) { printf("C::g"); return arg + 10; }
}
void main()
{
    PrepareVTable(C);

    MethodPointer<C, void()> f = GetMethodPointer(&C::f);
    MethodPointer<C, int(int)> g = GetMethodPointer(&C::g);
```

And we can call the methods by binding a 'this' pointer at call-time.

```C++
    C instance;
    
    f.call(&instance);
    int i = g.call(&instance, 10);
    printf("%d", i);
}
```

You may wonder how this is different than pointer-to-members? The key difference is that this is a 'raw'
or direct call to the method. It's the size of a single pointer, not a compiler-specific struct.  
The functional difference is that pointer-to-members will perform a virtual call, they may call though
some manner of call shim's, and the call-site may be padded with extra code that handles the logic to
distinguish between virtual and non-virtual calls.
The only code that results from making a method call this way, loading the 'this' pointer into an argument
register, and branching to the function.

It is useful in various circumstances to be able to take a function pointer to a particular virtual function.  
Consider:
```C++
class C
{
    virtual void f() { printf("C::f"); }
}
class D : public C
{
    void f() override { printf("D::f"); }
}

void main()
{
    PrepareVTable(C);

    D instance;

    void(C::*ptr_to_member)() = &C::f; // capture C::f()
    (instance.*ptr_to_member)(); // this will make a virtual-call to D::f(), since 'instance' is a 'D'

    auto method_ptr = GetMethodPointer(&C::f); // *actually* capture C::f()
    method_ptr.call(&instance); // this will call C::f() as may be desired
}
```

## Outstanding/gotchas!
I have not yet taken the time to support multiple-inheritance.  
I believe it's quite possible, and not even particularly difficult, but I've never used multiple-inheritance
and have no test cases to drive the development.  
PR's welcome! ;)

The main gotcha is compiler support. Currently I have tested VS2015, and GCC/Clang 4.8 and up.  
I suspect implementation for other compilers is possible, but I haven't done the work.  
PR's welcome! :)
