#ifndef MOJA_FLINT_OPERATIONTRANSFEREIGENFULL_H_
#define MOJA_FLINT_OPERATIONTRANSFEREIGENFULL_H_

#include "moja/flint/ioperationtransfer.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------
class OperationTransferEigenFull : public IOperationTransfer {
  public:
   OperationTransferEigenFull() = default;
   OperationTransferEigenFull(OperationTransferType type, int source, int sink, double value,
                              const ModuleMetaData* metaData)
       : _type(type), _source(source), _sink(sink), _value(value), _metaData(metaData) {}
   virtual ~OperationTransferEigenFull() override;

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

inline OperationTransferEigenFull::~OperationTransferEigenFull() {}

inline int OperationTransferEigenFull::source() const { return _source; }

inline int OperationTransferEigenFull::sink() const { return _sink; }

inline double OperationTransferEigenFull::value() const { return _value; }

inline OperationTransferType OperationTransferEigenFull::transferType() const { return _type; }

inline const ModuleMetaData* OperationTransferEigenFull::metaData() const { return _metaData; }

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OPERATIONTRANSFEREIGENFULL_H_