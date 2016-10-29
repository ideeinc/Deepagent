TARGET = model
TEMPLATE = lib
CONFIG += shared c++11 x86_64
QT += sql
QT -= gui
greaterThan(QT_MAJOR_VERSION, 4): QT += qml
DEFINES += TF_DLL
DESTDIR = ../lib
INCLUDEPATH += ../helpers sqlobjects mongoobjects
DEPENDPATH  += ../helpers sqlobjects mongoobjects
LIBS += -L../lib -lhelper

include(../appbase.pri)

HEADERS += sqlobjects/solverprotoobject.h
HEADERS += solverproto.h
SOURCES += solverproto.cpp
HEADERS += sqlobjects/caffecontextobject.h
HEADERS += caffecontext.h
SOURCES += caffecontext.cpp
HEADERS += service/caffeprocess.h
SOURCES += service/caffeprocess.cpp
HEADERS += sqlobjects/workspaceobject.h
HEADERS += workspace.h
SOURCES += workspace.cpp
HEADERS += sqlobjects/neuralnetworkobject.h
HEADERS += neuralnetwork.h
SOURCES += neuralnetwork.cpp
