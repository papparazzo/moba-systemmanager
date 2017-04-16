AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = \
    moba-systemmanager
moba_systemmanager_SOURCES = \
    main.cpp \
    activeapps.cpp \
    frmmain.cpp


moba_systemmanager_LDADD = -lMoBaCommon $(GTKMM_LIBS)
LDFLAGS += -L/usr/local/lib

AM_CPPFLAGS = $(GTKMM_CFLAGS)

CLEANFILES = *~
