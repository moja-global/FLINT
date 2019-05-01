#ifndef MOJA_DATAREPOSITORY_LAYER_H_
#define MOJA_DATAREPOSITORY_LAYER_H_

#include "moja/datarepository/iproviderspatialrasterinterface.h"

namespace moja {
namespace datarepository {

class Layer : public IProviderLayer {
public:
	Layer(const std::string& name, const std::string& path, const std::string& prefix,
	      const TileBlockCellIndexer& baseIndexer, TileBlockCellIndexer indexer, const DynamicObject& settings);

	virtual DynamicVar getValueByCellIndex(const CellIdx& cell) const override = 0;
	virtual ~Layer() = default;
	const TileBlockCellIndexer& indexer() const override;
protected:
	std::string _name;
	std::string _path;
	std::string _prefix;
	bool atBaseResolution;
	TileBlockCellIndexer _indexer;
	DynamicObject _settings;
};

inline const TileBlockCellIndexer& Layer::indexer() const {
	return _indexer;
}

}} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_LAYER_H_
