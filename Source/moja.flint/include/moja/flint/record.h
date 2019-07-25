#ifndef MOJA_FLINT_RECORD_H_
#define MOJA_FLINT_RECORD_H_

#include "moja/flint/_flint_exports.h"

#include <moja/hash.h>
#include <moja/types.h>

#include <Poco/Nullable.h>
#include <Poco/Tuple.h>

#include <vector>

namespace std {

// --------------------------------------------------------------------------------------------

// Hash function for nullable should check for null and insert an appropriate value
template <typename T1>
struct hash<Poco::Nullable<T1>> {
  public:
   size_t operator()(const Poco::Nullable<T1>& x) const {
      if (x.isNull())
         return moja::hash::twang_mix64(T1(0.0));
      else
         return moja::hash::twang_mix64(x);
   }
};

}  // namespace std

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

template <class TPersistable>
class Record {
   // template<class T> friend class RecordAccumulator;
  public:
   virtual ~Record() = default;

   virtual bool operator==(const Record<TPersistable>& other) const = 0;
   virtual size_t hash() const = 0;
   virtual TPersistable asPersistable() const = 0;
   virtual void merge(Record<TPersistable>* other) = 0;

   Int64 getId() const { return _id; }
   void setId(Int64 id) { _id = id; }

  protected:
   Int64 _id = -1;
};

// --------------------------------------------------------------------------------------------

// id, step, substep, year, month, day, frac of step, years in step
typedef Poco::Tuple<Int64, int, int, int, int, int, double, double> DateRow;
class DateRecord : public flint::Record<DateRow> {
  public:
   DateRecord(int step, int substep, int year, int month, int day, double fracOfStep, double yearsInStep);
   ~DateRecord() {}

   bool operator==(const Record<DateRow>& other) const override;
   size_t hash() const override;
   DateRow asPersistable() const override;
   void merge(Record<DateRow>* other) override;

  private:
   mutable size_t _hash = -1;

   // Data
   int _step;
   int _substep;
   int _year;
   int _month;
   int _day;
   double _fracOfStep;
   double _yearsInStep;
};

// --------------------------------------------------------------------------------------------

// id, year
typedef Poco::Tuple<Int64, int> Date2Row;
class Date2Record : public flint::Record<Date2Row> {
  public:
   Date2Record(int year);
   ~Date2Record() {}

   bool operator==(const Record<Date2Row>& other) const override;
   size_t hash() const override;
   Date2Row asPersistable() const override;
   void merge(Record<Date2Row>* other) override;

  private:
   mutable size_t _hash = -1;

   // Data
   int _year;
};

// --------------------------------------------------------------------------------------------

// id, pool name
typedef Poco::Tuple<Int64, std::string, std::string, int, int, double, std::string> PoolInfoRow;
class PoolInfoRecord : public flint::Record<PoolInfoRow> {
  public:
   explicit PoolInfoRecord(const std::string& name, const std::string& desc, int idx, int order, double scale,
                           const std::string& units);
   explicit PoolInfoRecord(const std::string& name);
   ~PoolInfoRecord() {}

   bool operator==(const Record<PoolInfoRow>& other) const override;
   size_t hash() const override;
   PoolInfoRow asPersistable() const override;
   void merge(Record<PoolInfoRow>* other) override;

  private:
   mutable size_t _hash = -1;

   // Data
   std::string _name;
   std::string _desc;
   int _idx;
   int _order;
   double _scale;
   std::string _units;
};

// --------------------------------------------------------------------------------------------

// id, library type, library info id, module type, module id, module name
typedef Poco::Tuple<Int64, int, int, int, int, std::string> ModuleInfoRow;
class ModuleInfoRecord : public flint::Record<ModuleInfoRow> {
  public:
   ModuleInfoRecord(int libType, int libInfoId, int moduleType, int moduleId, std::string moduleName);
   ~ModuleInfoRecord() {}

   bool operator==(const Record<ModuleInfoRow>& other) const override;
   size_t hash() const override;
   ModuleInfoRow asPersistable() const override;
   void merge(Record<ModuleInfoRow>* other) override;

  private:
   mutable size_t _hash = -1;

   // Data
   int _libType;
   int _libInfoId;
   int _moduleType;
   int _moduleId;
   std::string _moduleName;
};

// --------------------------------------------------------------------------------------------

// id, localDomainId, date id, moduleInfoId, src pool id, dst pool id, flux value
typedef Poco::Tuple<Int64, int, Int64, Poco::Nullable<Int64>, Int64, int, int, double> FluxRow;
class FLINT_API FluxRecord : public flint::Record<FluxRow> {
  public:
   FluxRecord(bool doAggregation, int localdomainId, Int64 dateId, Poco::Nullable<Int64> moduleInfoId, Int64 itemCount,
              int srcPoolInfoId, int sinkPoolInfoId, double flux);
   ~FluxRecord() {}

   bool operator==(const Record<FluxRow>& other) const override;
   size_t hash() const override;
   FluxRow asPersistable() const override;
   void merge(Record<FluxRow>* other) override;

  private:
   // Flags, utility
   bool _doAggregation;
   mutable size_t _hash = -1;
   static Int64 _count;

   // Data
   int _localDomainId;                   // 1
   Int64 _dateId;                        // 2
   Poco::Nullable<Int64> _moduleInfoId;  // 3
   Int64 _itemCount;                     // 4
   int _srcPoolInfoId;                   // 5
   int _sinkPoolInfoId;                  // 6
   double _flux;                         // 7
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_RECORD_H_