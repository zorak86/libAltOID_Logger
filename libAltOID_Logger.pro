#--++-----------------------------------------------
#
# Project created by QtCreator 2015-10-08T19:04:13
#
#-------------------------------------------------

QT       -= gui core
CONFIG += c++11

SOURCES += \
    src/loggerhive.cpp

HEADERS += \
    src/loggerhive.h

!win32:isEmpty(PREFIX) {
    PREFIX = /usr/local
}

# C++ standard.
QMAKE_CXX += -Wno-write-strings -Wno-unused-parameter -Wno-unused-function -O3 -std=c++11 -Wunused -Wno-unused-result
# LIB DEFS:
# for Qt projects on Win32...
win32:LIBS += -LC:\Qt\Tools\mingw492_32\opt\lib -L$$PREFIX\lib -lAltOID_Mutex1 -lzSQLite3
#win32:LIBS += -LC:\Qt\Tools\mingw492_32\opt\lib -LC:\libAltOIDS_ROOT\lib -lAltOID_Mutex1 -lsqlite3.dll
TARGET = AltOID_Logger
TEMPLATE = lib
VERSION      = 1.0.1
# INSTALLATION:
target.path = $$PREFIX/lib
header_files.files = $$HEADERS
header_files.path = $$PREFIX/include/alt_logger
INSTALLS += target
INSTALLS += header_files
# PKGCONFIG
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_LIBDIR = $$PREFIX/lib/
QMAKE_PKGCONFIG_INCDIR = $$PREFIX/include/alt_logger
QMAKE_PKGCONFIG_CFLAGS = -I$$PREFIX/include/
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

DISTFILES += \
    LICENSE \
    AUTHORS \
    ChangeLog \
    README.md \
    INSTALL
