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
   const std::vector<double>& distribution();
   virtual void build_distribution(int iterations) = 0;

  protected:
   explicit UncertaintyField(field_type type) : type(type) {}
   std::vector<double> distribution_;

  public:
   std::string selector;
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
   std::vector<std::shared_ptr<UncertaintyField>>& fields();
   const std::vector<std::shared_ptr<UncertaintyField>>& fields() const;

  private:
   const ILandUnitController* land_unit_controller_;
   std::shared_ptr<IVariable> variable_;
   DynamicObject selector_;
   std::vector<std::shared_ptr<UncertaintyField>> fields_;
   mutable DynamicVar value_;
};

inline std::vector<std::shared_ptr<UncertaintyField>>& UncertaintyVariable::fields() { return fields_; }
inline const std::vector<std::shared_ptr<UncertaintyField>>& UncertaintyVariable::fields() const { return fields_; }

class FLINT_API Uncertainty {
  public:
   int iteration() const { return iteration_; }

  private:
   int iteration_ = 0;
   int iterations_ = 0;
};
}  // namespace flint
}  // namespace moja
