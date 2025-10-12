QT       += core gui widgets xml

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG += c++17

TARGET = iadl-editor-test1
TEMPLATE = app

# Set the build directory
DESTDIR = build
MOC_DIR = build
RCC_DIR = build
OBJECTS_DIR = build

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    assetmodel.cpp \
    assetitem.cpp \
    asseteditor.cpp \
    assetvisualizer.cpp \
    iadlsyntaxhighlighter.cpp \
    importexportmanager.cpp \
    importexportdialog.cpp \
    plugins/avevapiexporter.cpp \
    plugins/jsonexporter.cpp \
    plugins/bimimporter.cpp \
    plugins/omniverseexporter.cpp \
    plugins/etapexporter.cpp \
    plugins/intouchexporter.cpp \
    plugins/systemplatformexporter.cpp \
    plugins/edgeexporter.cpp \
    plugins/historianexporter.cpp \
    plugins/ignitionexporter.cpp

HEADERS += \
    mainwindow.h \
    assetmodel.h \
    assetitem.h \
    asseteditor.h \
    assetvisualizer.h \
    iadlsyntaxhighlighter.h \
    importexportinterface.h \
    importexportmanager.h \
    importexportdialog.h \
    plugins/avevapiexporter.h \
    plugins/jsonexporter.h \
    plugins/bimimporter.h \
    plugins/omniverseexporter.h \
    plugins/etapexporter.h \
    plugins/intouchexporter.h \
    plugins/systemplatformexporter.h \
    plugins/edgeexporter.h \
    plugins/historianexporter.h \
    plugins/ignitionexporter.h

FORMS += \
    mainwindow.ui \
    asseteditor.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

