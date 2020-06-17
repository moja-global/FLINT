#include "moja/instrumentor.h"

namespace moja {
Instrumentor& Instrumentor::Get() {
   static Instrumentor instance;
   return instance;
}
}  // namespace moja