TARGET = controller
TEMPLATE = lib
CONFIG += shared c++11 x86_64
QT += network sql xml
QT -= gui
greaterThan(QT_MAJOR_VERSION, 4): QT += qml
DEFINES += TF_DLL
DESTDIR = ../lib
INCLUDEPATH += ../helpers ../models
DEPENDPATH  += ../helpers ../models
LIBS += -L../lib -lhelper -lmodel
MOC_DIR = .obj/
OBJECTS_DIR = .obj/

include(../appbase.pri)

HEADERS += applicationcontroller.h
SOURCES += applicationcontroller.cpp
HEADERS += maincontroller.h
SOURCES += maincontroller.cpp
HEADERS += applicationendpoint.h
SOURCES += applicationendpoint.cpp
HEADERS += logtailendpoint.h
SOURCES += logtailendpoint.cpp
HEADERS += solverprotocontroller.h
SOURCES += solverprotocontroller.cpp
HEADERS += caffecontextcontroller.h
SOURCES += caffecontextcontroller.cpp
HEADERS += workspacecontroller.h
SOURCES += workspacecontroller.cpp
HEADERS += neuralnetworkcontroller.h
SOURCES += neuralnetworkcontroller.cpp
