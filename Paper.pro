QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 resources_big

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cpp\forusers.cpp \
    cpp\login.cpp \
    cpp\main.cpp \
    cpp\mainwindow.cpp \
    cpp\python_dl.cpp \
    cpp\register.cpp

HEADERS += \
    include\forusers.h \
    include\login.h \
    include\mainwindow.h \
    include\python_dl.h \
    include\register.h

FORMS += \
    ui\login.ui \
    ui\mainwindow.ui \
    ui\python_dl.ui \
    ui\register.ui

win32:CONFIG(release, debug|release): {
    LIBS += -LD:/Python39/libs/ -lpython39
    LIBS += -LD:/OneDrive/QT/Qt_projects/sqlitecipher/Desktop_Qt_6_8_1_MSVC2022_64bit-Release/sqlitecipher/release/ -lsqlitecipher
    INCLUDEPATH += D:/Python39/include
    DEFINES += NDEBUG
}

win32:CONFIG(debug, debug|release): {
    LIBS += -LD:/Python39/libs/ -lpython39_d
    LIBS += -LD:/OneDrive/QT/Qt_projects/sqlitecipher/Desktop_Qt_6_8_1_MSVC2022_64bit-Debug/sqlitecipher/debug/ -lsqlitecipher_d
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

RESOURCES += \
    resources.qrc
