#-------------------------------------------------
#
# Project created by QtCreator 2014-09-28T11:41:09
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += DEBUG

TARGET = Scercog
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    debug.cpp \
    videoselectiondialog.cpp \
    multiprogressbar.cpp

HEADERS  += mainwindow.h \
    debug.h \
    videoselectiondialog.h \
    multiprogressbar.h

FORMS    += mainwindow.ui \
    videoselectiondialog.ui \
    multiprogressbar.ui
