#-------------------------------------------------
#
#   mehstation-config
#
#   Rémy 'remeh' Mathieu - me@remy.io
#
#-------------------------------------------------

QT += core network gui widgets uitools sql
# C++0x yay!
QMAKE_CXXFLAGS += -std=c++0x
LIBS +=

TARGET = mehstation-config
TEMPLATE = app

OBJECTS_DIR = build/
MOC_DIR = build/
UI_DIR = build/

SOURCES += src/main.cpp \
           src/app.cpp \
           src/models/platform.cpp \
           src/models/executable.cpp \
           src/db.cpp

HEADERS += src/app.h \
           src/models/model.h \
           src/models/platform.h \
           src/models/executable.h \
           src/db.h
