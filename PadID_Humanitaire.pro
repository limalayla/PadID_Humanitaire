#-------------------------------------------------
#
# Project created by QtCreator 2015-11-05T13:56:51
#
#-------------------------------------------------

QT       += core gui
QT		 += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PadID_Humanitaire
TEMPLATE = app


SOURCES += main.cpp\
        mainwin.cpp \
    onglet_vueensemble.cpp \
    onglet_recherche.cpp \
    onglet_gestionhumaine.cpp \
    onglet_stocks.cpp \
    general.cpp

HEADERS  += mainwin.h

FORMS    += mainwin.ui

TRANSLATIONS = humanitaire_fr.ts \
               humanitaire_en.ts
