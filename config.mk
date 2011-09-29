# See LICENSE file for license and copyright information

# paths
PREFIX ?= /usr

# libs
GTK_INC ?= $(shell pkg-config --cflags gtk+-2.0)
GTK_LIB ?= $(shell pkg-config --libs gtk+-2.0)

SPECTRE_INC ?= $(shell pkg-config --cflags libspectre)
SPECTRE_LIB ?= $(shell pkg-config --libs libspectre)

INCS = ${GTK_INC} ${SPECTRE_INC}
LIBS = ${GTK_LIB} ${SPECTRE_LIB}

# flags
CFLAGS += -std=c99 -fPIC -pedantic -Wall -Wno-format-zero-length $(INCS)

# debug
DFLAGS ?= -g

# build with cairo support?
WITH_CAIRO ?= 1

# compiler
CC ?= gcc
LD ?= ld
