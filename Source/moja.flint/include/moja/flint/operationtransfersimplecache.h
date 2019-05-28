#ifndef MOJA_FLINT_OPERATIONTRANSFERSIMPLECACHE_H_
#define MOJA_FLINT_OPERATIONTRANSFERSIMPLECACHE_H_

#include "moja/flint/ioperationtransfer.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------
class OperationTransferSimpleCache : public IOperationTransfer {
  public:
   OperationTransferSimpleCache() = default;
   OperationTransferSimpleCache(OperationTransferType type, int source, int sink, double value,
                                const ModuleMetaData* metaData)
       : _type(type), _source(source), _sink(sink), _value(value), _metaData(metaData) {}
   virtual ~OperationTransferSimpleCache() override;

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

inline OperationTransferSimpleCache::~OperationTransferSimpleCache() {}

inline int OperationTransferSimpleCache::source() const { return _source; }

inline int OperationTransferSimpleCache::sink() const { return _sink; }

inline double OperationTransferSimpleCache::value() const { return _value; }

inline OperationTransferType OperationTransferSimpleCache::transferType() const { return _type; }

inline const ModuleMetaData* OperationTransferSimpleCache::metaData() const { return _metaData; }

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OPERATIONTRANSFERSIMPLECACHE_H_