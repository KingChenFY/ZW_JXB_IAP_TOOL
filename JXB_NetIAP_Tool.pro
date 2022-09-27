QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
#不加QT += core5compat，会报__imp__ZN7QRegExpD1Ev一连串错误，原因待查
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

CONFIG += c++17
CONFIG += warn_off
CONFIG += resources_big

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    iapaction.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    iapaction.h \
    mainwindow.h \
    head.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += $$PWD/unit
INCLUDEPATH += $$PWD/hardshare

include ($$PWD/unit/unit.pri)
include ($$PWD/hardshare/hardshare.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qwe.qrc

RC_ICONS = title.ico
