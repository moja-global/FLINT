#include "moja/stopwatch.h"

#include <boost/test/unit_test.hpp>

#include <thread>

using namespace std::chrono;
using namespace std::this_thread;

BOOST_AUTO_TEST_SUITE(TimespanTests);

BOOST_AUTO_TEST_CASE(core_stopwatch) {
   moja::Stopwatch<> sw;
   sw.start();
   sleep_for(200ms);
   sw.stop();
   auto d = sw.elapsed();
   BOOST_CHECK(d >= 180ms && d <= 300ms);
   sw.start();
   sleep_for(100ms);
   d = sw.elapsed();
   BOOST_CHECK(d >= 280ms && d <= 400ms);
   sleep_for(100ms);
   sw.stop();
   d = sw.elapsed();
   BOOST_CHECK(d >= 380ms && d <= 500ms);
   sw.restart();
   sleep_for(200ms);
   sw.stop();
   d = sw.elapsed();
   BOOST_CHECK(d >= 180ms && d <= 300ms);
}

BOOST_AUTO_TEST_SUITE_END();