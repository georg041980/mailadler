QT += core widgets testlib

CONFIG += c++17 console
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tst_mailadler

SOURCES = test_main.cpp

RESOURCES += ../translations.qrc

INCLUDEPATH += ..
