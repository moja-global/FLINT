#ifndef MOJA_FLINT_CONFIGURATION_ITERATIONASPATIALMONGOINDEX_H_
#define MOJA_FLINT_CONFIGURATION_ITERATIONASPATIALMONGOINDEX_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/iterationbase.h"

#include <string>
#include <vector>

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API IterationASpatialMongoIndex : public IterationBase {
public:
	IterationASpatialMongoIndex();
	virtual ~IterationASpatialMongoIndex() {}

	bool processAlldocuments() const { return _processAllDocuments; }
	void set_processAlldocuments(bool val) { _processAllDocuments = val; }

	const std::vector<std::string>& documentIds() const { return _documentIds; }
	std::vector<std::string>& documentIds() { return _documentIds; }

	virtual void addDocumentId(std::string& documentId) { _documentIds.push_back(documentId); }

private:

	bool _processAllDocuments;
	std::vector<std::string> _documentIds;
};

}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_ITERATIONASPATIALMONGOINDEX_H_
