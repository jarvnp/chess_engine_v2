TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        board.cpp \
        boardpoint.cpp \
        main.cpp \
        piece.cpp

HEADERS += \
    board.h \
    boardpoint.h \
    piece.h
