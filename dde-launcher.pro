QT      += core gui dbus widgets

TARGET = dde-launcher
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkbase dtkwidget dtkbase

SOURCES += \
    mian.cpp \
    mainframe.cpp \
    dbus/dbuslauncher.cpp \
    model/appslistmodel.cpp \
    model/appsmanager.cpp \
    view/applistview.cpp \
    widget/categorytitlewidget.cpp \
    widget/searchwidget.cpp \
    widget/navigationlistwidget.cpp \
    widget/navigationbuttonframe.cpp \
    widget/basecheckedbutton.cpp \
    global_util/util.cpp \
    dbus/dbusfileinfo.cpp \
    dbus/dbusvariant/categoryinfo.cpp \
    dbus/dbusvariant/frequencyinfo.cpp \
    dbus/dbusvariant/installedtimeinfo.cpp \
    dbus/dbusvariant/iteminfo.cpp \
    delegate/appitemdelegate.cpp

HEADERS += \
    mainframe.h \
    dbus/dbuslauncher.h \
    model/appslistmodel.h \
    model/appsmanager.h \
    view/applistview.h \
    widget/categorytitlewidget.h \
    widget/searchwidget.h \
    global_util/constants.h \
    widget/navigationlistwidget.h \
    widget/navigationbuttonframe.h \
    widget/basecheckedbutton.h \
    global_util/util.h \
    dbus/dbusfileinfo.h \
    dbus/dbusvariant/categoryinfo.h \
    dbus/dbusvariant/frequencyinfo.h \
    dbus/dbusvariant/installedtimeinfo.h \
    dbus/dbusvariant/iteminfo.h \
    delegate/appitemdelegate.h

target.path = /usr/bin

qm_files.files = translations/*.qm
qm_files.path = /usr/share/dde-launcher/translations

service.path = /usr/share/dbus-1/services
service.files = dbusservices/com.deepin.dde.Launcher.service

INSTALLS += target qm_files services

RESOURCES += \
    skin.qrc
