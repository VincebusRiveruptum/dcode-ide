# Compile with Watcom 10.6 or OpenWatcom
CC = wcc386
APPNAME = dcode
OBJDIR = .\bin\dos
DEPSDIR = .\deps
LNK = $(OBJDIR)\$(APPNAME).lnk
CFLAGS_LNK = $(OBJDIR)\CFLAGS.lnk

# Split OBJS to avoid long expansion strings in some wmake versions
DEPS_OBJS_1 = $(DEPSDIR)\data.obj $(DEPSDIR)\env.obj $(DEPSDIR)\vsnprntf.obj $(DEPSDIR)\input.obj $(DEPSDIR)\log.obj 
DEPS_OBJS_2 = $(DEPSDIR)\mem.obj $(DEPSDIR)\vgaregs.obj $(DEPSDIR)\sort.obj $(DEPSDIR)\str.obj

DEPS_LIB = $(DEPSDIR)\deps.lib

# Core objs
CORE_OBJS_1 = $(OBJDIR)\test.obj $(OBJDIR)\vismem.obj $(OBJDIR)\config.obj $(OBJDIR)\draw.obj $(OBJDIR)\fs.obj $(OBJDIR)\video.obj 
CORE_OBJS_2 = $(OBJDIR)\ed_base.obj $(OBJDIR)\ed_cfg.obj $(OBJDIR)\ed_sel.obj $(OBJDIR)\ed_shell.obj $(OBJDIR)\ed_statb.obj $(OBJDIR)\ed_priv.obj 
CORE_OBJS_3 = $(OBJDIR)\f_base.obj $(OBJDIR)\f_qopen.obj $(OBJDIR)\f_nav.obj $(OBJDIR)\f_wnd.obj $(OBJDIR)\private.obj $(OBJDIR)\f_search.obj $(OBJDIR)\main.obj

DEPS_OBJS = $(DEPS_OBJS_1) $(DEPS_OBJS_2)
CORE_OBJS = $(CORE_OBJS_1) $(CORE_OBJS_2) $(CORE_OBJS_3) 

# ===== CAMBIO AQUÍ: El .exe solo depende de DEPS_LIB + CORE_OBJS =====

# Deps DEPS.LIB packaging
$(DEPS_LIB): $(DEPS_OBJS)
	@if not exist $(DEPSDIR) mkdir $(DEPSDIR)
	wlib -b $@ $(DEPSDIR)\input.obj $(DEPSDIR)\vgaregs.obj $(DEPSDIR)\data.obj
	wlib -b $@ $(DEPSDIR)\sort.obj $(DEPSDIR)\env.obj $(DEPSDIR)\mem.obj
	wlib -b $@ $(DEPSDIR)\vsnprntf.obj $(DEPSDIR)\log.obj $(DEPSDIR)\str.obj

$(APPNAME).exe: $(DEPS_LIB) $(CORE_OBJS)
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
	@%create $(LNK)
	@%append $(LNK) system dos4g
	@%append $(LNK) name $(APPNAME).exe
	@%append $(LNK) library clib3r.lib
	@%append $(LNK) library $(DEPS_LIB)
	@for %i in ($(CORE_OBJS)) do @%append $(LNK) file %i
	wlink @$(LNK)

# Compiler response file - putting all flags and includes here
$(CFLAGS_LNK): .ALWAYS
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
	@%create $@
	@%append $@ /3r /s /otexan
	@%append $@ -i=hal -i=app -i=core -i=deps\data -i=deps\sort -i=deps\env -i=deps\mem -i=deps\log -i=deps\ext
	@%append $@ -i=platform\dos\input -i=platform\dos\video -i=platform\dos\fs

# Compile rules
# DEPS
$(DEPSDIR)\input.obj: .\platform\dos\input\input.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\platform\dos\input\input.c

$(DEPSDIR)\vgaregs.obj: .\platform\dos\video\vgaregs.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\platform\dos\video\vgaregs.c

$(DEPSDIR)\data.obj: .\deps\data\data.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\data\data.c

$(DEPSDIR)\vsnprntf.obj: .\deps\ext\vsnprntf.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\ext\vsnprntf.c

$(DEPSDIR)\sort.obj: .\deps\sort\sort.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\sort\sort.c

$(DEPSDIR)\env.obj: .\deps\env\env.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\env\env.c

$(DEPSDIR)\str.obj: .\deps\str\str.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\str\str.c

$(DEPSDIR)\mem.obj: .\deps\mem\mem.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\mem\mem.c

$(DEPSDIR)\log.obj: .\deps\log\log.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\deps\log\log.c

# CORE
$(OBJDIR)\vismem.obj: .\core\vismem\vismem.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\vismem\vismem.c

$(OBJDIR)\test.obj: .\core\test\test.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\test\test.c

$(OBJDIR)\f_base.obj: .\core\files\f_base.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\files\f_base.c

# files
$(OBJDIR)\f_qopen.obj: .\core\files\f_qopen.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\files\f_qopen.c

$(OBJDIR)\f_wnd.obj: .\core\files\f_wnd.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\files\f_wnd.c

$(OBJDIR)\private.obj: .\core\files\private.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\files\private.c

$(OBJDIR)\f_search.obj: .\core\files\f_search.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\files\f_search.c

$(OBJDIR)\f_nav.obj: .\core\files\f_nav.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\files\f_nav.c

# fs
$(OBJDIR)\fs.obj: .\platform\dos\fs\fs.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\platform\dos\fs\fs.c

# editor
$(OBJDIR)\ed_priv.obj: .\core\editor\private.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\editor\private.c

$(OBJDIR)\ed_base.obj: .\core\editor\ed_base.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\editor\ed_base.c

$(OBJDIR)\ed_cfg.obj: .\core\editor\ed_cfg.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\editor\ed_cfg.c

$(OBJDIR)\ed_sel.obj: .\core\editor\ed_sel.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\editor\ed_sel.c

$(OBJDIR)\ed_shell.obj: .\core\editor\ed_shell.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\editor\ed_shell.c

$(OBJDIR)\ed_statb.obj: .\core\editor\ed_statb.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\editor\ed_statb.c

# config
$(OBJDIR)\config.obj: .\core\config\config.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\config\config.c

# draw
$(OBJDIR)\draw.obj: .\core\draw\draw.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\core\draw\draw.c

$(OBJDIR)\video.obj: .\platform\dos\video\video.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\platform\dos\video\video.c

$(OBJDIR)\main.obj: .\app\main.c $(CFLAGS_LNK)
	$(CC) @$(CFLAGS_LNK) -fo=$@ .\app\main.c

preclean: .SYMBOLIC
	@if exist $(OBJDIR)\*.obj del $(OBJDIR)\*.obj
	@if exist $(DEPSDIR)\*.obj del $(DEPSDIR)\*.obj
	@if exist $(OBJDIR)\*.lnk del $(OBJDIR)\*.lnk
	@if exist *.obj del *.obj
	@if exist *.OBJ del *.OBJ
	@if exist *.exe del *.exe
	@if exist *.ERR del *.ERR
	@if exist *.lnk del *.lnk

postclean: .SYMBOLIC
	@if exist *.obj del *.obj
	@if exist $(DEPSDIR)\*.obj del $(DEPSDIR)\*.obj
	@if exist bin\dos\*.obj del bin\dos\*.obj
	@if exist *.lnk del *.lnk
	@if exist bin\dos\*.lnk del bin\dos\*.lnk
	@if exist $(APPNAME).exe copy $(APPNAME).exe $(OBJDIR)\$(APPNAME).exe
	del $(APPNAME).exe

build: preclean $(APPNAME).exe postclean