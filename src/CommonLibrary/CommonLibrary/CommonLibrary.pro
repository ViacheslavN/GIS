#-------------------------------------------------
#
# Project created by QtCreator 2016-02-04T22:40:57
#
#-------------------------------------------------

QT       -= core gui

TARGET = CommonLibrary
TEMPLATE = lib
CONFIG += staticlib
INCLUDEPATH += c:\NDK\x64\android-ndk-r10e\sources\cxx-stl\stlport\stlport
SOURCES += commonlibrary.cpp \
    alloc_t.cpp \
    BitStream.cpp \
    blob.cpp \
    BoundaryBox.cpp \
    caseutils.cpp \
    crc.cpp \
    DebugTime.cpp \
    File.cpp \
    FilePosix.cpp \
    FileStream.cpp \
    FixedBitStream.cpp \
    FixedMemoryStream.cpp \
    GeoShape.cpp \
    GeoShapeBuf.cpp \
    IGeoShape.cpp \
    Interlocked.cpp \
    log.cpp \
    MemoryStream.cpp \
    multibyte.cpp \
    sprintf.cpp \
    stream.cpp \
    String.cpp \
    thread.cpp \
    Timer.cpp \
    Variant.cpp \
    WaitForGraph.cpp \
    WriteBitStream.cpp

HEADERS += commonlibrary.h \
    alloc_t.h \
    BitStream.h \
    blob.h \
    BoundaryBox.h \
    caseutils.h \
    crc.h \
    CSSection.h \
    DebugTime.h \
    delegate.h \
    Event.h \
    File.h \
    FilePosix.h \
    FileStream.h \
    FixedBitStream.h \
    FixedMemoryStream.h \
    general.h \
    GeneralTypes.h \
    GeoPoint.h \
    GeoShape.h \
    GeoShapeBuf.h \
    guid.h \
    HashKey.h \
    IGeoShape.h \
    Interlocked.h \
    IRefCnt.h \
    log.h \
    MemoryStream.h \
    multibyte.h \
    Mutex.h \
    Nocopy.h \
    PodVector.h \
    SpatialKey.h \
    SpatialVariant.h \
    sprintf.h \
    stdafx.h \
    stl_alloc.h \
    stream.h \
    String.h \
    string_buffer.h \
    targetver.h \
    thread.h \
    Timer.h \
    TimeUtils.h \
    Variant.h \
    VartTypeList.h \
    WaitForGraph.h \
    WriteBitStream.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
