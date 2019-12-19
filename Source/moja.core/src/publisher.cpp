#include "moja/publisher.h"

namespace moja {

void Publisher::AddSubscriber(std::shared_ptr<ISubscriber> subscriber) {
   if (subscriber == nullptr) throw std::invalid_argument("subscriber");
   _subscribers.push_back(subscriber);
}

void Publisher::SendMessage(std::shared_ptr<IMessage> message) const {
   for (auto& subscriber : _subscribers) {
      subscriber->ReceiveMessage(message);
   }
}

}  // namespace moja
