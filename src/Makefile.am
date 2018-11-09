lib_LTLIBRARIES = libalt_logger.la

pkginclude_HEADERS = loggerhive.h

libalt_logger_la_SOURCES = loggerhive.cpp loggerhive.h
libalt_logger_la_CPPFLAGS = -I$(prefix)/include
libalt_logger_la_LDFLAGS = -L$(prefix)/lib -lalt_mutex $(SQLITE3_LDFLAGS) -no-undefined -export-dynamic -version-info @version_info@
