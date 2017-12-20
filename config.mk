# See LICENSE file for license and copyright information

VERSION_MAJOR = 0
VERSION_MINOR = 2
VERSION_REV = 4
VERSION = ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REV}

# minimum required zathura version
ZATHURA_MIN_VERSION = 0.3.8
ZATHURA_VERSION_CHECK ?= $(shell pkg-config --atleast-version=$(ZATHURA_MIN_VERSION) zathura; echo $$?)
ZATHURA_GTK_VERSION ?= $(shell pkg-config --variable=GTK_VERSION zathura)

# paths
PREFIX ?= /usr
LIBDIR ?= ${PREFIX}/lib
DESKTOPPREFIX ?= ${PREFIX}/share/applications

# libs
CAIRO_INC ?= $(shell pkg-config --cflags cairo)
CAIRO_LIB ?= $(shell pkg-config --libs cairo)

GLIB_INC ?= $(shell pkg-config --cflags glib-2.0)
GLIB_LIB ?= $(shell pkg-config --libs glib-2.0)

SPECTRE_INC ?= $(shell pkg-config --cflags libspectre)
SPECTRE_LIB ?= $(shell pkg-config --libs libspectre)

GIRARA_INC ?= $(shell pkg-config --cflags girara-gtk${ZATHURA_GTK_VERSION})
GIRARA_LIB ?= $(shell pkg-config --libs girara-gtk${ZATHURA_GTK_VERSION})

ZATHURA_INC ?= $(shell pkg-config --cflags zathura)
PLUGINDIR ?= $(shell pkg-config --variable=plugindir zathura)
ifeq (,${PLUGINDIR})
PLUGINDIR = ${LIBDIR}/zathura
endif

INCS = ${GLIB_INC} ${SPECTRE_INC} ${GIRARA_INC} ${CAIRO_INC} ${ZATHURA_INC}
LIBS = ${GLIB_LIB} ${SPECTRE_LIB} ${GIRARA_LIB} ${CAIRO_LIB}

# compiler flags
CFLAGS += -std=c11 -fPIC -pedantic -Wall -Wno-format-zero-length $(INCS)

# linker flags
LDFLAGS += -fPIC

# debug
DFLAGS ?= -g

# compiler
CC ?= gcc
LD ?= ld

# set to something != 0 if you want verbose build output
VERBOSE ?= 0
