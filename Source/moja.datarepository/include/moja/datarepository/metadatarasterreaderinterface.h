#ifndef MOJA_DATAREPOSITORY_METADATARASTERREADERINTERFACE_H_
#define MOJA_DATAREPOSITORY_METADATARASTERREADERINTERFACE_H_

#include "moja/datarepository/_datarepository_exports.h"
#include "moja/utility.h"

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <Poco/Path.h>
#include <Poco/File.h>
#include "moja/dynamic.h"

namespace moja {
namespace datarepository {

class TileBlockCellIndexer;
struct BlockIdx;

class DATAREPOSITORY_API MetaDataRasterReader {
public:
	MetaDataRasterReader(const std::string& path, const Point& origin, const std::string& prefix) {};
	virtual ~MetaDataRasterReader() = default;

	virtual DynamicObject readMetaData(const BlockIdx& blkIdx, int nSeries, std::vector<double>*	block) const = 0;

protected:
};

// --------------------------------------------------------------------------------------------

class DATAREPOSITORY_API StackRasterReaderFactoryInterface {
public:
	StackRasterReaderFactoryInterface() {}
	virtual ~StackRasterReaderFactoryInterface() = default;
	virtual std::unique_ptr<MetaDataRasterReader> getRasterReader(const std::string& path, const Point& origin, const std::string& prefix, const TileBlockCellIndexer& indexer) const = 0;
};


}
} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_METADATARASTERREADERINTERFACE_H_