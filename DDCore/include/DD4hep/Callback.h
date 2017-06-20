//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_DDCORE_CALLBACK_H
#define DD4HEP_DDCORE_CALLBACK_H

// C/C++ include files
#include <algorithm>
#include <typeinfo>
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {


  /// Definition of the generic callback structure for member functions
  /**
   *  The callback structure allows to wrap any member function with up to
   *  3 arguments into an abstract objects, which could later be called
   *  using the argument list. The pointer to the hosting objects is stored
   *  in the callback object. The callback objects in this sense behaves
   *  very similar to a static function.
   *
   *  \author  M.Frank
   *  \date    01/03/2013
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  class Callback {
  public:
    typedef unsigned long (*func_t)(void* obj, const void* fun, const void* args[]);
    /// Structure definition to store callback related data  \ingroup DD4HEP
    typedef struct {
      void *first, *second;
    } mfunc_t;
    typedef unsigned long ulong;

    void* par;
    func_t call;
    mfunc_t func;

    /// Default constructor
    Callback()
      : par(0), call(0) {
      func.first = func.second = 0;
    }
    /// Constructor with object initialization
    Callback(void* p)
      : par(p), call(0) {
      func.first = func.second = 0;
    }
    /// Initializing constructor
    Callback(void* p, void* mf, func_t c)
      : par(p), call(c) {
      func = *(mfunc_t*) mf;
    }
    /// Check validity of the callback object
    operator bool() const {
      return (call && par && func.first);
    }
    /// Execute the callback with the required number of user parameters
    unsigned long execute(const void* user_param[]) const {
      return (*this) ? call(par, &func, user_param) : 0;
    }
    /// Template cast function used internally by the wrapper for type conversion to the object's type
    template <typename T> static T* cast(void* p) {
      return (T*) p;
    }
    /// Template const cast function used internally by the wrapper for type conversion to the object's type
    template <typename T> static const T* c_cast(const void* p) {
      return (const T*) p;
    }

    /// Wrapper around a C++ member function pointer
    /**
     *  Generic wrapper around a object member function.
     *
     *  \author  M.Frank
     *  \date    01/03/2013
     *  \version 1.0
     *  \ingroup DD4HEP
     */
    template <typename T> class Wrapper {
    public:
      typedef T pmf_t;
      /// Union to store pointers to member functions in callback objects
      union Functor {
        mfunc_t ptr;
        pmf_t pmf;
        Functor(const void* f) {
          ptr = *(mfunc_t*) f;
        }
        Functor(pmf_t f) {
          pmf = f;
        }
      };
      static mfunc_t pmf(pmf_t f) {
        const Functor func(f);
        return func.ptr;
      }
    };

    /** Callback setup with no arguments  */
    /// Callback setup function for Callbacks with member functions taking no arguments
    template <typename T> const Callback& _make(ulong (*fptr)(void* o, const void* f, const void* u[]), T pmf) {
      typename Wrapper<T>::Functor f(pmf);
      func = f.ptr;
      call = fptr;
      return *this;
    }
    /// Callback setup function for Callbacks with member functions with explicit return type taking no arguments
    template <typename R, typename T> const Callback& make(R (T::*pmf)()) {
      typedef R (T::*pfunc_t)();
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void*[]) {
          return (ulong) (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))();
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    /// Callback setup function for Callbacks with const member functions with explicit return type taking no arguments
    template <typename R, typename T> const Callback& make(R (T::*pmf)() const) {
      typedef R (T::*pfunc_t)() const;
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void*[]) {
          return (ulong) (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))();
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    /// Callback setup function for Callbacks with void member functions taking no arguments
    template <typename T> const Callback& make(void (T::*pmf)()) {
      typedef void (T::*pfunc_t)() const;
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void*[]) {
          (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))();
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    /// Callback setup function for Callbacks with const void member functions taking no arguments
    template <typename T> const Callback& make(void (T::*pmf)() const) {
      typedef void (T::*pfunc_t)() const;
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void*[]) {
          (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))();
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }

    /** Callbacks with 1 argument  */
    /// Callback setup function for Callbacks with member functions with explicit return type taking 1 argument
    template <typename R, typename T, typename A> const Callback& make(R (T::*pmf)(A)) {
      typedef R (T::*pfunc_t)(A);
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void* u[]) {
          return (ulong) (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))((A) u[0]);
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    /// Callback setup function for Callbacks with const member functions with explicit return type taking 1 argument
    template <typename R, typename T, typename A> const Callback& make(R (T::*pmf)(A) const) {
      typedef R (T::*pfunc_t)(A) const;
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void* u[]) {
          return (ulong) (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))((A) u[0]);
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    /// Callback setup function for Callbacks with void member functions taking 1 argument
    template <typename T, typename A> const Callback& make(void (T::*pmf)(A)) {
      typedef void (T::*pfunc_t)(const A);
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void* u[]) {
          (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))((A) u[0]);
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    /// Callback setup function for Callbacks with const void member functions taking 1 argument
    template <typename T, typename A> const Callback& make(void (T::*pmf)(A) const) {
      typedef void (T::*pfunc_t)(const A) const;
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void* u[]) {
          (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))((A) u[0]);
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }

    /** Callback with 2 arguments     */

    /// Callback setup function for Callbacks with member functions with explicit return type taking 2 arguments
    template <typename R, typename T, typename A0, typename A1> const Callback& make(R (T::*pmf)(A0, A1)) {
      typedef R (T::*pfunc_t)(A0, A1);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          return (ulong) (cast<T>(o)->*(typename _W::Functor(f).pmf))((A0) u[0], (A1) u[1]);
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    /// Callback setup function for Callbacks with const member functions with explicit return type taking 2 arguments
    template <typename R, typename T, typename A0, typename A1> const Callback& make(R (T::*pmf)(A0, A1) const) {
      typedef R (T::*pfunc_t)(A0, A1);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          return (ulong) (cast<T>(o)->*(typename _W::Functor(f).pmf))((A0) u[0], (A1) u[1]);
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    /// Callback setup function for Callbacks with const void member functions taking 2 arguments
    template <typename T, typename A0, typename A1> const Callback& make(void (T::*pmf)(A0, A1)) {
      typedef void (T::*pfunc_t)(A0, A1);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          (cast<T>(o)->*(typename _W::Functor(f).pmf))((A0) u[0], (A1) u[1]);
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    /// Callback setup function for Callbacks with const void member functions taking 2 arguments
    template <typename T, typename A0, typename A1> const Callback& make(void (T::*pmf)(A0, A1) const) {
      typedef void (T::*pfunc_t)(A0, A1);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          (cast<T>(o)->*(typename _W::Functor(f).pmf))((A0) u[0], (A1) u[1]);
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }

    /** Callback setup for callbacks with 3 arguments  */

    /// Callback setup function for Callbacks with member functions with explicit return type taking 3 arguments
    template <typename R, typename T, typename A0, typename A1, typename A2> const Callback& make(R (T::*pmf)(A0, A1, A2)) {
      typedef R (T::*pfunc_t)(A0, A1, A2);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          return (ulong) (cast<T>(o)->*(typename _W::Functor(f).pmf))((A0) u[0], (A1) u[1], (A2) u[2]);
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    /// Callback setup function for Callbacks with const member functions with explicit return type taking 3 arguments
    template <typename R, typename T, typename A0, typename A1, typename A2> const Callback& make(R (T::*pmf)(A0, A1, A2) const) {
      typedef R (T::*pfunc_t)(A0, A1, A2);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          return (ulong) (cast<T>(o)->*(typename _W::Functor(f).pmf))((A0) u[0], (A1) u[1], (A2) u[2]);
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    /// Callback setup function for Callbacks with const void member functions taking 3 arguments
    template <typename T, typename A0, typename A1, typename A2> const Callback& make(void (T::*pmf)(A0, A1, A2)) {
      typedef void (T::*pfunc_t)(A0, A1, A2);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          (cast<T>(o)->*(typename _W::Functor(f).pmf))((A0) u[0], (A1) u[1], (A2) u[2]);
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    /// Callback setup function for Callbacks with const void member functions taking 3 arguments
    template <typename T, typename A0, typename A1, typename A2> const Callback& make(void (T::*pmf)(A0, A1, A2) const) {
      typedef void (T::*pfunc_t)(A0, A1, A2);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          (cast<T>(o)->*(typename _W::Functor(f).pmf))((A0) u[0], (A1) u[1], (A2) u[2]);
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }

    template <typename T> static Callback make(void* p, T pmf) {
      return Callback(p).make(pmf);
    }

    template <typename P, typename R, typename T> static T* dyn_cast(P* p, R (T::*)()) {
      return dynamic_cast<T*>(p);
    }
    template <typename P, typename R, typename T> static const T* dyn_cast(const P* p, R (T::*)() const) {
      return dynamic_cast<const T*>(p);
    }

    template <typename P, typename R, typename T, typename A> static T* dyn_cast(P* p, R (T::*)(A)) {
      return dynamic_cast<T*>(p);
    }
    template <typename P, typename R, typename T, typename A> static const T* dyn_cast(const P* p, R (T::*)(A) const) {
      return dynamic_cast<const T*>(p);
    }
  };

  /// Definition of an actor on sequences of callbacks
  /**
   *  A callback sequence is a set of callbacks with identical signature, so that
   *  the can be called consecutively using the same arguments.
   *
   *  \author  M.Frank
   *  \date    01/03/2013
   *  \version 0.1
   *  \ingroup DD4HEP
   */
  struct CallbackSequence {
    typedef std::vector<Callback> Callbacks;
    enum Location { FRONT, END };
    Callbacks callbacks;
    /// Default constructor
    CallbackSequence() {
    }
    /// Copy constructor
    CallbackSequence(const CallbackSequence& c)
      : callbacks(c.callbacks) {
    }
    /// Assignment operator
    CallbackSequence& operator=(const CallbackSequence& c)  {
      if ( this != & c ) callbacks = c.callbacks;
      return *this;
    }

    //template <typename TYPE, typename R, typename OBJECT>
    //  CallbackSequence(const std::vector<TYPE*>& objects, R (TYPE::value_type::*pmf)())  {
    //}
    bool empty() const {
      return callbacks.empty();
    }
    /// Clear the sequence and remove all callbacks
    void clear() {
      callbacks.clear();
    }
    /// Generically Add a new callback to the sequence depending on the location arguments
    void add(const Callback& cb,Location where) {
      if ( where == CallbackSequence::FRONT )
        callbacks.insert(callbacks.begin(),cb);
      else
        callbacks.insert(callbacks.end(),cb);
    }
    /// Execution overload for callbacks with no arguments
    void operator()() const;
    /// Execution overload for callbacks with 1 argument
    template <typename A0> void operator()(A0 a0) const;
    /// Execution overload for callbacks with 2 arguments
    template <typename A0, typename A1> void operator()(A0 a0, A1 a1) const;
    /// Execution overload for callbacks with 3 arguments
    template <typename A0, typename A1, typename A2> void operator()(A0 a0, A1 a1, A2 a2) const;
    /// Check the compatibility of two typed objects. The test is the result of a dynamic_cast
    static void checkTypes(const std::type_info& typ1, const std::type_info& typ2, void* test);

    /** Callback setup for callbacks with no arguments  */
    /// Add a new callback to a member function with explicit return type and no arguments
    template <typename TYPE, typename R, typename OBJECT>
    void add(TYPE* pointer, R (OBJECT::*pmf)(),Location where=CallbackSequence::END) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf),where);
    }
    /// Add a new callback to a const member function with explicit return type and no arguments
    template <typename TYPE, typename R, typename OBJECT>
    void add(TYPE* pointer, R (OBJECT::*pmf)() const,Location where=CallbackSequence::END) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf),where);
    }
    /// Add a new callback to a void member function with no arguments
    template <typename TYPE, typename OBJECT>
    void add(TYPE* pointer, void (OBJECT::*pmf)(),Location where=CallbackSequence::END) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf),where);
    }
    /// Add a new callback to a const void member function and no arguments
    template <typename TYPE, typename OBJECT>
    void add(TYPE* pointer, void (OBJECT::*pmf)() const,Location where=CallbackSequence::END) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf),where);
    }

    /** Callback setup for callbacks with 1 argument  */
    /// Add a new callback to a member function with explicit return type and 1 argument
    template <typename TYPE, typename R, typename OBJECT, typename A>
    void add(TYPE* pointer, R (OBJECT::*pmf)(A),Location where=CallbackSequence::END) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf),where);
    }
    /// Add a new callback to a void member function and 1 argument
    template <typename TYPE, typename OBJECT, typename A>
    void add(TYPE* pointer, void (OBJECT::*pmf)(A),Location where=CallbackSequence::END) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf),where);
    }
    /// Add a new callback to a const member function with explicit return type and 1 argument
    template <typename TYPE, typename R, typename OBJECT, typename A>
    void add(TYPE* pointer, R (OBJECT::*pmf)(A) const,Location where=CallbackSequence::END) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf),where);
    }
    /// Add a new callback to a const void member function and 1 argument
    template <typename TYPE, typename OBJECT, typename A>
    void add(TYPE* pointer, void (OBJECT::*pmf)(A) const,Location where=CallbackSequence::END) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf),where);
    }


    /** Callback setup for callbacks with 2 arguments  */
    /// Add a new callback to a member function with explicit return type and 2 arguments
    template <typename TYPE, typename R, typename OBJECT, typename A1, typename A2>
    void add(TYPE* pointer, R (OBJECT::*pmf)(A1, A2),Location where=CallbackSequence::END) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf),where);
    }
    /// Add a new callback to a const member function with explicit return type and 2 arguments
    template <typename TYPE, typename R, typename OBJECT, typename A1, typename A2>
    void add(TYPE* pointer, R (OBJECT::*pmf)(A1, A2) const,Location where=CallbackSequence::END) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf),where);
    }
    /// Add a new callback to a void member function with 2 arguments
    template <typename TYPE, typename OBJECT, typename A1, typename A2>
    void add(TYPE* pointer, void (OBJECT::*pmf)(A1, A2),Location where=CallbackSequence::END) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf),where);
    }
    /// Add a new callback to a const void member function with 2 arguments
    template <typename TYPE, typename OBJECT, typename A1, typename A2>
    void add(TYPE* pointer, void (OBJECT::*pmf)(A1, A2) const,Location where=CallbackSequence::END) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf),where);
    }
  };

  /// Execution overload for callbacks with no arguments
  inline void CallbackSequence::operator()() const {
    if (!callbacks.empty()) {
      const void* args[1] = { 0 };
      for (Callbacks::const_iterator i = callbacks.begin(); i != callbacks.end(); ++i)
        (*i).execute(args);
    }
  }
  /// Execution overload for callbacks with 1 argument
  template <typename A0> inline
  void CallbackSequence::operator()(A0 a0) const {
    if (!callbacks.empty()) {
      const void* args[1] = { a0 };
      for (Callbacks::const_iterator i = callbacks.begin(); i != callbacks.end(); ++i)
        (*i).execute(args);
    }
  }
  /// Execution overload for callbacks with 2 arguments
  template <typename A0, typename A1> inline
  void CallbackSequence::operator()(A0 a0, A1 a1) const {
    if (!callbacks.empty()) {
      const void* args[2] = { a0, a1 };
      for (Callbacks::const_iterator i = callbacks.begin(); i != callbacks.end(); ++i)
        (*i).execute(args);
    }
  }
  /// Execution overload for callbacks with 3 arguments
  template <typename A0, typename A1, typename A2> inline
  void CallbackSequence::operator()(A0 a0, A1 a1, A2 a2) const {
    if (!callbacks.empty()) {
      const void* args[3] = { a0, a1, a2 };
      for (Callbacks::const_iterator i = callbacks.begin(); i != callbacks.end(); ++i)
        (*i).execute(args);
    }
  }

}       // End namespace dd4hep
#endif  // DD4HEP_DDCORE_CALLBACK_H
