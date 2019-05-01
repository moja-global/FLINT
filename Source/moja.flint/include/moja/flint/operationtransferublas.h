#ifndef MOJA_FLINT_OPERATIONTRANSFERUBLAS_H_
#define MOJA_FLINT_OPERATIONTRANSFERUBLAS_H_

#include "moja/flint/ioperationtransfer.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------
class OperationTransferUblas : public IOperationTransfer {
public:
	OperationTransferUblas() = default;
	OperationTransferUblas(OperationTransferType type, int source, int sink, double value, const ModuleMetaData* metaData) : _type(type), _source(source), _sink(sink), _value(value), _metaData(metaData) {}
	virtual ~OperationTransferUblas() override;

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

inline OperationTransferUblas::~OperationTransferUblas() {}

inline int OperationTransferUblas::source() const { return _source; }

inline int OperationTransferUblas::sink() const { return _sink;  }

inline double OperationTransferUblas::value() const { return _value; }

inline OperationTransferType OperationTransferUblas::transferType() const { return _type; }

inline const ModuleMetaData* OperationTransferUblas::metaData() const { return _metaData; }

}
} // moja::flint

#endif // MOJA_FLINT_OPERATIONTRANSFERUBLAS_H_