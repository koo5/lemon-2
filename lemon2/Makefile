# Makefile to build the SDL tests



srcdir  = .
# 
CC      = gcc-4  -g -ggdb 
GLLIBS  =  -DGL -DHAVE_OPENGL -lGL
CFLIBS = -Dthreaded -D_REENTRANT -D_XTHREADS -DREENTRANT -DXTHREADS \
  -I/usr/local/include \
 -L/usr/lib -lSDL -L/usr/X11R6/lib  -lutil -L/usr/local/lib  \
    -L/usr/local/lib   `pkg-config --libs x11`\
      `pkg-config --cflags x11`\
       -I/usr/include -I/usr/local/include -I./ \
        -I/usr/local/include/vte -lncurses -L/usr/X11R6/lib \
          ../roteterm/inject_csi.c ../roteterm/inject.c\
           ../roteterm/rote_keymap.c ../roteterm/rote.c \
           ../more-mess/wtf.c -lncurses 

pl=$(shell uname)

ifeq ($(pl),Linux)
CC      = gcc  -g -ggdb 
PYLIBS = `/usr/bin/python2.5-config --cflags`  `/usr/bin/python2.5-config --libs ` 
CFLIBS += -DLINUX -pthread `pkg-config --libs sdl` `pkg-config --cflags sdl` 
GLLIBS += -lGL
# -lSDL_ttf 

else ifeq ($(pl), openbsd)
py=/usr/ports/lang/python/2.5/w-Python-2.5.4/Python-2.5.4/
CFLIBS += -I$(py) -I$(py)/Include  $(py)/libpython2.5.a -R/usr/X11R6/lib -lintl  -liconv -lreadline

else ifeq ($(pl),CYGWIN_NT-6.0)
CFLIBS += -DWINDOWS -I/usr/include/ncurses -I/usr/local/include/SDL

endif



TARGETS = lsdl lgl
all: $(TARGETS)


lgl$(EXE): $(srcdir)/../gltext.c ./glterm.c $(srcdir)/asd.c ../roteterm/* ../linetextdata
	$(CC)   -o $@ asd.c   -lpng $(GLLIBS) $(CFLIBS) -Dnerve

lsdl$(EXE): $(srcdir)/asd.c $(srcdir)/../gltext.c ./glterm.c  ../roteterm/* ../linetextdata
	$(CC)  -o $@ asd.c -DSDLD  $(CFLIBS)   -lpng   -Xlinker   \
	    -g  /usr/local/lib/libSDL_draw.a

clean:
	rm ./lgl ./lsdl