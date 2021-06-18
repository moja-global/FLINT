#pragma once

#include "moja/flint/_flint_exports.h"

#include <string>
#include <utility>
#include <stdexcept>

namespace moja::flint {

class FLINT_API flint_exception : public std::runtime_error {
  public:
   flint_exception(const std::string& message) : std::runtime_error(message) {}
   flint_exception(const char* message) : std::runtime_error(message) {}
};

class FLINT_API local_domain_error : public flint_exception {
  public:
   local_domain_error(const std::string& message, std::string library_name = "", std::string module_name = "",
                    int error_code = -1)
       : flint_exception(message),
         library_name_(std::move(library_name)),
         module_name_(std::move(module_name)),
         error_code_(error_code) {}

   [[nodiscard]] const std::string& library_name() const noexcept { return library_name_; }
   [[nodiscard]] const std::string& module_name() const noexcept { return module_name_; }
   [[nodiscard]] int error_code() const noexcept { return error_code_; }

  private:
   std::string library_name_;
   std::string module_name_;
   int error_code_;
};

class FLINT_API simulation_error : public flint_exception {
  public:
   simulation_error(const std::string& message, std::string library_name = "", std::string module_name = "",
                   int error_code = -1)
       : flint_exception(message),
         library_name_(std::move(library_name)),
         module_name_(std::move(module_name)),
         error_code_(error_code) {}

   [[nodiscard]] const std::string& library_name() const noexcept { return library_name_; }
   [[nodiscard]] const std::string& module_name() const noexcept { return module_name_; }
   [[nodiscard]] int error_code() const noexcept { return error_code_; }

  private:
   std::string library_name_;
   std::string module_name_;
   int error_code_;
};

}  // namespace moja::flint
