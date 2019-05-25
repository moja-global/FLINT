#include "moja/hash.h"
#include "moja/types.h"

#include <boost/test/unit_test.hpp>

#include <unordered_map>

BOOST_AUTO_TEST_SUITE(HashingTests);

using namespace moja::hash;

BOOST_AUTO_TEST_CASE(core_hashing_TwangMix64) {
   moja::UInt64 i1 = 0x78a87873e2d31dafULL;
   moja::UInt64 i1_res = 3389151152926383528ULL;
   BOOST_CHECK_EQUAL(i1_res, twang_mix64(i1));
   BOOST_CHECK_EQUAL(i1, twang_unmix64(i1_res));

   moja::UInt64 i2 = 0x0123456789abcdefULL;
   moja::UInt64 i2_res = 3061460455458984563ull;
   BOOST_CHECK_EQUAL(i2_res, twang_mix64(i2));
   BOOST_CHECK_EQUAL(i2, twang_unmix64(i2_res));
}

namespace {
void checkTWang(uint64_t r) {
   moja::UInt64 result = twang_mix64(r);
   BOOST_CHECK_EQUAL(r, twang_unmix64(result));
}
}  // namespace

BOOST_AUTO_TEST_CASE(core_hashing_TwangUnmix64) {
   // We'll try (1 << i), (1 << i) + 1, (1 << i) - 1
   for (int i = 1; i < 64; i++) {
      checkTWang((1U << i) - 1);
      checkTWang(1U << i);
      checkTWang((1U << i) + 1);
   }
}

BOOST_AUTO_TEST_CASE(core_hashing_JenkinsRevMix32) {
   moja::UInt32 i1 = 3805486511ul;
   moja::UInt32 i1_res = 381808021ul;
   BOOST_CHECK_EQUAL(i1_res, jenkins_rev_mix32(i1));
   BOOST_CHECK_EQUAL(i1, jenkins_rev_unmix32(i1_res));

   moja::UInt32 i2 = 2309737967ul;
   moja::UInt32 i2_res = 1834777923ul;
   BOOST_CHECK_EQUAL(i2_res, jenkins_rev_mix32(i2));
   BOOST_CHECK_EQUAL(i2, jenkins_rev_unmix32(i2_res));
}

namespace {
void checkJenkins(moja::UInt32 r) {
   moja::UInt32 result = jenkins_rev_mix32(r);
   BOOST_CHECK_EQUAL(r, jenkins_rev_unmix32(result));
}
}  // namespace

BOOST_AUTO_TEST_CASE(core_hashing_JenkinsRevUnmix32) {
   // We'll try (1 << i), (1 << i) + 1, (1 << i) - 1
   for (int i = 1; i < 32; i++) {
      checkJenkins((1U << i) - 1);
      checkJenkins(1U << i);
      checkJenkins((1U << i) + 1);
   }
}

BOOST_AUTO_TEST_CASE(core_hashing_Hasher) {
   // Basically just confirms that things compile ok.
   std::unordered_map<moja::Int32, moja::Int32, moja::hasher<moja::Int32>> m;
   m.insert(std::make_pair(4, 5));
   BOOST_CHECK_EQUAL(m[4], 5);
}

// Not a full hasher since only handles one type.
class TestHasher {
  public:
   //	static size_t hash(const std::pair<int, int>& p) {
   // return p.first + p.second;
   //}
   size_t operator()(const std::pair<int, int>& p) const { return p.first + p.second; }
};

template <typename T, typename... Ts>
size_t hashCombineTest(const T& t, const Ts&... ts) {
   return hash_combine_generic(TestHasher{}, t, ts...);
}

BOOST_AUTO_TEST_CASE(core_hashing_Pair) {
   auto a = std::make_pair(1, 2);
   auto b = std::make_pair(3, 4);
   auto c = std::make_pair(1, 2);
   auto d = std::make_pair(2, 1);
   BOOST_CHECK_EQUAL(hash_combine(a), hash_combine(c));
   BOOST_CHECK_NE(hash_combine(b), hash_combine(c));
   BOOST_CHECK_NE(hash_combine(d), hash_combine(c));

   // With composition
   BOOST_CHECK_EQUAL(hash_combine(a, b), hash_combine(c, b));
   // Test order dependence
   BOOST_CHECK_NE(hash_combine(a, b), hash_combine(b, a));

   // Test with custom hasher
   BOOST_CHECK_EQUAL(hashCombineTest(a), hashCombineTest(c));
   // 3 + 4 != 1 + 2
   BOOST_CHECK_NE(hashCombineTest(b), hashCombineTest(c));
   // This time, thanks to a terrible hash function, these are equal
   BOOST_CHECK_EQUAL(hashCombineTest(d), hashCombineTest(c));
   // With composition
   BOOST_CHECK_EQUAL(hashCombineTest(a, b), hashCombineTest(c, b));
   // Test order dependence
   BOOST_CHECK_NE(hashCombineTest(a, b), hashCombineTest(b, a));
   // Again, 1 + 2 == 2 + 1
   BOOST_CHECK_EQUAL(hashCombineTest(a, b), hashCombineTest(d, b));
}

BOOST_AUTO_TEST_CASE(core_hashing_HashCombine) { BOOST_CHECK_NE(hash_combine(1, 2), hash_combine(2, 1)); }

BOOST_AUTO_TEST_CASE(core_hashing_Std_Tuple) {
   typedef std::tuple<int64_t, std::string, int32_t> tuple3;
   tuple3 t(42, "foo", 1);

   std::unordered_map<tuple3, std::string, moja::Hash> m;
   m[t] = "bar";
   BOOST_CHECK_EQUAL("bar", m[t]);
}

BOOST_AUTO_TEST_CASE(core_hashing_Enum_Type) {
   const auto hash = moja::Hash();

   enum class Enum32 : moja::Int32 { Foo, Bar };
   BOOST_CHECK_EQUAL(hash(static_cast<moja::Int32>(Enum32::Foo)), hash(Enum32::Foo));
   BOOST_CHECK_EQUAL(hash(static_cast<moja::Int32>(Enum32::Bar)), hash(Enum32::Bar));
   BOOST_CHECK_NE(hash(Enum32::Foo), hash(Enum32::Bar));

   std::unordered_map<Enum32, std::string, moja::Hash> m32;
   m32[Enum32::Foo] = "foo";
   BOOST_CHECK_EQUAL("foo", m32[Enum32::Foo]);

   enum class Enum64 : moja::Int64 { Foo, Bar };
   BOOST_CHECK_EQUAL(hash(static_cast<moja::Int64>(Enum64::Foo)), hash(Enum64::Foo));
   BOOST_CHECK_EQUAL(hash(static_cast<moja::Int64>(Enum64::Bar)), hash(Enum64::Bar));
   BOOST_CHECK_NE(hash(Enum64::Foo), hash(Enum64::Bar));

   std::unordered_map<Enum64, std::string, moja::Hash> m64;
   m64[Enum64::Foo] = "foo";
   BOOST_CHECK_EQUAL("foo", m64[Enum64::Foo]);
}

BOOST_AUTO_TEST_CASE(core_hashing_Pair_Moja_Hash) {
   typedef std::pair<moja::Int64, moja::Int32> pair2;
   pair2 p(42, 1);

   std::unordered_map<pair2, std::string, moja::Hash> m;
   m[p] = "bar";
   BOOST_CHECK_EQUAL("bar", m[p]);
}

BOOST_AUTO_TEST_CASE(core_hashing_Tuple_Moja_Hash) {
   typedef std::tuple<moja::Int64, moja::Int32, moja::Int32> tuple3;
   tuple3 t(42, 1, 1);

   std::unordered_map<tuple3, std::string, moja::Hash> m;
   m[t] = "bar";
   BOOST_CHECK_EQUAL("bar", m[t]);
}

namespace {
template <class T>
size_t hash_vector(const std::vector<T>& v) {
   return hash_range(v.begin(), v.end());
}
}  // namespace

BOOST_AUTO_TEST_CASE(core_hashing_HashRange) {
   // Not true at on windows:
   // BOOST_CHECK_EQUAL(hashVector<moja::Int32>({ 1, 2 }), hashVector<moja::Int16>({ 1, 2 }));
   BOOST_CHECK_NE(hash_vector<int>({2, 1}), hash_vector<int>({1, 2}));
   BOOST_CHECK_EQUAL(hash_vector<int>({}), hash_vector<float>({}));
}

BOOST_AUTO_TEST_CASE(core_hashing_Std_Tuple_Different_Hash) {
   typedef std::tuple<moja::Int64, std::string, moja::Int32> tuple3;
   tuple3 t1(42, "foo", 1);
   tuple3 t2(9, "bar", 3);
   tuple3 t3(42, "foo", 3);

   BOOST_CHECK_NE(std::hash<tuple3>()(t1), std::hash<tuple3>()(t2));
   BOOST_CHECK_NE(std::hash<tuple3>()(t1), std::hash<tuple3>()(t3));
}

BOOST_AUTO_TEST_CASE(core_hashing_Strings) {
   using namespace moja;

   std::string a1 = "10050517", b1 = "51107032", a2 = "10050518", b2 = "51107033", a3 = "10050519", b3 = "51107034",
               a4 = "10050525", b4 = "51107040";

   folly::Hash hash;
   BOOST_CHECK_EQUAL(hash(a1), hash(a1));
   BOOST_CHECK_EQUAL(hash(a2), hash(a2));
   BOOST_CHECK_EQUAL(hash(a3), hash(a3));
   BOOST_CHECK_EQUAL(hash(a4), hash(a4));

   BOOST_CHECK_NE(hash(a1), hash(b1));
   BOOST_CHECK_NE(hash(a2), hash(b2));
   BOOST_CHECK_NE(hash(a3), hash(b3));
   BOOST_CHECK_NE(hash(a4), hash(b4));
}

BOOST_AUTO_TEST_SUITE_END();