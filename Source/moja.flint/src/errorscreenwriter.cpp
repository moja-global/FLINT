#include "moja/flint/errorscreenwriter.h"

#include <moja/flint/ivariable.h>
#include <moja/flint/spatiallocationinfo.h>

#include <moja/logging.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

void ErrorScreenWriter::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::LocalDomainInit, &ErrorScreenWriter::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::Error, &ErrorScreenWriter::onError, *this);
}

// --------------------------------------------------------------------------------------------

void ErrorScreenWriter::onLocalDomainInit() {
   _spatiallocationinfo = std::static_pointer_cast<flint::SpatialLocationInfo>(
       _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
}

// --------------------------------------------------------------------------------------------

void ErrorScreenWriter::onError(std::string msg) {
   if (_spatiallocationinfo != nullptr) {
      boost::replace_all(msg, "\n", " ");
      auto errorMsg =
          (boost::format("Libarry %5%, Module %6% - Error at location [tileIdx %1%, blockIdx %2%, cellIdx %3%], random "
                         "seed [global %8%, block %9%, cell %10%], code %4% : Message - %7%") %
           _spatiallocationinfo->_tileIdx % _spatiallocationinfo->_blockIdx % _spatiallocationinfo->_cellIdx %
           _spatiallocationinfo->_errorCode % _spatiallocationinfo->_library % _spatiallocationinfo->_module % msg %
           _spatiallocationinfo->_randomSeedGlobal % _spatiallocationinfo->_randomSeedBlock %
           _spatiallocationinfo->_randomSeedCell)
              .str();
      MOJA_LOG_ERROR << errorMsg;
   } else {
      boost::replace_all(msg, "\n", " ");
      auto errorMsg = (boost::format("Error: %1%") % msg).str();
      MOJA_LOG_ERROR << errorMsg;
   }
}

}  // namespace flint
}  // namespace moja
