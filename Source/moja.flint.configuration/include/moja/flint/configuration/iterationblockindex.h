#ifndef MOJA_FLINT_CONFIGURATION_ITERATIONBLOCKINDEX_H_
#define MOJA_FLINT_CONFIGURATION_ITERATIONBLOCKINDEX_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/iterationbase.h"

#include <vector>

namespace moja {
namespace flint {
namespace configuration {

class ConfigBlockIdx {
  public:
   ConfigBlockIdx(const UInt32 tileIdx, const UInt32 blockIdx) : _tileIdx(tileIdx), _blockIdx(blockIdx) {}

   UInt32 tileIdx() const { return _tileIdx; }
   UInt32 blockIdx() const { return _blockIdx; }

   void set_tileIdx(const UInt32 val) { _tileIdx = val; }
   void set_blockIdx(const UInt32 val) { _blockIdx = val; }

  private:
   UInt32 _tileIdx;
   UInt32 _blockIdx;
};

class CONFIGURATION_API IterationBlockIndex : public IterationBase {
  public:
   IterationBlockIndex();
   virtual ~IterationBlockIndex() {}

   virtual const std::vector<ConfigBlockIdx>& blockIndexList() const { return _blockIndexList; }
   virtual std::vector<ConfigBlockIdx>& blockIndexList() { return _blockIndexList; }

   virtual void addBlockIndex(ConfigBlockIdx index) { _blockIndexList.push_back(index); }

  private:
   std::vector<ConfigBlockIdx> _blockIndexList;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_ITERATIONBLOCKINDEX_H_
