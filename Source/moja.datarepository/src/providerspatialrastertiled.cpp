#include "moja/datarepository/providerspatialrastertiled.h"

#include <moja/datarepository/gridlayer.h>
#include <moja/datarepository/stacklayer.h>
#include <moja/datarepository/layer.h>

namespace moja {
namespace datarepository {

ProviderSpatialRasterTiled::ProviderSpatialRasterTiled(
	std::shared_ptr<RasterReaderFactoryInterface> rasterReaderFactory,
	DynamicObject settings) {
	double tileLatSize = settings["tileLatSize"];
	double tileLonSize = settings["tileLonSize"];
	double blockLatSize = settings["blockLatSize"];
	double blockLonSize = settings["blockLonSize"];
	double cellLatSize = settings["cellLatSize"];
	double cellLonSize = settings["cellLonSize"];

	_indexer = std::make_unique<TileBlockCellIndexer>(tileLatSize, tileLonSize, blockLatSize, blockLonSize, cellLatSize, cellLonSize);

	auto layers = settings["layers"].extract<DynamicVector>();
	for (auto& l : layers)
	{
        auto& layerOrig = l.extract<const DynamicObject>();
		DynamicObject layer = layerOrig;	// get rid of CONST 

		// These are required properties in a spatial layer 
		std::string layerName = layer["name"];
		std::string layerPath = layer["layer_path"];
		std::string layerPrefix = layer["layer_prefix"];

		//Add all items from top level that are NOT in the layer setting
		for (auto& topSettingsItem : settings) {
			if (topSettingsItem.first != "layers" && !layer.contains(topSettingsItem.first)) {
				layer.insert(topSettingsItem);
			}
		}

		// No check meta data
		const auto metadataReader = rasterReaderFactory->getMetaDataRasterReader(layerPath, layerPrefix, layer);
		if (metadataReader != nullptr) {
			auto metadata = metadataReader->readMetaData();
			for (auto& metaItem : metadata) {
				layer[metaItem.first] = metaItem.second;
			}
		}

		std::string layerType = layer["layer_type"];
		std::string layerData = layer["layer_data"];

		const double layertileLatSize = layer["tileLatSize"];
		const double layertileLonSize = layer["tileLonSize"];
		const double layerblockLatSize = layer["blockLatSize"];
		const double layerblockLonSize = layer["blockLonSize"];
		const double layercellLatSize = layer["cellLatSize"];
		const double layercellLonSize = layer["cellLonSize"];

        DynamicObject attributeTable;
        if (layer.contains("attributes")) {
            attributeTable = layer["attributes"].extract<DynamicObject>();
        }

        if (layerType == "GridLayer") {
			//if (rasterReaderFactory.tileReaderFactory() == nullptr) {
			//	// TODO: should throw exception 
			//	return;
			//}
			if (layerData == "UInt8" || layerData == "Byte") {
                _layers.insert(std::make_pair(layerName, std::make_shared<GridLayer<UInt8>>(rasterReaderFactory, layerName, layerPath, layerPrefix, *_indexer,
                    TileBlockCellIndexer(layertileLatSize, layertileLonSize, layerblockLatSize, layerblockLonSize, layercellLatSize, layercellLonSize), layer["nodata"], layer, attributeTable)));
            } else if (layerData == "UInt16") {
                _layers.insert(std::make_pair(layerName, std::make_shared<GridLayer<UInt16>>(rasterReaderFactory, layerName, layerPath, layerPrefix, *_indexer,
                    TileBlockCellIndexer(layertileLatSize, layertileLonSize, layerblockLatSize, layerblockLonSize, layercellLatSize, layercellLonSize), layer["nodata"], layer, attributeTable)));
            } else if (layerData == "Int16") {
                _layers.insert(std::make_pair(layerName, std::make_shared<GridLayer<Int16>>(rasterReaderFactory, layerName, layerPath, layerPrefix, *_indexer,
                    TileBlockCellIndexer(layertileLatSize, layertileLonSize, layerblockLatSize, layerblockLonSize, layercellLatSize, layercellLonSize), layer["nodata"], layer, attributeTable)));
            } else if (layerData == "Int32") {
                _layers.insert(std::make_pair(layerName, std::make_shared<GridLayer<Int32>>(rasterReaderFactory, layerName, layerPath, layerPrefix, *_indexer,
                    TileBlockCellIndexer(layertileLatSize, layertileLonSize, layerblockLatSize, layerblockLonSize, layercellLatSize, layercellLonSize), layer["nodata"], layer, attributeTable)));
            } else if (layerData == "UInt32") {
                _layers.insert(std::make_pair(layerName, std::make_shared<GridLayer<UInt32>>(rasterReaderFactory, layerName, layerPath, layerPrefix, *_indexer,
                    TileBlockCellIndexer(layertileLatSize, layertileLonSize, layerblockLatSize, layerblockLonSize, layercellLatSize, layercellLonSize), layer["nodata"], layer, attributeTable)));
            } else if (layerData == "Float32") {
                _layers.insert(std::make_pair(layerName, std::make_shared<GridLayer<float>>(rasterReaderFactory, layerName, layerPath, layerPrefix, *_indexer,
                    TileBlockCellIndexer(layertileLatSize, layertileLonSize, layerblockLatSize, layerblockLonSize, layercellLatSize, layercellLonSize), layer["nodata"], layer, attributeTable)));
            } else {
                BOOST_THROW_EXCEPTION(ProviderUnsupportedException() << ProviderType(layerData));
            }
		} else if (layerType == "StackLayer") {
			//if (rasterReaderFactorySet.stackReaderFactory() == nullptr) {
			//	// TODO: should throw exception 
			//	return;
			//}
			int nLayers = layer["nLayers"];

            if (layerData == "UInt8" || layerData == "Byte") {
				_layers.insert(std::make_pair(layerName, std::make_shared<StackLayer<UInt8>>(rasterReaderFactory, layerName, layerPath, layerPrefix, nLayers, *_indexer,
					TileBlockCellIndexer(layertileLatSize, layertileLonSize, layerblockLatSize, layerblockLonSize, layercellLatSize, layercellLonSize), layer["nodata"], layer)));
			} else if (layerData == "UInt16") {
				_layers.insert(std::make_pair(layerName, std::make_shared<StackLayer<UInt16>>(rasterReaderFactory, layerName, layerPath, layerPrefix, nLayers, *_indexer,
					TileBlockCellIndexer(layertileLatSize, layertileLonSize, layerblockLatSize, layerblockLonSize, layercellLatSize, layercellLonSize), layer["nodata"], layer)));
			} else if (layerData == "Int16") {
				_layers.insert(std::make_pair(layerName, std::make_shared<StackLayer<Int16>>(rasterReaderFactory, layerName, layerPath, layerPrefix, nLayers, *_indexer,
					TileBlockCellIndexer(layertileLatSize, layertileLonSize, layerblockLatSize, layerblockLonSize, layercellLatSize, layercellLonSize), layer["nodata"], layer)));
			} else if (layerData == "Int32") {
				_layers.insert(std::make_pair(layerName, std::make_shared<StackLayer<Int32>>(rasterReaderFactory, layerName, layerPath, layerPrefix, nLayers, *_indexer,
					TileBlockCellIndexer(layertileLatSize, layertileLonSize, layerblockLatSize, layerblockLonSize, layercellLatSize, layercellLonSize), layer["nodata"], layer)));
			} else if (layerData == "UInt32") {
				_layers.insert(std::make_pair(layerName, std::make_shared<StackLayer<UInt32>>(rasterReaderFactory, layerName, layerPath, layerPrefix, nLayers, *_indexer,
					TileBlockCellIndexer(layertileLatSize, layertileLonSize, layerblockLatSize, layerblockLonSize, layercellLatSize, layercellLonSize), layer["nodata"], layer)));
			} else if (layerData == "Float32") {
				_layers.insert(std::make_pair(layerName, std::make_shared<StackLayer<float>>(rasterReaderFactory, layerName, layerPath, layerPrefix, nLayers, *_indexer,
					TileBlockCellIndexer(layertileLatSize, layertileLonSize, layerblockLatSize, layerblockLonSize, layercellLatSize, layercellLonSize), layer["nodata"], layer)));
			} else {
                BOOST_THROW_EXCEPTION(ProviderUnsupportedException() << ProviderType(layerData));
			}
		} else {
			// TODO: should throw exception 
			return;
		}
	}
}

DynamicVar ProviderSpatialRasterTiled::GetValue(const std::string& name, double latitude, double longitude) {
	const auto layer = getLayer(name);
	auto cell = _indexer->getCellIndexFromLatLon({ latitude, longitude });
	return layer->getValueByCellIndex(cell);
}

const IProviderLayer* ProviderSpatialRasterTiled::getLayer(const std::string& name) const {
	if (_layers.find(name) == _layers.end())
		BOOST_THROW_EXCEPTION(LayerNotFoundException() << LayerName(name));
	return _layers.at(name).get();
}

inline const TileBlockCellIndexer& ProviderSpatialRasterTiled::indexer(const std::string& layerName) {
	if (_layers.find(layerName) == _layers.end())
		BOOST_THROW_EXCEPTION(LayerNotFoundException() << LayerName(layerName));
	auto layer = _layers.at(layerName);
	return layer->indexer();
}

DynamicVar ProviderSpatialRasterTiled::GetValueStack(const std::string& name, double latitude, double longitude) {
	const auto layer = getLayer(name);
	auto cell = _indexer->getCellIndexFromLatLon({ latitude, longitude });
	return layer->getValueByCellIndex(cell);
}

DynamicVar ProviderSpatialRasterTiled::GetValue(const std::string& name, const CellIdx& cell) {
	const auto layer = getLayer(name);
	return layer->getValueByCellIndex(cell);
}

DynamicVar ProviderSpatialRasterTiled::GetValueStack(const std::string& name, const CellIdx& cell) {
	const auto layer = getLayer(name);
	return layer->getValueByCellIndex(cell);
}

CellIdx ProviderSpatialRasterTiled::getCellIdx(double lat, double lon) {
	return _indexer->getCellIndexFromLatLon({ lat, lon });
}

BlockIdx ProviderSpatialRasterTiled::getBlockIdx(double lat, double lon) {
	return _indexer->getBlockIndexFromLatLon({ lat, lon });
}

TileIdx ProviderSpatialRasterTiled::getTileIdx(double lat, double lon) {
	return _indexer->getTileIndexFromLatLon({ lat, lon });
}

CellIdx ProviderSpatialRasterTiled::getLayerCellIdx(const std::string& name, double lat, double lon) const {
	auto layer = _layers.at(name);
	return layer->indexer().getCellIndexFromLatLon({ lat, lon });
}

CellIdx ProviderSpatialRasterTiled::getLayerCellIdx(const std::string& name, UInt32 tileIdx, UInt32 blockIdx, UInt32 cellIdx) const {
	auto layer = _layers.at(name);
	CellIdx cell(tileIdx, blockIdx, cellIdx, *(_indexer.get()));
	return layer->indexer().convertIndex(cell);
}

}} // namespace moja::datarepository