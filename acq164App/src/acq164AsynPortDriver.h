/* ------------------------------------------------------------------------- */
/* acq164asynPortDriver.h
 * Project: ACQ420_FMC
 * Created: Thu Dec 31 15:16:04 2020                      / User: pgm
 * ------------------------------------------------------------------------- *
 *   Copyright (C) 2020/2021 Peter Milne, D-TACQ Solutions Ltd         *
 *                      <peter dot milne at D hyphen TACQ dot com>           *
 *                                                                           *
 *  This program is free software; you can redistribute it and/or modify     *
 *  it under the terms of Version 2 of the GNU General Public License        *
 *  as published by the Free Software Foundation;                            *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program; if not, write to the Free Software              *
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                *
 *
 * TODO
 * TODO
\* ------------------------------------------------------------------------- */

/*
 * testAsynPortDriver.h
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

#include "asynPortDriver.h"

int acq200_debug;

#define NUM_VERT_SELECTIONS 4


/* These are the drvInfo strings that are used to identify the parameters.
 * They are used by asyn clients, including standard asyn device support */
#define P_RunString                "SCOPE_RUN"                  /* asynInt32,    r/w */
#define P_MaxPointsString          "SCOPE_MAX_POINTS"           /* asynInt32,    r/o */
#define P_NoiseAmplitudeString     "SCOPE_NOISE_AMPLITUDE"      /* asynFloat64,  r/w */
#define P_UpdateTimeString         "SCOPE_UPDATE_TIME"          /* asynFloat64,  r/w */
#define P_WaveformString           "SCOPE_WAVEFORM"             /* asynFloat64Array,  r/o */
#define P_AIString           	   "SCOPE_SCALAR"               /* asynFloat64,  r/o */
#define P_TimeBaseString           "SCOPE_TIME_BASE"            /* asynFloat64Array,  r/o */
#define P_MinValueString           "SCOPE_MIN_VALUE"            /* asynFloat64,  r/o */
#define P_MaxValueString           "SCOPE_MAX_VALUE"            /* asynFloat64,  r/o */
#define P_MeanValueString          "SCOPE_MEAN_VALUE"           /* asynFloat64,  r/o */

/** Class that demonstrates the use of the asynPortDriver base class to greatly simplify the task
  * of writing an asyn port driver.
  * This class does a simple simulation of a digital oscilloscope.  It computes a waveform, computes
  * statistics on the waveform, and does callbacks with the statistics and the waveform data itself.
  * I have made the methods of this class public in order to generate doxygen documentation for them,
  * but they should really all be private. */
class acq164AsynPortDriver : public asynPortDriver {
public:
    acq164AsynPortDriver(const char *portName, int maxArraySize, int nchan);

    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus readFloat64Array(asynUser *pasynUser, epicsFloat64 *value,
                                        size_t nElements, size_t *nIn);
    virtual asynStatus readEnum(asynUser *pasynUser, char *strings[], int values[], int severities[],
                                size_t nElements, size_t *nIn);

    virtual void task() = 0;

    static int factory(const char *portName, int maxPoints, int nchan);

protected:
    /** Values used for pasynUser->reason, and indexes into the parameter library. */
    int P_Run;
    int P_MaxPoints;

    int P_NoiseAmplitude;
    int P_UpdateTime;
    int P_Waveform;
    int P_TimeBase;
    int P_MinValue;
    int P_MaxValue;
    int P_MeanValue;

    /* Our data */
    epicsEventId eventId_;
    epicsFloat64 *pTimeBase_;

    int nchan;
    epicsFloat64 *pData_;

    int get_maxPoints() {
    	int maxPoints;
    	getIntegerParam(P_MaxPoints,        &maxPoints);
    	return maxPoints;
    }
};
