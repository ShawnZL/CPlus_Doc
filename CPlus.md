# 要看的代码

[indicators](https://github.com/p-ranav/indicators)

[spdlog](https://github.com/gabime/spdlog)

googleTest



# explicit

**explicit关键字的作用就是防止类构造函数的隐式自动转换.**且类的构造函数只能有一个参数.**explicit关键字只对有一个参数的类构造函数有效, 如果类构造函数参数大于或等于两个时, 是不会产生隐式转换的,所以explicit关键字也就无效了**.

```c++
class CxString  // 没有使用explicit关键字的类声明, 即默认为隐式声明  
{  
public:  
    char *_pstr;  
    int _size;  
    CxString(int size)  
    {  
        _size = size;                // string的预设大小  
        _pstr = malloc(size + 1);    // 分配string的内存  
        memset(_pstr, 0, size + 1);  
    }  
    CxString(const char *p)  
    {  
        int size = strlen(p);  
        _pstr = malloc(size + 1);    // 分配string的内存  
        strcpy(_pstr, p);            // 复制字符串  
        _size = strlen(_pstr);  
    }  
    // 析构函数这里不讨论, 省略...  
};  

    CxString string1(24);     // 这样是OK的, 为CxString预分配24字节的大小的内存  
    CxString string2 = 10;    // 这样是OK的, 为CxString预分配10字节的大小的内存  
    CxString string3;         // 这样是不行的, 因为没有默认构造函数, 错误为: “CxString”: 没有合适的默认构造函数可用  
    CxString string4("aaaa"); // 这样是OK的  
    CxString string5 = "bbb"; // 这样也是OK的, 调用的是CxString(const char *p)  
    CxString string6 = 'c';   // 这样也是OK的, 其实调用的是CxString(int size), 且size等于'c'的ascii码  
    string1 = 2;              // 这样也是OK的, 为CxString预分配2字节的大小的内存  
    string2 = 3;              // 这样也是OK的, 为CxString预分配3字节的大小的内存  
    string3 = string1;        // 这样也是OK的, 至少编译是没问题的, 但是如果析构函数里用free释放_pstr内存指针的时候可能会报错, 完整的代码必须重载运算符"=", 并在其中处理内存释放  
```

使用了explicit。

```c++
class CxString  // 使用关键字explicit的类声明, 显示转换  
{  
public:  
    char *_pstr;  
    int _size;  
    explicit CxString(int size)  
    {  
        _size = size;  
        // 代码同上, 省略...  
    }  
    CxString(const char *p)  
    {  
        // 代码同上, 省略...  
    }  
};  
    // 下面是调用:  
    CxString string1(24);     // 这样是OK的  
    CxString string2 = 10;    // 这样是不行的, 因为explicit关键字取消了隐式转换  
    CxString string3;         // 这样是不行的, 因为没有默认构造函数  
    CxString string4("aaaa"); // 这样是OK的  
    CxString string5 = "bbb"; // 这样也是OK的, 调用的是CxString(const char *p)  
    CxString string6 = 'c';   // 这样是不行的, 其实调用的是CxString(int size), 且size等于'c'的ascii码, 但explicit关键字取消了隐式转换  
    string1 = 2;              // 这样也是不行的, 因为取消了隐式转换  
    string2 = 3;              // 这样也是不行的, 因为取消了隐式转换  
    string3 = string1;        // 这样也是不行的, 因为取消了隐式转换, 除非类实现操作符"="的重载  
```

# static

我们知道在函数内部定义的变量，当程序执行到它的定义处时，编译器为它在栈上分配空间，函数在栈上分配的空间在此函数执行结束时会释放掉，这样就产生了一个问题: 如果想将函数中此变量的值保存至下一次调用时，如何实现？ 最容易想到的方法是定义为全局的变量，但定义一个全局变量有许多缺点，最明显的缺点是破坏了此变量的访问范围（使得在此函数中定义的变量，不仅仅只受此函数控制）。static 关键字则可以很好的解决这个问题。

另外，在 C++ 中，需要一个数据对象为整个类而非某个对象服务,同时又力求不破坏类的封装性,即要求此成员隐藏在类的内部，对外不可见时，可将其定义为静态数据。

## 静态数据存储

**全局（静态）存储区**：分为 DATA (数据段)段和 BSS 段。DATA 段（全局初始化区）存放初始化的全局变量和静态变量；BSS 段（全局未初始化区）存放未初始化的全局变量和静态变量。程序运行结束时自动释放。其中BBS段在程序执行之前会被系统自动清0，所以未初始化的全局变量和静态变量在程序执行之前已经为0。**存储在静态数据区的变量会在程序刚开始运行时就完成初始化，也是唯一的一次初始化。**

在 C++ 中 static 的内部实现机制：静态数据成员要在程序一开始运行时就必须存在。因为函数在程序运行中被调用，所以静态数据成员不能在任何函数内分配空间和初始化。

这样，它的空间分配有三个可能的地方，一是作为类的外部接口的头文件，那里有类声明；二是类定义的内部实现，那里有类的成员函数定义；三是应用程序的 main() 函数前的全局数据声明和定义处。

静态数据成员要实际地分配空间，故不能在类的声明中定义（只能声明数据成员）。类声明只声明一个类的"尺寸和规格"，并不进行实际的内存分配，所以在类声明中写成定义是错误的。它也不能在头文件中类声明的外部定义，因为那会造成在多个使用该类的源文件中，对其重复定义。

static 被引入以告知编译器，将变量存储在程序的静态存储区而非栈上空间，静态数据成员按定义出现的先后顺序依次初始化，注意静态成员嵌套时，要保证所嵌套的成员已经初始化了。消除时的顺序是初始化的反顺序。

**优势：**可以节省内存，因为它是所有对象所公有的，因此，对多个对象来说，静态数据成员只存储一处，供所有对象共用。静态数据成员的值对每个对象都是一样，但它的值是可以更新的。只要对静态数据成员的值更新一次，保证所有对象存取更新后的相同的值，这样可以提高时间效率。

## static 用法

### 在C++中

static 关键字最基本的用法是：

- 1、被 static 修饰的变量属于类变量，可以通过**类名.变量名**直接引用，而不需要 new 出一个类来
- 2、被 static 修饰的方法属于类方法，可以通过**类名.方法名**直接引用，而不需要 new 出一个类来

被 static 修饰的变量、被 static 修饰的方法统一属于类的静态资源，是类实例之间共享的，换言之，一处变、处处变。

在 C++ 中，静态成员是属于整个类的而不是某个对象，静态成员变量只存储一份供所有对象共用。所以在所有对象中都可以共享它。使用静态成员变量实现多个对象之间的数据共享不会破坏隐藏的原则，保证了安全性还可以节省内存。

静态成员的定义或声明要加个关键 static。静态成员可以通过双冒号来使用即 **<类名>::<静态成员名>**。

**结论 1：**不能通过类名来调用类的非静态成员函数。通过类的对象调用静态成员函数和非静态成员函数。

**结论 2**：类的对象可以使用静态成员函数和非静态成员函数。**在类的静态成员函数中使用类的非静态成员。又可能出错，因为静态成员函数属于整个类，在类实例化对象之前就已经分配空间了，而类的非静态成员必须在类实例化对象后才有内存空间，所以这个调用就出错了，就好比没有声明一个变量却提前使用它一样。**

**结论3：**静态成员函数中不能引用非静态成员。

**结论 4**：类的非静态成员函数可以调用用静态成员函数，但反之不能。

**结论 5**：类的静态成员变量必须先初始化再使用。

## 总结：

- （1）静态成员函数中不能调用非静态成员。
- （2）非静态成员函数中可以调用静态成员。因为静态成员属于类本身，在类的对象产生之前就已经存在了，所以在非静态成员函数中是可以调用静态成员的。
- （3）静态成员变量使用前必须先初始化(如 **int MyClass::m_nNumber = 0;**)，否则会在 linker 时出错。

**一般总结**：在类中，static 可以用来修饰静态数据成员和静态成员方法。

**静态数据成员**

- （1）静态数据成员可以实现多个对象之间的数据共享，它是类的所有对象的共享成员，它在内存中只占一份空间，如果改变它的值，则各对象中这个数据成员的值都被改变。
- （2）静态数据成员是在程序开始运行时被分配空间，到程序结束之后才释放，只要类中指定了静态数据成员，即使不定义对象，也会为静态数据成员分配空间。
- （3）静态数据成员可以被初始化，但是只能在类体外进行初始化，若未对静态数据成员赋初值，则编译器会自动为其初始化为 0。
- （4）静态数据成员既可以通过对象名引用，也可以通过类名引用。

**静态成员函数**

- （1）静态成员函数和静态数据成员一样，他们都属于类的静态成员，而不是对象成员。
- （2）非静态成员函数有 this 指针，而静态成员函数没有 this 指针。
- （3）静态成员函数主要用来方位静态数据成员而不能访问非静态成员

# 可变模版参数

```c++
template<typename Signature>
class FnOnce;
// ...A是可变的模版参数包
template<typename R, typename... A>
class FnOnce<R(A...)> {
    //并且使用了部分特化，其中特化的模板参数为 R(A...)，表示匹配一个返回类型为 R，参数列表为 A... 的函数类型。
public:
    FnOnce() = default;

    template <typename Fn,
            typename = typename std::enable_if<std::is_convertible<
                    decltype(std::declval<Fn&&>()(std::declval<A>()...)), R>::value>::type>
    FnOnce(Fn fn) : impl_(new FnImpl<Fn>(std::move(fn))) { // NOLINT runtime/explicit
    }
    explicit operator bool() const { return impl_ != nullptr; }

    R operator()(A... a) && {
        auto bye = std::move(impl_); // 直接改变对于所属内存的控制权限
        return bye->invoke(std::forward<A&&>(a)...);
    }

private:
    struct Impl {
        virtual ~Impl() = default;
        virtual R invoke(A&&... a) = 0;
        };

    template<typename Fn>
    struct FnImpl : Impl {
        explicit FnImpl(Fn fn) : fn_(std::move(fn)) {}
        R invoke(A&&... a) override { return std::move(fn_)(std::forward<A&&>(a)...); }
        Fn fn_;
    };
    std::unique_ptr<Impl> impl_; // 指针
};
```

这段代码定义了一个类模板 `FnOnce`，用于表示一次性可调用对象。

在类模板定义中，使用了部分特化，其中特化的模板参数为 `R(A...)`，表示匹配一个返回类型为 `R`，参数列表为 `A...` 的函数类型。

在 `FnOnce` 类中，有以下成员函数和成员变量：

- `FnOnce()`：默认构造函数。
- `FnOnce(Fn fn)`：构造函数模板，接受一个可调用对象 `fn`，并创建一个新的 `FnImpl` 对象来持有该可调用对象。
- `operator bool()`：显式转换运算符，用于判断 `FnOnce` 对象是否有效（非空）。
- `operator()(A... a) &&`：调用运算符，用于执行一次性的调用操作。它将内部的 `impl_` 指针转移到一个临时对象，并调用 `invoke` 函数执行实际的调用操作。
- `Impl`：虚基类，定义了 `invoke` 函数接口。
- `FnImpl`：`Impl` 的具体实现类，持有一个可调用对象，并在 `invoke` 函数中执行实际的调用操作。
- `impl_`：使用 `std::unique_ptr` 管理的指向 `Impl` 对象的指针。

通过这样的设计，`FnOnce` 类可以用来表示一次性的可调用对象，它可以接受不同类型的可调用对象，并在需要时执行相应的调用操作。

## 默认构造函数

`FnOnce() = default;` 表示使用默认构造函数。这种写法可以在需要时使用默认构造函数的情况下使用。

如果您不显式定义默认构造函数，编译器将会为您生成一个默认构造函数。因此，如果您不需要在默认构造函数中执行任何特定的操作，可以不编写默认构造函数的定义。

```c++
// 两种写法等效
FnOnce() = default;
// 不定义默认构造函数，让编译器自动生成默认构造函数
```

## 运算符执行调用操作

```c++
R operator()(A... a) && {
    auto bye = std::move(impl_);
    return bye->invoke(std::forward<A&&>(a)...);
  // std::forward 完美转发
  }
```

这段代码是 `FnOnce` 类中的调用运算符 `operator()(A... a) &&` 的实现。

在该调用运算符中，首先通过 `std::move` 将 `impl_` 的所有权转移到一个临时变量 `bye` 中。这样做是为了在调用 `invoke` 函数时使用移动语义，将 `impl_` 的控制权转移给 `bye`。

然后，调用 `bye->invoke(std::forward<A&&>(a)...)` 来执行实际的调用操作。这里使用了完美转发 `std::forward` 来保持参数的值类别，并将参数传递给 `invoke` 函数。

最后，将 `invoke` 函数的返回值作为结果返回。

总而言之，这段代码实现了将一次性可调用对象的控制权转移到临时对象中，并执行实际的调用操作，然后返回调用结果。

当我们将一个右值引用传入函数时，他在实参中有了命名，所以继续往下传或者调用其他函数时，根据C++ 标准的定义，这个参数变成了一个左值。那么他永远不会调用接下来函数的右值版本，这可能在一些情况下造成拷贝。为了解决这个问题 C++ 11引入了完美转发，根据右值判断的推倒，调用forward 传出的值，若原来是一个右值，那么他转出来就是一个右值，否则为一个左值。

右值引用类型是独立于值的，一个右值引用参数作为函数的形参，在函数内部再转发该参数的时候它已经变成一个左值，并不是他原来的类型。如果我们需要一种方法能够按照参数原来的类型转发到另一个函数，这种转发类型称为完美转发。

## 左右值引用

### 左右值

[原文链接🔗](https://zhuanlan.zhihu.com/p/335994370)

首先不考虑引用以减少干扰，可以从2个角度判断：左值**可以取地址、位于等号左边**；而右值**没法取地址，位于等号右边**。

```cpp
int a = 5;
```

- a可以通过 & 取地址，位于等号左边，所以a是左值。
- 5位于等号右边，5没法通过 & 取地址，所以5是个右值。

再举个例子：

```cpp
struct A {
    A(int a = 0) {
        a_ = a;
    }
 
    int a_;
};
 
A a = A();
```

- 同样的，a可以通过 & 取地址，位于等号左边，所以a是左值。
- A()是个临时值，没法通过 & 取地址，位于等号右边，所以A()是个右值。

**被声明出来的左、右值引用都是左值。 因为被声明出的左右值引用是有地址的，也位于等号左边。**

### 右值引用(A&&)

`A&&` 是称为右值引用（Rvalue Reference）的类型。

在 C++11 中引入的右值引用允许我们将其绑定到临时对象（右值），并且具有以下特性：

1. 它可以扩展为左值引用或右值引用，具体取决于传递给它的表达式。
2. 它可以用于区分函数重载，例如通过一个函数接受左值引用参数，另一个函数接受右值引用参数。
3. 它可以与移动语义一起使用，实现高效的资源管理，如移动构造函数和移动赋值运算符。

在函数模板或函数参数中，使用 `A&&` 表示类型 `A` 的右值引用。当参数使用右值引用时，可以在函数调用中接受右值参数，并且可以修改右值参数或使用移动语义来提高性能。

例如，以下函数声明中的 `A&&` 表示右值引用参数：

```cpp
void foo(A&& value);
```

当调用 `foo` 函数时，可以将右值作为参数传递给该函数：

```cpp
A a;
foo(std::move(a));  // 传递右值参数
```

在这个例子中，`std::move(a)` 将左值 `a` 转换为右值，并将右值传递给 `foo` 函数。**std::move() 将对象的状态或者所有权从一个对象转移到另一个对象，只是转移，没有内存的搬迁或者内存拷贝，所以可以提高效率，改善性能。**

std::move会返回一个右值引用`int &&`，它是左值还是右值呢？ 从表达式`int &&ref = std::move(a)`来看，右值引用`ref`指向的必须是右值，所以move返回的`int &&`是个右值。所以右值引用既可能是左值，又可能是右值吗？ 确实如此：**右值引用既可以是左值也可以是右值，如果有名称则为左值，否则是右值**。

右值引用对于实现移动语义、完美转发和构造可移动类型等方面非常有用。

### 左值引用(A&)

左值引用大家都很熟悉，**能指向左值，不能指向右值的就是左值引用**：

```cpp
int a = 5;
int &ref_a = a; // 左值引用指向左值，编译通过
int &ref_a = 5; // 左值引用指向了右值，会编译失败
```

**引用是变量的别名，由于右值没有地址，没法被修改，所以左值引用无法指向右值。**

但是，const左值引用是可以指向右值的：

```cpp
const int &ref_a = 5;  // 编译通过
```

const左值引用不会修改指向值，因此可以指向右值，这也是为什么要使用`const &`作为函数参数的原因之一，如`std::vector`的`push_back`：

```cpp
void push_back (const value_type& val);
```

如果没有`const`，`vec.push_back(5)`这样的代码就无法编译通过了。

是的，C++ 中除了右值引用（Rvalue Reference）之外，还有左值引用（Lvalue Reference）。

左值引用使用 `&` 符号表示，它用于将引用绑定到左值（可以称为“具名对象”），并具有以下特点：

1. 它只能绑定到左值（具名对象），而不能绑定到右值（临时对象）。
2. 它不能延长左值的生命周期。
3. 它可以被重新赋值，即可以指向不同的对象。

例如，以下函数声明中的 `A&` 表示左值引用参数：

```cpp
void foo(A& value);
```

当调用 `foo` 函数时，只能将左值作为参数传递给该函数：

```cpp
A a;
foo(a);  // 传递左值参数
```

在这个例子中，`a` 是一个左值（具名对象），可以直接传递给接受左值引用的函数。

左值引用对于实现对象的别名和传递对象的引用非常有用，可以用于修改传入的参数或共享大型对象的开销。

需要注意的是，一个对象可以同时有一个左值引用和一个右值引用的绑定，但不能同时有两个左值引用或两个右值引用的绑定。

## std::forward

和`std::move`一样，它的兄弟`std::forward`也充满了迷惑性，虽然名字含义是转发，但他并不会做转发，同样也是做类型转换.

与move相比，forward更强大，move只能转出来右值，forward都可以。

> std::forward<T>(u)有两个参数：T与 u。 a. 当T为左值引用类型时，u将被转换为T类型的左值； b. 否则u将被转换为T类型右值。

完美转发

```c++
template <typename T>
void warp(T&& param) {
  func(std::forward<T>(param));
}
```

## 万能引用T&&

使用`T&&`类型的形参既能绑定右值，又能绑定左值。

```c++
template <typename T>
void func(T&& param) {
  cout << param << endl;
}
int main() {
  int num = 2019;
  func(num);
  func(2019);
  return 0;
}
```

### 引用折叠

万能引用说完了，接着来聊引用折叠（Univers Collapse），因为完美转发（Perfect Forwarding）的概念涉及引用折叠。一个模板函数，根据定义的形参和传入的实参的类型，我们可以有下面四种组合：

- 左值-左值 T& & # 函数定义的形参类型是左值引用，传入的实参是左值引用
- 左值-右值 T& && # 函数定义的形参类型是左值引用，传入的实参是右值引用
- 右值-左值 T&& & # 函数定义的形参类型是右值引用，传入的实参是左值引用
- 右值-右值 T&& && # 函数定义的形参类型是右值引用，传入的实参是右值引用

但是C++中不允许对引用再进行引用，对于上述情况的处理有如下的规则：

所有的折叠引用最终都代表一个引用，要么是左值引用，要么是右值引用。规则是：**如果任一引用为左值引用，则结果为左值引用。否则（即两个都是右值引用），结果为右值引用**。

# 计算

```c++
const auto should_secede = [&]() -> bool {
        return state->workers_.size() > static_cast<size_t>(state->desired_capacity_);
    };
```

这段代码定义了一个 lambda 函数 `should_secede`，该函数用于判断是否应该进行分离（secede）操作。它捕获了外部的状态 `state`，并通过访问 `state->workers_.size()` 和 `state->desired_capacity_` 来进行判断。

具体而言，它通过比较 `state->workers_.size()` 和 `state->desired_capacity_` 的大小来确定是否应该进行分离操作。如果 `state->workers_.size()` 大于 `state->desired_capacity_`，即当前工作线程的数量超过了期望的容量，那么函数返回 `true`，表示应该进行分离操作；否则返回 `false`，表示不需要分离。

该 lambda 函数使用了捕获列表 `[&]`，表示捕获外部作用域的所有变量（按引用捕获），以便在函数体内使用它们。在这种情况下，它捕获了 `state` 变量，以便在函数体内访问该变量的成员。

# Thread local Storage

线程局部存储(tls)是一种机制，通过这一机制分配的变量，每个当前线程有一个该变量的实例。

# 内存对齐

```c++
inline static size_t _Index(size_t size, size_t align)
	{
		size_t alignnum = 1 << align;  //库里实现的方法
		return ((size + alignnum - 1) >> align) - 1;
	}

inline static size_t _Roundup(size_t size, size_t align)
	{
		size_t alignnum = 1 << align;
		return (size + alignnum - 1)&~(alignnum - 1);
	}
```

这段代码是一个内存管理中常见的用于计算内存对齐和索引的函数。

1. `_Index(size, align)` 函数用于计算内存块的索引位置。参数 `size` 表示内存块的大小，`align` 表示对齐的位数。这里使用了位运算来计算对齐数 `alignnum`，然后通过对 `size` 进行位移和减法运算得到索引值。

   具体的计算过程如下：

   - 首先，将 `align` 转换为对齐数 `alignnum`，即 2 的 `align` 次方。
   - 接下来，计算 `(size + alignnum - 1) >> align`，即将 `size` 加上 `alignnum - 1` 后进行右移 `align` 位。
   - 最后，减去 1，得到最终的索引值。

   返回的索引值表示该内存块在特定对齐要求下的位置。

2. `_Roundup(size, align)` 函数用于将内存块的大小向上取整到指定的对齐边界。参数 `size` 表示内存块的大小，`align` 表示对齐的位数。同样，这里也使用了位运算来进行取整操作。

   具体的计算过程如下：

   - 首先，将 `align` 转换为对齐数 `alignnum`，即 2 的 `align` 次方。
   - 接下来，计算 `(size + alignnum - 1) & ~(alignnum - 1)`，即将 `size` 加上 `alignnum - 1` 后进行按位与操作，与上取整边界。
   - 最后，返回取整后的结果。

   返回的结果表示将 `size` 向上取整到指定对齐边界后的大小。

这些函数在内存管理中常用于分配和释放内存块时的对齐操作，以及在内存池中进行索引计算。

# C++输出二进制数字

```c++
#include <bitset>

cout << bitset<8>((a>>2)) << endl;
// 输出bitset指定数字的位数
```



# 将拷贝函数声明为delete

```c++
SpanList(const SpanList&) = delete;
SpanList& operator=(const SpanList&) = delete;
```

这段代码使用了C++中的特殊成员函数删除语法，通过将拷贝构造函数和拷贝赋值运算符声明为 `delete`，禁止了 `SpanList` 类的拷贝操作。

1. `SpanList(const SpanList&) = delete;`: 这行代码将拷贝构造函数声明为 `delete`，意味着禁止通过拷贝构造函数创建 `SpanList` 对象的副本。如果尝试使用拷贝构造函数进行对象的拷贝操作，将会导致编译错误。

2. `SpanList& operator=(const SpanList&) = delete;`: 这行代码将拷贝赋值运算符声明为 `delete`，意味着禁止使用拷贝赋值运算符将一个 `SpanList` 对象赋值给另一个 `SpanList` 对象。如果尝试进行这样的赋值操作，将会导致编译错误。

通过删除拷贝构造函数和拷贝赋值运算符，可以防止意外的对象拷贝，从而强制确保 `SpanList` 对象在使用时不能进行拷贝操作。这样可以避免可能的资源管理问题，例如指针的重复释放等，同时也限制了对象的复制行为，使得类更符合设计意图。

# 常量引用

[讲解链接](https://blog.csdn.net/qq_39583450/article/details/110006989)

```c++
T const& pivot 
const T &pivot
```

**其实就是不允许一个普通引用指向一个临时变量，只允许将“常量引用”指向临时对象。**

在函数参数中，使用常量引用非常重要。因为函数有可能接受临时对象，而且同时需要禁止对所引用对象的一切修改。

下面程序执行发生错误，因为不可以将一个字面值常量赋值给普通引用；函数的返回值如果是非引用类型时，实际上是作为一个临时变量返回的，经过上面的讨论，不允许一个普通引用指向临时对象。

```c
int test() {
	return 1;
}

void fun(int &x) {
    cout << x << endl;
}
 
int main()
{
	int m = 1;
	fun(m);         // ok
	fun(1);         // error
    fun(test());    // error
 
	return 0;
}
```

你的示例程序中的问题与你之前提到的问题是相关的。让我解释一下发生的情况：

在 C++ 中，普通的引用（非常量引用）需要绑定到一个具有持久性的对象。而字面值常量（如整数1）或者临时返回的值不具备持久性。因此，你无法将普通引用绑定到字面值常量或者临时对象上。

在你的代码中，`fun(1);` 和 `fun(test());` 都尝试将普通引用绑定到不具备持久性的临时对象上，因此会导致编译错误。

> 当你调用 `fun(1)` 时，发生以下步骤：
>
> 1. 编译器发现 `fun` 函数接受一个 `int` 引用作为参数。
> 2. 由于参数是一个非常量引用，它需要被绑定到一个具有持久性的对象。但是你提供了一个字面值常量 `1`，字面值常量没有持久性。
> 3. 编译器尝试将字面值常量 `1` 转换为一个可以被绑定的临时 `int` 对象。
> 4. 编译器会生成一个临时的匿名 `int` 变量，并将 `1` 的值赋给它。
> 5. 然后，编译器尝试将这个临时 `int` 对象传递给 `fun` 函数。
> 6. `fun` 函数的参数是一个非常量引用，但是它被传递的是一个临时对象，这是不允许的。
>
> 因此，调用 `fun(1)` 会导致编译错误，因为你尝试将一个非常量引用绑定到一个临时对象上。

如果你想在 `fun` 函数中使用临时对象，你可以将参数声明为 `const` 引用，这样就可以将临时对象传递给函数而不会产生错误：

```cpp
void fun(const int &x) {
    cout << x << endl;
}
```

这样的话，`fun(1);` 和 `fun(test());` 都会正常工作。

# 左值引用右值引用

[链接](https://blog.csdn.net/weixin_49199646/article/details/109861756)

```c++
class X
{
private:
  int* data;

public:
  X():
    data(new int[1000000])
  {}

  ~X()
  {
    delete [] data;
  }

  X(const X& other):  // 1
   data(new int[1000000])
  {
    std::copy(other.data,other.data+1000000,data);
  }

  X(X&& other):  // 2
    data(other.data)
  {
    other.data=nullptr;
  }
};
```

一般情况下，拷贝构造函数①都是这么定义：分配一块新内存，然后将数据拷贝进去。不过，现在有了一个新的构造函数，可以接受右值引用来获取老数据②，就是移动构造函数。在这个例子中，只是将指针拷贝到数据中，将other以空指针的形式留在了新实例中；使用右值里创建变量，就能避免了空间和时间上的多余消耗。

# 快排并行版本

```c++
template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input) {
    if (input.empty()) {
        return input;
    }
    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    // splice 把input.begin()元素剪切到result.begin()位置
    T const& pivot = *result.begin();
    auto divide_point = std::partition(input.begin(), input.end(),
                                       [&](T const& t) { return t < pivot ; });
   // lambda 表达式 [&](T const& t) { return t < pivot ; } 是作为谓词函数传递给 std::partition 的。
   // 这个谓词函数会对容器中的每个元素进行评估，如果该元素小于 pivot，则返回 true，否则返回 false。

   std::list<T> lower_part;
   lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
   //把input.begin()到divide_point，剪切到lower_part.end()处
   /* 异步运行版本
   auto new_lower(sequential_quick_sort(std::move(lower_part)));
   auto new_higher(sequential_quick_sort(std::move(input)));
   */
   std::future<std::list<T>> new_lower(
           std::async(sequential_quick_sort<T>, std::move(lower_part)));
   auto new_higher(sequential_quick_sort(std::move(input)));

    result.splice(result.end(), new_higher);  // 合并higher
    result.splice(result.begin(), new_lower.get());  // 合并lower
    return result;
}
```

# list.splice

C++中list:splice函数详解
splice()函数是list中的一个剪贴函数，将另外一个list中的元素剪贴到本list中。共有3个重载：

list1为要操作的list；
list2为被剪去的list；
position为list1中的某个位置的迭代器

list1调用splice函数

list1.splice(position, list2): 将list2中的所有元素剪贴到list1的position位置；
list1.splice(position, list2, iter): 将list2中某个位置的迭代器iter指向的元素剪贴到list1中的position位置；
list1.splice(position, list2, iter1, iter2): 将list2中的某一段位置iter1 ~ iter2(不包括iter2)的元素剪贴到list1中的position位置

# static_cast

`static_cast` 是 C++ 中的一个类型转换操作符，用于在编译时进行类型转换。它可以用于各种类型之间的显式转换，包括基本数据类型、类层次结构中的基类和派生类之间的转换等。

以下是 `static_cast` 的用法和一些常见的应用场景：

1. 基本数据类型之间的转换：
   ```cpp
   int num = 10;
   double numDouble = static_cast<double>(num);
   ```

2. 类型转换避免隐式转换：
   ```cpp
   int num = 10;
   char ch = static_cast<char>(num); // 显式地将整数转换为字符
   ```

3. 指针和引用之间的转换：
   ```cpp
   Base* basePtr = new Derived();
   Derived* derivedPtr = static_cast<Derived*>(basePtr); // 将基类指针转换为派生类指针
   ```

4. 安全地将 `void*` 指针转回原始类型指针：
   ```cpp
   void* voidPtr = someObjectPointer;
   MyType* originalPtr = static_cast<MyType*>(voidPtr); // 将 void* 转换回原始类型指针
   ```

5. 在类之间进行显式转换（如果有定义类型转换操作符）：
   ```cpp
   class A {
   public:
       operator int() const { return 42; }
   };
   
   A aObj;
   int num = static_cast<int>(aObj); // 使用自定义的类型转换操作符
   ```

需要注意的是，`static_cast` 在进行类型转换时，编译器会尽量保证转换的安全性。但是，开发者需要自行确保转换是合理且类型兼容的。如果转换不符合规范，可能会导致编译错误或运行时问题。

与 `dynamic_cast`（用于动态类型转换）和 `const_cast`（用于移除 `const` 限定）等不同，`static_cast` 主要用于在编译时进行类型转换，不进行运行时的类型检查。

# 大端存储小端存储

### 大端存储(Big-Endian)

数据的高字节存储在低地址中，数据的低字节存储在高地址中 如：

```cpp
int a = 1;   
// int在内存中占 4Byte; 
// a在内存中的分布为 0x1 0x0 0x0 0x0; 
// 从左到右内存地址降低，也就是高字节地址存放的是a的低字节数据
```

应用：C51是大端存储；socket编程中网络字节序一般是大端存储

### 小端存储(Little-Endian)

数据的高字节存储在高地址中，数据的低字节存储在低地址中 如：

```cpp
int a = 1;   
// int在内存中占 4Byte; 
// a在内存中的分布为 0x0 0x0 0x0 0x1; 
// 从左到右内存地址降低，也就是低字节地址存放的是a的低字节数据
```

应用：x86结构、ARM和DSP都是小端存储

# 函数

## 删除函数

有时让类去做拷贝是没有意义的。`std::mutex`就是一个例子——拷贝一个互斥量，意义何在？`std::unique_lock<>`是另一个例子——一个实例只能拥有一个锁；如果要复制，拷贝的那个实例也能获取相同的锁，这样`std::unique_lock<>`就没有存在的意义了。实例中转移所有权(A.1.2节)是有意义的，其并不是使用的拷贝。当然其他例子就不一一列举了。

通常为了避免进行拷贝操作，会将拷贝构造函数和拷贝赋值操作符声明为私有成员，并且不进行实现。如果对实例进行拷贝，将会引起编译错误；如果有其他成员函数或友元函数想要拷贝一个实例，那将会引起链接错误(因为缺少实现)：

```c
class no_copies
{
public:
  no_copies(){}
private:
  no_copies(no_copies const&);  // 无实现
  no_copies& operator=(no_copies const&);  // 无实现
};

no_copies a;
no_copies b(a);  // 编译错误
```

在C++11中，委员会意识到这种情况，但是没有意识到其会带来攻击性。因此，委员会提供了更多的通用机制：可以通过添加`= delete`将一个函数声明为删除函数。

no_copise类就可以写为：

```c
class no_copies
{
public:
  no_copies(){}
  no_copies(no_copies const&) = delete;
  no_copies& operator=(no_copies const&) = delete;
};
```

这样的描述要比之前的代码更加清晰。也允许编译器提供更多的错误信息描述，当成员函数想要执行拷贝操作的时候，可将连接错误转移到编译时。

拷贝构造和拷贝赋值操作删除后，需要显式写一个移动构造函数和移动赋值操作符，与`std::thread`和`std::unique_lock<>`一样，你的类是只移动的。

下面清单中的例子，就展示了一个只移动的类。

清单A.2 只移动类

```c
class move_only
{
  std::unique_ptr<my_class> data;
public:
  move_only(const move_only&) = delete;
  move_only(move_only&& other):
    data(std::move(other.data))
  {}
  move_only& operator=(const move_only&) = delete;
  move_only& operator=(move_only&& other)
  {
    data=std::move(other.data);
    return *this;
  }
};

move_only m1;
move_only m2(m1);  // 错误，拷贝构造声明为“已删除”
move_only m3(std::move(m1));  // OK，找到移动构造函数
```

**只移动对象可以作为函数的参数进行传递，并且从函数中返回，不过当想要移动左值，通常需要显式的使用`std::move()`或`static_cast<T&&>`。**

可以为任意函数添加`= delete`说明符，添加后就说明这些函数是不能使用的。当然，还可以用于很多的地方；删除函数可以以正常的方式参与重载解析，并且如果被使用只会引起编译错误。这种方式可以用来删除特定的重载。比如，当函数以short作为参数，为了避免扩展为int类型，可以写出重载函数(以int为参数)的声明，然后添加删除说明符：

```c
void foo(short);
void foo(int) = delete;
```

现在，任何向foo函数传递int类型参数都会产生一个编译错误，不过调用者可以显式的将其他类型转化为short：

```c
foo(42);  // 错误，int重载声明已经删除
foo((short)42);  // OK
```

## 默认函数

删除函数是在函数后面添加`= delete`一样，默认函数需要在函数后面添加`= default`，例如：

```c
class Y
{
private:
  Y() = default;  // 改变访问级别
public:
  Y(Y&) = default;  // 以非const引用作为参数
  T& operator=(const Y&) = default; // 作为文档的形式，声明为默认函数
protected:
  virtual ~Y() = default;  // 改变访问级别，以及添加虚函数标签
};
```

编译器生成函数都有独特的特性，这是用户定义版本所不具备的。最大的区别就是编译器生成的函数都很简单。

```c
X::X() = default;  // 应用默认初始化规则
```

不过，将构造函数显示声明成默认，就能强制编译器为你生成一个默认构造函数，并且刚才说的那种特性会保留：

# constexpr

因为常量表达式的初始化过程是静态初始化，所以就能避免条件竞争和初始化顺序的问题：

当构造函数被声明为constexpr，且构造函数参数是常量表达式时，那么初始化过程就是常数初始化(可能作为静态初始化的一部分)。随着并发的发展，C++11标准中有一个重要的改变：允许用户定义构造函数进行静态初始化，就可以在初始化的时候避免条件竞争，因为静态过程能保证初始化过程在代码运行前进行。C++11标准中有一个重要的改变：允许用户定义构造函数进行静态初始化，就可以在初始化的时候避免条件竞争，因为静态过程能保证初始化过程在代码运行前进行。

`constexpr` 是 C++11 引入的关键字，用于声明可以在编译时求值的表达式或函数。通过使用 `constexpr`，你可以在编译时计算出表达式的值，从而在某些情况下避免运行时的开销。

在你提供的代码中，如果你想要在编译时就计算出某些值，可以考虑使用 `constexpr`：

```cpp
constexpr int forty_two = 42;

struct my_aggregate
{
  int a;
  int b;
};

constexpr my_aggregate ma1 = {forty_two, 123};  // 静态初始化
int dummy = 257;
constexpr my_aggregate ma2 = {dummy, dummy};    // 动态初始化

int main() {
    // 使用 ma1 和 ma2
    return 0;
}
```

在上面的代码中，`forty_two` 被声明为 `constexpr`，这意味着它的值在编译时就可以被计算出来。然后，你可以在聚合体的静态初始化和动态初始化中使用 `forty_two`。

请注意，`constexpr` 的使用有一些限制，例如表达式必须在编译时求值等。在某些情况下，编译器可能无法在编译时求值，这时会退化为运行时计算。

静态初始化可以避免初始化顺序和条件变量的问题。

这些都不是新添加的——你可以在1998版本的C++标准中找到对应上面实例的条款。不过，新标准中常量表达式进行了扩展，并添加了新的关键字——`constexpr`。

`constexpr`会对功能进行修改，当参数和函数返回类型符合要求，并且实现很简单，那么这样的函数就能够被声明为`constexpr`，这样函数可以当做常数表达式来使用：

```c
constexpr int square(int x)
{
  return x*x;
}
int array[square(5)];
```

在这个例子中，array有25个元素，因为square函数的声明为`constexpr`。当然，这种方式可以当做常数表达式来使用，不意味着什么情况下都是能够自动转换为常数表达式：

```c
int dummy=4;
int array[square(dummy)];  // 错误，dummy不是常数表达式
```

dummy不是常数表达式，所以square(dummy)也不是——就是一个普通函数调用——所以其不能用来指定array的长度

**目前，已经了解了constexpr在函数上的应用。constexpr也可以用在对象上，主要是用来做判断的；验证对象是否是使用常量表达式，constexpr构造函数或组合常量表达式进行初始化。**

且这个对象需要声明为const：

```c
constexpr int i=45;  // ok
constexpr std::string s(“hello”);  // 错误，std::string不是字面类型

int foo();
constexpr int j=foo();  // 错误，foo()没有声明为constexpr
```

将一个函数声明为constexpr，也是有几点要求的；当不满足这些要求，constexpr声明将会报编译错误。

- 所有参数都必须是字面类型。
- 返回类型必须是字面类型。
- 函数体内必须有一个return。
- return的表达式需要满足常量表达式的要求。
- 构造返回值/表达式的任何构造函数或转换操作，都需要是constexpr。

看起来很简单，要在内联函数中使用到常量表达式，返回的还是个常量表达式，还不能对任何东西进行改动。constexpr函数就是无害的纯洁的函数。

constexpr类成员函数，需要追加几点要求：

- constexpr成员函数不能是虚函数。
- 对应类必须有字面类的成员。

constexpr构造函数的规则也有些不同：

- 构造函数体必须为空。
- 每一个基类必须可初始化。
- 每个非静态数据成员都需要初始化。
- 初始化列表的任何表达式，必须是常量表达式。
- 构造函数可选择要进行初始化的数据成员，并且基类必须有constexpr构造函数。
- 任何用于构建数据成员的构造函数和转换操作，以及和初始化表达式相关的基类必须为constexpr。

这些条件同样适用于成员函数，除非函数没有返回值，也就没有return语句。

另外，构造函数对初始化列表中的所有基类和数据成员进行初始化。一般的拷贝构造函数会隐式的声明为constexpr。

# 字面类

"字面类"（Literal Class）是 C++11 引入的概念，它指的是可以在编译时进行完全初始化和计算的类。字面类需要满足一些特定的条件，以便在编译时对其进行优化。

以下是字面类的一些特点和要求：

1. **成员变量**：字面类只能有公共成员变量，且这些成员变量必须是字面类型（例如整数、指针、枚举、引用等）。

2. **构造函数**：字面类可以有构造函数，但必须是 constexpr 构造函数，这意味着它们可以在编译时执行。

3. **析构函数**：如果字面类有析构函数，它也必须是 trivial（平凡的）析构函数，或者是 constexpr 析构函数。

4. **函数成员**：字面类可以有函数成员，但这些函数必须是 constexpr 函数。

5. **继承**：如果字面类有基类，基类也必须是字面类。

6. **无虚函数**：字面类不能有虚函数，也不能继承自包含虚函数的类。

字面类的一个主要应用是在编译时进行优化，从而避免运行时的开销。这在一些要求高性能和资源受限的场景下特别有用。例如，你可以使用字面类来表示一些固定的配置信息、常量表达式的计算等。

以下是一个示例字面类的代码：

```cpp
class Point {
public:
    constexpr Point(int x, int y) : x_(x), y_(y) {}

    constexpr int getX() const {
        return x_;
    }

    constexpr int getY() const {
        return y_;
    }

private:
    int x_;
    int y_;
};

int main() {
    constexpr Point p(5, 10);
    int x = p.getX();
    int y = p.getY();
    return 0;
}
```

在上述代码中，`Point` 是一个字面类，因为它满足了字面类的要求：包含了成员变量和 constexpr 构造函数。因此，可以在编译时对 `Point` 进行优化和计算。

**`std::string` 不是字面类，设计目标是提供一个动态可变长度的字符串存储方案，它可以在运行时分配和释放内存，并支持字符串的动态操作。而字面类的一个特点是在编译时进行优化，适用于静态的、在编译时已知的数据结构。**

# Lamdba函数

最简单的情况下，lambda表达式就一个自给自足的函数，不需要传入函数仅依赖管局变量和函数，甚至都可以不用返回一个值。这样的lambda表达式的一系列语义都需要封闭在括号中，还要以方括号作为前缀：

```c
[]{  // lambda表达式以[]开始
  do_stuff();
  do_more_stuff();
}();  // 表达式结束，可以直接调用
```

例子中，lambda表达式通过后面的括号调用，不过这种方式不常用。一方面，如果想要直接调用，可以在写完对应的语句后，就对函数进行调用。对于函数模板，传递一个参数进去时很常见的事情，甚至可以将可调用对象作为其参数传入；可调用对象通常也需要一些参数，或返回一个值，亦或两者都有。如果想给lambda函数传递参数，可以参考下面的lambda函数，其使用起来就像是一个普通函数。例如，下面代码是将vector中的元素使用`std::cout`进行打印：

```c
std::vector<int> data=make_data();
std::for_each(data.begin(),data.end(),[](int i){std::cout<<i<<"\n";});
```

返回值也是很简单的，当lambda函数体包括一个return语句，返回值的类型就作为lambda表达式的返回类型。例如，使用一个简单的lambda函数来等待`std::condition_variable`(见4.1.1节)中的标志被设置。

清单A.4 lambda函数推导返回类型

```c
std::condition_variable cond;
bool data_ready;
std::mutex m;
void wait_for_data()
{
  std::unique_lock<std::mutex> lk(m);
  cond.wait(lk,[]{return data_ready;});  // 1
}
```

lambda的返回值传递给cond.wait()①，函数就能推断出data_ready的类型是bool。当条件变量从等待中苏醒后，上锁阶段会调用lambda函数，并且当data_ready为true时，仅返回到wait()中。

当lambda函数体中有多个return语句，就需要显式的指定返回类型。只有一个返回语句的时候，也可以这样做，不过这样可能会让你的lambda函数体看起来更复杂。返回类型可以使用跟在参数列表后面的箭头(->)进行设置。如果lambda函数没有任何参数，还需要包含(空)的参数列表，这样做是为了能显式的对返回类型进行指定。对条件变量的预测可以写成下面这种方式：

```c
cond.wait(lk,[]()->bool{return data_ready;});
```

还可以对lambda函数进行扩展，比如：加上log信息的打印，或做更加复杂的操作：

```c
cond.wait(lk,[]()->bool{
  if(data_ready)
  {
    std::cout<<”Data ready”<<std::endl;
    return true;
  }
  else
  {
    std::cout<<”Data not ready, resuming wait”<<std::endl;
    return false;
  }
});
```

虽然简单的lambda函数很强大，能简化代码，不过其真正的强大的地方在于对本地变量的捕获。

## 引用本地变量的lambda函数

lambda函数使用空的`[]`(lambda introducer)就不能引用当前范围内的本地变量；其只能使用全局变量，或将其他值以参数的形式进行传递。当想要访问一个本地变量，需要对其进行捕获。最简单的方式就是将范围内的所有本地变量都进行捕获，使用`[=]`就可以完成这样的功能。函数被创建的时候，就能对本地变量的副本进行访问了。

实践一下，看一下下面的例子：

```c
std::function<int(int)> make_offseter(int offset)
{
  return [=](int j){return offset+j;};
}
```

当调用make_offseter时，就会通过`std::function<>`函数包装返回一个新的lambda函数体。

**非常重要这段话！！！**

**这种本地变量捕获的方式相当安全，所有的东西都进行了拷贝，所以可以通过lambda函数对表达式的值进行返回，并且可在原始函数之外的地方对其进行调用。这也不是唯一的选择，也可以通过选择通过引用的方式捕获本地变量。在本地变量被销毁的时候，lambda函数会出现未定义的行为。**



下面的例子，就介绍一下怎么使用`[&]`对所有本地变量进行引用：

```c
int main()
{
  int offset=42;  // 1
  std::function<int(int)> offset_a=[&](int j){return offset+j;};  // 2
  offset=123;  // 3
  std::function<int(int)> offset_b=[&](int j){return offset+j;};  // 4
  std::cout<<offset_a(12)<<”,”<<offset_b(12)<<std::endl;  // 5
  offset=99;  // 6
  std::cout<<offset_a(12)<<”,”<<offset_b(12)<<std::endl;  // 7
}
```

之前的例子中，使用`[=]`来对要偏移的变量进行拷贝，offset_a函数就是个使用`[&]`捕获offset的引用的例子②。所以，offset初始化成42也没什么关系①；offset_a(12)的例子通常会依赖与当前offset的值。在③上，offset的值会变为123，offset_b④函数将会使用到这个值，同样第二个函数也是使用引用的方式。

现在，第一行打印信息⑤，offset为123，所以输出为135,135。不过，第二行打印信息⑦就有所不同，offset变成99⑥，所以输出为111,111。offset_a和offset_b都对当前值进行了加12的操作。

尘归尘，土归土，C++还是C++；这些选项不会让你感觉到特别困惑，你可以选择以引用或拷贝的方式对变量进行捕获，并且你还可以通过调整中括号中的表达式，来对特定的变量进行显式捕获。如果想要拷贝所有变量，而非一两个，可以使用`[=]`，通过参考中括号中的符号，对变量进行捕获。**下面的例子将会打印出1239，因为i是拷贝进lambda函数中的，而j和k是通过引用的方式进行捕获的：**

```c
int main()
{
  int i=1234,j=5678,k=9;
  std::function<int()> f=[=,&j,&k]{return i+j+k;};
  i=1;
  j=2;
  k=3;
  std::cout<<f()<<std::endl;
}
```

或者，也可以通过默认引用方式对一些变量做引用，而对一些特别的变量进行拷贝。这种情况下，就要使用`[&]`与拷贝符号相结合的方式对列表中的变量进行拷贝捕获。下面的例子将打印出5688，因为i通过引用捕获，但j和k 通过拷贝捕获：

```c
int main()
{
  int i=1234,j=5678,k=9;
  std::function<int()> f=[&,j,k]{return i+j+k;};
  i=1;
  j=2;
  k=3;
  std::cout<<f()<<std::endl;
}
```

如果你只想捕获某些变量，那么你可以忽略=或&，仅使用变量名进行捕获就行；加上&前缀，是将对应变量以引用的方式进行捕获，而非拷贝的方式。下面的例子将打印出5682，因为i和k是通过引用的范式获取的，而j是通过拷贝的方式：

```c
int main()
{
  int i=1234,j=5678,k=9;
  std::function<int()> f=[&i,j,&k]{return i+j+k;};
  i=1;
  j=2;
  k=3;
  std::cout<<f()<<std::endl;
}
```

最后一种方式，**是为了确保预期的变量能被捕获，在捕获列表中引用任何不存在的变量都会引起编译错误。**当选择这种方式，就要小心类成员的访问方式，确定类中是否包含一个lambda函数的成员变量。**类成员变量不能直接捕获，如果想通过lambda方式访问类中的成员，需要在捕获列表中添加this指针，以便捕获。**下面的例子中，lambda捕获this后，就能访问到some_data类中的成员：

```c
struct X
{
  int some_data;
  void foo(std::vector<int>& vec)
  {
    std::for_each(vec.begin(),vec.end(),
         [this](int& i){i+=some_data;});
  }
};
```

**在 lambda 表达式 `[this](int& i){i+=some_data;}` 中，`[this]` 指明了 lambda 表达式捕获了外部的 `this` 指针，允许在 lambda 内部访问包含它的类的成员变量和成员函数。Lambda 表达式中的 `int& i` 表示 lambda 将会按引用捕获向量中的每个元素，从而可以修改它们。**

**具体而言，`foo` 函数使用 `std::for_each` 算法遍历传入的向量 `vec`，并对其中的每个元素执行 lambda 表达式中的操作，将元素的值增加了 `some_data`。这种操作会修改原始的向量中的元素。**

并发的上下文中，lambda是很有用的，其可以作为谓词放在`std::condition_variable::wait()`(见4.1.1节)和`std::packaged_task<>`(见4.2.1节)中；或是用在线程池中，对小任务进行打包。也可以线程函数的方式`std::thread`的构造函数(见2.1.1)，以及作为一个并行算法实现，在parallel_for_each()(见8.5.1节)中使用。

# std::function

`std::function` 是 C++ 标准库中的一个类模板，用于表示函数对象的封装。它可以用于存储和调用任何可调用对象，例如普通函数、成员函数、lambda 表达式、函数指针等。

`std::function` 提供了一种通用的方式来处理函数对象，使得代码更加灵活和可扩展。它在 C++11 标准中被引入，为 C++ 中的函数式编程和泛型编程提供了更大的支持。

以下是一些关键点和用法示例：

1. **创建 `std::function` 对象：** 使用 `std::function` 可以封装各种可调用对象。例如，以下代码演示了如何创建一个可以调用 `int` 类型参数并返回 `int` 类型结果的函数对象：

   ```cpp
   std::function<int(int)> func = [](int x) { return x * 2; };
   ```

2. **调用函数对象：** 使用 `std::function` 的 `operator()` 运算符来调用封装的函数对象。

   ```cpp
   int result = func(5); // 调用函数对象并得到结果，result 将等于 10
   ```

3. **类型安全：** `std::function` 在编译时执行类型检查，确保调用函数对象时传递的参数和返回值类型与函数对象的签名匹配。

4. **泛型编程：** `std::function` 允许在编写泛型代码时传递不同类型的函数对象，从而提高代码的灵活性和复用性。

5. **存储回调函数：** 在事件驱动的程序中，可以使用 `std::function` 来存储回调函数，以便在特定事件发生时执行相应的操作。

6. **作为函数参数传递：** 可以将 `std::function` 作为函数参数传递，以便在运行时传递不同的函数实现。

总之，`std::function` 提供了一种方便的方式来操作和封装各种可调用对象，使得函数对象的使用更加灵活和可维护。

# 扩展参数包

变参模板主要依靠包括扩展功能，因为不能限制有更多的类型添加到模板参数中。首先，列表中的参数类型使用到的时候，可以使用包扩展，比如：需要给其他模板提供类型参数。

```c
template<typename ... Params>
struct dummy
{
  std::tuple<Params...> data;
};
```

成员变量data是一个`std::tuple<>`实例，包含所有指定类型，所以dummy的成员变量就为`std::tuple<int, double, char>`。

可以将包扩展和普通类型相结合：

```c
template<typename ... Params>
struct dummy2
{
  std::tuple<std::string,Params...> data;
};
```

这次，元组中添加了额外的(第一个)成员类型`std::string`。其优雅指出在于，可以通过包扩展的方式创建一种模式，这种模式会在之后将每个元素拷贝到扩展之中，可以使用`...`来表示扩展模式的结束。

例如，创建使用参数包来创建元组中所有的元素，不如在元组中创建指针，或使用`std::unique_ptr<>`指针，指向对应元素：

```c
template<typename ... Params>
struct dummy3
{
  std::tuple<Params* ...> pointers;
  std::tuple<std::unique_ptr<Params> ...> unique_pointers;
};
```

不需要变长的参数了；如果不需要了，参数包就要对模板参数的要求进行准确的匹配：

```c
template<typename ... Types>
struct dummy4
{
  std::pair<Types...> data;
};
dummy4<int,char> a;  // 1 ok，为std::pair<int, char>
dummy4<int> b;  // 2 错误，无第二个类型
dummy4<int,int,int> c;  // 3 错误，类型太多
```

可以使用包扩展的方式，对函数的参数进行声明：

```c
template<typename ... Args>
void foo(Args ... args);
```

这将会创建一个新参数包args，其是一组函数参数，而非一组类型，并且这里`...`也能像之前一样进行扩展。例如，可以在`std::thread`的构造函数中使用，使用右值引用的方式获取函数所有的参数(见A.1节)：

```c
template<typename CallableType,typename ... Args>
thread::thread(CallableType&& func, Args&& ... args);
```

函数参数包也可以用来调用其他函数，将制定包扩展成参数列表，匹配调用的函数。如同类型扩展一样，也可以使用某种模式对参数列表进行扩展。

例如，使用`std::forward()`以右值引用的方式来保存提供给函数的参数：

```c
template<typename ... ArgTypes>
void bar(ArgTypes&& ... args)
{
  foo(std::forward<ArgTypes>(args)...);
}
```

注意一下这个例子，包扩展包括对类型包ArgTypes和函数参数包args的扩展，并且省略了其余的表达式。

当这样调用bar函数：

```c
int i;
bar(i,3.141,std::string("hello "));
```

将会扩展为

```c
template<>
void bar<int&,double,std::string>(
         int& args_1,
         double&& args_2,
         std::string&& args_3)
{
  foo(std::forward<int&>(args_1),
      std::forward<double>(args_2),
      std::forward<std::string>(args_3));
}
```

最后一件事，参数包中使用`sizeof...`操作可以获取类型参数类型的大小，`sizeof...(p)`就是p参数包中所包含元素的个数。不管是类型参数包或函数参数包，结果都是一样的。这可能是唯一一次在使用参数包的时候，没有加省略号；这里的省略号是作为`sizeof...`操作的一部分，所以不算是用到省略号。

下面的函数会返回参数的数量：

```c
template<typename ... Args>
unsigned count_args(Args ... args)
{
  return sizeof... (Args);
}
```

就像普通的sizeof操作一样，`sizeof...`的结果为常量表达式，所以其可以用来指定定义数组长度，等等。

# join wait

![](/Users/shawnzhao/Desktop/pic/join_wait.png)

`join` 保障执行顺序

`wait` 等待。需要其它的接口来唤醒，变为阻塞状态。

**同步队列（锁池）**：假设线程A已经拥有了某个对象（注意:不是类）的锁，而其它的线程想要调用这个对象的某个synchronized方法(或者synchronized块)，由于这些线程在进入对象的synchronized方法之前必须先获得该对象的锁的拥有权，但是该对象的锁目前正被线程A拥有，所以这些线程就进入了该对象的同步队列（锁池）中，这些线程状态为Blocked。**当线程试图执行某个操作（如访问共享资源、获取互斥锁等）时，如果该操作无法立即完成，线程可能会被阻塞。被阻塞的线程被称为"blocked"，它暂时无法继续执行。这种情况通常是因为线程在等待某个条件满足，或者在等待某个事件发生。**

**等待队列（等待池）**：假设一个线程A调用了某个对象的wait()方法，线程A就会释放该对象的锁（因为wait()方法必须出现在synchronized中，这样自然在执行wait()方法之前线程A就已经拥有了该对象的锁），同时线程A就进入到了该对象的等待队列（等待池）中，此时线程A状态为Waiting。如果另外的一个线程调用了相同对象的notifyAll()方法，那么 处于该对象的等待池中的线程就会全部进入该对象的同步队列（锁池）中，准备争夺锁的拥有权。如果另外的一个线程调用了相同对象的notify()方法，那么 仅仅有一个处于该对象的等待池中的线程（随机）会进入该对象的同步队列（锁池）。**当线程主动调用某些线程同步机制（如条件变量的 `wait` 操作、信号量的 `wait` 操作等）时，线程会进入"waiting"状态。在这种状态下，线程主动放弃了CPU的执行权，直到某个条件被满足，线程被唤醒并继续执行。**

```c
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <unistd.h>

std::mutex  mtx_syn;
std::condition_variable cv_syn;
std::condition_variable cv_syn_1;
bool ready = false;
void threadA (int id) {
    while (1)
    {
        std::unique_lock<std::mutex> lck(mtx_syn);
        while (!ready)
        {
            cv_syn.wait(lck);
        }

        std::cout << "threadA " << id << '\n';
        usleep(500*1000);
        cv_syn.notify_one();   // 唤醒等待线程中的一个
        cv_syn.wait(lck);      // 等待
    }
}

void threadB (int id) {
    while (1)
    {
        //新创建的 unique_lock 对象管理 Mutex 对象 m，并尝试调用 m.lock() 对 Mutex 对象进行上锁，如果此时另外某个 unique_lock 对象已经管理了该 Mutex 对象 m，则当前线程将会被阻塞
        std::unique_lock<std::mutex> lck(mtx_syn);
        while (!ready)
        {
            cv_syn.wait(lck);
        }
        // ...
        std::cout << "threadB " << id << '\n';
        usleep(500*1000);
        cv_syn.notify_one();// 唤醒等待线程中的一个
        cv_syn.wait(lck);
    }
}

void threadC(int id)
{
    while (1)
    {
        std::unique_lock<std::mutex> lck(mtx_syn);
        while (!ready) cv_syn.wait(lck);
        // ...
        std::cout << "threadC " << id << '\n';
        usleep(500*1000);
        cv_syn.notify_one();  // 唤醒等待线程中的一个线程
        cv_syn.wait(lck);
    }
}


void hello() {
    std::unique_lock<std::mutex> lck(mtx_syn);
    ready = true;
    //cv_syn.notify_one(); // 唤醒等待线程中的一个线程，notify_one()唤醒的线程顺序执行
    cv_syn.notify_all();  //唤醒的线程抢占式知性
}

int main() {
    std::thread threads[5];
    threads[0] = std::thread(threadA, 0);
    threads[1] = std::thread(threadB, 1);
    threads[2] = std::thread(threadC, 2);
    std::cout << "3 threads ready to race...\n";
    hello();                       // go!
    for (auto& th: threads) th.join();
    std::cout << "done" << std::endl;
    return 0;
}
```

