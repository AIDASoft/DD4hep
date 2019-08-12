# Gaudi::PluginService              {#GaudiPluginService-readme}

## Introduction

The Gaudi Plugin Service is a small tool to add to a C++ application the
possibility of dynamically instantiate (via _factories_) objects from classes
defined in plug-in (or component) libraries.

While being part of Gaudi, it only depends on a Posix system (support for other
systems is possible, but very low priority).

## Usage

To be able to use plug-ins from an application you need:
- a base class (abstract or not) from a library
- a library that provides a class that inherits from the base class

In the base class you should declare the signature of the the factory for your
derived classes.  For example, if your base class is called `Foo` and you want
to instantiate the derived classes with one `std::string` argument, you can
write something like:
```cpp
#include <Gaudi/PluginService.h>
#include <string>
class Foo {
public:
  using Factory = Gaudi::PluginService::Factory<Foo*( const std::string& )>;

  /// Constructor
  Foo( const std::string& name );

  // ...
};
```

The templated class `Gaudi::PluginService::Factory` takes as template
argument the _ideal_ signature of the factory.  The in the above example, the
actual signature of the factory is `std::unique_ptr<Foo>(const std::string&)`,
but we declare is as returning `Foo*` for brevity.

The plug-in class `Bar` defined in the dynamically loaded library will require
a declaration to the Plugin Service to use it, so in the source file you have to
have something like:
```cpp
#include "Bar.h"
DECLARE_COMPONENT( Bar )
```

The library with `Foo` and the library with `Bar` will have to be linked against
the library `libGaudiPluginService.so`.

To enable the automatic discovery of plugins, the library with `Bar` must be
processed by the program `listcomponents` and the output must be stored in a
file with extension `.components` in a directory in the `LD_LIBRARY_PATH`.
For example, if the `lib` directory contains `libBar.so` and it is specified in
the `LD_LIBRARY_PATH`, you can call the commands:
```sh
listcomponents lib/libBar.so >> lib/MyApp.components
```

Note that the `.components` file does not need to be in the same directory as
`libBar.so`.

The application code, linked against the library providing `Foo` can now
instantiate objects of class `Bar` like this:
```cpp
#include "Foo.h"

// ...
std::unique_ptr<Foo> myBar = Foo::Factory::create( "Bar", "myBar" );
// ...
```

where the first argument to the function `create` is the name of the class you
want to instantiate, and the other arguments are passed to the constructor of
the class.

## Special cases

### Factory aliases

Together with the simple usage described above, the Gaudi Plugin Service allows
you to use aliases to refer to the plug-in class.
For example, for a templated plug-in class you may have:
```cpp
#include "TemplatedBar.h"
typedef TemplatedBar<int, std::vector<double>> MyBar;
DECLARE_COMPONENT( MyBar )
```

but to instantiate it you must call
```cpp
auto b = Foo::Factory::create( "TemplatedBar<int, std::vector<double> >",
                               "MyTemplatedBar" );
```

Which is error prone and unreadable, but you can declare the component class
with and _id_ (an alias):
```cpp
DECLARE_COMPONENT_WITH_ID( MyBar, "MyBar" )
```

(note that the _id_ must support the `<<` operator of `std::ostream`).
The call in the application becomes:
```cpp
auto b = Foo::Factory::create( "MyBar", "MyTemplatedBar" );
```

### Namespaces

When dealing with components in namespaces, you have several ways to invoke
`DECLARE_COMPONENT`. For example, if you have the class `Baz::Fun` you can
declare it as a component class in any of the following ways:
```cpp
DECLARE_COMPONENT( Baz::Fun )

using Baz::Fun;
DECLARE_COMPONENT( Fun )

namespace Baz {
  DECLARE_COMPONENT( Fun )
}

typedef Baz::Fun BF;
DECLARE_COMPONENT( BF )
```

In all cases the name of the factory to be passed to the `create` function will
be `Baz::Fun`.

### Custom Factories

When using `DECLARE_COMPONENT`, we register as factory for our class a function
equivalent to
```cpp
std::unique_ptr<Foo> factory(Args... args) {
  return std::make_unique<Bar>(args...);
}
```
but it's possible to use custom factory functions. This is a rather convoluted
example:
```cpp
// -- declaration --
struct MyInterface {
  virtual ~MyInterface() = default;

  virtual const std::string& name() const = 0;
};

struct BaseSetupHelper;

struct MyBase : MyInterface {
  using Factory = Gaudi::PluginService::Factory<MyInterface*( const std::string& )>;

  const std::string& name() const override { return m_name; }
private:
  friend BaseSetupHelper;
  std::string m_name;
};

// -- implementation --
struct MyComponent : MyBase {
  MyComponent() {}
};

struct BaseSetupHelper {
  static void setName( MyBase* base, const std::string& name ) { base->m_name = name; }
};

namespace
{
  std::unique_ptr<MyInterface> creator( const std::string& name )
  {
    auto p = std::make_unique<MyComponent>();
    BaseSetupHelper::setName( p.get(), name );
    return std::move( p );
  }
  Gaudi::PluginService::DeclareFactory<MyComponent> _{creator};
}

// -- use --
void useComponent()
{
  auto c = MyBase::Factory::create( "MyComponent", "TheName" );
  // ...
}

```
