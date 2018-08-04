#-------------------------------------------------
#
# Project created by QtCreator 2012-07-21T13:56:38
#
#-------------------------------------------------

QT       += webkitwidgets network widgets sql
CONFIG += c++11

qtHaveModule(uitools):!embedded: QT += uitools
else: DEFINES += QT_NO_UITOOLS
TARGET = SlimMail
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    email.cpp \
    models/emailmodel.cpp \
    certverifier.cpp \
    models/mailboxtreeitem.cpp \
    models/mailboxmodel.cpp \
    mailbox.cpp \
    mailboxdirectories.cpp \
    mailsyncthread.cpp \
    views/emailview.cpp \
    emailaddress.cpp \
    delegates/bolddelegate.cpp \
    newmailwindow.cpp \
    delimitedcompleter.cpp

HEADERS  += mainwindow.h \
    email.h \
    models/emailmodel.h \
    certverifier.h \
    mailboxtreeitem.h \
    models/mailboxmodel.h \
    mailbox.h \
    mailboxdirectories.h \
    mailsyncthread.h \
    views/emailview.h \
    emailaddress.h \
    delegates/bolddelegate.h \
    newmailwindow.h \
    delimitedcompleter.h

FORMS    += mainwindow.ui \
    views/emailview.ui \
    newmailwindow.ui
LIBS += -lvmime

RESOURCES += \
    main.qrc
