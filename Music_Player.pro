TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        displayplaylist.cpp \
        logger.cpp \
        main.cpp \
        song.cpp

HEADERS += \
    displayplaylist.h \
    logger.h \
    song.h
