QT += core
QT += serialport
QT += gui

TEMPLATE = app
CONFIG += console
CONFIG += c++11

TARGET = DeltaRobot


SOURCES += main.cpp \ 
    dynamixel.cpp \
    dxl_hal.cpp \
    ax12.cpp

HEADERS += \
    dxl_hal.h \
    dynamixel.h \
    ax12.h
