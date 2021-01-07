#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ipool.h"

#include <vector>

namespace moja::flint {

class FLINT_API PoolCohort : public IPool {
  public:
   PoolCohort() = delete;
   PoolCohort(std::vector<double>& pools, const std::string& name, int idx, double value);
   PoolCohort(std::vector<double>& pools, const std::string& name, const std::string& description,
              const std::string& units, double scale, int order, int idx, double value);
   ~PoolCohort() = default;

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

   int idx_;
   double init_value_;
   double& value_;
};

inline double PoolCohort::value() const { return value_; }

inline void PoolCohort::set_value(double value) { value_ = value; }

inline int PoolCohort::idx() const { return idx_; }

}  // namespace moja::flint
