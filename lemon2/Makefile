
srcdir  = .
# 
CC      = gcc-4  -g -ggdb 
GLLIBS  =  -DGL -DHAVE_OPENGL -lGL
CFLIBS =  -Dthreaded -D_REENTRANT -D_XTHREADS -DREENTRANT -DXTHREADS \
  -I/usr/local/include \
 -L/usr/lib -lSDL -L/usr/X11R6/lib  -lutil -L/usr/local/lib  \
    -L/usr/local/lib   `pkg-config --libs x11`\
      `pkg-config --cflags x11`\
       -I/usr/include -I/usr/local/include -I./ \
        -I/usr/local/include/vte -lncurses -L/usr/X11R6/lib \
          ../roteterm/inject_csi.c ../roteterm/inject.c\
           ../roteterm/rote_keymap.c ../roteterm/rote.c \
           ../roteterm/wtf.c -lncurses  ../tpl/tpl.c

pl=$(shell uname)

ifeq ($(pl),Linux)
CC      = gcc  -g -ggdb 
PYLIBS = -Dpython `/usr/bin/python2.6-config --cflags`  `/usr/bin/python2.6-config --libs ` 
CFLIBS += -DLINUX -pthread `pkg-config --libs sdl` `pkg-config --cflags sdl` 
GLLIBS += -lGL
# -lSDL_ttf 

else ifeq ($(pl), openbsd)
py=/usr/ports/lang/python/2.5/w-Python-2.5.4/Python-2.5.4/
CFLIBS += -I$(py) -I$(py)/Include  $(py)/libpython2.5.a -R/usr/X11R6/lib -lintl  -liconv -lreadline

else ifeq ($(pl),CYGWIN_NT-6.0)
CFLIBS += -DWINDOWS -I/usr/include/ncurses -I/usr/local/include/SDL -I/usr/include/SDL

endif



TARGETS = lsdl lgl
all: lgl
a: $(TARGETS)

X=$(srcdir)/../gltext.c ./glterm.c $(srcdir)/asd.c ../roteterm/* ../linetextdata.c

lgl$(EXE): $(X)
	$(CC)   -o $@ asd.c  -Dlibpng -lpng $(GLLIBS) $(CFLIBS) -Dnerve -lGLU  -lm
gpp$(EXE): $(X)
	g++   -o $@ asd.c  -Dlibpng -lpng $(GLLIBS) $(CFLIBS) -Dnerve -lGLU  -lm -Wno-write-strings

lgls3d$(EXE): $(X) ../s3d-0.2.1.1/server/*
	$(CC)   -o $@ -D swallows3d asd.c   -lpng $(GLLIBS) $(CFLIBS) -Dnerve ../s3d-0.2.1.1/server/*c -I../s3d-0.2.1.1/

lglpy$(EXE): $(X)
	$(CC)   -o $@ asd.c  -Dlibpng -lpng $(GLLIBS) $(CFLIBS) -Dnerve $(PYLIBS) -lGLU

lsdl$(EXE): $(srcdir)/asd.c $(srcdir)/../gltext.c ./glterm.c  ../roteterm/* ../linetextdata.c
	$(CC)  -o $@ asd.c -DSDLD  $(CFLIBS)   -lpng   -Xlinker   \
	    -g  /usr/local/lib/libSDL_draw.a

clean:
	rm ./lgl ./lsdl ./lglpy