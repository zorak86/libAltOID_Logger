# libAltOID_Logger

Alternative OpenSource Thread Safe Logger Library - by Zorak x86  
License: LGPL v3  
Version: 3.0.1  
Requires: libpthread libsqlite3 libalt_mutex  

You should compile with the following flags (or use the .pc)  
-lalt_mutex -lsqlite3

## Functionality

This library provides thread-safe C++ Log abstraction on:

- Sqlite files
- Syslog
- Screen

## Build instructions (using libtool)

```
autoconf -i (or ./autogen.sh)
./configure --prefix=/usr
make -j8
make install
```