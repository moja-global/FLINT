#pragma once

#include "moja/flint/sequencermodulebase.h"

namespace moja::flint {

// Fixed timing length steps when doing monthly simulations
// Year length is: 365
// Month lenght is: 365/12
class FLINT_API EqualStepSequencer : public SequencerModuleBase {
  public:
   EqualStepSequencer() : _startYear(0), _startStep(0), _endYear(0), _endStep(0), _nSteps(0), StepsPerYear(12) {}

   EqualStepSequencer(int startYear, int startStep, int endYear, int endStep, int nSteps, int stepsPerYear)
       : _startYear(startYear),
         _startStep(startStep),
         _endYear(endYear),
         _endStep(endStep),
         _nSteps(nSteps),
         StepsPerYear(stepsPerYear) {}

   virtual ~EqualStepSequencer() = default;

   bool Run(NotificationCenter& _notificationCenter, ILandUnitController& luc) override;

   void SetStartYear(int val) { _startYear = val; }
   void SetStartStep(int val) { _startStep = val; }
   void SetEndYear(int val) { _endYear = val; }
   void SetEndStep(int val) { _endStep = val; }

  private:
   int _startYear;
   int _startStep;
   int _endYear;
   int _endStep;
   int _nSteps;

   const int StepsPerYear = 12;
};

}  // namespace moja::flint
