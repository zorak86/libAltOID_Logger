ALTOIDMUTEX_SONAME = 2:3:0

lib_LTLIBRARIES = libalt_logger.la

pkginclude_HEADERS = loggerhive.h

libalt_logger_la_SOURCES = loggerhive.cpp loggerhive.h
libalt_logger_la_CPPFLAGS = -I$(prefix)/include
libalt_logger_la_LDFLAGS = -L$(prefix)/lib -lalt_mutex -lsqlite3 -no-undefined -export-dynamic -version-info $(ALTOIDMUTEX_SONAME)
