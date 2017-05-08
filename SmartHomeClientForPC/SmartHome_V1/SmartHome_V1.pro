#-------------------------------------------------
#
# Project created by QtCreator 2016-12-28T09:38:47
#
#-------------------------------------------------

QT       += core gui network multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartHome_V1
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    smarthome.cpp \
    room.cpp \
    register.cpp \
    message.cpp \
    setting.cpp \
    camera.cpp \
    boxgameview.cpp \
    boxgroup.cpp \
    onebox.cpp \
    changepwd.cpp \
    forgetpas.cpp \
    mainwindow.cpp \
    videoplayer.cpp

HEADERS  += widget.h \
    smarthome.h \
    room.h \
    register.h \
    message.h \
    setting.h \
    camera.h \
    boxgameview.h \
    boxgroup.h \
    onebox.h \
    changepwd.h \
    forgetpas.h \
    mainwindow.h \
    videoplayer.h

FORMS    += widget.ui \
    smarthome.ui \
    room.ui \
    register.ui \
    photo.ui \
    image.ui \
    setting.ui \
    camera.ui \
    changepwd.ui \
    forgetpas.ui \
    mainwindow.ui \
    videoplayer.ui

RESOURCES += \
    src.qrc

DISTFILES +=

RC_FILE = SmartHome.rc
