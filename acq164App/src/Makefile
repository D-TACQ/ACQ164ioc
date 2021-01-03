TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

# Use typed rset structure (see 3.16.1 release notes)
USR_CPPFLAGS += -DUSE_TYPED_RSET

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord

# Install xxxSupport.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Build an IOC support library
LIBRARY_IOC += acq164Support

# Compile and add code to the support library
acq164Support_SRCS += xxxRecord.c
acq164Support_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application, that
# causes problems on Windows DLL builds
acq164Support_SRCS += dbSubExample.c
acq164Support_SRCS += devacq164Version.c
acq164Support_SRCS += acq164Hello.c
acq164Support_SRCS += initTrace.c
acq164Support_SRCS += testAsynPortDriver.cpp

acq164Support_LIBS += $(EPICS_BASE_IOC_LIBS)

# Auto-generate a header file containing a version string.
# Version comes from the VCS if available, else date+time.
GENVERSION = acq164Version.h
# Macro name
GENVERSIONMACRO = acq164VERSION

# Build the IOC application
PROD_IOC = acq164

# acq164.dbd will be created and installed
DBD += acq164.dbd

# acq164.dbd will include these files:
acq164_DBD += base.dbd
acq164_DBD += xxxSupport.dbd
acq164_DBD += dbSubExample.dbd
acq164_DBD += devacq164Version.dbd
acq164_DBD += acq164Hello.dbd
acq164_DBD += initTrace.dbd
acq164_DBD += testAsynPortDriver.dbd

# acq164_registerRecordDeviceDriver.cpp derives from acq164.dbd
acq164_SRCS += acq164_registerRecordDeviceDriver.cpp

# Build the main IOC entry point where needed
acq164_SRCS_DEFAULT += acq164Main.cpp
acq164_SRCS_vxWorks += -nil-

# Link in the code from our support library
acq164_LIBS += acq164Support
acq164_LIBS += stream
acq164_LIBS += asyn

# To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file
ifneq ($(SNCSEQ),)
    # Build sncExample into acq164Support
    sncExample_SNCFLAGS += +r
    acq164_DBD += sncExample.dbd
    # A .stt sequence program is *not* pre-processed:
    acq164Support_SRCS += sncExample.stt
    acq164Support_LIBS += seq pv
    acq164_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    # A .st sequence program *is* pre-processed:
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Link QSRV (pvAccess Server) if available
ifdef EPICS_QSRV_MAJOR_VERSION
    acq164_LIBS += qsrv
    acq164_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)
    acq164_DBD += PVAServerRegister.dbd
    acq164_DBD += qsrv.dbd
endif

# Finally link IOC to the EPICS Base libraries
acq164_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD EXTRA GNUMAKE RULES BELOW HERE

# Explicit dependency needed for generated header file
devacq164Version$(DEP): $(COMMON_DIR)/$(GENVERSION)
