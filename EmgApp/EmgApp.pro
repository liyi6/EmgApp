#-------------------------------------------------
#
# Project created by QtCreator 2018-11-18T15:11:36
#
#-------------------------------------------------

QT       +=  core gui network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET =  EmgApp
TEMPLATE =  app

CONFIG += debug_and_release
CONFIG(debug, debug|release){
    DESTDIR = $$PWD/bin/debug
} else {
    DESTDIR = $$PWD/bin/release
}
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    StartDlg.cpp \
    NetConnectHelper.cpp \
    MainWindow.cpp \
    DataProcessThread.cpp \
    axistag.cpp \
    qcustomplot.cpp

HEADERS += \
    StartDlg.h \
    StartDlg.h \
    NetConnectHelper.h \
    MainWindow.h \
    DataProcessThread.h \
    axistag.h \
    qcustomplot.h \
    Defines.h

FORMS += \
    StartDlg.ui \
    MainWindow.ui
