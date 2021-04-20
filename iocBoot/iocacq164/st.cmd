#!../../bin/linux-x86_64/acq164

#- You may have to change acq164 to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/acq164.dbd"
acq164_registerRecordDeviceDriver pdbbase

# Turn on asynTraceFlow and asynTraceError for global trace, i.e. no connected asynUser.
asynSetTraceMask("", 0, 17)

acq164AsynPortDriverConfigure("acq164_080", 1024, 8)

dbLoadRecords("db/testAsynPortDriver.db","P=acq164_080:,R=1,PORT=acq164_080,ADDR=0,TIMEOUT=1,NPOINTS=1000")
dbLoadRecords("db/asynWaveform.db","P=acq164_080:,R=1,PORT=acq164_080,CH=01,ADDR=0,TIMEOUT=1,NPOINTS=1000")
dbLoadRecords("db/asynWaveform.db","P=acq164_080:,R=1,PORT=acq164_080,CH=02,ADDR=1,TIMEOUT=1,NPOINTS=1000")
dbLoadRecords("db/asynWaveform.db","P=acq164_080:,R=1,PORT=acq164_080,CH=03,ADDR=2,TIMEOUT=1,NPOINTS=1000")
dbLoadRecords("db/asynWaveform.db","P=acq164_080:,R=1,PORT=acq164_080,CH=04,ADDR=3,TIMEOUT=1,NPOINTS=1000")
dbLoadRecords("db/asynWaveform.db","P=acq164_080:,R=1,PORT=acq164_080,CH=05,ADDR=4,TIMEOUT=1,NPOINTS=1000")
dbLoadRecords("db/asynWaveform.db","P=acq164_080:,R=1,PORT=acq164_080,CH=06,ADDR=5,TIMEOUT=1,NPOINTS=1000")
dbLoadRecords("db/asynWaveform.db","P=acq164_080:,R=1,PORT=acq164_080,CH=07,ADDR=6,TIMEOUT=1,NPOINTS=1000")
dbLoadRecords("db/asynWaveform.db","P=acq164_080:,R=1,PORT=acq164_080,CH=08,ADDR=7,TIMEOUT=1,NPOINTS=1000")
dbLoadRecords("db/asynRecord.db","P=acq164_080:,R=asyn1,PORT=acq164_080,ADDR=0,OMAX=80,IMAX=80")


## Load record instances
dbLoadTemplate "db/user.substitutions"
dbLoadRecords "db/acq164Version.db", "user=pgm"
dbLoadRecords "db/dbSubExample.db", "user=pgm"

#- Set this to see messages from mySub
#var mySubDebug 1

#- Run this to trace the stages of iocInit
#traceIocInit

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncExample, "user=pgm"
