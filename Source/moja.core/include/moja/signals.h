#ifndef MOJA_SIGNALS_H_
#define MOJA_SIGNALS_H_

namespace moja {
namespace signals {

const short SystemInit = 0;
const short SystemShutdown = 1;
const short LocalDomainInit = 2;
const short LocalDomainShutdown = 3;
const short LocalDomainProcessingUnitInit = 4;
const short LocalDomainProcessingUnitShutdown = 5;
const short PreTimingSequence = 6;
const short TimingInit = 7;
const short TimingPrePostInit = 8;
const short TimingPostInit = 9;
const short TimingPostInit2 = 10;
const short TimingShutdown = 11;
const short TimingStep = 12;
const short TimingPreEndStep = 13;
const short TimingEndStep = 14;
const short TimingPostStep = 15;
const short OutputStep = 16;
const short Error = 17;
const short DisturbanceEvent = 18;
const short PrePostDisturbanceEvent = 19;
const short PostDisturbanceEvent = 20;
const short PostDisturbanceEvent2 = 21;
const short PostNotification = 22;
const short Unknown = 23;

}  // namespace signals
}  // namespace moja

#endif  // MOJA_SIGNALS_H_