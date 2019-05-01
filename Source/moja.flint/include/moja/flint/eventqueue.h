#ifndef MOJA_FLINT_EVENTQUEUE_H_
#define MOJA_FLINT_EVENTQUEUE_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/iflintdata.h"

#include "moja/dynamic.h"
#include "moja/datetime.h"

namespace moja {
namespace flint {

class FLINT_API EventBase : public IFlintData {
public:
	virtual ~EventBase() = default;

	void configure(DynamicObject config, const flint::ILandUnitController& landUnitController, moja::datarepository::DataRepository& dataRepository) override {
		name = config["name"].extract<const std::string>();
	}

	DynamicVar getProperty(const std::string& key) const override {
		if (key == "id")	return id;
		if (key == "type")	return type;
		if (key == "name")	return name;
		return DynamicVar();
	}

	DynamicObject exportObject() const override {
		DynamicObject result;
		result["id"]	= id;
		result["type"]	= type;
		result["name"]	= name;
		return result;
	}

	int id;
	std::string type;
	std::string name;

protected:
	EventBase(int id, const std::string& type, const std::string& name)
		: id(id),
		type(type),
		name(name) {}
};

// --------------------------------------------------------------------------------------------
// For testing with Modules
//

class FLINT_API DebugModuleWithAnythingInItEvent : public EventBase {
public:
	DebugModuleWithAnythingInItEvent(int id, const std::string& name)
		: EventBase(id, "internal.flint.DebugModuleWithAnythingInIt", name) {}
	virtual ~DebugModuleWithAnythingInItEvent() = default;

	void configure(DynamicObject config, const flint::ILandUnitController& landUnitController, moja::datarepository::DataRepository& dataRepository) override;
	DynamicObject exportObject() const override;

	DynamicObject _data;
};


class FLINT_API EventQueueItem {
public:
	EventQueueItem() = default;
	EventQueueItem(DateTime& date, std::shared_ptr<const EventBase> event) : _date(date), _event(event) {}
	DateTime _date;
	std::shared_ptr<const EventBase> _event;
};

class FLINT_API EventQueue : public IFlintData {
public:
	typedef std::vector<EventQueueItem>::iterator iterator;
	typedef std::vector<EventQueueItem>::const_iterator const_iterator;
	typedef std::vector<EventQueueItem>::size_type size_type;

	EventQueue() = default;
	virtual ~EventQueue() = default;
	EventQueue(const EventQueue&) = delete;
	EventQueue& operator= (const EventQueue&) = delete;

	void configure(DynamicObject config, const flint::ILandUnitController& landUnitController, datarepository::DataRepository& dataRepository) override;
	DynamicObject exportObject() const override;

	void registerEventFactoryFunc(FactoryFunc func) override {
		_eventFactoryMethods.push_back(func);
	}

	void registerEventFactoryFunc(std::function<std::shared_ptr<IFlintData>(const std::string&, int, const std::string&, const DynamicObject&)> func) override {
		_eventFactoryMethodsNew.push_back(func);
	}

	std::vector<FactoryFunc> _eventFactoryMethods;
	std::vector<std::function<std::shared_ptr<IFlintData>(const std::string&, int, const std::string&, const DynamicObject&)>> _eventFactoryMethodsNew;

	iterator begin() MOJA_NOEXCEPT;
	const_iterator begin() const MOJA_NOEXCEPT;
	iterator end() MOJA_NOEXCEPT;
	const_iterator end() const MOJA_NOEXCEPT;
	void clear();
	size_type size() const MOJA_NOEXCEPT;
	template <class... Args>
	void emplace_back(Args&&... args);

	iterator erase(const_iterator position);
	iterator erase(const_iterator first, const_iterator last);

private:
	std::vector<EventQueueItem> _queue;
};

inline EventQueue::iterator EventQueue::begin() MOJA_NOEXCEPT {
	return _queue.begin();
}

inline EventQueue::const_iterator EventQueue::begin() const MOJA_NOEXCEPT {
	return _queue.begin();
}

inline EventQueue::iterator EventQueue::end() MOJA_NOEXCEPT {
	return _queue.end();
}

inline EventQueue::const_iterator EventQueue::end() const MOJA_NOEXCEPT {
	return _queue.end();
}

inline void EventQueue::clear() {
	_queue.clear();
}

inline EventQueue::size_type EventQueue::size() const MOJA_NOEXCEPT {
	return _queue.size();
}


template <class... Args>
void EventQueue::emplace_back(Args&&... args) {
	_queue.emplace_back(std::forward<Args>(args)...);
}

inline EventQueue::iterator EventQueue::erase(const_iterator pos) {
	return _queue.erase(pos);
}

inline EventQueue::iterator EventQueue::erase(const_iterator first, const_iterator last) {
	return _queue.erase(first, last);
}

}
} // namespace moja::flint

#endif // MOJA_FLINT_EVENTQUEUE_H_