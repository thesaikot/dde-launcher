QT      += core gui dbus widgets

TARGET = dde-launcher
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkbase dtkwidget dtkbase

SOURCES += \
    mian.cpp \
    mainframe.cpp \
    dbus/dbuslauncher.cpp \
    dbus/categoryinfo.cpp \
    dbus/frequencyinfo.cpp \
    dbus/iteminfo.cpp \
    dbus/installedtimeinfo.cpp \
    model/appslistmodel.cpp \
    model/appsmanager.cpp \
    appitemdelegate.cpp \
    view/applistview.cpp \
    widget/categorytitlewidget.cpp \
    widget/categorylistwidget.cpp \
    widget/searchwidget.cpp

HEADERS += \
    mainframe.h \
    dbus/dbuslauncher.h \
    dbus/categoryinfo.h \
    dbus/frequencyinfo.h \
    dbus/iteminfo.h \
    dbus/installedtimeinfo.h \
    model/appslistmodel.h \
    model/appsmanager.h \
    appitemdelegate.h \
    view/applistview.h \
    widget/categorytitlewidget.h \
    constants.h \
    widget/categorylistwidget.h \
    widget/searchwidget.h

target.path = /usr/bin

qm_files.files = translations/*.qm
qm_files.path = /usr/share/dde-launcher/translations

service.path = /usr/share/dbus-1/services
service.files = dbusservices/com.deepin.dde.Launcher.service

INSTALLS += target qm_files services
