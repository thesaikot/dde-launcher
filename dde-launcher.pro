QT      += core gui dbus widgets

TARGET = dde-launcher
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkbase dtkwidget dtkbase

SOURCES += \
    mian.cpp \
    mainframe.cpp \
    applistview.cpp \
    dbus/dbuslauncher.cpp \
    dbus/categoryinfo.cpp \
    dbus/frequencyinfo.cpp \
    dbus/iteminfo.cpp \
    dbus/installedtimeinfo.cpp \
    model/appslistmodel.cpp \
    model/appsmanager.cpp

HEADERS += \
    mainframe.h \
    applistview.h \
    dbus/dbuslauncher.h \
    dbus/categoryinfo.h \
    dbus/frequencyinfo.h \
    dbus/iteminfo.h \
    dbus/installedtimeinfo.h \
    model/appslistmodel.h \
    model/appsmanager.h
