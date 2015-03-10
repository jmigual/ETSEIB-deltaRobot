QT += core gui serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DeltaRobot
TEMPLATE = app
CONFIG += c++11

#--------------
# SFML include
#--------------
CONFIG(release, debug|release){
    LIBS += -LC:/MinGW/Libraries/SFML-2.2/lib -lsfml-audio -lsfml-graphics -lsfml-network -lsfml-window -lsfml-system
} else {
    LIBS += -LC:/MinGW/Libraries/SFML-2.2/lib -lsfml-audio-d -lsfml-graphics-d -lsfml-network-d -lsfml-window-d -lsfml-system-d
}

INCLUDEPATH += C:/MinGW/Libraries/SFML-2.2/include
DEPENDPATH += C:/MinGW/Libraries/SFML-2.2/include



SOURCES += main.cpp \ 
    dynamixel.cpp \
    dxl_hal.cpp \
    ax12.cpp \
    mainwindow.cpp \
    Options/optionsservos.cpp \
    Options/optionswindow.cpp

HEADERS += \
    dxl_hal.h \
    dynamixel.h \
    ax12.h \
    mainwindow.h \
    Options/optionsservos.h \
    Options/optionswindow.h

FORMS += \
    mainwindow.ui \
    Options/optionsservos.ui \
    Options/optionswindow.ui

RESOURCES += \
    resources.qrc

win32:CONFIG(release, debug|release): LIBS += -LC:/MinGW/Libraries/XJoystick/ -lXJoystick
else:win32:CONFIG(debug, debug|release): LIBS += -LC:/MinGW/Libraries/XJoystick/ -lXJoystickd
else:unix: LIBS += -LC:/MinGW/Libraries/XJoystick/ -lXJoystick

INCLUDEPATH += C:/MinGW/Libraries/XJoystick
DEPENDPATH += C:/MinGW/Libraries/XJoystick

DISTFILES +=
