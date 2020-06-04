#pragma once

#pragma once
#include <cstdint>
#include <string>

namespace moja::flint::morton {

struct vec {
   uint32_t x;
   uint32_t y;
};

#if defined(__BMI2__) || __AVX2__
#include <immintrin.h>
inline uint64_t expand_bits_2(uint64_t v) { return _pdep_u64(v, 0x5555555555555555); }

inline uint64_t compact_bits_2(uint64_t v) { return _pext_u64(v, 0x5555555555555555); }
#else

inline uint64_t expand_bits_2(uint64_t v) {
   v = (v ^ (v << 16)) & 0x0000ffff0000ffff;
   v = (v ^ (v << 8)) & 0x00ff00ff00ff00ff;
   v = (v ^ (v << 4)) & 0x0f0f0f0f0f0f0f0f;
   v = (v ^ (v << 2)) & 0x3333333333333333;
   v = (v ^ (v << 1)) & 0x5555555555555555;
   return v;
}

inline uint64_t compact_bits_2(uint64_t v) {
   v &= 0x5555555555555555;
   v = (v ^ (v >> 1)) & 0x3333333333333333;
   v = (v ^ (v >> 2)) & 0x0f0f0f0f0f0f0f0f;
   v = (v ^ (v >> 4)) & 0x00ff00ff00ff00ff;
   v = (v ^ (v >> 8)) & 0x0000ffff0000ffff;
   v = (v ^ (v >> 16)) & 0x00000000ffffffff;
   return v;
}

#endif

inline uint64_t encode(vec v) { return {(expand_bits_2(v.x) << 0) | (expand_bits_2(v.y) << 1)}; }

inline vec decode(uint64_t code) {
   return {
       static_cast<uint32_t>(compact_bits_2(code >> 0)),
       static_cast<uint32_t>(compact_bits_2(code >> 1)),
   };
}

inline std::string morton_to_string(uint64_t N) {
   if (N < 4) {
      return std::to_string(N);
   }
   return morton_to_string(N / 4) + std::to_string(N % 4);
}
}  // namespace moja::flint::morton
