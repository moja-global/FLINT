#pragma once

#if 0
#include <Poco/Dynamic/Struct.h>
#include <Poco/Dynamic/Var.h>

namespace moja {
using DynamicVar = Poco::Dynamic::Var;
using DynamicVector = Poco::Dynamic::Vector;
using DynamicObject = Poco::DynamicStruct;
}  // namespace moja
#endif

#include <any>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace moja {
class DynamicVar;

template <typename K>
class Struct {
  public:
   typedef typename std::map<K, DynamicVar> Data;
   typedef typename std::set<K> NameSet;
   typedef typename Data::iterator Iterator;
   typedef typename Data::const_iterator ConstIterator;
   typedef typename Struct<K>::Data::value_type ValueType;
   typedef typename Struct<K>::Data::size_type SizeType;
   typedef typename std::pair<typename Struct<K>::Iterator, bool> InsRetVal;

   Struct() : data_() {}

   Struct(const Data& val) : data_(val) {}

   template <typename T>
   explicit Struct(const std::map<K, T>& val) {
      typedef typename std::map<K, T>::const_iterator MapConstIterator;
      MapConstIterator it = val.begin();
      MapConstIterator end = val.end();
      for (; it != end; ++it) data_.insert(ValueType(it->first, Var(it->second)));
   }

   virtual ~Struct() = default;

   DynamicVar& operator[](const K& name) { return data_[name]; }

   const DynamicVar& operator[](const K& name) const {
      ConstIterator it = find(name);
      if (it == end()) throw std::runtime_error("name not found: " + name);
      return it->second;
   }

   [[nodiscard]] bool contains(const K& name) const { return find(name) != end(); }

   Iterator find(const K& name) { return data_.find(name); }

   [[nodiscard]] ConstIterator find(const K& name) const { return data_.find(name); }

   Iterator end() { return data_.end(); }

   [[nodiscard]] ConstIterator end() const { return data_.end(); }

   Iterator begin() { return data_.begin(); }

   [[nodiscard]] ConstIterator begin() const { return data_.begin(); }

   template <typename T>
   InsRetVal insert(const K& key, const T& value) {
      return data_.emplace(key, value);
   }

   InsRetVal insert(const ValueType& aPair) { return data_.emplace(aPair); }

   SizeType erase(const K& key) { return data_.erase(key); }

   void erase(Iterator& it) { data_.erase(it); }

   [[nodiscard]] bool empty() const { return data_.empty(); }

   void clear() { data_.clear(); }

   [[nodiscard]] SizeType size() const { return data_.size(); }

   [[nodiscard]] NameSet members() const {
      NameSet keys;
      ConstIterator it = begin();
      ConstIterator itEnd = end();
      for (; it != itEnd; ++it) keys.insert(it->first);
      return keys;
   }

  private:
   Data data_;
};

class DynamicVar {
  public:
   DynamicVar() = default;
   template <typename T>
   DynamicVar(const T& val) : value_(val) {}
   template <typename T>
   operator T() const {
      if (!value_.has_value()) throw std::runtime_error("Can not convert empty value.");
      return extract<T>();
   }
   template <typename T>
   const T& extract() const {
      if (value_.has_value()) {
         return std::any_cast<const T&>(value_);
      }
      throw std::runtime_error("Can not extract empty value.");
   }
   template <typename T>
   DynamicVar& operator=(const T& other)
   /// Assignment operator for assigning POD to DynamicVar
   {
      value_ = other;
      return *this;
   }

   [[nodiscard]] bool isEmpty() const { return !value_.has_value(); }
   [[nodiscard]] const type_info& type() const { return value_.type(); }

   DynamicVar& operator[](const std::string& name);
   const DynamicVar& operator[](const std::string& name) const;
   bool isObject() const { return value_.type() == typeid(Struct<std::string>); }
   bool isVector() const { return value_.type() == typeid(std::vector<DynamicVar>); }
   bool isStruct() const { return value_.type() == typeid(Struct<DynamicVar>); }

  private:
   template <typename T, typename N>
   static DynamicVar& structIndexOperator(T* struct_pointer, N n);
   DynamicVar& get_at(const std::string& name);

   std::any value_;
};

inline DynamicVar& DynamicVar::operator[](const std::string& name) { return get_at(name); }

inline const DynamicVar& DynamicVar::operator[](const std::string& name) const {
   return const_cast<DynamicVar*>(this)->get_at(name);
}

inline DynamicVar& DynamicVar::get_at(const std::string& name) {
   return structIndexOperator(std::any_cast<Struct<std::string>*>(value_), name);
}

template <typename T, typename N>
DynamicVar& DynamicVar::structIndexOperator(T* struct_pointer, N n) {
   return struct_pointer->operator[](n);
}

using DynamicObject = Struct<std::string>;
using DynamicVector = std::vector<DynamicVar>;

}  // namespace moja
