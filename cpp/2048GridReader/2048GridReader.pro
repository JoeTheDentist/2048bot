#-------------------------------------------------
#
# Project created by QtCreator 2014-11-08T14:56:00
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = 2048GridReader
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS+=-std=c++11 -I..

SOURCES += main.cpp \
            ../GameMatrix.cpp

HEADERS += ../GameMatrix.h
