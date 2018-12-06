#-------------------------------------------------
#
# Project created by QtCreator 2012-04-17T15:56:21
#
#-------------------------------------------------

QT       += core gui
QT       += network
TARGET = newClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    clientsocket.cpp \
    controlwindow.cpp \
    mytool.cpp \
    historydialog.cpp

HEADERS  += mainwindow.h \
    clientsocket.h \
    controlwindow.h \
    mytool.h \
    historydialog.h \
    myFileInfo.h

FORMS    += mainwindow.ui \
    controlwindow.ui \
    historydialog.ui

RESOURCES += \
    images.qrc

OTHER_FILES += \
    icon.rc
RC_FILE = icon.rc
