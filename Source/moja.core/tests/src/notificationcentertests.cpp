#include "moja/notificationcenter.h"
#include "moja/signals.h"

#include <boost/test/unit_test.hpp>

const short testSignal = 1;
const std::string testMessage = "foo";

class Subscriber {
public:
    bool signalReceived = false;
    std::string message;
    short signal;
    int signalCount = 0;

    void subscribe(moja::NotificationCenter& notificationCenter, bool withArgs = false) {
        if (withArgs) {
            notificationCenter.subscribe(testSignal, &Subscriber::onMessageSignal, *this);
        } else {
            notificationCenter.subscribe(testSignal, &Subscriber::onSignal, *this);
        }
    }

    void onSignal() {
        signalReceived = true;
        signalCount++;
    }

    void onMessageSignal(const std::string message) {
        signalReceived = true;
        this->message = message;
        signalCount++;
    }

    void onPostSignal(short signal) {
        signalReceived = true;
        this->signal = signal;
        signalCount++;
    }
};

struct NotificationCenterTestsFixture {
    moja::NotificationCenter notificationCenter;
    Subscriber subscriber;
};

BOOST_FIXTURE_TEST_SUITE(NotificationCenterTests, NotificationCenterTestsFixture);

BOOST_AUTO_TEST_CASE(core_notificationcenter_Subscribe_External) {
    notificationCenter.subscribe(testSignal, &Subscriber::onSignal, subscriber);
    notificationCenter.postNotification(testSignal);

    BOOST_CHECK_EQUAL(subscriber.signalReceived, true);
}

BOOST_AUTO_TEST_CASE(core_notificationcenter_Subscribe_ExternalWithArgs) {
    notificationCenter.subscribe(testSignal, &Subscriber::onMessageSignal, subscriber);
    notificationCenter.postNotification(testSignal, testMessage);

    BOOST_CHECK_EQUAL(subscriber.signalReceived, true);
    BOOST_CHECK_EQUAL(subscriber.message, testMessage);
}

BOOST_AUTO_TEST_CASE(core_notificationcenter_Subscribe_Internal) {
    subscriber.subscribe(notificationCenter);
    notificationCenter.postNotification(testSignal);

    BOOST_CHECK_EQUAL(subscriber.signalReceived, true);
}

BOOST_AUTO_TEST_CASE(core_notificationcenter_Subscribe_InternalWithArgs) {
    subscriber.subscribe(notificationCenter, true);
    notificationCenter.postNotification(testSignal, testMessage);

    BOOST_CHECK_EQUAL(subscriber.signalReceived, true);
    BOOST_CHECK_EQUAL(subscriber.message, testMessage);
}

BOOST_AUTO_TEST_CASE(core_notificationcenter_Subscribe_PostNotification) {
    // Post notification is fired once for each subscriber to the original event,
    // so need at least one to check that the post notification gets sent.
    Subscriber testSignalSubscriber;
    testSignalSubscriber.subscribe(notificationCenter);
    notificationCenter.subscribe(moja::signals::PostNotification,
                                 &Subscriber::onPostSignal,
                                 subscriber);

    notificationCenter.postNotificationWithPostNotification(testSignal);

    BOOST_CHECK_EQUAL(subscriber.signalReceived, true);
    BOOST_CHECK_EQUAL(subscriber.signal, testSignal);
}

BOOST_AUTO_TEST_CASE(core_notificationcenter_Subscribe_MultipleSubscribers) {
    std::vector<Subscriber> subscribers(10);
    for (auto& subscriber : subscribers) {
        subscriber.subscribe(notificationCenter);
    }

    notificationCenter.postNotification(testSignal);

    for (auto& subscriber : subscribers) {
        BOOST_CHECK_EQUAL(subscriber.signalReceived, true);
    }
}

BOOST_AUTO_TEST_CASE(core_notificationcenter_Subscribe_MultipleSubscribersWithPostNotification) {
    Subscriber postSubscriber;
    notificationCenter.subscribe(moja::signals::PostNotification,
                                 &Subscriber::onPostSignal,
                                 postSubscriber);

    std::vector<Subscriber> subscribers(10);
    for (auto& subscriber : subscribers) {
        subscriber.subscribe(notificationCenter);
    }

    notificationCenter.postNotificationWithPostNotification(testSignal);

    BOOST_CHECK_EQUAL(postSubscriber.signalCount, subscribers.size());
}

BOOST_AUTO_TEST_SUITE_END();