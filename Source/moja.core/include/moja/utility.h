#ifndef MOJA_CORE_UTILITY_H_
#define MOJA_CORE_UTILITY_H_

#include "moja/_core_exports.h"

namespace moja {

struct CORE_API Point {
   double lat;  // y
   double lon;  // x
};

struct CORE_API XYIndex {
   UInt32 X;
   UInt32 Y;
};

struct CORE_API Size {
   double height;
   double width;
};

}  // namespace moja

#endif  // MOJA_CORE_UTILITY_H_
