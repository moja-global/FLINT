#ifndef MOJA_FLINT_TIMING_H_
#define MOJA_FLINT_TIMING_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/itiming.h"

#include <moja/floatcmp.h>

namespace moja {
namespace flint {

class FLINT_API Timing : public ITiming {
  public:
   Timing()
       : _fractionOfStep(0),
         _stepLengthInYears(0),
         _nSteps(0),
         _step(0),
         _subStep(0),
         _isFullStep(true),
         _stepping(TimeStepping::Monthly) {}
   ~Timing() = default;
   void init() override;

   DateTime startDate() const override;  // sim start date
   DateTime endDate() const override;    // sim end date

   DateTime startStepDate() const override;  // current step start date
   DateTime endStepDate() const override;    // current step end date

   DateTime curStartDate() const override;  // if fracOfStep = 1 same as startStepDate, else new start date
   DateTime curEndDate() const override;    // if fracOfStep = 1 same as endStepDate, else new end date

   double fractionOfStep() const override;     // fraction of Step we are processing
   double stepLengthInYears() const override;  // how long is a full step in years

   int nSteps() const override;   // number of steps in simulation
   int step() const override;     // current step number
   int subStep() const override;  // current sub step number

   bool isFullStep() const override;  // is this a full step
   TimeStepping stepping() const override;

   void setStartDate(DateTime startDate) override;
   void setEndDate(DateTime endDate) override;
   void setStartStepDate(DateTime val) override;
   void setEndStepDate(DateTime val) override;
   void setCurStartDate(DateTime val) override;
   void setCurEndDate(DateTime val) override;
   void setFractionOfStep(double fractionOfStep) override;
   void setStepLengthInYears(double stepLengthInYears) override;
   void setStep(int step) override;
   void setSubStep(int subStep) override;
   void setIsFullStep(bool val) override;
   void setStepping(TimeStepping stepping) override;

  private:
   DateTime _startDate;
   DateTime _endDate;
   DateTime _startStepDate;
   DateTime _endStepDate;
   DateTime _curStartDate;
   DateTime _curEndDate;
   double _fractionOfStep;
   double _stepLengthInYears;
   int _nSteps;
   int _step;
   int _subStep;
   bool _isFullStep;
   TimeStepping _stepping;
};

inline DateTime Timing::startDate() const { return _startDate; }
inline DateTime Timing::endDate() const { return _endDate; }

inline DateTime Timing::startStepDate() const { return _startStepDate; }
inline DateTime Timing::endStepDate() const { return _endStepDate; }

inline DateTime Timing::curStartDate() const { return _curStartDate; }
inline DateTime Timing::curEndDate() const { return _curEndDate; }

inline double Timing::fractionOfStep() const { return _fractionOfStep; }
inline double Timing::stepLengthInYears() const { return _stepLengthInYears; }

inline int Timing::nSteps() const {
   switch (_stepping) {
      case TimeStepping::Daily: {
         auto span = _startDate - _endDate;
         return span.days() + 1;
      }
      case TimeStepping::Annual:
         return _endDate.year() - _startDate.year() + 1;
      case TimeStepping::Monthly:
         return 1 + (_endDate.year() - _startDate.year()) * 12 + _endDate.month() - _startDate.month() + 1;
   }
   return 0;
}

inline int Timing::step() const { return _step; }
inline int Timing::subStep() const { return _subStep; }
inline bool Timing::isFullStep() const { return _isFullStep; }
inline TimeStepping Timing::stepping() const { return _stepping; }

inline void Timing::setStartDate(DateTime val) { _startDate = val; }
inline void Timing::setEndDate(DateTime val) { _endDate = val; }
inline void Timing::setStartStepDate(DateTime val) { _startStepDate = val; }
inline void Timing::setEndStepDate(DateTime val) { _endStepDate = val; }
inline void Timing::setCurStartDate(DateTime value) { _curStartDate = value; }
inline void Timing::setCurEndDate(DateTime value) { _curEndDate = value; }
inline void Timing::setFractionOfStep(double val) {
   _fractionOfStep = val;
   _isFullStep = FloatCmp::equalTo(_fractionOfStep, 1.0) ? true : false;
}
inline void Timing::setStepLengthInYears(double val) { _stepLengthInYears = val; }
inline void Timing::setStep(int val) { _step = val; }
inline void Timing::setSubStep(int val) { _subStep = val; }
inline void Timing::setIsFullStep(bool val) { _isFullStep = val; }
inline void Timing::setStepping(TimeStepping stepping) { _stepping = stepping; }

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_TIMING_H_