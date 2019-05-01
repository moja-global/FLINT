#include <boost/test/unit_test.hpp>

#include "moja/flint/recordaccumulator.h"
#include "moja/flint/recordaccumulatorwithmutex.h"
#include <iostream>
#include "moja/hash.h"

using namespace moja;
using namespace moja::flint;

namespace moja {
namespace modules {
namespace flint {
namespace test {

// --------------------------------------------------------------------------------------------

// id, date id, locn id, module id, src pool id, dst pool id, flux value
typedef  Poco::Tuple<Int64, Int64, Int64, Int64, Int64, Int64, double> FluxRow;
class FluxRecord : public ::Record<FluxRow> {
public:
	FluxRecord(Int64 dateId, Int64 locationId, Int64 moduleId,
		Int64 srcPoolId, Int64 dstPoolId, double flux);

	~FluxRecord() {}

	bool operator==(const Record<FluxRow>& other) const override;
	size_t hash() const override;
	FluxRow asPersistable() const override;
	void merge(Record<FluxRow>* other) override;

private:
	Int64 _dateId;
	Int64 _locationId;
	Int64 _moduleId;
	Int64 _srcPoolId;
	Int64 _dstPoolId;
	double _flux;
};

// --------------------------------------------------------------------------------------------

// id, classifier values
typedef  Poco::Tuple<Int64, std::vector<std::string>> ClassifierSetRow;
class ClassifierSetRecord : public Record<ClassifierSetRow> {
public:
	explicit ClassifierSetRecord(std::vector<std::string> classifierValues);
	~ClassifierSetRecord() {}

	bool operator==(const Record<ClassifierSetRow>& other) const override;
	size_t hash() const override;
	ClassifierSetRow asPersistable() const override;
	void merge(Record<ClassifierSetRow>* other) override;

private:
	std::vector<std::string> _classifierValues;
};

// --------------------------------------------------------------------------------------------

// -- FluxRecord
FluxRecord::FluxRecord(Int64 dateId, Int64 locationId, Int64 moduleId,
	Int64 srcPoolId, Int64 dstPoolId, double flux)
	: _dateId(dateId), _locationId(locationId), _moduleId(moduleId),
	_srcPoolId(srcPoolId), _dstPoolId(dstPoolId), _flux(flux) {}

bool FluxRecord::operator==(const Record<FluxRow>& other) const {
	auto otherRow = other.asPersistable();
	return _dateId == otherRow.get<1>()
		&& _locationId == otherRow.get<2>()
		&& _moduleId == otherRow.get<3>()
		&& _srcPoolId == otherRow.get<4>()
		&& _dstPoolId == otherRow.get<5>();
}

size_t FluxRecord::hash() const {
	return moja::hash::hash_combine(
		_dateId, _locationId, _moduleId, _srcPoolId, _dstPoolId);
}

FluxRow FluxRecord::asPersistable() const {
	return FluxRow{
		_id, _dateId, _locationId, _moduleId, _srcPoolId, _dstPoolId, _flux
	};
}

void FluxRecord::merge(Record<FluxRow>* other) {
	auto otherRow = other->asPersistable();
	_flux += otherRow.get<6>();
}
// --

// --------------------------------------------------------------------------------------------

// -- ClassifierSetRecord
ClassifierSetRecord::ClassifierSetRecord(std::vector<std::string> classifierValues)
	: _classifierValues(classifierValues) {}

bool ClassifierSetRecord::operator==(const Record<ClassifierSetRow>& other) const {
	auto otherValues = other.asPersistable().get<1>();
	for (int i = 0; i < otherValues.size(); i++) {
		if (_classifierValues[i] != otherValues[i]) {
			return false;
		}
	}

	return true;
}

size_t ClassifierSetRecord::hash() const {
	return moja::hash::hash_range(_classifierValues.begin(),
		_classifierValues.end(),
		0, moja::Hash());
}

ClassifierSetRow ClassifierSetRecord::asPersistable() const {
	return ClassifierSetRow{ _id, _classifierValues };
}

void ClassifierSetRecord::merge(Record<ClassifierSetRow>* other) {}
// --

struct Flux2Key {
	short dateId; short locationId; short moduleId;
	short srcPoolId; short dstPoolId;
	bool operator==(const Flux2Key& other) const;
	bool operator<(const Flux2Key& other) const;
};

inline bool Flux2Key::operator<(const Flux2Key& other) const {
	if (dateId < other.dateId) return true;
	if (dateId > other.dateId) return false;
	if (locationId < other.locationId) return true;
	if (locationId > other.locationId) return false;
	if (srcPoolId < other.srcPoolId) return true;
	if (srcPoolId > other.srcPoolId) return false;
	if (dstPoolId < other.dstPoolId) return true;
	if (dstPoolId > other.dstPoolId) return false;
	if (moduleId < other.moduleId) return true;
	if (moduleId > other.moduleId) return false;
	return false;
}

inline bool Flux2Key::operator==(const Flux2Key& other) const {
	return dateId == other.dateId
		&& locationId == other.locationId
		&& srcPoolId == other.srcPoolId
		&& dstPoolId == other.dstPoolId
		&& moduleId == other.moduleId;
}

struct Flux2Value {
	Flux2Value(): _id(-1), _itemCount(0), _flux(0.0) {}
	Flux2Value(double flux): _id(-1), _itemCount(1), _flux(flux) {}

	Int64 _id;
	Int64 _itemCount;
	double _flux;
	Flux2Value& operator+=(const Flux2Value& rhs);
};

inline Flux2Value& Flux2Value::operator+=(const Flux2Value& rhs) {
	_itemCount++;
	_flux += rhs._flux;
	return *this;
}

typedef  Poco::Tuple<Int64, Int64, Int64, Int64, Int64, Int64, double> Flux2Row;
struct Flux2RecordConverter {
	static Flux2Row asPersistable(const Flux2Key& key, const Flux2Value& value) {
		return Flux2Row{ value._id, key.dateId, key.locationId, key.moduleId, key.srcPoolId, key.dstPoolId, value._flux };
	}
};


typedef  Poco::Tuple<Int64, int> Date2Row;
class Date2Record {
public:
	Date2Record(int year) : _id(-1), _year(year) {}
	~Date2Record() {}

	bool operator==(const Date2Record& other) const;
	size_t hash() const;
	Date2Row asPersistable() const;
	void merge(const Date2Record& other) {}
	void setId(Int64 id) { _id = id; }

private:
	mutable size_t _hash = -1;

	// Data
	Int64 _id;
	int _year;
};

inline bool Date2Record::operator==(const Date2Record& other) const {
	return _year == other._year;
}

inline size_t Date2Record::hash() const {
	if (_hash == -1)
		_hash = moja::hash::hash_combine(_year);
	return _hash;
}

inline Date2Row Date2Record::asPersistable() const {
	return Date2Row{ _id, _year };
}

// --
// --------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Flint_RecordAccumulatorIntegrationTests)

BOOST_AUTO_TEST_CASE(Flint_RecordAccumulatorWithMutex3) {
	RecordAccumulatorWithMutex2<Date2Row, Date2Record> accumulator;

	// Check to make sure the order of items matters in hashes based on a
	// collection of IDs - these should not get merged by the accumulator.
	accumulator.accumulate({ 1999 });
	accumulator.accumulate({ 2000 });
	accumulator.accumulate({ 1999 });

	auto accumulatedItems = accumulator.getPersistableCollection();
	BOOST_CHECK_EQUAL(accumulatedItems.size(), 2);

	BOOST_CHECK_EQUAL(accumulatedItems[0].get<1>(), 1999);
	BOOST_CHECK_EQUAL(accumulatedItems[1].get<1>(), 2000);

}

BOOST_AUTO_TEST_CASE(Flint_RecordAccumulatorMap) {
	RecordAccumulatorMap<Flux2Row, Flux2RecordConverter, Flux2Key, Flux2Value> accumulator;

	// Check to make sure the order of items matters in hashes based on a
	// collection of IDs - these should not get merged by the accumulator.
	accumulator.accumulate({ 1, 2, 3, 4, 5 }, { 1.0 });
	accumulator.accumulate({ 1, 2, 3, 4, 5 }, { 1.0 });
	accumulator.accumulate({ 5, 4, 3, 2, 1 }, { 1.0 });

	auto accumulatedItems = accumulator.getPersistableCollection();
	BOOST_CHECK_EQUAL(accumulatedItems.size(), 2);

	BOOST_CHECK_EQUAL(accumulatedItems[0].get<6>(), 2.0);
	BOOST_CHECK_EQUAL(accumulatedItems[1].get<6>(), 1.0);

}

BOOST_AUTO_TEST_CASE(Flint_HashesDoNotCollide) {
	moja::flint::RecordAccumulator<FluxRow> accumulator;

	// Check to make sure the order of items matters in hashes based on a
	// collection of IDs - these should not get merged by the accumulator.
	auto first = std::make_shared<FluxRecord>(1, 2, 3, 4, 5, 1.0);
	auto second = std::make_shared<FluxRecord>(5, 4, 3, 2, 1, 1.0);
	accumulator.accumulate(first);
	accumulator.accumulate(second);

	auto accumulatedItems = accumulator.getPersistableCollection();
	BOOST_CHECK_EQUAL(accumulatedItems.size(), 2);

	for (auto item : accumulatedItems) {
		BOOST_CHECK_EQUAL(item.get<6>(), 1);
	}
}

BOOST_AUTO_TEST_CASE(Flint_GivenIdWorks) {
	moja::flint::RecordAccumulatorWithMutex<FluxRow> accumulator;

	// Check to make sure the order of items matters in hashes based on a
	// collection of IDs - these should not get merged by the accumulator.
	std::vector<int> expectedIds = { 10, 20 };
	auto first = std::make_shared<FluxRecord>(1, 2, 3, 4, 5, 1.0);
	auto second = std::make_shared<FluxRecord>(5, 4, 3, 2, 1, 1.0);
	accumulator.accumulate(first, 10);
	accumulator.accumulate(second, 20);

	auto accumulatedItems = accumulator.getPersistableCollection();
	BOOST_CHECK_EQUAL(accumulatedItems.size(), 2);

	int index = 0;
	for (auto item : accumulatedItems) {
		BOOST_CHECK_EQUAL(item.get<6>(), 1);
		BOOST_CHECK_EQUAL(item.get<0>(), expectedIds[index]);
		index++;
	}
}

BOOST_AUTO_TEST_CASE(Flint_HashesDoNotCollide_Classifiers) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();

	moja::flint::RecordAccumulator<ClassifierSetRow> accumulator;

	// Check to make sure the order of items matters in hashes based on a
	// collection of IDs - these should not get merged by the accumulator.
	std::vector<std::vector<std::string>> recordData = {
		{ "Prince Edward Island", "Atlantic Maritime", "PEI_NIR2015", "Forest Land converted to Wetlands", "Undetermined", "Prince Edward Island", "Atlantic Maritime", "BS", "NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","LA","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","NS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","RP","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","WS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","WS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","JL","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","BF","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","RP","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","JL","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","BF","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","JL","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","RP","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","NS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","BF","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","BF","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","LA","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","JL","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","RP","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","LA","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","LA","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","BF","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","JL","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","BF","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","JL","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","BF","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","BS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","RP","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","RP","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","BS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","TH","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","LA","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","TH","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","RP","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","RP","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","NS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","NS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","NS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","NS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","NS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","RS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","BS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","WS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","LA","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","LA","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","LA","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","LA","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","BF","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","LA","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","WS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","LA","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","TH","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","TH","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","BF","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","TH","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","TH","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","TH","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","LA","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","TH","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","BF","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","WBPO","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","TH","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","BS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","TH","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","WS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","TH","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","WS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","WS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","WS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","LA","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","BS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","LA","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","WBPO","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","WS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","LA","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","BS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","BS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","BS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","WS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","TH","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","TH","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","TH","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","WS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","WS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","WS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","WS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","WS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","WS","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","WBPO","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","WS","OF" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","WBPO","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","WBPO","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","WBPO","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","WBPO","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","BS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","BS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","BS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","LA","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","BS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","BS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","BS","T1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","RP","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","RP","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","RP","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","RP","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","RP","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","RS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","BF","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","BF","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","BF","PN" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","WS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","WS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","WS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","WS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","WS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","RS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","RS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","RS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","RS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","RS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","BS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","BS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","BS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","BS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","BS","NAT" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","LA","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","LA","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","LA","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","LA","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","LA","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","Undetermined","Prince Edward Island","Atlantic Maritime","WS","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Wetlands","Undetermined","Prince Edward Island","Atlantic Maritime","WS","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","Undetermined","Prince Edward Island","Atlantic Maritime","WS","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","WS","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Settlements","D 3.3 - Deforestation","Prince Edward Island","Atlantic Maritime","WS","PNT1" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land remaining Forest Land","Undetermined","Prince Edward Island","Atlantic Maritime","WS","OF" },
		{ "Prince Edward Island","Atlantic Maritime","PEI_NIR2015","Forest Land converted to Cropland","D 3.3 / CM – Deforestation that might otherwise be reported under 3.4 CM","Prince Edward Island","Atlantic Maritime","WS","OF" }
	};

	std::vector<std::shared_ptr<ClassifierSetRecord>> records;

	std::cout << testSuiteName << ": " << testName << ": data records: " << recordData.size() << std::endl;

	int count = 0;
	for (auto& data : recordData) {
		records.push_back(std::make_shared<ClassifierSetRecord>(recordData[count++]));
	};

	std::cout << testSuiteName << ": " << testName << ": records: " << records.size() << std::endl;

	for (auto& record : records) {
		accumulator.accumulate(record);
	}

	auto accumulatedItems = accumulator.getPersistableCollection();

	std::cout << testSuiteName << ": " << testName << ": accumulatedItems: " << accumulatedItems.size() << std::endl;

	BOOST_CHECK_EQUAL(accumulatedItems.size(), recordData.size());
}

}
}
}
}

BOOST_AUTO_TEST_SUITE_END();
