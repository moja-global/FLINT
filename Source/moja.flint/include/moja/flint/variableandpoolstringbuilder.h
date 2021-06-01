#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ilandunitdatawrapper.h"

#include <moja/dynamic.h>

#include <boost/algorithm/string/join.hpp>

#include <string>

namespace moja::flint {

class IPool;

class FLINT_API VariableAndPoolStringBuilder {
  public:
   VariableAndPoolStringBuilder() = default;
   ~VariableAndPoolStringBuilder() = default;

   VariableAndPoolStringBuilder(const ILandUnitController& landUnitController, const std::string& workingStr);

   VariableAndPoolStringBuilder(const ILandUnitDataWrapper* landUnitWrapper, const std::string& workingStr);

   void configure(const ILandUnitController& landUnitController, const std::string& workingStr);
   void configure(const ILandUnitDataWrapper* landUnitWrapper, const std::string& workingStr);
   std::string result() const;

  private:
   static std::vector<std::string> extractTokens(const std::string& tokenType, const std::string& query);
   static void replaceTokens(const std::string& tokenType, std::string& query, std::vector<std::string> values);

   static std::string formatVariableValues(const IVariable& var, const std::string& property);

   const ILandUnitController* _landUnitController;
   const ILandUnitDataWrapper* _landUnitDataWrapper;

   const std::string varMarker = "var";
   const std::string poolMarker = "pool";

   std::string _workingStr;
   std::vector<std::tuple<const IVariable*, std::string>> _variables;
   std::vector<const IPool*> _pools;
};

}  // namespace flint
