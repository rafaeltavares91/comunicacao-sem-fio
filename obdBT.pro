#-------------------------------------------------
#
# Project created by QtCreator 2015-09-24T21:17:17
#
#-------------------------------------------------

QT       += core gui bluetooth qml quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets quickwidgets

TARGET = obdBT
TEMPLATE = app

#OTHER_FILES += qml/*.qml


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
