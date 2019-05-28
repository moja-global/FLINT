#include "moja/flint/eventqueue.h"

#include <moja/datetime.h>
#include <moja/logging.h>

#include <boost/format.hpp>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

void DebugModuleWithAnythingInItEvent::configure(DynamicObject config,
                                                 const flint::ILandUnitController& landUnitController,
                                                 moja::datarepository::DataRepository& dataRepository) {
   EventBase::configure(config, landUnitController, dataRepository);
   _data = config;
}

// --------------------------------------------------------------------------------------------

DynamicObject DebugModuleWithAnythingInItEvent::exportObject() const {
   DynamicObject object;
   object = EventBase::exportObject();
   for (auto& item : _data) {
      object[item.first] = item.second;
   }
   return object;
}

// --------------------------------------------------------------------------------------------

void EventQueue::configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                           datarepository::DataRepository& dataRepository) {
   _queue.clear();
   if (!config.contains("events")) return;
   const auto& eventqueue = config["events"].extract<DynamicVector>();
   if (eventqueue.size() > 0) {
      for (auto& evItem : eventqueue) {
         const auto& ev = evItem.extract<const DynamicObject>();
         auto eventDate = ev["date"].extract<const DateTime>();
         eventDate.addHours(12);  // always make events mid day -- TODO: check this

         int id = ev["id"];
         const auto type = ev["type"].extract<const std::string>();
         const auto name = ev["name"].extract<const std::string>();

         if (!_eventFactoryMethodsNew.empty()) {
            for (auto& func : _eventFactoryMethodsNew) {
               auto eventObject = func(type, id, name, ev);
               if (eventObject) {
                  eventObject->configure(ev, landUnitController, dataRepository);
                  _queue.emplace_back(
                      EventQueueItem{eventDate, std::static_pointer_cast<flint::EventBase>(eventObject)});
                  break;
               }
            }
         }
         // TODO: throw an exception or not?
      }
   }
}

// --------------------------------------------------------------------------------------------

DynamicObject EventQueue::exportObject() const {
   DynamicObject object;

   auto arr = DynamicVector();
   for (auto& item : _queue) {
      auto result = item._event->exportObject();
      auto dateStr = (boost::format("%1%/%2%/%3%") % item._date.year() % item._date.month() % item._date.day()).str();
      DynamicObject date({{"$date", dateStr}});
      result["date"] = date;
      result["id"] = item._event->id;
      result["name"] = item._event->name;
      result["type"] = item._event->type;

      arr.push_back(result);
   }
   object["events"] = arr;
   return object;
}
}  // namespace flint
}  // namespace moja
