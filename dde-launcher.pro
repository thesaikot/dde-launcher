QT      += core gui dbus widgets x11extras svg

TARGET = dde-launcher
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkbase dtkwidget dtkbase gtk+-2.0 gio-unix-2.0

include(./widgets/widgets.pri)

SOURCES += \
    mian.cpp \
    dbus/dbuslauncher.cpp \
    model/appslistmodel.cpp \
    model/appsmanager.cpp \
    view/applistview.cpp \
    global_util/util.cpp \
    dbus/dbusvariant/categoryinfo.cpp \
    dbus/dbusvariant/frequencyinfo.cpp \
    dbus/dbusvariant/installedtimeinfo.cpp \
    dbus/dbusvariant/iteminfo.cpp \
    dbus/fileInfo_interface.cpp \
    global_util/themeappicon.cpp \
    app/mainframe.cpp \
    delegate/appitemdelegate.cpp \
    global_util/calculateutil.cpp

HEADERS += \
    dbus/dbuslauncher.h \
    model/appslistmodel.h \
    model/appsmanager.h \
    view/applistview.h \
    global_util/constants.h \
    global_util/global.h \
    global_util/util.h \
    dbus/dbusvariant/categoryinfo.h \
    dbus/dbusvariant/frequencyinfo.h \
    dbus/dbusvariant/installedtimeinfo.h \
    dbus/dbusvariant/iteminfo.h \
    dbus/fileInfo_interface.h \
    global_util/themeappicon.h \
    app/mainframe.h \
    delegate/appitemdelegate.h \
    global_util/singleton.h \
    global_util/signalmanager.h \
    global_util/calcuateutil.h

target.path = /usr/bin/

qm_files.files = translations/*.qm
qm_files.path = /usr/share/dde-launcher/translations

services.path = /usr/share/dbus-1/services
services.files = dbusservices/com.deepin.dde.Launcher.service

INSTALLS += target qm_files services

RESOURCES += \
    skin.qrc
