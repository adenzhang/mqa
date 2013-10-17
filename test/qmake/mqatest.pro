TARGET = mqatest
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt


win32 {

    CONFIG(debug, debug|release) : DESTDIR = $$PWD/../bin/x64/debug
    else: CONFIG(release, debug|release) : DESTDIR = $$PWD/../bin/x64/release

    #libary boost
    LIBS += -LD:/onprogram/clib/boost/lib/x64
    INCLUDEPATH += D:/onprogram/clib/boost/include

    LIBS += -lws2_32

    #libary mqa
    CONFIG(debug, debug|release) : MQA_LIBDIR = $$PWD/../../msbuild/bin/x64/debug
    else: CONFIG(release, debug|release) : MQA_LIBDIR = $$PWD/../../msbuild/bin/x64/release

    LIBS += -L$$MQA_LIBDIR -lmqa


} else { #linux -------------------

    CONFIG(debug, debug|release) : DESTDIR = $$PWD/../bin-linux/x64/debug
    else: CONFIG(release, debug|release) : DESTDIR = $$PWD/../bin-linux/x64/release

    #libary boost
    CONFIG(debug, debug|release) : LIBS += -L/program/linux/usr/boost/lib-x64-debug
    else: CONFIG(release, debug|release) : LIBS += -L/program/linux/usr/boost/lib-x64-release
    INCLUDEPATH += /program/linux/usr/boost/include

    #libary mqa
    CONFIG(debug, debug|release) : MQA_LIBDIR = $$PWD/../../msbuild/bin-linux/x64/debug
    else: CONFIG(release, debug|release) : MQA_LIBDIR = $$PWD/../../msbuild/bin-linux/x64/release

    LIBS += -L$$MQA_LIBDIR -lmqa

}  # linux

#-------------- copy files ----------------

FILES2DESTDIR += \
    $$MQA_LIBDIR/mqa.dll \

win32 {
    DESTDIR_WIN = $${DESTDIR}
    DESTDIR_WIN ~= s,/,\,g
    PWD_WIN = $${PWD}
    PWD_WIN ~= s,/,\,g
    for(FILE, $$FILES2DESTDIR){
        QMAKE_POST_LINK += $$quote(cmd /c copy /y $${PWD_WIN}/$${FILE} $${DESTDIR_WIN}$$escape_expand(\\n\\t))
#        $$QMAKE_COPY $$quote($$FILE) $$quote($$DESTDIR_WIN) $$escape_expand(\\n\\t)
    }
} else { #-- linux
    for(FILE, $$FILES2DESTDIR){
        QMAKE_POST_LINK += $$quote(cp $${PWD}/$${FILE} $${DESTDIR}$$escape_expand(\\n\\t))
    }
}

#---------------------------------------------

INCLUDEPATH += ..\..\include
INCLUDEPATH += ..\..\AnalyzerCommon


SOURCES += \
           ..\src\LogUtility.cpp \
           ..\src\PCapReader.cpp \
           ..\src\mqatest.cpp \

HEADERS += \
           ..\src\LogUtility.h \
           ..\src\PCapReader.h \
           ..\src\stdint.h \
           
