#-------------------------------------------------
#
# Project created by QtCreator 2016-03-14T15:31:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WordsClipper
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tclipper.cpp

HEADERS  += mainwindow.h \
    tclipper.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11

RESOURCES += \
    images.qrc
