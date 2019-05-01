#ifndef MOJA_FLINT_OPERATIONTRANSFEREIGEN_H_
#define MOJA_FLINT_OPERATIONTRANSFEREIGEN_H_

#include "moja/flint/ioperationtransfer.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------
class OperationTransferEigen : public IOperationTransfer {
public:
	OperationTransferEigen() = default;
	OperationTransferEigen(OperationTransferType type, int source, int sink, double value, const ModuleMetaData* metaData) : _type(type), _source(source), _sink(sink), _value(value), _metaData(metaData) {}
	virtual ~OperationTransferEigen() override;

	virtual int source() const override;
	virtual int sink() const override;
	virtual double value() const override;
	virtual OperationTransferType transferType() const override;
	virtual const ModuleMetaData* metaData() const override;

private:
	OperationTransferType _type;
	int _source;
	int _sink;
	double _value;
	const ModuleMetaData* _metaData;
};

inline OperationTransferEigen::~OperationTransferEigen() {}

inline int OperationTransferEigen::source() const { return _source; }

inline int OperationTransferEigen::sink() const { return _sink;  }

inline double OperationTransferEigen::value() const { return _value; }

inline OperationTransferType OperationTransferEigen::transferType() const { return _type; }

inline const ModuleMetaData* OperationTransferEigen::metaData() const { return _metaData; }

}
} // moja::flint

#endif // MOJA_FLINT_OPERATIONTRANSFEREIGEN_H_