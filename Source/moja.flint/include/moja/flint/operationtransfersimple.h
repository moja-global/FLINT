#ifndef MOJA_FLINT_OPERATIONTRANSFERSIMPLE_H_
#define MOJA_FLINT_OPERATIONTRANSFERSIMPLE_H_

#include "moja/flint/ioperationtransfer.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------
class OperationTransferSimple : public IOperationTransfer {
public:
	OperationTransferSimple() = default;
	OperationTransferSimple(OperationTransferType type, int source, int sink, double value, const ModuleMetaData* metaData) : _type(type), _source(source), _sink(sink), _value(value), _metaData(metaData) {}
	~OperationTransferSimple() override;

	int source() const override;
	int sink() const override;
	double value() const override;
	OperationTransferType transferType() const override;
	const ModuleMetaData* metaData() const override;

private:
	OperationTransferType _type;
	int _source;
	int _sink;
	double _value;
	const ModuleMetaData* _metaData;
};

inline OperationTransferSimple::~OperationTransferSimple() {}

inline int OperationTransferSimple::source() const { return _source; }

inline int OperationTransferSimple::sink() const { return _sink;  }

inline double OperationTransferSimple::value() const { return _value; }

inline OperationTransferType OperationTransferSimple::transferType() const { return _type; }

inline const ModuleMetaData* OperationTransferSimple::metaData() const { return _metaData; }

}
} // moja::flint

#endif // MOJA_FLINT_OPERATIONTRANSFERSIMPLE_H_