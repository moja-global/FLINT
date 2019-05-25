#ifndef MOJA_CORE_PUBLISHER_H_
#define MOJA_CORE_PUBLISHER_H_

#include "_core_exports.h"
#include "datetime.h"

#include <memory>
#include <vector>

namespace moja {

class IMessage {
  public:
   virtual ~IMessage() {}
};

class TimingInitMessage : public IMessage {
  public:
   TimingInitMessage(DateTime startDate, int nSteps, int startYear, int startStep, int endYear, int endStep,
                     int stepsPerYr)
       : _startDate(startDate),
         _nSteps(nSteps),
         _startYear(startYear),
         _startStep(startStep),
         _endYear(endYear),
         _endStep(endStep),
         _stepsPerYr(stepsPerYr) {}

   int nSteps() const;
   int startYear() const;
   int startStep() const;
   int endYear() const;
   int endStep() const;
   int stepsPerYr() const;

  private:
   DateTime _startDate;
   int _nSteps;
   int _startYear;
   int _startStep;
   int _endYear;
   int _endStep;
   int _stepsPerYr;
};

inline int TimingInitMessage::nSteps() const { return _nSteps; }
inline int TimingInitMessage::startYear() const { return _startYear; }
inline int TimingInitMessage::startStep() const { return _startStep; }
inline int TimingInitMessage::endYear() const { return _endYear; }
inline int TimingInitMessage::endStep() const { return _endStep; }
inline int TimingInitMessage::stepsPerYr() const { return _stepsPerYr; }

class TimingStepStartMessage : public IMessage {
  public:
   TimingStepStartMessage(int step, int numberOfSteps, double stepLengthInYears, DateTime stepTime, DateTime startDate,
                          DateTime endDate)
       : _step(step),
         _numberOfSteps(numberOfSteps),
         _stepLengthInYears(stepLengthInYears),
         _startDate(startDate),
         _endDate(endDate),
         _stepTime(stepTime) {}

   int step() const;
   int numberOfSteps() const;
   double stepLengthInYears() const;
   DateTime startDate() const;
   DateTime endDate() const;
   DateTime stepTime() const;

  private:
   int _step;
   int _numberOfSteps;
   double _stepLengthInYears;
   DateTime _startDate;
   DateTime _endDate;
   DateTime _stepTime;
};

inline int TimingStepStartMessage::step() const { return _step; }
inline int TimingStepStartMessage::numberOfSteps() const { return _numberOfSteps; }
inline double TimingStepStartMessage::stepLengthInYears() const { return _stepLengthInYears; }
inline DateTime TimingStepStartMessage::startDate() const { return _startDate; }
inline DateTime TimingStepStartMessage::endDate() const { return _endDate; }
inline DateTime TimingStepStartMessage::stepTime() const { return _stepTime; }

class TimingStepEndMessage : public IMessage {
  public:
   TimingStepEndMessage(int step, int numberOfSteps, double stepLengthInYears, DateTime stepTime, DateTime startDate,
                        DateTime endDate)
       : _step(step),
         _numberOfSteps(numberOfSteps),
         _stepLengthInYears(stepLengthInYears),
         _startDate(startDate),
         _endDate(endDate),
         _stepTime(stepTime) {}

   int step() const;
   int numberOfSteps() const;
   double stepLengthInYears() const;
   DateTime startDate() const;
   DateTime endDate() const;
   DateTime stepTime() const;

  private:
   int _step;
   int _numberOfSteps;
   double _stepLengthInYears;
   DateTime _startDate;
   DateTime _endDate;
   DateTime _stepTime;
};

inline int TimingStepEndMessage::step() const { return _step; }
inline int TimingStepEndMessage::numberOfSteps() const { return _numberOfSteps; }
inline double TimingStepEndMessage::stepLengthInYears() const { return _stepLengthInYears; }
inline DateTime TimingStepEndMessage::startDate() const { return _startDate; }
inline DateTime TimingStepEndMessage::endDate() const { return _endDate; }
inline DateTime TimingStepEndMessage::stepTime() const { return _stepTime; }

class IRunnable {
  public:
   virtual void Run() = 0;
};

class ISubscriber {
  public:
   virtual void ReceiveMessage(const std::shared_ptr<IMessage>& message) = 0;
};

class IPublisher {
  public:
   virtual void AddSubscriber(std::shared_ptr<ISubscriber> subscriber) = 0;
   virtual void SendMessage(std::shared_ptr<IMessage> message) const = 0;
};

class Publisher : public IPublisher {
   std::vector<std::shared_ptr<ISubscriber>> _subscribers;

  public:
   void AddSubscriber(std::shared_ptr<ISubscriber> subscriber);
   void SendMessage(std::shared_ptr<IMessage> message) const;
};

}  // namespace moja
#endif  // MOJA_CORE_PUBLISHER_H_