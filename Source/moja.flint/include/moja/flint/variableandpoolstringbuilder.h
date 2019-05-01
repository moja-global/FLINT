#ifndef MOJA_FLINT_VARIABLEANDPOOLSTRINGBUILDER_H_
#define MOJA_FLINT_VARIABLEANDPOOLSTRINGBUILDER_H_

#include "moja/flint/_flint_exports.h"
#include "moja/dynamic.h"
#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/itransform.h"
#include "moja/flint/ilandunitdatawrapper.h"

#include <string>
#include <boost/algorithm/string/join.hpp>

namespace moja {
namespace flint {
class IPool;
// --------------------------------------------------------------------------------------------

class FLINT_API VariableAndPoolStringBuilder {
public:
	VariableAndPoolStringBuilder() = default;
	~VariableAndPoolStringBuilder() = default;

	VariableAndPoolStringBuilder(
		const flint::ILandUnitController& landUnitController,
		const std::string& workingStr);

	VariableAndPoolStringBuilder(
		const flint::ILandUnitDataWrapper* landUnitWrapper,
		const std::string& workingStr);

	void configure(
		const flint::ILandUnitController& landUnitController,
		const std::string& workingStr);
	void configure(
		const flint::ILandUnitDataWrapper* landUnitWrapper, 
		const std::string& workingStr);
	std::string result() const;

private:

	static std::vector<std::string> extractTokens(const std::string& tokenType, const std::string& query);
	static void replaceTokens(const std::string& tokenType, std::string& query, std::vector<std::string> values);

	static std::string formatVariableValues(const flint::IVariable& var, const std::string& property);

	const flint::ILandUnitController* _landUnitController;
	const flint::ILandUnitDataWrapper* _landUnitDataWrapper;

	const std::string varMarker = "var";
	const std::string poolMarker = "pool";

	std::string	_workingStr;
	std::vector<std::tuple<const flint::IVariable*, std::string>> _variables;
	std::vector<const IPool*> _pools;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_VARIABLEANDPOOLSTRINGBUILDER_H_