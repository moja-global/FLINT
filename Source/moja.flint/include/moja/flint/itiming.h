#ifndef MOJA_FLINT_ITIMING_H_
#define MOJA_FLINT_ITIMING_H_

#include "moja/datetime.h"

namespace moja {
namespace flint {

enum class TimeStepping {
	Monthly = 0,
	Daily = 1,
	Annual = 3
};

/** <summary>    Timing configuration and state for simulations. </summary> */
class ITiming {
public:
	virtual ~ITiming() = default;

	virtual TimeStepping stepping() const = 0;

    /**
    * <summary>    Prepares the timing object for use.
    *
    *              Generally only the start and end dates are set; calling this
    *              method prepares the other member variables for simulation. </summary>
    */
    virtual void init() = 0;

    /**
     * <summary>    The start date of the simulation. </summary>
     */
    virtual DateTime startDate() const = 0;

    /**
     * <summary>    The end date of the simulation. </summary>
     */
    virtual DateTime endDate() const = 0;

    /**
     * <summary>    The start date of the current timestep. </summary>
     */
    virtual DateTime startStepDate() const = 0;

    /**
     * <summary>    The end date of the current timestep. </summary>
     */
    virtual DateTime endStepDate() const = 0;

    /**
     * <summary>    Same as startStepDate if fracOfStep = 1, else the start date
     *              of the fractional timestep. </summary>
     */
    virtual DateTime curStartDate() const = 0;

    /**
     * <summary>    Same as endStepDate if fracOfStep = 1, else the end date of
     *              the fractional timestep. </summary>
     */
    virtual DateTime curEndDate() const = 0;

    /**
     * <summary>    The fraction of the timestep currently being processed. </summary>
     */
    virtual double fractionOfStep() const = 0;

    /**
     * <summary>    The length of a full timestep in years. </summary>
     */
    virtual double stepLengthInYears() const = 0;

    /**
     * <summary>    The number of timesteps in the simulation. </summary>
     */
    virtual int nSteps() const = 0;

    /**
     * <summary>    The current timestep. </summary>
     */
    virtual int step() const = 0;

    /**
     * <summary>    The current sub-timestep. </summary>
     */
    virtual int subStep() const = 0;

    /**
     * <summary>    True if this is a full timestep. </summary>
     */
    virtual bool isFullStep() const = 0;

    /**
     * <summary>    Sets the simulation start date. </summary>
     */
    virtual void setStartDate(DateTime val) = 0;

    /**
     * <summary>    Sets the simulation end date. </summary>
     */
    virtual void setEndDate(DateTime val) = 0;

    /**
     * <summary>    Sets the current timestep start date. </summary>
     */
    virtual void setStartStepDate(DateTime val) = 0;

    /**
     * <summary>    Sets the current timestep end date. </summary>
     */
    virtual void setEndStepDate(DateTime val) = 0;

    /**
     * <summary>    Sets the current fractional timestep start date. </summary>
     */
    virtual void setCurStartDate(DateTime val) = 0;

    /**
     * <summary>    Sets the current fractional timestep end date. </summary>
     */
    virtual void setCurEndDate(DateTime val) = 0;

    /**
     * <summary>    Sets the fraction of the current timestep being processed. </summary>
     */
    virtual void setFractionOfStep(double val) = 0;

    /**
     * <summary>    Sets the simulation timestep length in years. </summary>
     */
    virtual void setStepLengthInYears(double val) = 0;

    /**
     * <summary>    Sets the current timestep. </summary>
     */
    virtual void setStep(int val) = 0;

    /**
     * <summary>    Sets current sub-timestep. </summary>
     */
    virtual void setSubStep(int val) = 0;

    /**
     * <summary>    Sets whether or not the current timestep is a full timestep (true)
     *              or a fractional timestep (false). </summary>
     */
    virtual void setIsFullStep(bool val) = 0;

    /**
     * <summary>    Sets the stepping period of the simulation. </summary>
     */
    virtual void setStepping(TimeStepping stepping) = 0;
};

}} // namespace moja

#endif // MOJA_FLINT_ITIMING_H_