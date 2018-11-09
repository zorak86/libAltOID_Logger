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

QMAKE_INCDIR += $$PREFIX/include
INCLUDEPATH += $$PREFIX/include

!win32:isEmpty(PREFIX) {
    PREFIX = /usr/local
}

# C++ standard.
QMAKE_CXX += -Wno-write-strings -Wno-unused-parameter -Wno-unused-function -O3 -std=c++11 -Wunused -Wno-unused-result
# LIB DEFS:

# for Qt projects on Win32...
win32 {
MINGWVERSION = mingw530_32
LIBS += -LC:/Qt/Tools/$$MINGWVERSION/opt/lib -L$$PREFIX\lib
CONFIG(debug, debug|release) {
LIBS += -lalt_mutex -lsqlite3
} else {
LIBS += -lalt_mutex -lsqlite3
}
}

TARGET = alt_logger
TEMPLATE = lib
VERSION      = 3.0.1
# INSTALLATION:
target.path = $$PREFIX/lib
header_files.files = $$HEADERS
header_files.path = $$PREFIX/include/$$TARGET
INSTALLS += target
INSTALLS += header_files

DISTFILES += \
    LICENSE \
    AUTHORS \
    ChangeLog \
    README.md \
    INSTALL \
    NEWS \
    autogen.sh \
    configure.ac \
    src/Makefile.am \
    Makefile.am \
    m4/ax_lib_sqlite3.m4

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,_debug)
    else: TARGET = $$join(TARGET,,,d)
}

# PKGCONFIG
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_LIBDIR = $$PREFIX/lib/
QMAKE_PKGCONFIG_INCDIR = $$PREFIX/include/alt_logger
QMAKE_PKGCONFIG_CFLAGS = -I$$PREFIX/include/
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
