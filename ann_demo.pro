#-------------------------------------------------
#
# Project created by QtCreator 2017-08-03T10:49:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ann_demo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ann/batchtrainer.cpp \
    ann/layer.cpp \
    ann/neuralnetwork.cpp \
    ann/trainer.cpp \
    dataReader/datareader.cpp \
    graphLib/qcustomplot.cpp \
    utils.cpp \
    dialog/createlayerdialog.cpp \
    graphLib/edge.cpp \
    graphLib/node.cpp \
    visualize/netgraphicsscene.cpp \
    visualize/loger.cpp \
    visualize/graphdrawer.cpp

HEADERS  += mainwindow.h \
    ann/batchtrainer.h \
    ann/layer.h \
    ann/neuralnetwork.h \
    ann/trainer.h \
    dataReader/dataentry.h \
    dataReader/datareader.h \
    graphLib/qcustomplot.h \
    dialog/createlayerdialog.h \
    graphLib/node.h \
    graphLib/edge.h \
    visualize/netgraphicsscene.h \
    visualize/loger.h \
    visualize/graphdrawer.h

FORMS    += mainwindow.ui \
    dialog/createlayerdialog.ui

DISTFILES += \
    data/t10k-images.idx3-ubyte \
    data/t10k-labels.idx1-ubyte \
    data/train-images.idx3-ubyte \
    data/train-labels.idx1-ubyte \
    data/sinData.txt \
    data/sinData_test.txt \
    data/sinInput.txt \
    data/sinInput_test.txt

LIBS += -larmadillo

RESOURCES += \
    datafile.qrc
