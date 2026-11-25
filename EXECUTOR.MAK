PROJ	=EXECUTOR
DEBUG	=0
PROGTYPE	=1
CALLER	= 
ARGS	= 
DLLS	= 
CVPACK	=1
CC	=cl -qc
RC	=rc
CFLAGS_G_WEXE	= /AS /W3 /Ze /G2w /Zp 
CFLAGS_D_WEXE	= /Zi /Od 
CFLAGS_R_WEXE	= /O /Os /Gs /DNDEBUG
CFLAGS_G_WDLL	=/AS /G2w /Zp /Aw /W3
CFLAGS_D_WDLL	=/Od /Zi
CFLAGS_R_WDLL	=/O /Os /DNDEBUG
CFLAGS_G_WTTY	=/AS /G2w /W3
CFLAGS_D_WTTY	=/Od /Zi
CFLAGS_R_WTTY	=/O /Os /DNDEBUG
CFLAGS_G_DEXE	= /AS /W2 /Ze 
CFLAGS_D_DEXE	= /Zi /Od 
CFLAGS_R_DEXE	= /O /Ot /Gs /DNDEBUG
CFLAGS	=$(CFLAGS_G_WEXE) $(CFLAGS_R_WEXE)
LFLAGS_G_WEXE	=/A:16/ST:5120 
LFLAGS_D_WEXE	=/CO 
LFLAGS_R_WEXE	= 
LFLAGS_G_WDLL	=/ST:5120 /A:16
LFLAGS_D_WDLL	=/CO
LFLAGS_R_WDLL	= 
LFLAGS_G_WTTY	=/ST:5120 /A:16
LFLAGS_D_WTTY	=/CO
LFLAGS_R_WTTY	= 
LFLAGS_G_DEXE	=/NOI /ST:2048
LFLAGS_D_DEXE	=/CO
LFLAGS_R_DEXE	= 
LFLAGS	=$(LFLAGS_G_WEXE) $(LFLAGS_R_WEXE)
RCFLAGS	= 
RESFLAGS	=-t
RUNFLAGS	= 
H = 	executor.h 
RESFILES = 	executor.ico 
DEFFILE = 	executor.def
OBJS_EXT = 	
LIBS_EXT = 	

.rc.res: ; $(RC) $(RCFLAGS) -r $*.rc

all:	$(PROJ).EXE

executor.obj:	executor.c $(H)

executor.res:	executor.rc $(RESFILES) $(H)

$(PROJ).EXE:	executor.obj $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).crf
executor.obj +
$(OBJS_EXT)
$(PROJ).EXE

c:\qcwin\lib\+
c:\c600\lib\+
c:\lib\+
c:\windev\lib\+
/NOD libw slibcew
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).crf
	rc $(RESFLAGS) executor.res $(PROJ).exe

$(PROJ).EXE:	executor.res
	rc $(RESFLAGS) executor.res $(PROJ).exe

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)

