#ifndef MOJA_FLINT_IOPERATIONTRANSFER_H_
#define MOJA_FLINT_IOPERATIONTRANSFER_H_

namespace moja {
namespace flint {

struct ModuleMetaData;
enum class OperationTransferType;

class IOperationTransfer {
  public:
   virtual ~IOperationTransfer() = default;

   virtual int source() const = 0;
   virtual int sink() const = 0;
   virtual double value() const = 0;

   virtual OperationTransferType transferType() const = 0;
   virtual const ModuleMetaData* metaData() const = 0;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_IOPERATIONTRANSFER_H_
