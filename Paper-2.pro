QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 resources_big

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cpp\main.cpp \
    cpp\mainwindow.cpp \
    cpp\python_deep_learning.cpp \

HEADERS += \
    include\mainwindow.h \
    include\python_deep_learning.h \

FORMS += \
    ui\mainwindow.ui \
    ui\python_deep_learning.ui \

win32:CONFIG(release, debug|release): {
    LIBS += -LD:/Python39/libs/ -lpython39
    INCLUDEPATH += D:/Python39/include
    DEFINES += NDEBUG
}

win32:CONFIG(debug, debug|release): {
    LIBS += -LD:/Python39/libs/ -lpython39_d
    INCLUDEPATH += D:/Python39/include
}

else:unix: {
    LIBS += -LD:/Python39/libs/ -lpython39
    INCLUDEPATH += D:/Python39/include
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=
