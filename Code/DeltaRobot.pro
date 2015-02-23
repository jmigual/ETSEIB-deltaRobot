QT += core
QT += serialport
QT -= gui

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TARGET = DeltaRobot


SOURCES += main.cpp \ 
    dynamixel.cpp \
    dxl_hal.cpp

HEADERS += \
    dxl_hal.h \
    dynamixel.h

include(deployment.pri)
qtcAddDeployment()
