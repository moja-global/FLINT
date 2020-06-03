#ifndef MOJA_FLINT_RECORD_H_
#define MOJA_FLINT_RECORD_H_

#include "moja/flint/_flint_exports.h"

#include <moja/hash.h>
#include <moja/types.h>

#include <Poco/Nullable.h>
#include <Poco/Tuple.h>
#include <Poco/Data/LOB.h>

#include <utility>
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
   void merge(Record<DateRow>* other) override {}

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
   void merge(Record<Date2Row>* other) override {}
   void merge(const Date2Record& other) {}

  private:
   mutable size_t _hash = -1;

   // Data
   int _year;
};

// --------------------------------------------------------------------------------------------
// id, classifier values
typedef Poco::Tuple<Int64, std::vector<Poco::Nullable<std::string>>> ClassifierSetRow;
class ClassifierSetRecord {
  public:
   explicit ClassifierSetRecord(int number_classifiers, std::vector<Poco::Nullable<std::string>> classifier_values);
   explicit ClassifierSetRecord(std::vector<Poco::Nullable<std::string>> classifier_values);
   ~ClassifierSetRecord() {}

   bool operator==(const ClassifierSetRecord& other) const;
   size_t hash() const;
   ClassifierSetRow asPersistable() const;
   void merge(const ClassifierSetRecord& other) {}
   void setId(Int64 id) { _id = id; }
   Int64 getId() const { return _id; }

  private:
   mutable size_t _hash = -1;
   Int64 _id{};

   // Data
   std::vector<Poco::Nullable<std::string>> _classifierValues;
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
   void merge(Record<PoolInfoRow>* other) override {}
   void merge(const PoolInfoRecord& other) {}

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
// id, tile index, block index, tile lat, tile Lon
typedef Poco::Tuple<Int64, Poco::Nullable<UInt32>, Poco::Nullable<UInt32>, Poco::Nullable<UInt32>,
                    Poco::Nullable<double>, Poco::Nullable<double>, Poco::Nullable<double>, Poco::Nullable<double>,
                    Poco::Nullable<double>, Poco::Nullable<double>, Poco::Nullable<UInt32>, Poco::Nullable<UInt32>,
                    Poco::Nullable<UInt32>, Poco::Nullable<UInt32>>
    TileInfoRow;
class TileInfoRecord {
  public:
   TileInfoRecord(Poco::Nullable<UInt32> tileIdx, Poco::Nullable<UInt32> blockIdx, Poco::Nullable<UInt32> cellIdx,
                  Poco::Nullable<double> tileLat, Poco::Nullable<double> tileLon, Poco::Nullable<double> blockLat,
                  Poco::Nullable<double> blockLon, Poco::Nullable<double> cellLat, Poco::Nullable<double> cellLon,
                  Poco::Nullable<UInt32> globalRandomSeed, Poco::Nullable<UInt32> tileRandomSeed,
                  Poco::Nullable<UInt32> blockRandomSeed, Poco::Nullable<UInt32> cellRandomSeed)
       : _id(-1),
         _tileIdx(tileIdx),
         _blockIdx(blockIdx),
         _cellIdx(cellIdx),
         _tileLat(tileLat),
         _tileLon(tileLon),
         _blockLat(blockLat),
         _blockLon(blockLon),
         _cellLat(cellLat),
         _cellLon(cellLon),
         _globalRandomSeed(globalRandomSeed),
         _tileRandomSeed(tileRandomSeed),
         _blockRandomSeed(blockRandomSeed),
         _cellRandomSeed(cellRandomSeed) {}

   ~TileInfoRecord() {}

   bool operator==(const TileInfoRecord& other) const;
   size_t hash() const;
   TileInfoRow asPersistable() const;
   void merge(const TileInfoRecord& other) {}
   void setId(Int64 id) { _id = id; }
   Int64 getId() const { return _id; }

  private:
   mutable size_t _hash = -1;
   // Data
   Int64 _id;
   Poco::Nullable<UInt32> _tileIdx;
   Poco::Nullable<UInt32> _blockIdx;
   Poco::Nullable<UInt32> _cellIdx;
   Poco::Nullable<double> _tileLat;
   Poco::Nullable<double> _tileLon;
   Poco::Nullable<double> _blockLat;
   Poco::Nullable<double> _blockLon;
   Poco::Nullable<double> _cellLat;
   Poco::Nullable<double> _cellLon;
   Poco::Nullable<UInt32> _globalRandomSeed;
   Poco::Nullable<UInt32> _tileRandomSeed;
   Poco::Nullable<UInt32> _blockRandomSeed;
   Poco::Nullable<UInt32> _cellRandomSeed;
};

// --------------------------------------------------------------------------------------------

// id, library type, library info id, module type, module id, module name
typedef Poco::Tuple<Int64, int, int, int, int, std::string> ModuleInfoRow;
class ModuleInfoRecord : public Record<ModuleInfoRow> {
  public:
   ModuleInfoRecord(int libType, int libInfoId, int moduleType, int moduleId, std::string moduleName);
   ~ModuleInfoRecord() {}

   bool operator==(const Record<ModuleInfoRow>& other) const override;
   size_t hash() const override;
   ModuleInfoRow asPersistable() const override;
   void merge(Record<ModuleInfoRow>* other) override {};
   void merge(const ModuleInfoRecord& other) {}

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
class FLINT_API FluxRecord : public Record<FluxRow> {
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


struct UncertaintyLandUnitFluxKey {
   int local_domain_id;
   Int64 date_id;
   Int64 tile_id;
   Poco::Nullable<Int64> classifier_set_id;
   Poco::Nullable<Int64> module_info_id;
   int src_pool_id;
   int sink_pool_id;

   bool operator==(const UncertaintyLandUnitFluxKey& other) const;
   bool operator<(const UncertaintyLandUnitFluxKey& other) const;
};
inline bool UncertaintyLandUnitFluxKey::operator==(const UncertaintyLandUnitFluxKey& other) const {
   return date_id == other.date_id && src_pool_id == other.src_pool_id && sink_pool_id == other.sink_pool_id &&
          tile_id == other.tile_id && classifier_set_id == other.classifier_set_id &&
          module_info_id == other.module_info_id && local_domain_id == other.local_domain_id;
}

inline bool UncertaintyLandUnitFluxKey::operator<(const UncertaintyLandUnitFluxKey& other) const {
   if (date_id < other.date_id) return true;
   if (date_id > other.date_id) return false;
   if (src_pool_id < other.src_pool_id) return true;
   if (src_pool_id > other.src_pool_id) return false;
   if (sink_pool_id < other.sink_pool_id) return true;
   if (sink_pool_id > other.sink_pool_id) return false;
   if (tile_id < other.tile_id) return true;
   if (tile_id > other.tile_id) return false;
   if (classifier_set_id < other.classifier_set_id) return true;
   if (classifier_set_id > other.classifier_set_id) return false;
   if (module_info_id < other.module_info_id) return true;
   if (module_info_id > other.module_info_id) return false;
   if (local_domain_id < other.local_domain_id) return true;
   if (local_domain_id > other.local_domain_id) return false;
   return false;
}

struct UncertaintyFluxKey {
   short date_id;
   Poco::Nullable<Int64> module_info_id;
   short src_pool_id;
   short sink_pool_id;

   bool operator==(const UncertaintyFluxKey& other) const;
   bool operator<(const UncertaintyFluxKey& other) const;
};

inline bool UncertaintyFluxKey::operator==(const UncertaintyFluxKey& other) const {
   return date_id == other.date_id && 
       src_pool_id == other.src_pool_id && 
       sink_pool_id == other.sink_pool_id &&
       module_info_id == other.module_info_id;
}

inline bool UncertaintyFluxKey::operator<(const UncertaintyFluxKey& other) const {
   if (date_id < other.date_id) return true;
   if (date_id > other.date_id) return false;
   if (src_pool_id < other.src_pool_id) return true;
   if (src_pool_id > other.src_pool_id) return false;
   if (sink_pool_id < other.sink_pool_id) return true;
   if (sink_pool_id > other.sink_pool_id) return false;
   if (module_info_id < other.module_info_id) return true;
   if (module_info_id > other.module_info_id) return false;
   return false;
}

struct UncertaintyFluxValue {
   UncertaintyFluxValue() : _id(-1), item_count(0) {}
   UncertaintyFluxValue(std::vector<double> fluxes) : _id(-1), fluxes(std::move(fluxes)), item_count(1) {}
   UncertaintyFluxValue(std::vector<double> fluxes, Int64 itemCount)
       : _id(-1), fluxes(std::move(fluxes)), item_count(itemCount) {}

   Int64 _id;
   std::vector<double> fluxes;
   Int64 item_count;
   UncertaintyFluxValue& operator+=(const UncertaintyFluxValue& rhs);
};

inline UncertaintyFluxValue& UncertaintyFluxValue::operator+=(const UncertaintyFluxValue& rhs) {
   item_count += rhs.item_count;
   const auto size = (std::min)(fluxes.size(), rhs.fluxes.size());
   for (size_t i = 0; i < size; ++i) {
      fluxes[i] += rhs.fluxes[i];
   }
   return *this;
}

// id, date id, module info id, src pool id, dst pool id, flux values
typedef Poco::Tuple<Int64, Int64, Poco::Nullable<Int64>, Int64, int, int, std::vector<double>> UncertaintyFluxRow;
typedef Poco::Tuple<Int64, Int64, Poco::Nullable<Int64>, Int64, int, int, std::vector<double>> UncertaintyFluxTuple;

struct UncertaintyFluxRecordConverter {
   static UncertaintyFluxRow asPersistable(const UncertaintyFluxKey& key, const UncertaintyFluxValue& value) {
      return UncertaintyFluxRow{value._id,       key.date_id,      key.module_info_id, value.item_count,
                                key.src_pool_id, key.sink_pool_id, value.fluxes};
   }

   static UncertaintyFluxTuple asTuple(const UncertaintyFluxKey& key, const UncertaintyFluxValue& value) {
      return UncertaintyFluxTuple{value._id,       key.date_id,      key.module_info_id, value.item_count,
                                  key.src_pool_id, key.sink_pool_id, value.fluxes};
   }
};

// id, local_domain_id, date id, tile_id, classifier_set_id, module info id, item_count, src pool id, dst pool id, flux values
typedef Poco::Tuple<Int64, int, Int64, Int64, Poco::Nullable<Int64>, Poco::Nullable<Int64>, Int64, int, int,
                    Poco::Data::BLOB>
    UncertaintyLandUnitFluxRow;
typedef Poco::Tuple<Int64, int, Int64, Int64, Poco::Nullable<Int64>, Poco::Nullable<Int64>, Int64, int, int,
                    Poco::Data::BLOB>
    UncertaintyLandUnitFluxTuple;


struct UncertaintyLandUnitFluxRecordConverter {
   static UncertaintyLandUnitFluxRow asPersistable(const UncertaintyLandUnitFluxKey& key,
                                                   const UncertaintyFluxValue& value) {
      return {value._id,
              key.local_domain_id,
              key.date_id,
              key.tile_id,
              key.classifier_set_id,
              key.module_info_id,
              value.item_count,
              key.src_pool_id,
              key.sink_pool_id,
              Poco::Data::BLOB(reinterpret_cast<const unsigned char*>(value.fluxes.data()),
                               value.fluxes.size() * sizeof(double))};
   }

   static UncertaintyLandUnitFluxTuple asTuple(const UncertaintyLandUnitFluxKey& key,
                                               const UncertaintyFluxValue& value) {
      return {value._id,
              key.local_domain_id,
              key.date_id,
              key.tile_id,
              key.classifier_set_id,
              key.module_info_id,
              value.item_count,
              key.src_pool_id,
              key.sink_pool_id,
              Poco::Data::BLOB(reinterpret_cast<const unsigned char*>(value.fluxes.data()),
                               value.fluxes.size() * sizeof(double))};
   }
};

/// -- Error handling records

// id, tile index, block index, grs, brs, crs, tile lat, tile Lon, date id, msg
typedef Poco::Tuple<Int64,                   //  0 id
                    int,                     //  1 localDomainId
                    Poco::Nullable<UInt32>,  //  2 tileIdx
                    Poco::Nullable<UInt32>,  //  3 blockIdx
                    Poco::Nullable<UInt32>,  //  4 cellIdx
                    Poco::Nullable<UInt32>,  //  5 randomSeedGlobal
                    Poco::Nullable<UInt32>,  //  6 randomSeedBlock
                    Poco::Nullable<UInt32>,  //  7 randomSeedCell
                    Poco::Nullable<double>,  //  8 tileLat
                    Poco::Nullable<double>,  //  9 tileLon
                    Poco::Nullable<double>,  // 10 blockLat
                    Poco::Nullable<double>,  // 11 blockLon
                    Poco::Nullable<double>,  // 12 cellLat
                    Poco::Nullable<double>,  // 13 cellLon
                    int,                     // 14 errorCode
                    std::string,             // 15 library
                    std::string,             // 16 module
                    std::string>             // 17 msg
    ErrorRow;  
class ErrorRecord {
  public:
   ErrorRecord(int localDomainId, Poco::Nullable<UInt32> tileIdx, Poco::Nullable<UInt32> blockIdx,
               Poco::Nullable<UInt32> cellIdx, Poco::Nullable<UInt32> randomSeedGlobal,
               Poco::Nullable<UInt32> randomSeedBlock, Poco::Nullable<UInt32> randomSeedCell,
               Poco::Nullable<double> tileLat, Poco::Nullable<double> tileLon, Poco::Nullable<double> blockLat,
               Poco::Nullable<double> blockLon, Poco::Nullable<double> cellLat, Poco::Nullable<double> cellLon,
               int errorCode, const std::string& library, const std::string& module,
               const std::string& msg);
   ~ErrorRecord() {}

   bool operator==(const ErrorRecord& other) const;
   size_t hash() const;
   ErrorRow asPersistable() const;
   void merge(const ErrorRecord& other);
   void setId(Int64 id) { _id = id; }
   Int64 getId() const { return _id; }

   int getLocalDomainId() const { return _localDomainId; }
   Poco::Nullable<UInt32> gettileIdx() const { return _tileIdx; }
   Poco::Nullable<UInt32> getblockIdx() const { return _blockIdx; }
   Poco::Nullable<UInt32> getcellIdx() const { return _cellIdx; }
   Poco::Nullable<UInt32> getrandomSeedGlobal() const { return _randomSeedGlobal; }
   Poco::Nullable<UInt32> getrandomSeedBlock() const { return _randomSeedBlock; }
   Poco::Nullable<UInt32> getrandomSeedCell() const { return _randomSeedCell; }
   Poco::Nullable<double> gettileLat() const { return _tileLat; }
   Poco::Nullable<double> gettileLon() const { return _tileLon; }
   Poco::Nullable<double> getblockLat() const { return _blockLat; }
   Poco::Nullable<double> getblockLon() const { return _blockLon; }
   Poco::Nullable<double> getcellLat() const { return _cellLat; }
   Poco::Nullable<double> getcellLon() const { return _cellLon; }
   int geterrorCode() const { return _errorCode; }
   std::string getlibrary() const { return _library; };
   std::string getmodule() const { return _module; };
   std::string getmsg() const { return _msg; };

  private:
   mutable size_t _hash = -1;
   // Data
   Int64 _id;                                 //    0
   int _localDomainId;                        //    1
   Poco::Nullable<UInt32> _tileIdx;           //    2
   Poco::Nullable<UInt32> _blockIdx;          //    3
   Poco::Nullable<UInt32> _cellIdx;           //    4
   Poco::Nullable<UInt32> _randomSeedGlobal;  //    5
   Poco::Nullable<UInt32> _randomSeedBlock;   //    6
   Poco::Nullable<UInt32> _randomSeedCell;    //    7
   Poco::Nullable<double> _tileLat;           //    8
   Poco::Nullable<double> _tileLon;           //    9
   Poco::Nullable<double> _blockLat;          //    10
   Poco::Nullable<double> _blockLon;          //    11
   Poco::Nullable<double> _cellLat;           //    12
   Poco::Nullable<double> _cellLon;           //    13
   int _errorCode;                            //    14
   std::string _library;                      //    15
   std::string _module;                       //    16
   std::string _msg;                          //    17
};

inline ErrorRecord::ErrorRecord(int localDomainId, Poco::Nullable<UInt32> tileIdx, Poco::Nullable<UInt32> blockIdx,
                                Poco::Nullable<UInt32> cellIdx, Poco::Nullable<UInt32> randomSeedGlobal,
                                Poco::Nullable<UInt32> randomSeedBlock, Poco::Nullable<UInt32> randomSeedCell,
                                Poco::Nullable<double> tileLat, Poco::Nullable<double> tileLon,
                                Poco::Nullable<double> blockLat, Poco::Nullable<double> blockLon,
                                Poco::Nullable<double> cellLat, Poco::Nullable<double> cellLon,
                                int errorCode, const std::string& library,
                                const std::string& module, const std::string& msg)
    : _id(-1),
      _localDomainId(localDomainId),
      _tileIdx(tileIdx),
      _blockIdx(blockIdx),
      _cellIdx(cellIdx),
      _randomSeedGlobal(randomSeedGlobal),
      _randomSeedBlock(randomSeedBlock),
      _randomSeedCell(randomSeedCell),
      _tileLat(tileLat),
      _tileLon(tileLon),
      _blockLat(blockLat),
      _blockLon(blockLon),
      _cellLat(cellLat),
      _cellLon(cellLon),
      _errorCode(errorCode),
      _library(library),
      _module(module),
      _msg(msg) {}

inline bool ErrorRecord::operator==(const ErrorRecord& other) const { return false; }

inline size_t ErrorRecord::hash() const {
   if (_hash == -1) _hash = hash::hash_combine(_id);
   return _hash;
}

inline ErrorRow ErrorRecord::asPersistable() const {
   return ErrorRow{
       _id,      _localDomainId, _tileIdx,  _blockIdx, _cellIdx, _randomSeedGlobal, _randomSeedBlock, _randomSeedCell,
       _tileLat, _tileLon,       _blockLat, _blockLon, _cellLat, _cellLon,          _errorCode,
       _library, _module,        _msg};
}

inline void ErrorRecord::merge(const ErrorRecord& other) {}


struct UncertaintyStockKey {
   short date_id;
   int location_id;
   short pool_id;
   bool operator==(const UncertaintyStockKey& other) const;
   bool operator<(const UncertaintyStockKey& other) const;
};

inline bool UncertaintyStockKey::operator==(const UncertaintyStockKey& other) const {
   return pool_id == other.pool_id && date_id == other.date_id && location_id == other.location_id;
}

inline bool UncertaintyStockKey::operator<(const UncertaintyStockKey& other) const {
   if (pool_id < other.pool_id) return true;
   if (pool_id > other.pool_id) return false;
   if (date_id < other.date_id) return true;
   if (date_id > other.date_id) return false;
   if (location_id < other.location_id) return true;
   if (location_id > other.location_id) return false;
   return false;
}

struct UncertaintyLandUnitStockKey {
   int local_domain_id;
   Int64 date_id;
   Int64 tile_id;
   Poco::Nullable<Int64> classifier_set_id;
   int pool_id;
   bool operator==(const UncertaintyLandUnitStockKey& other) const;
   bool operator<(const UncertaintyLandUnitStockKey& other) const;
};

inline bool UncertaintyLandUnitStockKey::operator==(const UncertaintyLandUnitStockKey& other) const {
   return pool_id == other.pool_id && date_id == other.date_id && classifier_set_id == other.classifier_set_id &&
          tile_id == other.tile_id && local_domain_id == other.local_domain_id;
}

inline bool UncertaintyLandUnitStockKey::operator<(const UncertaintyLandUnitStockKey& other) const {
   if (pool_id < other.pool_id) return true;
   if (pool_id > other.pool_id) return false;
   if (date_id < other.date_id) return true;
   if (date_id > other.date_id) return false;
   if (classifier_set_id < other.classifier_set_id) return true;
   if (classifier_set_id > other.classifier_set_id) return false;
   if (tile_id < other.tile_id) return true;
   if (tile_id > other.tile_id) return false;
   if (local_domain_id < other.local_domain_id) return true;
   if (local_domain_id > other.local_domain_id) return false;
   return false;
}


struct UncertaintyStockValue {
   UncertaintyStockValue() : _id(-1), item_count(0), stdev(0.0), mean(0.0) {}
   UncertaintyStockValue(std::vector<double> values) : _id(-1), values(std::move(values)), item_count(1), stdev(0.0), mean(0.0) {}
   UncertaintyStockValue(std::vector<double> values, Int64 itemCount)
       : _id(-1), values(std::move(values)), item_count(itemCount), stdev(0.0), mean(0.0) {}

   Int64 _id;
   std::vector<double> values;
   Int64 item_count;
   double stdev;
   double mean;
   UncertaintyStockValue& operator+=(const UncertaintyStockValue& rhs);
};

inline UncertaintyStockValue& UncertaintyStockValue::operator+=(const UncertaintyStockValue& rhs) {
   item_count += rhs.item_count;
   const auto size = (std::min)(values.size(), rhs.values.size());
   for (size_t i = 0; i < size; ++i) {
      values[i] += rhs.values[i];
   }
   return *this;
}

typedef Poco::Tuple<Int64, Int64, Int64, Int64, std::vector<double>, Int64, double, double> UncertaintyStockRow;
typedef std::tuple<Int64, Int64, Int64, Int64, std::vector<double>, Int64, double, double> UncertaintyStockTuple;
struct UncertaintyStockRecordConverter {
   static UncertaintyStockRow asPersistable(const UncertaintyStockKey& key, const UncertaintyStockValue& value) {
      return UncertaintyStockRow{value._id, key.date_id, key.location_id, key.pool_id, value.values, value.item_count, value.stdev, value.mean};
   }

   static UncertaintyStockTuple asTuple(const UncertaintyStockKey& key, const UncertaintyStockValue& value) {
      return UncertaintyStockTuple{value._id,   key.date_id,  key.location_id, key.pool_id, value.values, value.item_count, value.stdev, value.mean};
   }
};

typedef Poco::Tuple<Int64, Int64, int, Int64, Int64, Int64, Poco::Data::BLOB, Int64, double, double>
    UncertaintyLandUnitStockRow;
typedef std::tuple<Int64, Int64, int, Int64, Int64, Int64, Poco::Data::BLOB, Int64, double, double>
    UncertaintyLandUnitStockTuple;

struct UncertaintyLandUnitStockRecordConverter {
   static UncertaintyLandUnitStockRow asPersistable(const UncertaintyLandUnitStockKey& key,
                                                    const UncertaintyStockValue& value) {
      return {value._id,
              key.date_id,
              key.local_domain_id,
              key.tile_id,
              key.classifier_set_id,
              key.pool_id,
              Poco::Data::BLOB(reinterpret_cast<const unsigned char*>(value.values.data()),
                               value.values.size() * sizeof(double)),
              value.item_count,
              value.stdev,
              value.mean};
   }

   static UncertaintyLandUnitStockTuple asTuple(const UncertaintyLandUnitStockKey& key,
                                                const UncertaintyStockValue& value) {
      return {value._id,
              key.date_id,
              key.local_domain_id,
              key.tile_id,
              key.classifier_set_id,
              key.pool_id,
              Poco::Data::BLOB(reinterpret_cast<const unsigned char*>(value.values.data()),
                               value.values.size() * sizeof(double)),
              value.item_count,
              value.stdev,
              value.mean};
   }
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_RECORD_H_