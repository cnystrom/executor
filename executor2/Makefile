# Executor 2.1 — cross-compile with mingw-w64
#   make          builds 64-bit and 32-bit .exe
#   make 64       builds only bin/executor.exe
#   make 32       builds only bin/executor32.exe

PREFIX64  := x86_64-w64-mingw32
PREFIX32  := i686-w64-mingw32
CC64      := $(PREFIX64)-gcc
CC32      := $(PREFIX32)-gcc
WINDRES64 := $(PREFIX64)-windres
WINDRES32 := $(PREFIX32)-windres

CFLAGS  := -Wall -Wextra -O2 -std=c11 -municode -DUNICODE -D_UNICODE
LDFLAGS := -mwindows -static -static-libgcc
LIBS    := -lcomctl32 -lcomdlg32 -lshell32 -lshlwapi -luser32 -lgdi32 -ladvapi32 -lkernel32

SRCDIR  := src
OBJDIR  := obj
BINDIR  := bin

SRC     := $(SRCDIR)/executor.c
RC      := $(SRCDIR)/executor.rc

.PHONY: all 64 32 clean

all: 64 32

64: $(BINDIR)/executor.exe

32: $(BINDIR)/executor32.exe

$(OBJDIR) $(BINDIR):
	mkdir -p $@

$(OBJDIR)/executor.res: $(RC) $(SRCDIR)/executor.h $(SRCDIR)/executor.ico $(SRCDIR)/executor.manifest | $(OBJDIR)
	$(WINDRES64) -I $(SRCDIR) -O coff -o $@ $<

$(OBJDIR)/executor32.res: $(RC) $(SRCDIR)/executor.h $(SRCDIR)/executor.ico $(SRCDIR)/executor.manifest | $(OBJDIR)
	$(WINDRES32) -I $(SRCDIR) -DEXE32 -O coff -o $@ $<

$(BINDIR)/executor.exe: $(SRC) $(SRCDIR)/executor.h $(OBJDIR)/executor.res | $(BINDIR)
	$(CC64) $(CFLAGS) -o $@ $(SRC) $(OBJDIR)/executor.res $(LDFLAGS) $(LIBS)

$(BINDIR)/executor32.exe: $(SRC) $(SRCDIR)/executor.h $(OBJDIR)/executor32.res | $(BINDIR)
	$(CC32) $(CFLAGS) -o $@ $(SRC) $(OBJDIR)/executor32.res $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(OBJDIR) $(BINDIR)
