#include <boost/functional/hash.hpp>
#include "moja/flint/record.h"

namespace moja {
namespace flint {

// -- DateRecord
DateRecord::DateRecord(int step, int substep, int year, int month, int day, double fracOfStep, double yearsInStep)
	: _step(step), _substep(substep), _year(year), _month(month), _day(day), _fracOfStep(fracOfStep), _yearsInStep(yearsInStep) {}

bool DateRecord::operator==(const Record<DateRow>& other) const {
	auto otherRow = other.asPersistable();
	return _step == otherRow.get<1>() && _substep == otherRow.get<2>();
}

size_t DateRecord::hash() const {
	if (_hash == -1)
		_hash = moja::hash::hash_combine(_step, _substep);
	return _hash;
}

DateRow DateRecord::asPersistable() const {
	return DateRow{ _id, _step, _substep, _year, _month, _day, _fracOfStep, _yearsInStep };
}

void DateRecord::merge(Record<DateRow>* other) {}
// --

// -- Date2Record
Date2Record::Date2Record(int year)
	: _year(year) {}

bool Date2Record::operator==(const Record<Date2Row>& other) const {
	auto otherRow = other.asPersistable();
	return _year == otherRow.get<1>();
}

size_t Date2Record::hash() const {
	if (_hash == -1)
		_hash = moja::hash::hash_combine(_year);
	return _hash;
}

Date2Row Date2Record::asPersistable() const {
	return Date2Row{ _id, _year };
}

void Date2Record::merge(Record<Date2Row>* other) {}
// --

// -- PoolInfoRecord
PoolInfoRecord::PoolInfoRecord(const std::string& name, const std::string& desc, int idx, int order, double scale, const std::string& units)
	: _name(name), _desc(desc), _idx(idx), _order(order), _scale(scale), _units(units) {}

PoolInfoRecord::PoolInfoRecord(const std::string& name)
	: _name(name), _desc(""), _idx(-1), _order(-1), _scale(1.0), _units("") {}

bool PoolInfoRecord::operator==(const Record<PoolInfoRow>& other) const {
	auto otherRow = other.asPersistable();
	return _name == otherRow.get<1>();
}

size_t PoolInfoRecord::hash() const {
	if (_hash == -1)
		_hash = moja::hash::hash_combine(_name);
	return _hash;
}

PoolInfoRow PoolInfoRecord::asPersistable() const {
	return PoolInfoRow{ _id, _name, _desc, _idx, _order, _scale, _units };
}

void PoolInfoRecord::merge(Record<PoolInfoRow>* other) {}
// --


// -- ModuleInfoRecord
ModuleInfoRecord::ModuleInfoRecord(int libType, int libInfoId, int moduleType, int moduleId, std::string moduleName)
	: _libType(libType), _libInfoId(libInfoId), _moduleType(moduleType), _moduleId(moduleId), _moduleName(moduleName) {}

bool ModuleInfoRecord::operator==(const Record<ModuleInfoRow>& other) const {
	auto otherRow = other.asPersistable();
	return _moduleName == otherRow.get<5>();
}

size_t ModuleInfoRecord::hash() const {
	if (_hash == -1) _hash = moja::hash::hash_combine(_moduleName);
	return _hash;
}

ModuleInfoRow ModuleInfoRecord::asPersistable() const {
	return ModuleInfoRow{ _id, _libType, _libInfoId, _moduleType, _moduleId, _moduleName };
}

void ModuleInfoRecord::merge(Record<ModuleInfoRow>* other) {}
// --


// -- FluxRecord
Int64 FluxRecord::_count = 0;

FluxRecord::FluxRecord(bool doAggregation, int localDomainId, Int64 dateId, Poco::Nullable<Int64> moduleInfoId, Int64 itemCount, int srcPoolInfoId, int sinkPoolInfoId, double flux)
	: _doAggregation(doAggregation), _localDomainId(localDomainId), _dateId(dateId), _moduleInfoId(moduleInfoId), _itemCount(itemCount), _srcPoolInfoId(srcPoolInfoId), _sinkPoolInfoId(sinkPoolInfoId), _flux(flux) {}

bool FluxRecord::operator==(const Record<FluxRow>& other) const {
	if (!_doAggregation)
		return false;

	auto otherRow = other.asPersistable();
	return _localDomainId	== otherRow.get<1>()
		&& _dateId			== otherRow.get<2>()
		&& _moduleInfoId	== otherRow.get<3>()
		&& _srcPoolInfoId	== otherRow.get<5>()
		&& _sinkPoolInfoId	== otherRow.get<6>();
}

size_t FluxRecord::hash() const {
	if (_doAggregation) {
		if (_hash == -1)
			_hash = moja::hash::hash_combine(_localDomainId, _dateId, _moduleInfoId, _srcPoolInfoId, _sinkPoolInfoId);
		return _hash;
	}
	if (_hash == -1)
		_hash = _count++;
	return _hash;
}

FluxRow FluxRecord::asPersistable() const {
	return FluxRow{ _id, _localDomainId, _dateId, _moduleInfoId, _itemCount, _srcPoolInfoId, _sinkPoolInfoId, _flux };
}

void FluxRecord::merge(Record<FluxRow>* other) {
	auto otherRow = other->asPersistable();
	_itemCount += otherRow.get<4>();
	_flux += otherRow.get<7>();
}
// --


}} // namespace moja::flint
