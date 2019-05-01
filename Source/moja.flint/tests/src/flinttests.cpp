#include <boost/test/unit_test.hpp>
#include "flinttests.h"
#include "moja/flint/aggregatorstockstep.h"
#include "moja/flint/configuration/library.h"
#include "moja/flint/configuration/localdomain.h"
#include "moja/flint/localdomaincontrollerbase.h"
#include "moja/flint/ioperationmanager.h"
#include "moja/flint/ipool.h"
#include "moja/flint/configuration/configuration.h"

#include "moja/dynamic.h"

using namespace moja;
using namespace moja::flint;
namespace conf = moja::flint::configuration;

//// --------------------------------------------------------------------------------------------
//void TestObserved::handle1(const moja::Notification::Ptr pNf) {
//	poco_check_ptr(pNf);
//	auto nf = pNf;
//	set.insert("handle1");
//}
//
//// --------------------------------------------------------------------------------------------
//void TestObserved::handle2(const moja::Notification::Ptr pNf) {
//	poco_check_ptr(pNf);
//	auto nf = pNf;
//	set.insert("handle2");
//}
//
//// --------------------------------------------------------------------------------------------
//void TestObserved::handleTest(const std::shared_ptr<TestNotification>& pNf)
//{
//	poco_check_ptr(pNf);
//	auto nf = pNf;
//	set.insert("handleTest");
//}
//
// --------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(flinttests);

// --------------------------------------------------------------------------------------------
int helperFuncForTest(int* x)
{
	*x = 29929;
	return 0;
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(flint_Test1)
{
	//NotificationCenter nc;
	//TestObserved ob;
	//auto o1 = std::make_shared<Observer<TestObserved, Notification>>(ob, &TestObserved::handle1);
	//auto o2 = std::make_shared<Observer<TestObserved, Notification>>(ob, &TestObserved::handle2);
	//nc.addObserver(o1);
	//BOOST_CHECK(nc.hasObserver(o1));
	//nc.addObserver(o2);
	//BOOST_CHECK(nc.hasObserver(o2));
	//BOOST_CHECK(nc.hasObservers());
	//BOOST_CHECK(nc.countObservers() == 2);
	//nc.postNotification(std::make_shared<Notification>());
	//BOOST_CHECK(ob.set.size() == 2);
	//BOOST_CHECK(ob.set.find("handle1") != ob.set.end());
	//BOOST_CHECK(ob.set.find("handle2") != ob.set.end());
	//nc.removeObserver(Observer<TestObserved, Notification>(ob, &TestObserved::handle1));
	//BOOST_CHECK(!nc.hasObserver(o1));
	//nc.removeObserver(Observer<TestObserved, Notification>(ob, &TestObserved::handle2));
	//BOOST_CHECK(!nc.hasObserver(o2));
	//BOOST_CHECK(!nc.hasObservers());
	//BOOST_CHECK(nc.countObservers() == 0);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(flint_Test2)
{
	//auto mod = std::make_shared<moja::BuiltInModel::BuiltInModelTest>(nullptr);
	//auto b = mod->initialise();

	//NotificationCenter nc;
	//TestObserved ob;
	//nc.addObserver(Observer<TestObserved, Notification>(ob, &TestObserved::handle1));
	//nc.addObserver(Observer<TestObserved, TestNotification>(ob, &TestObserved::handleTest));
	//nc.postNotification(std::make_shared<Notification>());
	//BOOST_CHECK(ob.set.size() == 1);
	//BOOST_CHECK(ob.set.find("handle1") != ob.set.end());
	//ob.set.clear();
	//nc.postNotification(std::make_shared<TestNotification>());
	//BOOST_CHECK(ob.set.size() == 2);
	//BOOST_CHECK(ob.set.find("handle1") != ob.set.end());
	//BOOST_CHECK(ob.set.find("handleTest") != ob.set.end());
	//nc.removeObserver(Observer<TestObserved, Notification>(ob, &TestObserved::handle1));
	//nc.removeObserver(Observer<TestObserved, TestNotification>(ob, &TestObserved::handleTest));
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(flint_Test3)
{
	//NotificationCenter nc;
	//TestObserved ob;
	//Observer<TestObserved, Notification> o1(ob, &TestObserved::handle1);
	//nc.addObserver(o1);
	//BOOST_CHECK(nc.hasObserver(o1));
	//BOOST_CHECK(nc.hasObservers());
	//BOOST_CHECK(nc.countObservers() == 1);
	//nc.postNotification(std::make_shared<Notification>());
	//BOOST_CHECK(ob.set.size() == 1);
	//BOOST_CHECK(ob.set.find("handle1") != ob.set.end());
	//nc.removeObserver(Observer<TestObserved, Notification>(ob, &TestObserved::handle1));
	//BOOST_CHECK(!nc.hasObservers());
	//BOOST_CHECK(nc.countObservers() == 0);
}


BOOST_AUTO_TEST_SUITE_END();
