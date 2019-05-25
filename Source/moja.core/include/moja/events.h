//
// Events.h
//

#ifndef MOJA_Core_Events_INCLUDED
#define MOJA_Core_Events_INCLUDED

#include "datetime.h"

#include <cstddef>
#include <string>
#include <type_traits>

namespace moja {
class Event {
  public:
   Event() = default;
   ~Event() = default;
   Event(const std::string& name, moja::DateTime date) : _name(name), _date(date) {}

   std::string _name;
   moja::DateTime _date;
};

}  // namespace moja

#endif  // MOJA_Core_Events_INCLUDED
