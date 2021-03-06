#-------------------------------------------------
#
# Project created by QtCreator 2019-06-07T21:58:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pellet-man
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17 strict_c++
QMAKE_CXXFLAGS += -Wall -Wextra -pedantic

SOURCES += \
    src/blinky.cpp \
    src/character.cpp \
    src/clyde.cpp \
    src/distancecalculator.cpp \
    src/enemy.cpp \
    src/game.cpp \
    src/inky.cpp \
    src/lifecounter.cpp \
    src/main.cpp \
    src/node.cpp \
    src/pellet.cpp \
    src/pinky.cpp \
    src/player.cpp \
    src/regularpellet.cpp \
    src/score.cpp \
    src/superpellet.cpp

HEADERS += \
    inc/blinky.h \
    inc/character.h \
    inc/clyde.h \
    inc/directions.h \
    inc/distancecalculator.h \
    inc/enemy.h \
    inc/game.h \
    inc/gameconstants.h \
    inc/inky.h \
    inc/lifecounter.h \
    inc/node.h \
    inc/pellet.h \
    inc/pinky.h \
    inc/player.h \
    inc/regularpellet.h \
    inc/score.h \
    inc/superpellet.h

INCLUDEPATH += \
    inc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/res.qrc
