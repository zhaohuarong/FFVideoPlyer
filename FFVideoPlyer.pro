QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FFVideoPlyer
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += $$PWD/include

LIBS += $$PWD/lib/avcodec.lib
LIBS += $$PWD/lib/avdevice.lib
LIBS += $$PWD/lib/avfilter.lib
LIBS += $$PWD/lib/avformat.lib
LIBS += $$PWD/lib/avutil.lib
LIBS += $$PWD/lib/postproc.lib
LIBS += $$PWD/lib/swresample.lib
LIBS += $$PWD/lib/swscale.lib

SOURCES += \
        main.cpp \
    MyAudio.cpp \
    MyFFmpeg.cpp \
    PlayThread.cpp \
    VideoViewWidget.cpp \
    mainwindow.cpp

HEADERS += \
    MyAudio.h \
    MyFFmpeg.h \
    PlayThread.h \
    VideoViewWidget.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=
