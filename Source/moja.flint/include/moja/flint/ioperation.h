#pragma once

#include <moja/dynamic.h>

#include <memory>

namespace moja::flint {

class ITiming;
class IPool;
class IOperationResult;
struct ModuleMetaData;
enum class OperationTransferType;

enum class OperationTransferHandlingType { Aggregated = 0, Disaggregated = 1 };

inline std::string OperationTransferHandlingTypeToString(OperationTransferHandlingType t) {
   switch (t) {
      case OperationTransferHandlingType::Aggregated:
         return "Aggregated";
      case OperationTransferHandlingType::Disaggregated:
         return "Disaggregated";
      default:
         break;
   }
   return "Unknown OperationTransferHandlingType";
}

class IOperation {
  public:
   IOperation() = default;
   explicit IOperation(const ModuleMetaData* metaData) : _metaData(metaData), _hasDataPackage(false) {}
   IOperation(const ModuleMetaData* metaData, DynamicVar& dataPackage)
       : _metaData(metaData), _dataPackage(dataPackage), _hasDataPackage(true) {}
   virtual ~IOperation() = default;

   // Some issues to consider with an operation
   //	1/	Transfers can result in a zero move - 0.0 proportional move, pool value is zero on a proportional move,
   //0
   //           stock move, etc
   //           Should these be ignored or recorded as zero fluxes?
   //   2/	If an operation has no transfers or only zero result transfers should submit create an OperationResult
   //   or not? 3/	When we have a zero transfer on submit -> compute should a flux of zero be generated? 4/
   //   Operation transfers can be aggregated or not.

   virtual IOperation* addTransfer(const IPool* source, const IPool* sink, double value) = 0;

   // This will create an OperationResult, which contains a collection of OperationResultFlux
   // Implementations SHOULD create a OperationResult that has an empty collection of OperationResultFlux
   // if there are not transfers, or all transfers are zero result moves

   virtual std::shared_ptr<IOperationResult> computeOperation(ITiming& timing) = 0;
   virtual void submitOperation() = 0;

   virtual OperationTransferType transferType() const = 0;
   virtual const ModuleMetaData* metaData() const { return _metaData; }

   virtual const DynamicVar& dataPackage() const { return _dataPackage; }
   virtual bool hasDataPackage() const { return _hasDataPackage; }

   virtual OperationTransferHandlingType transferHandlingType() const = 0;

  protected:
   const ModuleMetaData* _metaData;
   DynamicVar _dataPackage;
   bool _hasDataPackage;
};

}  // namespace moja::flint
