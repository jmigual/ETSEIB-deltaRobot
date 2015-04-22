QT += core gui serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DeltaRobot
TEMPLATE = app
CONFIG += c++14 precompile_header

# Precompiled headers
PRECOMPILED_HEADER = stable.h

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
    dxl/dynamixel.cpp \
    dxl/dxl_hal.cpp \
    mainwindow.cpp \
    optionswindow.cpp \
    servothread.cpp \
    dxl/ax12.cpp

HEADERS += \
    dxl/dxl_hal.h \
    dxl/dynamixel.h \
    mainwindow.h \
    optionswindow.h \
    servothread.h \
    dxl/ax12.h \
    stable.h

FORMS += \
    mainwindow.ui \
    optionswindow.ui

RESOURCES += \
    resources.qrc

Release:DESTDIR = release
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui

# To copy files
win32 {
CONFIG(release, debug|release) {
    OTHER_FILES += \
        Libraries/XJoystick/XJoystick.dll \
        Libraries/SFML-2.2/bin/sfml-window-2.dll \
        Libraries/SFML-2.2/bin/sfml-system-2.dll
}
else {
    OTHER_FILES += \
        Libraries/XJoystick/XJoystickd.dll \
        Libraries/SFML-2.2/bin/sfml-window-d-2.dll \
        Libraries/SFML-2.2/bin/sfml-system-d-2.dll
}
}

win32 {
    DESTDIR_WIN = $${DESTDIR}
    DESTDIR_WIN ~= s,/,\\,g
    PWD_WIN = $${PWD}
    PWD_WIN ~= s,/,\\,g
    for(FILE, OTHER_FILES){
        FILE ~= s,/,\\,g
        QMAKE_POST_LINK += $$quote(cmd /c copy /y $${PWD_WIN}\\$${FILE} $${DESTDIR_WIN}$$escape_expand(\\n\\t))
    }
}
unix {
    for(FILE, OTHER_FILES){
        QMAKE_POST_LINK += $$quote(cp $${PWD}/$${FILE} $${DESTDIR}$$escape_expand(\\n\\t))
    }
}


#----------------------
# DEPLOY CONFIGURATION
#----------------------

isEmpty(TARGET_EXT) {
    win32: TARGET_CUSTOM_EXT = .exe
    macx: TARGET_CUSTOM_EXT = .app
} 
else: TARGET_CUSTOM_EXT = $${TARGET_EXT}

win32: DEPLOY_COMMAND = windeployqt
macx: DEPLOY_COMMAND = macdeployqt

CONFIG( debug, debug|release ): DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/debug/$${TARGET}$${TARGET_CUSTOM_EXT}))
else: DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/release/$${TARGET}$${TARGET_CUSTOM_EXT}))

QMAKE_POST_LINK += $${DEPLOY_COMMAND} $${DEPLOY_TARGET}
