#pragma once

#include <iostream>
#include <string>

namespace moja {

enum class status_code {
   /// Not an error; returned on success.
   Ok = 0,

   Error = 1,
   Cancelled = 2,
   Unknown = 3,
};

std::string StatusCodeToString(status_code code);
std::ostream& operator<<(std::ostream& os, status_code code);

/**
 * Reports error code and details.
 *
 * This class is modeled after `grpc::Status`.
 */
class status {
  public:
   status() = default;

   explicit status(status_code status_code, std::string message = "")
       : code_(status_code), message_(std::move(message)) {}

   bool ok() const { return code_ == status_code::Ok; }

   status_code code() const { return code_; }
   const std::string& message() const { return message_; }

  private:
   status_code code_{status_code::Ok};
   std::string message_;
};

inline std::ostream& operator<<(std::ostream& os, status const& rhs) {
   return os << rhs.message() << " [" << StatusCodeToString(rhs.code()) << "]";
}

inline bool operator==(status const& lhs, status const& rhs) {
   return lhs.code() == rhs.code() && lhs.message() == rhs.message();
}

inline bool operator!=(status const& lhs, status const& rhs) { return !(lhs == rhs); }

class runtime_status_error : public std::runtime_error {
  public:
   explicit runtime_status_error(status status);

   status const& status() const { return status_; }

  private:
   moja::status status_;
};

}  // namespace moja
