#include "moja/flint/uncertaintyvariable.h"

#include "moja/flint/itransform.h"

#include <moja/exception.h>

#include <fmt/format.h>

#include <array>
#include <random>
#include "moja/flint/ilandunitcontroller.h"

namespace moja {
namespace flint {

std::piecewise_linear_distribution<double> triangular_distribution(double min, double peak, double max) {
   std::array<double, 3> i{min, peak, max};
   std::array<double, 3> w{0, 1, 0};
   return std::piecewise_linear_distribution<double>{i.begin(), i.end(), w.begin()};
}

const std::vector<double>& UncertaintyField::distribution() { return distribution_; }

void UncertaintyFieldTriangular::build_distribution(int iterations) {
   std::random_device rd;
   std::mt19937 gen(seed > 0 ? seed : rd());
   auto dist = triangular_distribution(min, peak, max);
   distribution_.reserve(iterations);
   for (int i = 0; i < iterations; ++i) {
      double num = dist(gen);
      distribution_.emplace_back(num);
   }
}

void UncertaintyFieldNormal::build_distribution(int iterations) {
   std::random_device rd;
   std::mt19937 gen(seed > 0 ? seed : rd());
   std::normal_distribution<double> dist(mean, std_dev);

   for (int i = 0; i < iterations; ++i) {
      double num = dist(gen);
      distribution_.emplace_back(num);
   }
}

void UncertaintyFieldManual::set_distribution(const std::vector<double>& distribution) { distribution_ = distribution; }

const DynamicVar& UncertaintyVariable::value() const {
   value_ = nullptr;
   value_ = variable_->value();
   const int uncertainty_iteration = land_unit_controller_->uncertainty()->iteration();
   for (const auto& f : fields_) {
      auto v = f->distribution()[uncertainty_iteration];
   }
   // variable_->set_field()
   return value_;
}

void UncertaintyVariable::set_value(DynamicVar) {
   const auto msg = fmt::format("Can't set an Uncertainty Variable: {}}.", variable_->info().name);
   throw ApplicationException(msg);
}

void UncertaintyVariable::reset_value() { variable_->reset_value(); }

void UncertaintyVariable::controllerChanged(const ILandUnitController& controller) {
   land_unit_controller_ = &controller;
   variable_->controllerChanged(controller);
}

}  // namespace flint
}  // namespace moja
