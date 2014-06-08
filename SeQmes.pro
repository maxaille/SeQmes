#-------------------------------------------------
#
# Project created by QtCreator 2013-08-20T18:39:41
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SeQmes
TEMPLATE = app


SOURCES += main.cpp\
        SeQmes.cpp \
    client.cpp \
    server.cpp \
    lib/vcard/vcardproperty.cpp \
    lib/vcard/vcardparam.cpp \
    lib/vcard/vcard.cpp \
    userItem.cpp \
    mainwindow.cpp \
    user.cpp \
    itemuser.cpp \
    connectto.cpp

HEADERS  += SeQmes.h \
    client.h \
    server.h \
    lib/vcard/vcardproperty.h \
    lib/vcard/vcardparam.h \
    lib/vcard/vcard.h \
    lib/vcard/libvcard_global.h \
    userItem.h \
    mainwindow.h \
    user.h \
    itemuser.h \
    connectto.h

TRANSLATIONS = SeQmes_en.ts SeQmes_fr.ts

RESOURCES += \
    ressources.qrc

FORMS += \
    mainwindow.ui \
    itemuser.ui \
    connectto.ui
