# Compile with Watcom 10.6 or OpenWatcom
CC = wcc386
APPNAME = dcode
OBJDIR = .\bin\dos
DEPSDIR = .\bin\dos
LNK = $(OBJDIR)\$(APPNAME).lnk
CFLAGS_LNK = $(OBJDIR)\CFLAGS.lnk

# Split OBJS to avoid long expansion strings in some wmake versions
OBJS_1 = $(OBJDIR)\input.obj $(OBJDIR)\vgaregs.obj $(OBJDIR)\data.obj $(OBJDIR)\sort.obj $(OBJDIR)\draw.obj
OBJS_2 = $(OBJDIR)\test.obj $(OBJDIR)\vismem.obj $(OBJDIR)\env.obj $(OBJDIR)\mem.obj $(OBJDIR)\vsnprntf.obj
OBJS_3 = $(OBJDIR)\log.obj $(OBJDIR)\editor.obj $(OBJDIR)\config.obj $(OBJDIR)\files.obj $(OBJDIR)\fs.obj $(OBJDIR)\video.obj $(OBJDIR)\main.obj 
OBJS = $(OBJS_1) $(OBJS_2) $(OBJS_3)

# Final executable
$(APPNAME).exe: $(OBJS)
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
	@%create $(LNK)
	@%append $(LNK) system dos4g
	@%append $(LNK) name $(APPNAME).exe
	@%append $(LNK) library clib3r.lib
	# Appending files individually to avoid "argument list too big" shell errors
	@for %i in ($(OBJS_1)) do @%append $(LNK) file %i
	@for %i in ($(OBJS_2)) do @%append $(LNK) file %i
	@for %i in ($(OBJS_3)) do @%append $(LNK) file %i
	wlink @$(LNK)

# Compiler response file - putting all flags and includes here
$(CFLAGS_LNK): .ALWAYS
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
	@%create $@
	@%append $@ /3r /s /otexan
	@%append $@ -i=hal -i=app -i=core -i=deps\data -i=deps\sort -i=deps\env -i=deps\mem -i=deps\log -i=deps\ext
	@%append $@ -i=platform\dos\input -i=platform\dos\video -i=platform\dos\fs

# Compile rules
$(OBJDIR)\input.obj: .\platform\dos\input\input.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\platform\dos\input\input.c

$(OBJDIR)\vgaregs.obj: .\platform\dos\video\vgaregs.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\platform\dos\video\vgaregs.c

$(OBJDIR)\data.obj: .\deps\data\data.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\data\data.c

$(OBJDIR)\vsnprntf.obj: .\deps\ext\vsnprntf.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\ext\vsnprntf.c

$(OBJDIR)\sort.obj: .\deps\sort\sort.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\sort\sort.c

$(OBJDIR)\env.obj: .\deps\env\env.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\env\env.c

$(OBJDIR)\mem.obj: .\deps\mem\mem.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\mem\mem.c

$(OBJDIR)\log.obj: .\deps\log\log.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\log\log.c

# core ENGINE
$(OBJDIR)\vismem.obj: .\core\vismem\vismem.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\vismem\vismem.c

$(OBJDIR)\test.obj: .\core\test\test.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\test\test.c

$(OBJDIR)\files.obj: .\core\files\files.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\files\files.c

$(OBJDIR)\fs.obj: .\platform\dos\fs\fs.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\platform\dos\fs\fs.c

$(OBJDIR)\editor.obj: .\core\editor\editor.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\editor\editor.c

$(OBJDIR)\config.obj: .\core\config\config.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\config\config.c

$(OBJDIR)\draw.obj: .\core\draw\draw.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\draw\draw.c

$(OBJDIR)\video.obj: .\platform\dos\video\video.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\platform\dos\video\video.c

$(OBJDIR)\main.obj: .\app\main.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\app\main.c

preclean: .SYMBOLIC
	@if exist $(OBJDIR)\*.obj del $(OBJDIR)\*.obj
	@if exist $(OBJDIR)\*.lnk del $(OBJDIR)\*.lnk
	@if exist *.obj del *.obj
	@if exist *.exe del *.exe
	@if exist *.ERR del *.ERR
	@if exist *.lnk del *.lnk

postclean: .SYMBOLIC
	@if exist *.obj del *.obj
	@if exist bin\dos\*.obj del bin\dos\*.obj
	@if exist *.lnk del *.lnk
	@if exist bin\dos\*.lnk del bin\dos\*.lnk
	@if exist $(APPNAME).exe copy $(APPNAME).exe $(OBJDIR)\$(APPNAME).exe
	del $(APPNAME).exe

build: preclean $(APPNAME).exe postclean
