AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = \
    moba-systemmanager

moba_systemmanager_SOURCES = \
    activeapps.cpp \
    automaticcontrol.cpp \
    clock.cpp \
    environmentcontrol.cpp \
    frmbase.cpp \
    frmmain.cpp \
    main.cpp \
    noticelogger.cpp \
    serverdata.cpp \
    systemcontrol.cpp

moba_systemmanager_LDADD = \
    -lmobacommon \
    $(GTKMM_LIBS) \
    $(srcdir)/../modules/lib-msghandling/src/moba/libmsghandling.a

AM_LDFLAGS = -L/usr/local/lib

AM_CPPFLAGS = \
    $(GTKMM_CFLAGS) \
    -I$(srcdir)/../modules/lib-msghandling/src \
    -ggdb -g3 -O3 -Wall -Wno-deprecated-declarations

CLEANFILES = *~
