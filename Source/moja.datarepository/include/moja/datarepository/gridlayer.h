#ifndef MOJA_DATAREPOSITORY_GRIDLAYER_H_
#define MOJA_DATAREPOSITORY_GRIDLAYER_H_

#include "moja/datarepository/layer.h"
#include "moja/datarepository/tile.h"
#include <boost/tokenizer.hpp>

#include <string>
#include <unordered_map>

namespace moja {
	namespace datarepository {

		template<class T>
		class GridLayer : public Layer {
		public:
			GridLayer(
				std::shared_ptr<RasterReaderFactoryInterface> rasterReaderFactory,
				const std::string& name,
				const std::string& path,
				const std::string& prefix,
				const TileBlockCellIndexer& baseIndexer,
				TileBlockCellIndexer indexer,
				T nodata,
				const DynamicObject& settings,
				DynamicObject attributeTable = DynamicObject())
				: Layer(name, path, prefix, baseIndexer, indexer, settings)
				, _rasterReaderFactory(rasterReaderFactory)
				, _nodata(nodata)
				, _settings(settings) {

				_interpreted = !attributeTable.empty();
				buildAttributeLookup(attributeTable);
			}

			~GridLayer() = default;

			DynamicVar getValueByCellIndex(const CellIdx& cell) const override;

		private:
			std::shared_ptr<RasterReaderFactoryInterface> _rasterReaderFactory;
			mutable UInt32 _currentTileIdx = std::numeric_limits<UInt32>::max();
			mutable std::shared_ptr<Tile<T>> _tile;
			T _nodata;
			DynamicObject _settings;
			bool _interpreted;
			mutable std::unordered_map<T, DynamicVar> _interpretation;

			void buildAttributeLookup(const DynamicObject& interpretation);
		};

		template<class T>
		inline DynamicVar GridLayer<T>::getValueByCellIndex(const CellIdx& cellIdx) const {
			if (cellIdx.tileIdx != _currentTileIdx) {
				auto point = _indexer.getLatLonFromIndex({ cellIdx.tileIdx, _indexer });
				_tile = std::make_shared<Tile<T>>(_rasterReaderFactory, _path, point, _prefix, _indexer, _settings);
				_currentTileIdx = cellIdx.tileIdx;
			}
			T value = _tile->getValueByLocation(_indexer.convertIndex(cellIdx));
			if (value == _nodata) {
				return DynamicVar();
			}

			if (_interpreted) {
				auto interp = _interpretation.find(value);
				return interp == _interpretation.end() ? DynamicVar() : interp->second;
			}

			return DynamicVar(value);
		}

		template<class T>
		inline void GridLayer<T>::buildAttributeLookup(const DynamicObject& interpretation) {
			for (const auto& pixelMapping : interpretation) {
				auto key = pixelMapping.first;
				T pixelValue = T(std::stol(key));
				auto attributes = pixelMapping.second;
				_interpretation[pixelValue] = attributes;
			}
		}

	}
} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_GRIDLAYER_H_
