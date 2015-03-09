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
    mainwindow.cpp

HEADERS += \
    dxl_hal.h \
    dynamixel.h \
    ax12.h \
    mainwindow.h

FORMS += \
    mainwindow.ui
