#ifndef MOJA_FLINT_POOLSIMPLECACHE_H_
#define MOJA_FLINT_POOLSIMPLECACHE_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ipool.h"

#include <string>
#include <vector>

namespace moja {
namespace flint {

class FLINT_API PoolSimpleCache : public IPool {
  public:
   PoolSimpleCache() = delete;
   PoolSimpleCache(std::vector<double>& pools, const std::string& name, int idx, double value);
   PoolSimpleCache(std::vector<double>& pools, const std::string& name, const std::string& description,
                   const std::string& units, double scale, int order, int idx, double value);
   ~PoolSimpleCache() = default;

   const std::string& name() const override;
   const std::string& description() const override;
   const std::string& units() const override;
   double scale() const override;
   int order() const override;
   double initValue() const override;
   int idx() const override;

   double value() const override;
   void set_value(double value) override;

   void init() override;

  protected:
   PoolMetaData _metadata;

   int _idx;
   double _initValue;
   bool _initialised = false;

   std::vector<double>* _pools;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_POOLSIMPLECACHE_H_