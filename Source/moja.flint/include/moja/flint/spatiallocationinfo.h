#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/iflintdata.h"

#include <moja/utility.h>

#include <random>

namespace moja::flint {

class FLINT_API SpatialLocationInfo : public IFlintData {
  public:
   SpatialLocationInfo() = default;
   SpatialLocationInfo(UInt32 tileIdx, UInt32 blockIdx, UInt32 cellIdx, Point tileLatLon, Point blockLatLon,
                       Point cellLatLon, UInt32 tileRows, UInt32 tileCols, UInt32 blockRows, UInt32 blockCols,
                       UInt32 cellRows, UInt32 cellCols, double landUnitArea)
       : _tileIdx(tileIdx),
         _blockIdx(blockIdx),
         _cellIdx(cellIdx),
         _tileLatLon(tileLatLon),
         _blockLatLon(blockLatLon),
         _cellLatLon(cellLatLon),
         _tileRows(tileRows),
         _tileCols(tileCols),
         _blockRows(blockRows),
         _blockCols(blockCols),
         _cellRows(cellRows),
         _cellCols(cellCols),
         _landUnitArea(landUnitArea),
         _errorCode(0) {}
   virtual ~SpatialLocationInfo() = default;

   virtual void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                          moja::datarepository::DataRepository& dataRepository) override {
      if (config.contains("tileIdx")) {
         _tileIdx = config["tileIdx"];
      } else
         _tileIdx = 0;
      if (config.contains("blockIdx")) {
         _blockIdx = config["blockIdx"];
      } else
         _blockIdx = 0;
      if (config.contains("cellIdx")) {
         _cellIdx = config["cellIdx"];
      } else
         _cellIdx = 0;

      if (config.contains("latTile")) {
         _tileLatLon.lat = config["latTile"];
      } else
         _tileLatLon.lat = 0.0;
      if (config.contains("lonTile")) {
         _tileLatLon.lon = config["lonTile"];
      } else
         _tileLatLon.lon = 0.0;
      if (config.contains("latBlock")) {
         _blockLatLon.lat = config["latBlock"];
      } else
         _blockLatLon.lat = 0.0;
      if (config.contains("lonBlock")) {
         _blockLatLon.lon = config["lonBlock"];
      } else
         _blockLatLon.lon = 0.0;
      if (config.contains("latCell")) {
         _cellLatLon.lat = config["latCell"];
      } else
         _cellLatLon.lat = 0.0;
      if (config.contains("lonCell")) {
         _cellLatLon.lon = config["lonCell"];
      } else
         _cellLatLon.lon = 0.0;

      if (config.contains("rowsTile")) {
         _tileRows = config["tileRows"];
      } else
         _tileRows = 0;
      if (config.contains("colsTile")) {
         _tileCols = config["tileCols"];
      } else
         _tileCols = 0;
      if (config.contains("rowsBlock")) {
         _blockRows = config["BlockRows"];
      } else
         _blockRows = 0;
      if (config.contains("colsBlock")) {
         _blockCols = config["BlockCols"];
      } else
         _blockCols = 0;
      if (config.contains("rowsCell")) {
         _cellRows = config["CellRows"];
      } else
         _cellRows = 0;
      if (config.contains("colsCell")) {
         _cellCols = config["CellCols"];
      } else
         _cellCols = 0;

      if (config.contains("landUnitArea")) {
         _landUnitArea = config["landUnitArea"];
      } else
         _landUnitArea = 0.0;

      if (config.contains("randomSeedGlobal")) {
         _randomSeedGlobal = config["randomSeedGlobal"];
      } else
         _randomSeedGlobal = 0;
      if (config.contains("randomSeedTile")) {
         _randomSeedTile = config["randomSeedTile"];
      } else
         _randomSeedTile = 0;
      if (config.contains("randomSeedBlock")) {
         _randomSeedBlock = config["randomSeedBlock"];
      } else
         _randomSeedBlock = 0;
      if (config.contains("randomSeedCell")) {
         _randomSeedCell = config["randomSeedCell"];
      } else
         _randomSeedCell = 0;
   }

   DynamicObject exportObject() const override {
      DynamicObject object;

      object["tileIdx"] = _tileIdx;
      object["blockIdx"] = _blockIdx;
      object["cellIdx"] = _cellIdx;
      object["latTile"] = _tileLatLon.lat;
      object["lonTile"] = _tileLatLon.lon;
      object["latBlock"] = _blockLatLon.lat;
      object["lonBlock"] = _blockLatLon.lon;
      object["latCell"] = _cellLatLon.lat;
      object["lonCell"] = _cellLatLon.lon;
      object["tileRows"] = _tileRows;
      object["tileCols"] = _tileCols;
      object["BlockRows"] = _blockRows;
      object["BlockCols"] = _blockCols;
      object["CellRows"] = _cellRows;
      object["CellCols"] = _cellCols;
      object["landUnitArea"] = _landUnitArea;
      object["randomSeedGlobal"] = _randomSeedGlobal;
      object["randomSeedTile"] = _randomSeedTile;
      object["randomSeedBlock"] = _randomSeedBlock;
      object["randomSeedCell"] = _randomSeedCell;

      return object;
   };

   DynamicVar getProperty(const std::string& key) const override {
      if (key == "errorCode") return _errorCode;
      if (key == "library") return _library;
      if (key == "module") return _module;
      if (key == "message") return _message;
      if (key == "tileIdx") return _tileIdx;
      if (key == "blockIdx") return _blockIdx;
      if (key == "cellIdx") return _cellIdx;
      if (key == "latTile") return _tileLatLon.lat;
      if (key == "lonTile") return _tileLatLon.lon;
      if (key == "latBlock") return _blockLatLon.lat;
      if (key == "lonBlock") return _blockLatLon.lon;
      if (key == "latCell") return _cellLatLon.lat;
      if (key == "lonCell") return _cellLatLon.lon;
      if (key == "tileRows") return _tileRows;
      if (key == "tileCols") return _tileCols;
      if (key == "BlockRows") return _blockRows;
      if (key == "BlockCols") return _blockCols;
      if (key == "CellRows") return _cellRows;
      if (key == "CellCols") return _cellCols;
      if (key == "landUnitArea") return _landUnitArea;
      if (key == "randomSeedGlobal") return _randomSeedGlobal;
      if (key == "randomSeedTile") return _randomSeedTile;
      if (key == "randomSeedBlock") return _randomSeedBlock;
      if (key == "randomSeedCell") return _randomSeedCell;

      return DynamicVar();
   };

   UInt32 _tileIdx;
   UInt32 _blockIdx;
   UInt32 _cellIdx;

   Point _tileLatLon;
   Point _blockLatLon;
   Point _cellLatLon;

   UInt32 _tileRows;
   UInt32 _tileCols;
   UInt32 _blockRows;
   UInt32 _blockCols;
   UInt32 _cellRows;
   UInt32 _cellCols;

   double _landUnitArea;

   // random seed values
   UInt32 _randomSeedGlobal;
   UInt32 _randomSeedTile;
   UInt32 _randomSeedBlock;
   UInt32 _randomSeedCell;

   std::default_random_engine _engGlobal;
   std::default_random_engine _engTile;
   std::default_random_engine _engBlock;
   std::default_random_engine _engCell;

   // Error reporting
   int _errorCode;  // System specific, could be 0 = no error, 1 = ... etc
   std::string _library;
   std::string _module;
   std::string _message;
};

}  // namespace flint
