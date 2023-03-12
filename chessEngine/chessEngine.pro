TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        board.cpp \
        boardpoint.cpp \
        cachedmove.cpp \
        cachedposition.cpp \
        main.cpp \
        piece.cpp

HEADERS += \
    board.h \
    boardpoint.h \
    cachedmove.h \
    cachedposition.h \
    constants.h \
    move.h \
    piece.h
