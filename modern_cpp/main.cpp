//
//  main.cpp
//  Modern CPP
//
//  Created by Bernardt Duvenhage on 2019/03/10.
//  Copyright © 2019 Bernardt Duvenhage. All rights reserved.
//
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <thread>
#include <future>
#include <random>

// See https://github.com/AnthonyCalandra/modern-cpp-features

void CPP_exceptions()
{
    
}

void CPP_constness()
{
    const int x = 1;
    int y = 2;
    int z = 3;

    int const *p = &x; // value pointed to is const.
    __attribute__((unused))  const int *q = &x; // const can be either side of type specifier!
    
    int *r = &y;
    int * const s = &y; // pointer is const.
    __attribute__((unused))  int const * const t = &y; // pointer and value pointed to is const.
    
    r = &z; //ok, because the pointer is not const.
    p = &z; //ok, because the pointer is not const, but not one cannot modify z via p!
    // *p = 4; //fail, see above.
    
    // s = &z; //fail, s is const and connot point somewhere else.
    *s = 1; //ok, because the value pointed to is not const!
}

void CPP11_RVO()
{
    
}

class CPP11_MS
{
public:
    CPP11_MS() = default;
    CPP11_MS(const CPP11_MS &CPP11_MS) = default;
    CPP11_MS(CPP11_MS &&CPP11_MS) = default;
    
    CPP11_MS &operator = (const CPP11_MS &v) = default;
};

void CPP11_move_semantics()
{
    int x = 1;
    __attribute__((unused)) int &xl = x;
    // __attribute__((unused)) int &&xr = x; // Compile error: cannot bind to l-value x.
    __attribute__((unused)) int &&xr = 1; // 1 is not an l-value. It is a temporary.
    
    CPP11_MS ref; // default constructed.
    CPP11_MS a = ref; // copy constructed.
    __attribute__((unused)) CPP11_MS b = CPP11_MS(a); // move constructed.
    __attribute__((unused)) CPP11_MS c = std::move(a); // move constructed.
    
}

void CPP11_forward_references()
{
    int x = 0;
    __attribute__((unused)) auto&& xl = x; //xl becomes type int& and binds to l-value x.
    __attribute__((unused)) auto&& xr = 1; //xr becomes type int&& and binds to r-value 1.
}

template <typename... T>
struct arity {
    constexpr static int value = sizeof...(T);
};
void CPP11_variadic_templates()
{
    static_assert(arity<>::value == 0, "Arity of zero arguments not zero!");
    static_assert(arity<char, short, int>::value == 3, "Arity of three arguments not three!");
}

int sum(const std::initializer_list<int> &list)
{
    int s = 0;
    
    for (auto &v : list)
    {
        s += v;
    }
    
    return s;
}
void CPP11_initialiser_lists()
{
    __attribute__((unused)) const int s = sum({1, 2, 3});
}

void CPP11_static_assertions()
{
    constexpr int x = 1;
    constexpr int y = 2;
    static_assert(x != y, "x == y"); //Will assert at compile if x==y !
}

template <typename X, typename Y>
auto add(X x, Y y) -> decltype(x + y) // type of expression x + y. E.g. if x is an integer and y is a double, decltype(x + y) is a double
{
    return x + y;
}
void CPP11_return_type_auto()
{
    add(1, 2); // == 3
    add(1, 2.0); // == 3.0
    add(1.5, 1.5); // == 3.0
}

// [] - captures nothing.
// [=] - capture local objects (local variables, parameters) in scope by value.
// [&] - capture local objects (local variables, parameters) in scope by reference.
// [this] - capture this pointer by value.
// [a, &b] - capture objects a by value, b by reference.
void CPP11_lambda_expressions()
{
    int x = 1;
    
    auto getX = [=] { return x; };
    __attribute__((unused)) int u = getX(); // == 1
    
    auto addX = [=](int y) { return x + y; };
    __attribute__((unused)) int v = addX(1); // == 2
    
    auto getXRef = [&]() -> int& { return x; };
    int& w = getXRef(); // int& to `x`
    w = 2; // Update value of x.
    
    std::cout << x << " (should be 2)\n";
}

template <typename T>
using Vec = std::vector<T>;

void CPP11_using()
{
    Vec<int> v; // std::vector<int>

    using String = std::string;
    String s {"foo"};
}

void foo(int) {};
void foo(void*) {};

void CPP11_nullptr()
{
    //foo(NULL); // error -- ambiguous
    foo(nullptr); // calls foo(void*)
}

void CPP11_strongly_typed_enums()
{
    // Specifying underlying type as `unsigned int`
    enum class Color : unsigned int { Red = 0xff0000, Green = 0xff00, Blue = 0xff };
    
    // `Red`/`Green` in `Alert` don't conflict with `Color`
    enum class Alert : bool { Red, Green };
    
    __attribute__((unused)) Color c = Color::Red;
}

// `unsigned long long` parameter required for integer literal.
long long operator "" _celsius(unsigned long long tempCelsius) {
    return std::llround(tempCelsius * 1.8 + 32);
}

// `long double` parameter required for double literal.
long double operator "" _celsius(long double tempCelsius) {
    return tempCelsius * 1.8 + 32.0;
}

void CPP11_user_defined_literals()
{
    std::cout << 24_celsius << "\n"; // == 75
    std::cout << 24.0_celsius << "\n"; // == 75.0
}

// CPP11_explicit_virtual_overrides
struct A {
    virtual void foo();
    void bar();
};

struct B : A {
    void foo() override; // correct -- B::foo overrides A::foo
    //void bar() override; // error -- A::bar is not virtual
    //void baz() override; // error -- B::baz does not override A::baz
};

// CPP11_final_specifier
struct C : A {
    virtual void foo() final;
};

struct D : C {
    //virtual void foo(); // error -- declaration of 'foo' overrides a 'final' function
};

struct E
{
    E() = default;
    E(const E&) = delete;
    E& operator=(const E&) = delete;
    E(int x) : x(x) {}
    int x {1}; // default value.
};

void CPP11_default_and_deleted_functions()
{
    __attribute__((unused)) E a; // a.x == 1
    __attribute__((unused)) E a2(123); // a.x == 123
    __attribute__((unused)) E a3 {123}; // a.x == 123
    //E a4(a); // error -- call to deleted copy constructor
    //E a5 = a; // error -- call to deleted copy constructor
    // a2 = a3; // error -- operator= deleted
}

std::ostream& operator << (std::ostream& output, const std::array<int,5> &values)
{
    for (auto const& value : values)
    {
        output << value << " ";
    }
    output << std::endl;
    
    return output;
}

void CPP11_ranged_based_for()
{
    std::array<int, 5> a {0,1,2,3,4};
    
    for (auto &x : a) {x *= 2;}
    std::cout << a;
}

class CPP11_DefaultInit
{
public:
    CPP11_DefaultInit() // Similar to initialising m to '1' here!
    {}
    //Could even leave out the default constructor.
    
    int get_m() {return m;}
    
private:
    int m {1};
};

void CPP11_default_initialisation()
{
    __attribute__((unused)) auto di = CPP11_DefaultInit();
}

void CPP11_to_string()
{
    std::to_string(1.2); // == "1.2"
    std::to_string(123); // == "123"
}

void CPP11_type_traits()
{
}

void CPP11_smart_pointers()
{ // std::unique_ptr, std::shared_ptr, std::weak_ptr. Use std::make_unique and std::make_shared..
    
}

void CPP11_tuples_and_ties()
{
    // `playerProfile` has type `std::tuple<int, const char*, const char*>`.
    auto playerProfile = std::make_tuple(51, "Frans Nielsen", "NYI");
    std::get<0>(playerProfile); // 51
    std::get<1>(playerProfile); // "Frans Nielsen"
    std::get<2>(playerProfile); // "NYI"
}

void CPP11_random()
{
    std::random_device rd;
    std::mt19937 rg(rd());
    std::uniform_int_distribution<int> rand_dist(0, 1);
    
    std::cout << rand_dist(rg) << "\n";
    std::cout << rand_dist(rg) << "\n";
    std::cout << rand_dist(rg) << "\n";
}

void CPP11_threads()
{
}

int CPP11_async_and_futures_foo() {
    /* Do something here, then return the result. */
    return 1000;
}

void CPP11_async_and_futures()
{
    
    auto future_handle = std::async(/*std::launch::async | */ std::launch::deferred,
                                    CPP11_async_and_futures_foo);  // create an async task
    __attribute__((unused)) auto result = future_handle.get();  // wait for the result
}

void CPP14_binary_literals()
{
    __attribute__((unused)) auto b = 0b00001111;
    __attribute__((unused)) auto i = 123443;
}

void CPP14_generic_lambda_expressions()
{
}

void CPP17_string_view()
{
    
}

int main(int argc, const char * argv[])
{
    CPP11_random();
    CPP11_lambda_expressions();
    CPP11_user_defined_literals();
    CPP11_ranged_based_for();
    
    return 0;
}
