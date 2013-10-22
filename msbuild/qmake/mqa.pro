QT       -= core gui

TARGET = mqa
TEMPLATE = lib
CONFIG += sharedlib

#------------- arch(x64/x86), buildmode(debug/release) -----------
ARCH=x64

contains(QMAKE_HOST.arch, x86_64):      ARCH=x64
contains(QMAKE_HOST.arch, x86):         ARCH=x86

contains(QMAKE_TARGET.arch, x86):       ARCH=x86
contains(QMAKE_TARGET.arch, x86_64):    ARCH=x64

linux-g++-32:                           ARCH=x86
linux-g++-64:                           ARCH=x64

CONFIG(debug, debug|release) :          BUILDMODE=debug
CONFIG(release, debug|release) :        BUILDMODE=release

#----------------- Flags ---------------------
DEFINES += MQA_EXPORTS
DEFINES += BOOST_ALL_DYN_LINK

#----------------- use libraries ---------------------

# DESTDIR
DESTDIR = $$PWD/../../lib/$$ARCH/$$BUILDMODE

win32 {
#    QMAKE_CXXFLAGS_RELEASE += /GS-
    # library boost path
    !BOOSTDIR:    BOOSTDIR = D:\onprogram\clib\boost
    LIBS += -lws2_32
} else { #linux -------------------
    # library boost path
    !BOOSTDIR:    BOOSTDIR = /home/jiezhang/test/boost/boost-gcc-shared
}  # linux

# library boost
LIBS += -L$$BOOSTDIR/lib/$$ARCH/$$BUILDMODE
INCLUDEPATH += $$BOOSTDIR/include

# library AnalyzerCommon
!ANALYZERCOMMONDIR: ANALYZERCOMMONDIR = ../../AnalyzerCommon
INCLUDEPATH += $$ANALYZERCOMMONDIR

INCLUDEPATH += ../../include


SOURCES += \
    ../../src/VQStatsRtsmDumper.cpp \
    ../../src/VQStatsKeyMap.cpp \
    ../../src/ThreadPool.cpp \
    ../../src/StatsFrameParser.cpp \
    ../../src/RtpTypes.cpp \
    ../../src/RtpStream.cpp \
    ../../src/RtpPacketParser.cpp \
    ../../src/MQmonIf.cpp \
    ../../src/CalculateRFactor.cpp \
    ../../src/AnalyzerImpl.cpp

HEADERS += \
    ../../include/VQStatsRtsmDumper.h \
    ../../include/VQStatsKeyMap.h \
    ../../include/timesec.h \
    ../../include/ThreadPool.h \
    ../../include/StatsFrameParser.h \
    ../../include/RtpTypes.h \
    ../../include/RtpStreamManager.h \
    ../../include/RtpStream.h \
    ../../include/RtpPacketParser.h \
    ../../include/PacketParser.h \
    ../../include/MQmonIf.h \
    ../../include/mqainterface.h \
    ../../include/MqaAssert.h \
    ../../include/mqa_shared.h \
    ../../include/mqa_global.h \
    ../../include/mqa_flowkeys.h \
    ../../include/mqa_flowentries.h \
    ../../include/classifyconsts.h \
    ../../include/CalculateRFactor.h \
    ../../include/AnalyzerImpl.h \
    ../../include/mqa_global.h \
    ../../include/linuxbasetsd.h
           
