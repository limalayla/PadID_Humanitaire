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


SOURCES +=  main.cpp\
            mainwin.cpp \
            tab_overview.cpp \
            tab_supplies.cpp  \
            tab_search.cpp     \
            tab_management.cpp  \
            general.cpp \
    refugeeinfowin.cpp

HEADERS  += mainwin.h \
    refugeeinfowin.h \
    refugeewin.h

FORMS    += mainwin.ui \
    refugeeinfowin.ui

TRANSLATIONS = humanitaire_fr.ts \
               humanitaire_en.ts
