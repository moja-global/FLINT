#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ivariable.h"

namespace moja {
namespace flint {

class ILandUnitController;

class FLINT_API UncertaintyField {
  public:
   virtual ~UncertaintyField() = default;

   enum class field_type { manual = 0, normal = 1, triangular = 2 };
   const std::vector<double>& distribution() const;
   virtual void build_distribution(int iterations) = 0;

  protected:
   explicit UncertaintyField(field_type type) : type(type) {}
   std::vector<double> distribution_;

  public:
   std::string key;
   field_type type;
};

class FLINT_API UncertaintyFieldTriangular : public UncertaintyField {
  public:
   UncertaintyFieldTriangular() : UncertaintyField(field_type::triangular) {}
   void build_distribution(int iterations) override;
   int seed{};
   double min{};
   double peak{};
   double max{};
};

class FLINT_API UncertaintyFieldNormal : public UncertaintyField {
  public:
   UncertaintyFieldNormal() : UncertaintyField(field_type::normal) {}
   void build_distribution(int iterations) override;
   int seed{};
   double mean{};
   double std_dev{};
};

class FLINT_API UncertaintyFieldManual : public UncertaintyField {
  public:
   UncertaintyFieldManual() : UncertaintyField(field_type::manual) {}
   void build_distribution(int iterations) override {}
   void set_distribution(const std::vector<double>& distribution);
};
class FLINT_API Replacement {
public:
   explicit Replacement(const DynamicObject& query) : query_(query) {}
   std::vector<std::shared_ptr<UncertaintyField>>& fields();
   const std::vector<std::shared_ptr<UncertaintyField>>& fields() const;
   const DynamicObject& query() const { return query_; }

  private:
   DynamicObject query_;
   std::vector<std::shared_ptr<UncertaintyField>> fields_;
};

inline std::vector<std::shared_ptr<UncertaintyField>>& Replacement::fields() { return fields_; }
inline const std::vector<std::shared_ptr<UncertaintyField>>& Replacement::fields() const { return fields_; }


class FLINT_API UncertaintyVariable : public IVariable {
  public:
   UncertaintyVariable(const ILandUnitController& landUnitController, const std::shared_ptr<IVariable> variable)
       : land_unit_controller_(&landUnitController), variable_(variable) {}

   ~UncertaintyVariable() = default;

   const VariableInfo& info() const override { return variable_->info(); }
   const DynamicVar& value() const override;
   void set_value(DynamicVar) override;
   void reset_value() override;
   bool isExternal() const override { return false; }
   bool isFlintData() const override { return false; }
   void controllerChanged(const ILandUnitController& controller) override;
   std::vector<Replacement>& replacements();
   const std::vector<Replacement>& replacements() const;
   void set_name(const std::string& name);

private:
   const ILandUnitController* land_unit_controller_;
   std::shared_ptr<IVariable> variable_;
   std::string name_;
   std::vector<Replacement> replacements_;
   mutable DynamicVar value_;
};
inline std::vector<Replacement>& UncertaintyVariable::replacements() { return replacements_; }
inline const std::vector<Replacement>& UncertaintyVariable::replacements() const { return replacements_; }


class FLINT_API Uncertainty {
  public:
   int iteration() const { return iteration_; }
   int iterations() const { return iterations_; }
   void increment_iteration() { iteration_++; }
   void set_iterations(int iterations) { iterations_ = iterations; }
   bool enabled() const { return enabled_; }
   void set_enabled(bool enabled) { enabled_ = enabled; }

private:
   int iteration_ = 0;
   int iterations_ = 0;
   bool enabled_ = false;
};
}  // namespace flint
}  // namespace moja
