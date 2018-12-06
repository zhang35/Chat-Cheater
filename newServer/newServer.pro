#-------------------------------------------------
#
# Project created by QtCreator 2012-04-17T15:56:06
#
#-------------------------------------------------

QT       += core gui\
            sql\
            network


TARGET = newServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    pipeserver.cpp \
    clientsocket.cpp \
    runthread.cpp \
    historydialog.cpp

HEADERS  += mainwindow.h \
    pipeserver.h \
    clientsocket.h \
    runthread.h \
    historydialog.h

FORMS    += mainwindow.ui \
    historydialog.ui

RESOURCES += \
    images.qrc

OTHER_FILES += \
    icon.rc
RC_FILE = icon.rc
