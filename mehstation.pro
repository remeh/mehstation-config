#-------------------------------------------------
#
#   mehstation-config
#
#   remeh - me@remy.io
#
#-------------------------------------------------

QT += core network gui
# C++0x yay!
QMAKE_CXXFLAGS += -std=c++0x
LIBS +=

TARGET = mehstation-config
TEMPLATE = app

OBJECTS_DIR = build/
MOC_DIR = build/
UI_DIR = build/

SOURCES += src/main.cpp \

HEADERS +=
