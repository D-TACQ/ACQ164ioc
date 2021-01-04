/*
 * testAsynPortDriver.cpp
 *
 * Asyn driver that inherits from the asynPortDriver class to demonstrate its use.
 * It simulates a digital scope looking at a 1kHz 1000-point noisy sine wave.  Controls are
 * provided for time/division, volts/division, volt offset, trigger delay, noise amplitude, update time,
 * and run/stop.
 * Readbacks are provides for the waveform data, min, max and mean values.
 *
 * Author: Mark Rivers
 *
 * Created Feb. 5, 2009
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <iocsh.h>

#include "acq164AsynPortDriver.h"
#include <epicsExport.h>

#define FREQUENCY 1000       /* Frequency in Hz */
#define AMPLITUDE 1.0        /* Plus and minus peaks of sin wave */
#define NUM_DIVISIONS 10     /* Number of scope divisions in X and Y */
#define MIN_UPDATE_TIME 0.02 /* Minimum update time, to prevent CPU saturation */

#define MAX_ENUM_STRING_SIZE 20
static int allVoltsPerDivSelections[NUM_VERT_SELECTIONS]={1,2,5,10};

static const char *driverName="acq164AsynPortDriver";

void task_runner(void *drvPvt)
{
    acq164AsynPortDriver *pPvt = (acq164AsynPortDriver *)drvPvt;

    pPvt->task();
}

/** Constructor for the testAsynPortDriver class.
  * Calls constructor for the asynPortDriver base class.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] maxPoints The maximum  number of points in the volt and time arrays */
acq164AsynPortDriver::acq164AsynPortDriver(const char *portName, int maxPoints, int _nchan)
   : asynPortDriver(portName,
                    _nchan, /* maxAddr */
                    asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynEnumMask | asynDrvUserMask, /* Interface mask */
                    asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynEnumMask,  /* Interrupt mask */
                    0, /* asynFlags.  This driver does not block and it is not multi-device, so flag is 0 */
                    1, /* Autoconnect */
                    0, /* Default priority */
                    0) /* Default stack size*/,
					nchan(_nchan)
{
    asynStatus status;
    int i;

    /* Make sure maxPoints is positive */
    if (maxPoints < 1) maxPoints = 100;

    /* Allocate the waveform array */
    pData_ = (epicsFloat64 *)calloc(maxPoints*nchan, sizeof(epicsFloat64));

    /* Allocate the time base array */
    pTimeBase_ = (epicsFloat64 *)calloc(maxPoints, sizeof(epicsFloat64));
    /* Set the time base array */
    for (i=0; i<maxPoints; i++) pTimeBase_[i] = (double)i / (maxPoints-1) * NUM_DIVISIONS;

    eventId_ = epicsEventCreate(epicsEventEmpty);
    createParam(P_RunString,                asynParamInt32,         &P_Run);
    createParam(P_MaxPointsString,          asynParamInt32,         &P_MaxPoints);
    createParam(P_TimePerDivString,         asynParamFloat64,       &P_TimePerDiv);
    createParam(P_TimePerDivSelectString,   asynParamInt32,         &P_TimePerDivSelect);
    createParam(P_VertGainString,           asynParamFloat64,       &P_VertGain);
    createParam(P_VertGainSelectString,     asynParamInt32,         &P_VertGainSelect);
    createParam(P_VoltsPerDivString,        asynParamFloat64,       &P_VoltsPerDiv);
    createParam(P_VoltsPerDivSelectString,  asynParamInt32,         &P_VoltsPerDivSelect);
    createParam(P_VoltOffsetString,         asynParamFloat64,       &P_VoltOffset);
    createParam(P_TriggerDelayString,       asynParamFloat64,       &P_TriggerDelay);
    createParam(P_NoiseAmplitudeString,     asynParamFloat64,       &P_NoiseAmplitude);
    createParam(P_UpdateTimeString,         asynParamFloat64,       &P_UpdateTime);
    createParam(P_WaveformString,           asynParamFloat64Array,  &P_Waveform);
    createParam(P_TimeBaseString,           asynParamFloat64Array,  &P_TimeBase);
    createParam(P_MinValueString,           asynParamFloat64,       &P_MinValue);
    createParam(P_MaxValueString,           asynParamFloat64,       &P_MaxValue);
    createParam(P_MeanValueString,          asynParamFloat64,       &P_MeanValue);

    for (i=0; i<NUM_VERT_SELECTIONS; i++) {
        // Compute vertical volts per division in mV
        voltsPerDivValues_[i] = 0;
        voltsPerDivStrings_[i] = (char *)calloc(MAX_ENUM_STRING_SIZE, sizeof(char));
        voltsPerDivSeverities_[i] = 0;
    }

    /* Set the initial values of some parameters */
    setIntegerParam(P_MaxPoints,         maxPoints);
    setIntegerParam(P_Run,               0);
    setIntegerParam(P_VertGainSelect,    10);
    setVertGain();
    setDoubleParam (P_VoltsPerDiv,       1.0);
    setDoubleParam (P_VoltOffset,        0.0);
    setDoubleParam (P_TriggerDelay,      0.0);
    setDoubleParam (P_TimePerDiv,        0.001);
    setDoubleParam (P_UpdateTime,        0.5);
    setDoubleParam (P_NoiseAmplitude,    0.1);
    setDoubleParam (P_MinValue,          0.0);
    setDoubleParam (P_MaxValue,          3.3);
    setDoubleParam (P_MeanValue,         0.0);



    /* Create the thread that computes the waveforms in the background */
    status = (asynStatus)(epicsThreadCreate("testAsynPortDriverTask",
                          epicsThreadPriorityMedium,
                          epicsThreadGetStackSize(epicsThreadStackMedium),
						  (EPICSTHREADFUNC)::task_runner,
                          this) == NULL);
    if (status) {
        printf("%s:%s: epicsThreadCreate failure\n", driverName, __FUNCTION__);
        return;
    }
}





/** Called when asyn clients call pasynInt32->write().
  * This function sends a signal to the simTask thread if the value of P_Run has changed.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus acq164AsynPortDriver::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;
    const char *paramName;
    const char* functionName = "writeInt32";

    /* Set the parameter in the parameter library. */
    status = (asynStatus) setIntegerParam(function, value);

    /* Fetch the parameter string name for possible use in debugging */
    getParamName(function, &paramName);

    if (function == P_Run) {
        /* If run was set then wake up the simulation task */
        if (value) epicsEventSignal(eventId_);
    }
    else if (function == P_VertGainSelect) {
        setVertGain();
    }
    else if (function == P_VoltsPerDivSelect) {
        setVoltsPerDiv();
    }
    else if (function == P_TimePerDivSelect) {
        setTimePerDiv();
    }
    else {
        /* All other parameters just get set in parameter list, no need to
         * act on them here */
    }

    /* Do callbacks so higher layers see any changes */
    status = (asynStatus) callParamCallbacks();

    if (status)
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, name=%s, value=%d",
                  driverName, functionName, status, function, paramName, value);
    else
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d, name=%s, value=%d\n",
              driverName, functionName, function, paramName, value);
    return status;
}

/** Called when asyn clients call pasynFloat64->write().
  * This function sends a signal to the simTask thread if the value of P_UpdateTime has changed.
  * For all  parameters it  sets the value in the parameter library and calls any registered callbacks.
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus acq164AsynPortDriver::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;
    epicsInt32 run;
    const char *paramName;
    const char* functionName = "writeFloat64";

    /* Set the parameter in the parameter library. */
    status = (asynStatus) setDoubleParam(function, value);

    /* Fetch the parameter string name for possible use in debugging */
    getParamName(function, &paramName);

    if (function == P_UpdateTime) {
        /* Make sure the update time is valid. If not change it and put back in parameter library */
        if (value < MIN_UPDATE_TIME) {
            asynPrint(pasynUser, ASYN_TRACE_WARNING,
                "%s:%s: warning, update time too small, changed from %f to %f\n",
                driverName, functionName, value, MIN_UPDATE_TIME);
            value = MIN_UPDATE_TIME;
            setDoubleParam(P_UpdateTime, value);
        }
        /* If the update time has changed and we are running then wake up the simulation task */
        getIntegerParam(P_Run, &run);
        if (run) epicsEventSignal(eventId_);
    } else {
        /* All other parameters just get set in parameter list, no need to
         * act on them here */
    }

    /* Do callbacks so higher layers see any changes */
    status = (asynStatus) callParamCallbacks();

    if (status)
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, name=%s, value=%f",
                  driverName, functionName, status, function, paramName, value);
    else
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d, name=%s, value=%f\n",
              driverName, functionName, function, paramName, value);
    return status;
}


/** Called when asyn clients call pasynFloat64Array->read().
  * Returns the value of the P_Waveform or P_TimeBase arrays.
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Pointer to the array to read.
  * \param[in] nElements Number of elements to read.
  * \param[out] nIn Number of elements actually read. */
asynStatus acq164AsynPortDriver::readFloat64Array(asynUser *pasynUser, epicsFloat64 *value,
                                         size_t nElements, size_t *nIn)
{
    int function = pasynUser->reason;
    size_t ncopy;
    epicsInt32 itemp;
    asynStatus status = asynSuccess;
    epicsTimeStamp timeStamp;
    const char *functionName = "readFloat64Array";

    getTimeStamp(&timeStamp);
    pasynUser->timestamp = timeStamp;
    getIntegerParam(P_MaxPoints, &itemp); ncopy = itemp;
    if (nElements < ncopy) ncopy = nElements;
    if (function == P_Waveform) {
    	assert(0);
        *nIn = ncopy;
    }
    else if (function == P_TimeBase) {
        memcpy(value, pTimeBase_, ncopy*sizeof(epicsFloat64));
        *nIn = ncopy;
    }
    if (status)
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d",
                  driverName, functionName, status, function);
    else
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d\n",
              driverName, functionName, function);
    return status;
}

asynStatus acq164AsynPortDriver::readEnum(asynUser *pasynUser, char *strings[], int values[], int severities[], size_t nElements, size_t *nIn)
{
    int function = pasynUser->reason;
    size_t i;

    if (function == P_VoltsPerDivSelect) {
        for (i=0; ((i<NUM_VERT_SELECTIONS) && (i<nElements)); i++) {
            if (strings[i]) free(strings[i]);
            strings[i] = epicsStrDup(voltsPerDivStrings_[i]);
            values[i] = voltsPerDivValues_[i];
            severities[i] = 0;
        }
    }
    else {
        *nIn = 0;
        return asynError;
    }
    *nIn = i;
    return asynSuccess;
}

void acq164AsynPortDriver::setVertGain()
{
    epicsInt32 igain, i;
    double gain;

    getIntegerParam(P_VertGainSelect, &igain);
    gain = igain;
    setDoubleParam(P_VertGain, gain);
    for (i=0; i<NUM_VERT_SELECTIONS; i++) {
        epicsSnprintf(voltsPerDivStrings_[i], MAX_ENUM_STRING_SIZE, "%.2f", allVoltsPerDivSelections[i] / gain);
        // The values are in mV
        voltsPerDivValues_[i] = (int)(allVoltsPerDivSelections[i] / gain * 1000. + 0.5);
    }
    doCallbacksEnum(voltsPerDivStrings_, voltsPerDivValues_, voltsPerDivSeverities_, NUM_VERT_SELECTIONS, P_VoltsPerDivSelect, 0);
}

void acq164AsynPortDriver::setVoltsPerDiv()
{
    epicsInt32 mVPerDiv;

    // Integer volts are in mV
    getIntegerParam(P_VoltsPerDivSelect, &mVPerDiv);
    setDoubleParam(P_VoltsPerDiv, mVPerDiv / 1000.);
}

void acq164AsynPortDriver::setTimePerDiv()
{
    epicsInt32 microSecPerDiv;

    // Integer times are in microseconds
    getIntegerParam(P_TimePerDivSelect, &microSecPerDiv);
    setDoubleParam(P_TimePerDiv, microSecPerDiv / 1000000.);
}


//#include "local.h"
#include "acq2xx_api.h"
#include "acq_transport.h"
#include "AcqType.h"
#include "Frame.h"

#include "DataStreamer.h"
#include "DirfileFrameHandler.h"

class Acq164Device: public acq164AsynPortDriver, FrameHandler {
	int cursor;
	virtual void onFrame(
			Acq2xx& _card, const AcqType& _acqType,
			const Frame* frame);

public:
	Acq164Device(const char *portName, int maxArraySize, int nchan) :
		acq164AsynPortDriver(portName, maxArraySize, nchan),
		cursor(0)
	{}
	virtual void task();
};

class SimDevice: public acq164AsynPortDriver {
	int cursor;


public:
	SimDevice(const char *portName, int maxArraySize, int nchan) :
		acq164AsynPortDriver(portName, maxArraySize, nchan),
		cursor(0)
	{}
	virtual void task();
};


void Acq164Device::onFrame(
		Acq2xx& _card, const AcqType& _acqType,
		const Frame* frame)
{
	const ConcreteFrame<int> *cf =
				dynamic_cast<const ConcreteFrame<int> *>(frame);
	const int maxPoints = get_maxPoints();

	for (int ic = 0; ic < nchan; ++ic){
		int ix0 = ic*maxPoints;
		int consecutive_zeros = 0;
		const int* raw = cf->getChannel(ic+1);
		for (int id = 0; id < 64; ++id){
			int yy = raw[id];
			if (yy == 0){
				if (++consecutive_zeros > 60){
					printf("%s zeros detected at %lld\n", __FUNCTION__, cf->getStartSampleNumber());
					exit(1);
				}
			}
			double volts = 10.0*yy / (1<<24);
			pData_[ix0+cursor+id] = volts;
		}
	}
	cursor += 64;
	if (cursor >= maxPoints){
		//printf("%s %lld\n", __FUNCTION__, cf->getStartSampleNumber());
		setDoubleParam(P_UpdateTime, cf->getStartSampleNumber());
		callParamCallbacks();

		for (int ic=0; ic< nchan; ic++){
			int ix0 = ic*maxPoints;
		  	doCallbacksFloat64Array(pData_+ix0, maxPoints, P_Waveform, ic);
		}
		cursor = 0;
	}
}



void Acq164Device::task(void)
{
	Transport *t = Transport::getTransport(portName);
	Acq2xx card(t);

	DataStreamer* dataStreamer = DataStreamer::create(
				card, AcqType::getAcqType(card));

	dataStreamer->addFrameHandler(this);
/*
	dataStreamer->addFrameHandler(
				DataStreamer::createMeanHandler(
					AcqType::getAcqType(card), 1));
	dataStreamer->addFrameHandler(
				DataStreamer::createNewlineHandler());
*/
	dataStreamer->streamData();
}
/** Simulation task that runs as a separate thread.  When the P_Run parameter is set to 1
  * to rub the simulation it computes a 1 kHz sine wave with 1V amplitude and user-controllable
  * noise, and displays it on
  * a simulated scope.  It computes waveforms for the X (time) and Y (volt) axes, and computes
  * statistics about the waveform. */
void SimDevice::task(void)
{
    /* This thread computes the waveform and does callbacks with it */

    double timePerDiv, voltsPerDiv, voltOffset, triggerDelay, noiseAmplitude;
    double updateTime, minValue, maxValue, meanValue;
    double time, timeStep;
    double noise, yScale;
    epicsInt32 run, i, maxPoints;
    double pi=4.0*atan(1.0);

    lock();
    /* Loop forever */
    while (1) {
        getDoubleParam(P_UpdateTime, &updateTime);
        getIntegerParam(P_Run, &run);
        // Release the lock while we wait for a command to start or wait for updateTime
        unlock();
        if (run) epicsEventWaitWithTimeout(eventId_, updateTime);
        else     (void) epicsEventWait(eventId_);
        // Take the lock again
        lock();
        /* run could have changed while we were waiting */
        getIntegerParam(P_Run, &run);
        if (!run) continue;
        getIntegerParam(P_MaxPoints,        &maxPoints);
        getDoubleParam (P_TimePerDiv,       &timePerDiv);
        getDoubleParam (P_VoltsPerDiv,      &voltsPerDiv);
        getDoubleParam (P_VoltOffset,       &voltOffset);
        getDoubleParam (P_TriggerDelay,     &triggerDelay);
        getDoubleParam (P_NoiseAmplitude,   &noiseAmplitude);
        time = triggerDelay;
        timeStep = timePerDiv * NUM_DIVISIONS / maxPoints;
        minValue = 1e6;
        maxValue = -1e6;
        meanValue = 0.;


        yScale = 1.0 / voltsPerDiv;
        for (i=0; i<maxPoints; i++) {

        	double ft = sin(time*FREQUENCY*2*pi);
        	for (int c=0; c<nchan; c++){
        		int ix = c*maxPoints + i;

        		noise = noiseAmplitude * (rand()/(double)RAND_MAX - 0.5);
        		pData_[ix] = c+ AMPLITUDE *ft  + noise;
        		/* Compute statistics before doing the yOffset and yScale */
        		if (pData_[ix] < minValue) minValue = pData_[ix];
        		if (pData_[ix] > maxValue) maxValue = pData_[ix];
        		if (c==0) meanValue += pData_[ix];
        		pData_[ix] = NUM_DIVISIONS/2 + yScale * (voltOffset + pData_[ix]);
        	}
            time += timeStep;
        }
        updateTimeStamp();
        meanValue = meanValue/maxPoints;
        setDoubleParam(P_MinValue, minValue);
        setDoubleParam(P_MaxValue, maxValue);
        setDoubleParam(P_MeanValue, meanValue);

        callParamCallbacks();
        for (int c=0; c<nchan; c++){
        	doCallbacksFloat64Array(pData_+c*maxPoints, maxPoints, P_Waveform, c);
        }
    }
}

int acq164AsynPortDriver::factory(const char *portName, int maxPoints, int nchan)
{
	   if (::getenv("SIM") != 0 && atoi(::getenv("SIM")) != 0){
	    	new SimDevice(portName, maxPoints, nchan);
	    /* else ACQ400? */
	   }else{
	    	new Acq164Device(portName, maxPoints, nchan);
	   }

	    return(asynSuccess);
}

/* Configuration routine.  Called directly, or from the iocsh function below */

extern "C" {

/** EPICS iocsh callable function to call constructor for the testAsynPortDriver class.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] maxPoints The maximum  number of points in the volt and time arrays */
int acq164AsynPortDriverConfigure(const char *portName, int maxPoints, int nchan)
{
	return acq164AsynPortDriver::factory(portName, maxPoints, nchan);
}


/* EPICS iocsh shell commands */

static const iocshArg initArg0 = { "portName",iocshArgString};
static const iocshArg initArg1 = { "max points",iocshArgInt};
static const iocshArg initArg2 = { "max chan",iocshArgInt};
static const iocshArg * const initArgs[] = {&initArg0, &initArg1, &initArg2};
static const iocshFuncDef initFuncDef = {"acq164AsynPortDriverConfigure",3,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
	acq164AsynPortDriverConfigure(args[0].sval, args[1].ival, args[2].ival);
}

void acq164AsynPortDriverRegister(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

epicsExportRegistrar(acq164AsynPortDriverRegister);

}

