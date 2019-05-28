#ifndef MOJA_FLINT_CONFIGURATION_ITERATIONCELLINDEX_H_
#define MOJA_FLINT_CONFIGURATION_ITERATIONCELLINDEX_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/iterationbase.h"

#include <vector>

namespace moja {
namespace flint {
namespace configuration {

class ConfigCellIdx {
  public:
   ConfigCellIdx(const UInt32 tileIdx, const UInt32 blockIdx, const UInt32 cellIdx)
       : _tileIdx(tileIdx), _blockIdx(blockIdx), _cellIdx(cellIdx) {}

   UInt32 tileIdx() const { return _tileIdx; }
   UInt32 blockIdx() const { return _blockIdx; }
   UInt32 cellIdx() const { return _cellIdx; }

   void set_tileIdx(const UInt32 val) { _tileIdx = val; }
   void set_blockIdx(const UInt32 val) { _blockIdx = val; }
   void set_cellIdx(const UInt32 val) { _cellIdx = val; }

  private:
   UInt32 _tileIdx;
   UInt32 _blockIdx;
   UInt32 _cellIdx;
};

class CONFIGURATION_API IterationCellIndex : public IterationBase {
  public:
   IterationCellIndex();
   virtual ~IterationCellIndex() {}

   virtual const std::vector<ConfigCellIdx>& cellIndexList() const { return _cellIndexList; }
   virtual std::vector<ConfigCellIdx>& cellIndexList() { return _cellIndexList; }

   virtual void addCellIndex(ConfigCellIdx index) { _cellIndexList.push_back(index); }

  private:
   std::vector<ConfigCellIdx> _cellIndexList;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_ITERATIONCELLINDEX_H_