#ifndef MOJA_CORE_NOTIFICATIONCENTER_H_
#define MOJA_CORE_NOTIFICATIONCENTER_H_

#include "moja/_core_exports.h"
#include "moja/dispatcher.h"
#include "moja/signals.h"

#include <mutex>

namespace moja {

/**
 * <summary>
 * Handles subscription and broadcasting of signals in the simulation.  
 * Usually modules use this interface to subscribe to events, and sequencers
 * use this interface to notify modules of timing events to drive the simulation.
 * </summary>
 */
class CORE_API NotificationCenter : public DispatcherObject {
public:
    NotificationCenter() = default;
    ~NotificationCenter() = default;

    /**
     * <summary>    Subscribes to a signal using a callback function.
     *              Callback function signatures should match the arguments passed for
     *              the particular signal, if applicable; for example, if a signal
     *              is always fired with a single std::string "message" argument, the
     *              callback should be "void someCallback(std::string message)". </summary>
     *
     * <param name="signal">    The signal. </param>
     * <param name="func">      The function. </param>
     * <param name="obj">       A reference to the subscribing object. </param>
     */
    template<typename F, typename T>
    void subscribe(short signal, F func, T& obj) {
        connectSignal(signal, func, obj);
    }

    /**
     * <summary>    Subscribes to a signal using a callback function.
     *              Callback function signatures should match the arguments passed for
     *              the particular signal, if applicable; for example, if a signal
     *              is always fired with a single std::string "message" argument, the
     *              callback should be "void someCallback(std::string message)". </summary>
     *
     * <param name="signal">    The signal. </param>
     * <param name="func">      The function. </param>
     */
    template<typename F>
    void subscribe(short signal, F func) {
        connectSignal(signal, func);
    }

    /**
     * <summary>    Posts an event notification with optional extra data. </summary>
     *
     * <param name="signal">    The signal. </param>
     * <param name="args">      Variable arguments providing optional extra data. </param>
     */
    template<class ... Args>
    void postNotification(short signal, Args... args) {
        sendSignal(signal, std::forward<Args>(args)...);
    }

    /**
     * <summary>    Posts an event notification with optional extra data.
     *              Also posts a generic follow-up notification after each subscriber
     *              receives the original signal. </summary>
     *
     * <param name="signal">    The signal. </param>
     * <param name="args">      Variable arguments providing optional extra data. </param>
     */
    template<class ... Args>
    void postNotificationWithPostNotification(short originalSignal, Args... args) {
        sendSignalWithPostSendOnSuccess(originalSignal, moja::signals::PostNotification, std::forward<Args>(args)...);
    }

private:
    std::mutex _mutex;
};

} // namespace moja
#endif // MOJA_CORE_NOTIFICATIONCENTER_H_