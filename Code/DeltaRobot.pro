QT += core gui serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DeltaRobot
TEMPLATE = app
CONFIG += c++11

#--------------
# SFML include
#--------------

win32 {
CONFIG(release, debug|release) LIBS += -L$$PWD/Libraries/SFML-2.2/lib -lsfml-window -lsfml-system
else: LIBS += -L$$PWD/Libraries/SFML-2.2/lib -lsfml-window-d -lsfml-system-d
}
INCLUDEPATH += $$PWD/Libraries/SFML-2.2/include
DEPENDPATH += $$PWD/Libraries/SFML-2.2/include

#------------------
# XJoystick Include
#------------------

win32 {
CONFIG(release, debug|release): LIBS += -L$$PWD/Libraries/XJoystick/ -lXJoystick
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/Libraries/XJoystick/ -lXJoystickd
}

INCLUDEPATH += $$PWD/Libraries/XJoystick
DEPENDPATH += $$PWD/Libraries/XJoystick


SOURCES += main.cpp \ 
    dynamixel.cpp \
    dxl_hal.cpp \
    ax12.cpp \
    mainwindow.cpp \
    optionswindow.cpp

HEADERS += \
    dxl_hal.h \
    dynamixel.h \
    ax12.h \
    mainwindow.h \
    optionswindow.h

FORMS += \
    mainwindow.ui \
    optionswindow.ui

RESOURCES += \
    resources.qrc
