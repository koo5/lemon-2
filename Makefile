# Makefile to build the SDL tests



srcdir  = .

CC      = gcc-4  -g -ggdb 
GLLIBS  =  -DGL -DHAVE_OPENGL -lGL
CFLIBS =   -DXTHREADS  -DREENTRANT  -L/usr/lib -lSDL -L/usr/X11R6/lib  -lutil -L/usr/local/lib   -ltermcap  -L/usr/local/lib   `pkg-config --libs x11`  `pkg-config --cflags x11` -I/usr/include -I/usr/local/include -I./  -I/usr/local/include/vte -lncurses -L/usr/X11R6/lib   roteterm/inject_csi.c roteterm/inject.c roteterm/rote_keymap.c roteterm/rote.c more-mess/wtf.c -lncurses 

pl=$(shell uname)

ifeq ($(pl),Linux)
CC      = gcc  -g -ggdb 
PYLIBS = `/usr/bin/python2.5-config --cflags` + `/usr/bin/python2.5-config --libs ` 
CFLIBS += -DLINUX -pthread `pkg-config --libs sdl` `pkg-config --cflags sdl` 
GLLIBS += -lGL

else ifeq ($(pl), openbsd)
py=/usr/ports/lang/python/2.5/w-Python-2.5.4/Python-2.5.4/
CFLIBS += -I$(py) -I$(py)/Include  $(py)/libpython2.5.a -R/usr/X11R6/lib -lintl  -liconv -lreadline

else ifeq ($(pl),CYGWIN_NT-6.0)
CFLIBS += -DWINDOWS -I/usr/include/ncurses -I/usr/local/include/SDL

endif



TARGETS = s3dtv
all: $(TARGETS)

gl$(EXE): $(srcdir)/gl.c
	$(CC) -o $@ $? $(CFLIBS) -L/usr/X11R6/lib -lGL -lm inject_csi.c inject.c rote_keymap.c rote.c wtf.c $(PYLIBS)

sd$(EXE): $(srcdir)/sd.c
	$(CC) -o $@ $? $(CFLIBS) -L/usr/X11R6/lib  -lm inject_csi.c inject.c rote_keymap.c rote.c wtf.c -Xlinker  -g  /usr/local/lib/libSDL_draw.a
sdg$(EXE): $(srcdir)/sd.c
	$(CC)   -v -H -DGL -o $@ $? $(CFLIBS) -L/usr/X11R6/lib   inject_csi.c inject.c rote_keymap.c rote.c wtf.c -Xlinker  -g -lGL -lncurses  -lm




asd$(EXE): $(srcdir)/asd.c
	$(CC)   -o $@ $? -O2  $(GLLIBS) $(CFLIBS) 

asdf$(EXE): $(srcdir)/asd.c
	$(CC)  -o $@ $?  $(CFLIBS)  -llibSDL_draw





testgl$(EXE): $(srcdir)/testgl.c
	$(CC) -o $@ $? $(CFLIBS) -L/usr/X11R6/lib -lGL -lm inject_csi.c inject.c rote_keymap.c rote.c wtf.c $(PYLIBS)

curve$(EXE): $(srcdir)/curve.c 
	g++ curve.c -lGL -lGLU -lglut -lplibsg -lplibul



s3dtv$(EXE): $(srcdir)/s3dtv.c
    	$(CC) -o $@ $?   -L/usr/lib -L/usr/X11R6/lib  -lutil -L/usr/local/lib   -ltermcap  -L/usr/local/lib   -I/usr/include -I/usr/local/include -I./  -I/usr/local/include/vte -L/usr/X11R6/lib   roteterm/inject_csi.c roteterm/inject.c roteterm/rote_keymap.c roteterm/rote.c more-mess/wtf.c -lncurses  `pkg-config --cflags --libs libs3d`

    	


