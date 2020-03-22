#pragma once

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/ivariable.h"
#include "moja/flint/configuration/transform.h"

#include <string>

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API UncertaintyField {
  public:
   enum class FieldType { Manual = 0, Normal = 1, Triangular = 2 };

   virtual ~UncertaintyField() = default;

  protected:
   explicit UncertaintyField(FieldType type) : type(type) {}

  public:
   std::string key;
   FieldType type;
};

class CONFIGURATION_API UncertaintyFieldTriangular : public UncertaintyField {
  public:
   UncertaintyFieldTriangular() : UncertaintyField(FieldType::Triangular) {}
   int seed{};
   double min{};
   double peak{};
   double max{};
};

class CONFIGURATION_API UncertaintyFieldNormal : public UncertaintyField {
  public:
   UncertaintyFieldNormal() : UncertaintyField(FieldType::Normal) {}
   int seed{};
   double mean{};
   double std_dev{};
};

class CONFIGURATION_API UncertaintyFieldManual : public UncertaintyField {
  public:
   UncertaintyFieldManual() : UncertaintyField(FieldType::Manual) {}
   std::vector<double> distribution;
};

class CONFIGURATION_API Replacement {
  public:
   Replacement(const DynamicObject& query) : query_(query) {}
   std::vector<std::shared_ptr<UncertaintyField>>& fields();
   const std::vector<std::shared_ptr<UncertaintyField>>& fields() const;
   const DynamicObject& query() const;
  private:
   DynamicObject query_;
   std::vector<std::shared_ptr<UncertaintyField>> fields_;
};
inline std::vector<std::shared_ptr<UncertaintyField>>& Replacement::fields() { return fields_; }
inline const std::vector<std::shared_ptr<UncertaintyField>>& Replacement::fields() const { return fields_; }
inline const DynamicObject& Replacement::query() const { return query_; }

class CONFIGURATION_API UncertaintyVariable {
  public:
   explicit UncertaintyVariable(const std::string& variable) : variable_(variable) {}
   virtual ~UncertaintyVariable() = default;
   const std::string& variable() const { return variable_; }
   std::vector<Replacement>& replacements();
   const std::vector<Replacement>& replacements() const;

  private:
   std::string variable_;
   std::vector<Replacement> replacements_;
};
inline std::vector<Replacement>& UncertaintyVariable::replacements() { return replacements_; }
inline const std::vector<Replacement>& UncertaintyVariable::replacements() const { return replacements_; }

class CONFIGURATION_API Uncertainty {
  public:
   Uncertainty() : enabled_(false), iterations_(0) {}
   explicit Uncertainty(bool enabled, int iterations) : enabled_(enabled), iterations_(iterations) {}
   virtual ~Uncertainty() = default;
   bool enabled() const;
   int iterations() const;
   std::vector<UncertaintyVariable>& variables();
   const std::vector<UncertaintyVariable>& variables() const;
   void set_iterations(int iterations);
   void set_enabled(bool enabled);

  private:
   bool enabled_;
   int iterations_;
   std::vector<UncertaintyVariable> variables_;
};

inline bool Uncertainty::enabled() const { return enabled_; }
inline int Uncertainty::iterations() const { return iterations_; }

inline std::vector<UncertaintyVariable>& Uncertainty::variables() { return variables_; }
inline const std::vector<UncertaintyVariable>& Uncertainty::variables() const { return variables_; }

inline void Uncertainty::set_iterations(int iterations) { iterations_ = iterations; }

inline void Uncertainty::set_enabled(bool enabled) { enabled_ = enabled; }

}  // namespace configuration
}  // namespace flint
}  // namespace moja
