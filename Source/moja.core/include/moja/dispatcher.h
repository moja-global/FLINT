#ifndef MOJA_DISPATCHER_H_
#define MOJA_DISPATCHER_H_

#include "moja/_core_exports.h"

#include <functional>
#include <map>
#include <typeindex>
#include <vector>

namespace moja {

template <typename... Args>
class Signal {
  public:
   /** <summary>    Default constructor. </summary> */
   Signal() = default;

   /**
    * <summary>    Copy creates new signal. </summary>
    */
   Signal(Signal const& other) {}

   /**
    * <summary>    Connects a function to the signal. </summary>
    *
    * <param name="slot">  The function to connect. </param>
    */
   void connect(std::function<void(Args...)> const& slot) const { _slots.push_back(slot); }

   /** <summary>    Disconnects all previously connected functions. </summary> */
   void disconnectAll() const { _slots.clear(); }

   /**
    * <summary>    Calls all connected functions. </summary>
    *
    * <param name="p"> The arguments to pass to the connected functions. </param>
    */
   void emit(Args... p) {
      for (auto& slot : _slots) {
         slot(p...);
      }
   }

   /**
    * <summary>    Assignment creates new Signal. </summary>
    *
    * <param name="other"> The signal to assign. </param>
    *
    * <returns>    A shallow copy of this object. </returns>
    */
   Signal& operator=(Signal const& other) {
      disconnectAll();
      return *this;
   }

  private:
   mutable std::vector<std::function<void(Args...)>> _slots;
};

/** <summary>    Encapsulates a value and provides change notification. </summary> */
template <typename T>
class Property {
  public:
   using value_type = T;

   Property(Property<T> const& toCopy) = delete;
   Property(Property<T>&& toCopy) = delete;

   /**
    * <summary>
    * Properties for built-in types are automatically initialized to 0. See template
    * specialisations at the bottom of this file.
    * </summary>
    */
   Property() : _connection(nullptr) {}

   explicit Property(T const& val) : _connection(nullptr), _value(val) {}
   explicit Property(T&& val) : _connection(nullptr), _value(std::move(val)) {}

   /**
    * <summary>
    * Returns a Signal which is fired when the internal value has been changed. The new value is
    * passed as parameter.
    * </summary>
    *
    * <returns>    A reference to the onChange Signal. </returns>
    */
   Signal<T> const& onChange() const { return _onChange; }

   /**
    * <summary>
    * Sets the Property to a new value. before_change() and onChange() will be emitted.
    * </summary>
    *
    * <param name="value"> The new value. </param>
    */
   void set(T const& value) {
      if (value != _value) {
         _value = value;
         _onChange.emit(_value);
      }
   }

   /**
    * <summary>    Sets the Property to a new value. onChange() will not be emitted. </summary>
    *
    * <param name="value"> The new value. </param>
    */
   void setWithNoEmit(T const& value) { _value = value; }

   /** <summary>    Emits onChange() even if the value did not change. </summary> */
   void touch() { _onChange.emit(_value); }

   /**
    * <summary>    Returns the internal value. </summary>
    */
   T const& value() const { return _value; }

   /**
    * <summary>
    * Connects two Properties to each other. If the source's value is changed, this value will be
    * changed as well.
    * </summary>
    *
    * <param name="source">    The Property to synchronize values with. </param>
    */
   void connectFrom(Property<T> const& source) {
      _connection = &source;
      source.onChange().connect([this](T const& value) {
         set(value);
         return true;
      });

      set(source.value());
   }

   /**
    * <summary>
    * If there are any Properties connected to this Property, they won't be notified of any further
    * changes.
    * </summary>
    */
   void disconnectAuditors() { _onChange.disconnectAll(); }

   /**
    * <summary>    Assigns the value of another Property. </summary>
    *
    * <param name="rhs">   The Property to copy the value from. </param>
    *
    * <returns>    A shallow copy of this object. </returns>
    */
   Property<T>& operator=(Property<T> const& rhs) {
      set(rhs._value);
      return *this;
   }

   /**
    * <summary>    Assigns a new value to this Property. </summary>
    *
    * <param name="rhs">   The new value. </param>
    *
    * <returns>    A shallow copy of this object. </returns>
    */
   Property<T>& operator=(T const& rhs) {
      set(rhs);
      return *this;
   }

   /**
    * <summary>    Casts the property to its native type. </summary>
    *
    * <returns>    The property value. </returns>
    */
   operator T() const { return _value; }

   /**
    * <summary>    Compares the values of two Properties. </summary>
    *
    * <param name="rhs">   The property to compare. </param>
    *
    * <returns>    True if the parameters are considered equivalent. </returns>
    */
   bool operator==(Property<T> const& rhs) const { return Property<T>::value() == rhs.value(); }

   /**
    * <summary>    Compares the values of two Properties for inequality. </summary>
    *
    * <param name="rhs">   The property to compare. </param>
    *
    * <returns>    True if the parameters are not considered equivalent. </returns>
    */
   bool operator!=(Property<T> const& rhs) const { return Property<T>::value() != rhs.value(); }

   /**
    * <summary>    Adds to the value of the property. </summary>
    *
    * <param name="rhs">   The value to add. </param>
    *
    * <returns>    The result of the operation. </returns>
    */
   Property<T>& operator+=(T const& rhs) {
      set(_value + rhs);
      return *this;
   }

   /**
    * <summary>    Compares the value of the Property to another value. </summary>
    *
    * <param name="rhs">   The value to compare. </param>
    *
    * <returns>    True if the parameters are considered equivalent. </returns>
    */
   bool operator==(T const& rhs) const { return Property<T>::value() == rhs; }

   /**
    * <summary>    Compares the value of the Property to another value for inequality. </summary>
    *
    * <param name="rhs">   The value to compare. </param>
    *
    * <returns>    True if the parameters are not considered equivalent. </returns>
    */
   bool operator!=(T const& rhs) const { return Property<T>::value() != rhs; }

   /**
    * <summary>    Returns the value of this Property. </summary>
    */
   T const& operator()() const { return Property<T>::value(); }

  private:
   Signal<T> _onChange;
   Property<T> const* _connection;
   T _value;
};

/**
 * <summary>    Multiplies a value by a property wrapping a value of the same type. </summary>
 */
template <typename T>
inline T operator*(const T& other, const Property<T>& p) {
   return other * p.value();
}

/**
 * <summary>    Divides a value by a property wrapping a value of the same type. </summary>
 */
template <typename T>
inline T operator/(const T& other, const Property<T>& p) {
   return other / p.value();
}

/**
 * <summary>    Adds a property's value to a value of the same type. </summary>
 */
template <typename T>
inline T operator+(const T& other, const Property<T>& p) {
   return other + p.value();
}

/**
 * <summary>    Subtracts a property's value from a value of the same type. </summary>
 */
template <typename T>
inline T operator-(const T& other, const Property<T>& p) {
   return other - p.value();
}

/**
 * <summary>    Adds a property's value in-place to a value of the same type. </summary>
 */
template <typename T>
inline T operator+=(T& other, const Property<T>& p) {
   return other += p.value();
}

// Specializations for built-in default constructors.
template <>
inline Property<double>::Property() : _connection(nullptr), _value(0.0) {}

template <>
inline Property<float>::Property() : _connection(nullptr), _value(0.f) {}

template <>
inline Property<short>::Property() : _connection(nullptr), _value(0) {}

template <>
inline Property<int>::Property() : _connection(nullptr), _value(0) {}

template <>
inline Property<char>::Property() : _connection(nullptr), _value(0) {}

template <>
inline Property<unsigned>::Property() : _connection(nullptr), _value(0) {}

template <>
inline Property<bool>::Property() : _connection(nullptr), _value(false) {}

// Stream operators.
template <typename T>
std::ostream& operator<<(std::ostream& out_stream, Property<T> const& val) {
   out_stream << val.get();
   return out_stream;
}

template <typename T>
std::istream& operator>>(std::istream& in_stream, Property<T>& val) {
   T tmp;
   in_stream >> tmp;
   val.set(tmp);
   return in_stream;
}

/** <summary>    http://blog.coldflake.com/posts/C++-delegates-on-steroids/ </summary> */
class BaseDelegate {
  public:
   virtual ~BaseDelegate() = default;
   virtual std::type_index type() const = 0;

  protected:
   BaseDelegate() = default;
};

template <typename R, typename... Params>
class Invokable : public BaseDelegate {
  public:
   virtual ~Invokable() = default;
   virtual R invoke(Params... args) const = 0;
   virtual R operator()(Params... args) const = 0;
};

template <typename T>
class Delegate;

/**
 * <summary>    Specialization for member functions. </summary>
 *
 * <typeparam name="T">         Class-type of the object whose member function to call. </typeparam>
 * <typeparam name="R">         Return type of the function that gets captured. </typeparam>
 * <typeparam name="Params">    Variadic template list for possible arguments of the captured
 *                              function. </typeparam>
 */
template <typename T, typename R, typename... Params>
class Delegate<R (T::*)(Params...)> : public Invokable<R, Params...> {
  public:
   using FuncType = R (T::*)(Params...);

   Delegate(FuncType func, T& callee) : _callee(callee), _func(func) {}
   ~Delegate() = default;

   std::type_index type() const override { return typeid(R(Params...)); }

   virtual R invoke(Params... args) const override { return (_callee.*_func)(args...); }

   R operator()(Params... args) const override { return (_callee.*_func)(args...); }

   bool operator==(const Delegate& other) const { return (&_callee == &other._callee) && (_func == other._func); }

   bool operator!=(const Delegate& other) const { return !((*this) == other); }

  private:
   T& _callee;
   FuncType _func;
};

/**
 * <summary>    Specialization for const member functions. </summary>
 *
 * <typeparam name="T">         Class-type of the object whose member function to call. </typeparam>
 * <typeparam name="R">         Return type of the function that gets captured. </typeparam>
 * <typeparam name="Params">    Variadic template list for possible arguments of the captured
 *                              function. </typeparam>
 */
template <typename T, typename R, typename... Params>
class Delegate<R (T::*)(Params...) const> : public Invokable<R, Params...> {
  public:
   using FuncType = R (T::*)(Params...) const;

   Delegate(FuncType func, const T& callee) : _callee(callee), _func(func) {}
   ~Delegate() = default;

   std::type_index type() const override { return typeid(R(Params...)); }

   virtual R invoke(Params... args) const override { return (_callee.*_func)(args...); }

   R operator()(Params... args) const override { return (_callee.*_func)(args...); }

   bool operator==(const Delegate& other) const { return (&_callee == &other._callee) && (_func == other._func); }

   bool operator!=(const Delegate& other) const { return !(*this == other); }

  private:
   const T& _callee;
   FuncType _func;
};

/**
 * <summary>    Specialization for free functions. </summary>
 *
 * <typeparam name="R">         Return type of the function that gets captured. </typeparam>
 * <typeparam name="Params">    Variadic template list for possible arguments of the captured
 *                              function. </typeparam>
 */
template <typename R, typename... Params>
class Delegate<R (*)(Params...)> : public Invokable<R, Params...> {
  public:
   using FuncType = R (*)(Params...);

   explicit Delegate(FuncType func) : _func(func) {}
   ~Delegate() = default;

   std::type_index type() const override { return typeid(R(Params...)); }

   virtual R invoke(Params... args) const override { return (*_func)(args...); }

   R operator()(Params... args) const override { return (*_func)(args...); }

   bool operator==(const Delegate& other) const { return _func == other._func; }
   bool operator!=(const Delegate& other) const { return !((*this) == other); }

  private:
   FuncType _func;
};

/**
 * <summary>    Function to deduce template parameters from call-context. </summary>
 */
template <typename F, typename T>
std::unique_ptr<Delegate<F>> makeDelegate(F func, T& obj) {
   return std::make_unique<Delegate<F>>(func, obj);
}

template <typename F>
std::unique_ptr<Delegate<F>> makeDelegate(F func) {
   return std::make_unique<Delegate<F>>(func);
}

/**
 * <summary>    Class only defines the sendSignal() and connectSignal() methods.
 *              This class is only to be inherited from, not to be used directly. </summary>
 */
class DispatcherObject {
  public:
   // Generic container of listeners for any type of function.
   typedef std::multimap<std::type_index, std::unique_ptr<BaseDelegate>> Listeners;
   struct Event {
      ~Event() = default;
      Listeners listeners;
   };

   DispatcherObject() = default;
   virtual ~DispatcherObject() = default;

   void CORE_API clearSignals() { events.clear(); }

   /**
    * <summary>    Send a signal using the dispatcher module.
    *              The VariableKiosk of this SimulationObject is used as the sender of
    *              the signal. Additional arguments to the sendSignal() method are
    *              passed to dispatcher.send() </summary>
    *
    * <typeparam name="Args">  Type of the arguments. </typeparam>
    * <param name="signal">    The signal. </param>
    * <param name="args">      Additional arguments to pass to subscribers. </param>
    */
   template <class... Args>
   void sendSignal(short signal, Args... args);

   template <class... Args>
   void sendSignalWithPostSendOnSuccess(short signal, short postSignal, Args... args);

   /**
    * <summary>    Connect the handler to the signal using the dispatcher module.
    *              The handler will only react on signals that have the SimulationObjects
    *              VariableKiosk as sender. This ensure that different PCSE model instances
    *              in the same runtime environment will not react to each others signals. </summary>
    */
   template <typename F, typename T>
   void connectSignal(short signal, F func, T& obj);

   template <typename F>
   void connectSignal(short signal, F func);

  private:
   template <typename F, typename T>
   static void addListener(Listeners& listeners, F func, T& obj);

   template <typename F>
   static void addListener(Listeners& listeners, F func);

   template <typename... Args>
   void callListeners(const Listeners& listeners, Args... args);

   template <typename... Args>
   void callListeners(const Listeners& listeners, short originalSignal, const Listeners& postListeners, Args... args);

   std::map<short, Event> events;
};

template <class... Args>
void DispatcherObject::sendSignal(short signal, Args... args) {
   // MOJA_LOG_DEBUG << (boost::format("Sent signal: %s") % signal).str();
   callListeners(events[signal].listeners, std::forward<Args>(args)...);
}

template <class... Args>
void DispatcherObject::sendSignalWithPostSendOnSuccess(short signal, short postSignal, Args... args) {
   // MOJA_LOG_DEBUG << (boost::format("Sent signal: %s") % signal).str();
   callListeners(events[signal].listeners, signal, events[postSignal].listeners, std::forward<Args>(args)...);
}

template <typename F, typename T>
void DispatcherObject::connectSignal(short signal, F func, T& obj) {
   addListener<F, T>(events[signal].listeners, func, obj);
   // std::type_index index(typeid(F));
   // MOJA_LOG_DEBUG << (boost::format("Connected handler '%s' to signal '%s'.") % index.name() % signal).str();
}

template <typename F>
void DispatcherObject::connectSignal(short signal, F func) {
   addListener<F>(events[signal].listeners, func);
   // std::type_index index(typeid(F));
   // MOJA_LOG_DEBUG << (boost::format("Connected handler '%s' to signal '%s'.") % index.name() % signal).str();
}

template <typename F, typename T>
void DispatcherObject::addListener(Listeners& listeners, F func, T& obj) {
   auto d = moja::makeDelegate<F, T>(func, obj);
   const auto& index = d->type();
   listeners.insert(Listeners::value_type(index, std::move(d)));
}

template <typename F>
void DispatcherObject::addListener(Listeners& listeners, F func) {
   auto d = moja::makeDelegate<F>(func);
   const auto& index = d->type();
   listeners.insert(Listeners::value_type(index, std::move(d)));
}

template <typename... Args>
void DispatcherObject::callListeners(const Listeners& listeners, Args... args) {
   using Func = void(typename std::remove_reference<Args>::type...);
   std::type_index index(typeid(Func));

   // MOJA_LOG_DEBUG << (boost::format("callListeners '%s'") % index.name()).str();
   auto i = listeners.lower_bound(index);
   auto j = listeners.upper_bound(index);
   for (; i != j; ++i) {
      // std::string del_nm = (*i).second->type().name();
      auto d = static_cast<Invokable<void, Args...>*>((*i).second.get());
      d->invoke(args...);
   }
}

template <typename... Args>
void DispatcherObject::callListeners(const Listeners& listeners, short originalSignal, const Listeners& postListeners,
                                     Args... args) {
   using Func = void(typename std::remove_reference<Args>::type...);
   std::type_index index(typeid(Func));

   // MOJA_LOG_DEBUG << (boost::format("callListeners '%s'") % index.name()).str();
   auto i = listeners.lower_bound(index);
   auto j = listeners.upper_bound(index);
   for (; i != j; ++i) {
      // std::string del_nm = (*i).second->type().name();
      auto d = static_cast<Invokable<void, Args...>*>((*i).second.get());
      d->invoke(args...);
      callListeners(postListeners, originalSignal);
   }
}

}  // namespace moja
#endif  // MOJA_DISPATCHER_H_