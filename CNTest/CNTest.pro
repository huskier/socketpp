#-------------------------------------------------
#
# Project created by QtCreator 2015-06-04T13:29:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CNTest
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += /usr/include/glib-2.0 \
               /usr/lib64/glib-2.0/include

LIBS    += -L/usr/local/lib -lglog -llog4cplus -ludt\
           -lpicam -lvips-cpp -lvips -lglib-2.0 -lgobject-2.0\
           -L/lib64 -lrt\


LIBS   += -L/usr/local/lib -lboost_system


SOURCES += main.cpp\
        mainwindow.cpp \
    IL_VA.cpp \
    util.cpp \
    IL_global.cpp \
    IL_PICamera_over_UDT.cpp \
    udp_async_server.cpp \
    udp_async_client.cpp \
    udt_sync_server.cpp \
    udt_sync_client.cpp \
    tcp_async_server.cpp \
    tcp_async_client.cpp

HEADERS  += mainwindow.h \
    IL_VA.h \
    IL_types.h \
    util.h \
    IL_global.h \
    IL_net_struct.h \
    IL_PICamera_over_UDT.h \
    IL_PICamera.h \
    udp_async_server.h \
    udp_async_client.h \
    udt_sync_server.h \
    udt_sync_client.h \
    socketpp.h \
    tcp_async_server.h \
    tcp_async_client.h

FORMS    += mainwindow.ui
