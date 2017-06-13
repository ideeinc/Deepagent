TARGET = model
TEMPLATE = lib
CONFIG += shared c++11 x86_64
QT += sql xml concurrent
QT -= gui
greaterThan(QT_MAJOR_VERSION, 4): QT += qml
DEFINES += TF_DLL USE_OPENCV
DESTDIR = ../lib
INCLUDEPATH += ../helpers/ sqlobjects/ mongoobjects/ services/ logics/ contrainers/ ${CAFFE_ROOT}/build/include ${CAFFE_ROOT}/include /usr/local/cuda/include
DEPENDPATH  += ../helpers/ sqlobjects/ mongoobjects/ services/ logics/ contrainers/
macx {
  DEFINES += USE_ACCELERATE CPU_ONLY
  HOMEBREW_PREFIX = $$system(brew --prefix)
  ACCELERATE_FRAMEWORK = /System/Library/Frameworks/Accelerate.framework
  VECLIB_FRAMEWORK = $${ACCELERATE_FRAMEWORK}/Versions/Current/Frameworks/vecLib.framework
  INCLUDEPATH += $${HOMEBREW_PREFIX}/include $${VECLIB_FRAMEWORK}/Versions/Current/Headers
  LIBS += -framework Accelerate -L$${HOMEBREW_PREFIX}/lib -lopencv_imgproc -lopencv_highgui
}
LIBS += -L../lib -lhelper -Wl,-rpath,${CAFFE_ROOT}/build/lib -L${CAFFE_ROOT}/build/lib -lboost_system -lcaffe -lglog -lprotobuf -lopencv_core -lopencv_highgui -lopencv_imgproc
MOC_DIR = .obj/
OBJECTS_DIR = .obj/
QMAKE_CXXFLAGS += -Wno-sign-compare

include(../appbase.pri)

HEADERS += services/caffetrainedmodelservice.h
SOURCES += services/caffetrainedmodelservice.cpp
HEADERS += services/trainservice.h
SOURCES += services/trainservice.cpp
HEADERS += services/tagservice.h
SOURCES += services/tagservice.cpp
HEADERS += services/datasetmakerservice.h
SOURCES += services/datasetmakerservice.cpp
HEADERS += services/taggroup.h
SOURCES += services/taggroup.cpp
HEADERS += services/tag.h
SOURCES += services/tag.cpp

HEADERS += logics/caffeprocess.h
SOURCES += logics/caffeprocess.cpp
HEADERS += logics/formatconverter.h
SOURCES += logics/formatconverter.cpp
HEADERS += logics/prediction.h
SOURCES += logics/prediction.cpp
HEADERS += logics/ssddetector.h
SOURCES += logics/ssddetector.cpp
HEADERS += logics/roccurve.h
SOURCES += logics/roccurve.cpp

HEADERS += containers/caffetrainedmodelpredictcontainer.h
HEADERS += containers/boundingboxdrawcontainer.h
HEADERS += containers/boundingboxselecttypecontainer.h
HEADERS += containers/boundingboxsettingcontainer.h
HEADERS += containers/trainindexcontainer.h
HEADERS += containers/traincreatecontainer.h
SOURCES += containers/traincreatecontainer.cpp
HEADERS += containers/traincreatessdcontainer.h
HEADERS += containers/traindetectcontainer.h
HEADERS += containers/trainshowcontainer.h
HEADERS += containers/taginfocontainer.h
HEADERS += containers/taggedimageinfocontainer.h
HEADERS += containers/uploadresultcontainer.h
HEADERS += containers/datasetmakerindexcontainer.h
HEADERS += containers/datasetmakerpreviewcontainer.h
HEADERS += containers/datasetmakerresultcontainer.h

HEADERS += image.h
SOURCES += image.cpp
HEADERS += sqlobjects/solverprotoobject.h
HEADERS += solverproto.h
SOURCES += solverproto.cpp
HEADERS += sqlobjects/caffecontextobject.h
HEADERS += caffecontext.h
SOURCES += caffecontext.cpp
HEADERS += sqlobjects/workspaceobject.h
HEADERS += workspace.h
SOURCES += workspace.cpp
HEADERS += sqlobjects/neuralnetworkobject.h
HEADERS += neuralnetwork.h
SOURCES += neuralnetwork.cpp
HEADERS += sqlobjects/caffetrainedmodelobject.h
HEADERS += caffetrainedmodel.h
SOURCES += caffetrainedmodel.cpp
HEADERS += sqlobjects/classlabelobject.h
HEADERS += classlabel.h
SOURCES += classlabel.cpp
HEADERS += sqlobjects/traindatasetobject.h
HEADERS += traindataset.h
SOURCES += traindataset.cpp
HEADERS += sqlobjects/trainingmodelobject.h
HEADERS += trainingmodel.h
SOURCES += trainingmodel.cpp
HEADERS += sqlobjects/caffedataobject.h
HEADERS += caffedata.h
SOURCES += caffedata.cpp
HEADERS += sqlobjects/caffemodelobject.h
HEADERS += caffemodel.h
SOURCES += caffemodel.cpp
HEADERS += sqlobjects/pretrainedmodelobject.h
HEADERS += pretrainedmodel.h
SOURCES += pretrainedmodel.cpp
HEADERS += sqlobjects/datasetobject.h
HEADERS += dataset.h
SOURCES += dataset.cpp
