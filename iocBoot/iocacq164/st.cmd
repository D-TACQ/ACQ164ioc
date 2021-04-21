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

acq164AsynPortDriverConfigure("${UUT}", ${SIZE}, 32)

dbLoadRecords("db/testAsynPortDriver.db","P=${UUT}:,R=1,PORT=${UUT},ADDR=0,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=01,ADDR=0,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=02,ADDR=1,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=03,ADDR=2,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=04,ADDR=3,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=05,ADDR=4,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=06,ADDR=5,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=07,ADDR=6,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=08,ADDR=7,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=09,ADDR=8,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=10,ADDR=9,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=11,ADDR=10,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=12,ADDR=11,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=13,ADDR=12,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=14,ADDR=13,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=15,ADDR=14,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=16,ADDR=15,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=17,ADDR=16,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=18,ADDR=17,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=19,ADDR=18,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=20,ADDR=19,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=21,ADDR=20,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=22,ADDR=21,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=23,ADDR=22,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=24,ADDR=23,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=25,ADDR=24,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=26,ADDR=25,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=27,ADDR=26,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=28,ADDR=27,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=29,ADDR=28,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=30,ADDR=29,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=31,ADDR=30,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynWaveform.db","P=${UUT}:,R=1,PORT=${UUT},CH=32,ADDR=31,TIMEOUT=1,NPOINTS=${SIZE}")
dbLoadRecords("db/asynRecord.db","P=${UUT}:,R=asyn1,PORT=${UUT},ADDR=0,OMAX=80,IMAX=80")


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
