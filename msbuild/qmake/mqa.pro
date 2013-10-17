QT       -= core gui

TARGET = mqa
TEMPLATE = lib
CONFIG += sharedlib

DEFINES += MQA_EXPORTS

win32 {

CONFIG(debug, debug|release) : DESTDIR = $$PWD/../bin/x64/debug
else: CONFIG(release, debug|release) : DESTDIR = $$PWD/../bin/x64/release

#libary boost
LIBS += -LD:/onprogram/clib/boost/lib/x64
INCLUDEPATH += D:/onprogram/clib/boost/include

LIBS += -lws2_32

} else { #linux -------------------

CONFIG(debug, debug|release) : DESTDIR = $$PWD/../bin-linux/x64/debug
else: CONFIG(release, debug|release) : DESTDIR = $$PWD/../bin-linux/x64/release

#libary boost
CONFIG(debug, debug|release) : LIBS += -L/program/linux/usr/boost/lib-x64-debug
else: CONFIG(release, debug|release) : LIBS += -L/program/linux/usr/boost/lib-x64-release
INCLUDEPATH += /program/linux/usr/boost/include

}  # linux

INCLUDEPATH += ../../include
INCLUDEPATH += ../../AnalyzerCommon


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
    ../../include/mqa_global.h
           
