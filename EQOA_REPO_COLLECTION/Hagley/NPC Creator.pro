#-------------------------------------------------
#
# Project created by QtCreator 2016-04-01T00:40:08
#
#-------------------------------------------------
QT       += network
QT       += gui
QT       += core
QT       += sql
QT       += concurrent
QT       += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = NPC_Creator
TEMPLATE = app


SOURCES += \
    SpreadThread.cpp \
    main.cpp\
    charcreate.cpp \
    dns.cpp \
    dnsthread.cpp \
    facingdirection.cpp \
    mainwindow.cpp \
    objectpacket.cpp \
    packetsize.cpp \
    generator.cpp \
    increment.cpp \
    coaching.cpp \
    quests.cpp \
    xorpacket.cpp \
    merchants.cpp \
    add_or_sub.cpp \
    loot.cpp\
    checkthezone.cpp \
    combat.cpp \
    convert.cpp \
    crc.cpp \
    flipped.cpp \
    fortytwentyninedec.cpp \
    loadingcharacter.cpp \
    login.cpp \
    loginthread.cpp \
    maploading.cpp \
    opcodes.cpp \
    packetparsing.cpp \
    patch.cpp \
    patchthread.cpp \
    serverselect.cpp \
    serverselectthread.cpp \
    targeting.cpp \
    worldthread.cpp

HEADERS  += \
    SpreadThread.h \
    Ztest6.h \
    Ztest7.h \
    Ztest8.h \
    mainwindow.h \
    charcreate.h \
    combat.h \
    dns.h \
    dnsthread.h \
    facingdirection.h \
    objectpacket.h \
    packetsize.h \
    generator.h \
    increment.h \
    coaching.h \
    quests.h \
    targeting.h \
    graphiclist.h \
    xorpacket.h \    
    merchants.h \
    add_or_sub.h \
    convert.h \
    loot.h\
    checkthezone.h \
    crc.h \
    flipped.h \
    fortytwentyninedec.h \
    loadingcharacter.h \
    login.h \
    loginthread.h \
    maploading.h \
    opcodes.h \
    packetparsing.h \
    patch.h \
    patchthread.h \
    serverselect.h \
    serverselectthread.h \
    worldthread.h \
    Ztest0.h \
    Ztest1.h \
    Ztest2.h \
    Ztest3.h \
    Ztest4.h \
    Ztest5.h

FORMS    += mainwindow.ui

RESOURCES += \
    hagres.qrc

DISTFILES +=

