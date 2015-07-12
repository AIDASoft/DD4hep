Gaudi Plugin Service Instructions              {#GaudiPluginService-readme}
==================================

Introduction
------------

The Gaudi Plugin Service is a small tool to add to a C++ application the
possibility of dynamically instantiate (via _factories_) objects from classes
defined in plug-in (or component) libraries.

While being part of Gaudi, it only depends on a Posix system (support for other
systems is possible, but very low priority).


Usage
------

To be able to use plug-ins from an application you need:

-   a base class (abstract or not) from a library
-   a library that provides a class that inherits from the base class

In the base class you should declare the signature of the the factory for your
derived classes.  For example, if your base class is called `Foo` and you want
to instantiate the derived classes with one `std::string` argument, you can
write something like:

    #include <Gaudi/PluginService.h>
    #include <string>
    class Foo {
    public:
      typedef Gaudi::PluginService::Factory1<Foo*, const std::string&> Factory;

      /// Constructor
      Foo(const std::string& name);

      // ...
    };

The templated class `Gaudi::PluginService::Factory1` takes as first template
argument the return type of the factory and as second argument the type of the
first argument of the factory function (with all the required qualifiers
explicit).  There are several variants of the class for different number of
arguments required by the constructor (`Factory0`, `Factory1`, `Factory2`, ...).

The plug-in class `Bar` defined in the dynamically loaded library will require
a declaration to the Plugin Service to use it, so in the source file you have to
have something like:

    #include "Bar.h"
    DECLARE_COMPONENT(Bar)

The library with `Foo` and the library with `Bar` will have to be linked against
the library `libGaudiPluginService.so`.

To enable the automatic discovery of plugins, the library with `Bar` must be
processed by the program `listcomponents` and the output must be stored in a
file with extension `.comonents` in a directory in the `LD_LIBRARY_PATH`.
For example, if the `lib` directory contains `libBar.so` and it is specified in
the `LD_LIBRARY_PATH`, you can call the commands:

    listcomponents libBar.so >> lib/MyApp.components

Note that the `.components` file does not need to be in the same directory as
`libBar.so`.

The application code, linked against the library providing `Foo` can now
instantiate objects of class `Bar` like this:

    #include "Foo.h"

    // ...
    Foo* myBar = Foo::Factory::create("Bar", "myBar");
    // ...

where the first argument to the function `create` is the name of the class you
want to instantiate, and the other arguments are passed to the constructor of
the class.


Special cases
-------------

### Factory aliases

Together with the simple usage described above, the Gaudi Plugin Service allows
you to give to use aliases to refer to the plug-in class.
For example, for a templated plug-in class you may have:

    #include "TemplatedBar.h"
    typedef TemplatedBar<int, std::vector<double> > MyBar;
    DECLARE_COMPONENT(MyBar)

but to instantiate it you must call

    Foo* b = Foo::Factory::create("TemplatedBar<int, std::vector<double> >",
                                  "MyTemplatedBar");

Which is error prone and unreadable, but you can declare the component class
with and _id_ (an alias):

    DECLARE_COMPONENT_WITH_ID(MyBar, "MyBar")

(note that the _id_ must support the `<<` operator of `std::ostream`).
The call in the application becomes:

    Foo* b = Foo::Factory::create("MyBar", "MyTemplatedBar");


### Namespaces

You cannot use namespace delimiters in the call to `DECLARE_COMPONENT`, but you
can still use namespaces for you component classes. For example, if you have the
class `Baz::Fun` you can declare it as a component class in any of the following
ways:

    using Baz::Fun;
    DECLARE_COMPONENT(Fun)

    namespace Baz {
      DECLARE_COMPONENT(Fun)
    }

    typedef Baz::Fun BF;
    DECLARE_COMPONENT(BF)

In all cases the name of the factory to be passed to the `create` function will
be "Baz::Fun".
