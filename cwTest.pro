#-------------------------------------------------
#
# Project created by QtCreator 2012-08-22T13:32:40
#
#-------------------------------------------------

QT       += core gui widgets
QT       += multimedia

# greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#    INCLUDEPATH += /home/gvj/Qt5.0.0beta1/Desktop/Qt/5.0.0-beta1/gcc
    INCLUDEPATH += /home/gvj/Qt5.0.0beta1/Desktop/Qt/5.0.0-beta1/gcc/include/QtCore
    INCLUDEPATH += /home/gvj/Qt5.0.0beta1/Desktop/Qt/5.0.0-beta1/gcc/include/QtGui
    INCLUDEPATH += /home/gvj/Qt5.0.0beta1/Desktop/Qt/5.0.0-beta1/gcc/include/QtWidgets
    INCLUDEPATH += /home/gvj/Qt5.0.0beta1/Desktop/Qt/5.0.0-beta1/gcc/include/QtMultimedia


TARGET = cwTest
TEMPLATE = app


SOURCES += main.cpp\
        cwtest.cpp \
    morse.cpp \
    generator.cpp

HEADERS  += cwtest.h \
    morse.h \
    generator.h

FORMS    += cwtest.ui \
    morse.ui
