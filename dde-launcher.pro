QT      += core gui dbus widgets x11extras

TARGET = dde-launcher
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkbase dtkwidget dtkbase xcb xcb-ewmh

SOURCES += \
    mian.cpp \
    mainframe.cpp \
    dbus/dbuslauncher.cpp \
    model/appslistmodel.cpp \
    model/appsmanager.cpp \
    view/applistview.cpp \
    widget/categorytitlewidget.cpp \
    widget/searchwidget.cpp \
    widget/navigationbuttonframe.cpp \
    widget/basecheckedbutton.cpp \
    global_util/util.cpp \
    dbus/dbusfileinfo.cpp \
    dbus/dbusvariant/categoryinfo.cpp \
    dbus/dbusvariant/frequencyinfo.cpp \
    dbus/dbusvariant/installedtimeinfo.cpp \
    dbus/dbusvariant/iteminfo.cpp \
    delegate/appitemdelegate.cpp \
    dbus/dbustartmanager.cpp \
    global_util/xcb_misc.cpp \
    widget/navigationwidget.cpp \
    widget/categorybutton.cpp \
    dbus/dbusmenu.cpp \
    dbus/dbusmenumanager.cpp \
    worker/menuworker.cpp \
    dbus/dbusdockedappmanager.cpp

HEADERS += \
    mainframe.h \
    dbus/dbuslauncher.h \
    model/appslistmodel.h \
    model/appsmanager.h \
    view/applistview.h \
    widget/categorytitlewidget.h \
    widget/searchwidget.h \
    global_util/constants.h \
    widget/navigationbuttonframe.h \
    widget/basecheckedbutton.h \
    global_util/util.h \
    dbus/dbusfileinfo.h \
    dbus/dbusvariant/categoryinfo.h \
    dbus/dbusvariant/frequencyinfo.h \
    dbus/dbusvariant/installedtimeinfo.h \
    dbus/dbusvariant/iteminfo.h \
    delegate/appitemdelegate.h \
    dbus/dbustartmanager.h \
    global_util/xcb_misc.h \
    widget/navigationwidget.h \
    widget/categorybutton.h \
    dbus/dbusmenu.h \
    dbus/dbusmenumanager.h \
    worker/menuworker.h \
    dbus/dbusdockedappmanager.h

target.path = /usr/bin

qm_files.files = translations/*.qm
qm_files.path = /usr/share/dde-launcher/translations

service.path = /usr/share/dbus-1/services
service.files = dbusservices/com.deepin.dde.Launcher.service

INSTALLS += target qm_files services

RESOURCES += \
    skin.qrc
