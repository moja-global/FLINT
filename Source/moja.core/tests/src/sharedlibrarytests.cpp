#include <Poco/SharedLibrary.h>

#include <boost/test/unit_test.hpp>

// --------------------------------------------------------------------------------------------
#if defined(_WIN32)
const char* libPrefix = "";
#elif defined(MOJA_OS_FAMILY_UNIX)
const char* libPrefix = "lib";
#endif

BOOST_AUTO_TEST_SUITE(SharedLibraryTests);

BOOST_AUTO_TEST_CASE(core_sharedLibrary_load) {
   std::string prefix = libPrefix;
   std::string path = prefix + "testmodule" + Poco::SharedLibrary::suffix();
   Poco::SharedLibrary sl;
   BOOST_CHECK(!sl.isLoaded());
   sl.load(path);
   BOOST_CHECK(sl.getPath() == path);
   BOOST_CHECK(sl.isLoaded());
   BOOST_CHECK(sl.hasSymbol("initializeModule"));
   BOOST_CHECK(!sl.hasSymbol("fooBar123"));

   sl.unload();
   BOOST_CHECK(!sl.isLoaded());
}

BOOST_AUTO_TEST_SUITE_END();
