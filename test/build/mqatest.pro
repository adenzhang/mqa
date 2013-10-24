TARGET = mqatest
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
QT       -= core gui


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
DEFINES += BOOST_ALL_DYN_LINK

#----------------- use libraries ---------------------
# DESTDIR
DESTDIR = $$PWD/../../lib/$$ARCH/$$BUILDMODE

# libary mqa
#MQA_LIBDIR = $$PWD/../../msbuild/lib/$$ARCH/$$BUILDMODE
MQA_LIBDIR = $$PWD/../../lib/$$ARCH/$$BUILDMODE
LIBS += -L$$MQA_LIBDIR -lmqa

win32 {
#    QMAKE_CXXFLAGS_RELEASE += /GS-
    # libary boost path
    !BOOSTDIR:    BOOSTDIR = D:\onprogram\clib\boost

    LIBS += -lws2_32
}else { #--- linux -------------------
    # libary boost path
    !BOOSTDIR:    BOOSTDIR = /home/jiezhang/test/boost/boost-gcc-shared

    LIBS += -lboost_system -lboost_program_options -lboost_thread
}  # linux

# libary boost
LIBS += -L$$BOOSTDIR/lib/$$ARCH/$$BUILDMODE
INCLUDEPATH += $$BOOSTDIR/include

#-------------- copy files ----------------

FILES2DESTDIR += \
    $$MQA_LIBDIR/mqa.* \

win32 {
    DESTDIR_WIN = $${DESTDIR}
    DESTDIR_WIN ~= s,/,\,g
    PWD_WIN = $${PWD}
    PWD_WIN ~= s,/,\,g
    for(FILE, $$FILES2DESTDIR){
        QMAKE_POST_LINK += $$quote(cmd /c copy /y $${PWD_WIN}/$${FILE} $${DESTDIR_WIN}$$escape_expand(\\n\\t))
    }
} else { #-- linux
    for(FILE, $$FILES2DESTDIR){
        QMAKE_POST_LINK += $$quote(cp $${PWD}/$${FILE} $${DESTDIR}$$escape_expand(\\n\\t))
#        $$QMAKE_COPY $$quote($$FILE) $$quote($$DESTDIR) $$escape_expand(\\n\\t)
    }
}

#---------------------------------------------

INCLUDEPATH += ../../include
INCLUDEPATH += ../../AnalyzerCommon


SOURCES += \
           ../src/LogUtility.cpp \
           ../src/PCapReader.cpp \
           ../src/mqatest.cpp \

HEADERS += \
           ../src/LogUtility.h \
           ../src/PCapReader.h \
           ../src/winstdint.h
           
