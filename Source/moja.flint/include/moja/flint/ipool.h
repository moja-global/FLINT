#ifndef MOJA_FLINT_IPOOL_H_
#define MOJA_FLINT_IPOOL_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/poolmetadata.h"

#include <string>

namespace moja {
namespace flint {

class FLINT_API IPool {
  public:
   IPool() = default;
   virtual ~IPool() = default;

   virtual const std::string& name() const = 0;
   virtual const std::string& description() const = 0;
   virtual const std::string& units() const = 0;
   virtual double scale() const = 0;
   virtual int order() const = 0;
   virtual double initValue() const = 0;

   virtual int idx() const = 0;

   virtual double value() const = 0;
   virtual void set_value(double value) = 0;
   virtual void init() = 0;

   virtual const PoolMetaData& metadata() { return _metadata; };

  protected:
   PoolMetaData _metadata;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_IPOOL_H_
