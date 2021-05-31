#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/poolmetadata.h"

#include <string>
#include <vector>

namespace moja::flint {

class FLINT_API IPool {
  public:
   IPool() = default;
   virtual ~IPool() = default;

   virtual const std::string& name() const = 0;
   virtual const std::string& description() const = 0;
   virtual const std::string& units() const = 0;
   virtual double scale() const = 0;
   virtual int order() const = 0;
   virtual std::optional<double> initValue() const = 0;

   virtual int idx() const = 0;

   virtual double value() const = 0;
   virtual void set_value(double value) = 0;
   virtual void init() = 0;

   virtual const IPool* parent() const = 0;
   virtual const std::vector<const IPool*>& children() const = 0;
   virtual void add_child(IPool* pool) = 0;

   virtual const PoolMetaData& metadata() const { return _metadata; }

  protected:
   PoolMetaData _metadata;
};

}  // namespace moja::flint
