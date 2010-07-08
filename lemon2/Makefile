
DEPS  =  ../roteterm/* lemon.cpp ../* ../toys/atlantis/* ../toys/nerverot/* ../sdl/lib/libSDL.so ../yaml-cpp-0.2.5/libyaml-cpp.so
GLLIBS  =  -DGL -DHAVE_OPENGL -lGL
CFLIBS =  -Dthreaded -D_REENTRANT -D_XTHREADS -DREENTRANT -DXTHREADS \
  -I/usr/local/include \
 -L/usr/lib -L/usr/X11R6/lib  -lutil -L/usr/local/lib  \
    -L/usr/local/lib   `pkg-config --libs x11`\
      `pkg-config --cflags x11`\
       -I/usr/include -I/usr/local/include -I./ \
        -I/usr/local/include/vte -lncurses -L/usr/X11R6/lib \
          ../roteterm/inject_csi.c ../roteterm/inject.c\
           ../roteterm/rote_keymap.c ../roteterm/rote.c \
           ../roteterm/wtf.c -lncurses  \
           ../yaml-cpp-0.2.5/libyaml-cpp.so

pl=$(shell uname)

ifeq ($(pl),Linux)
PYLIBS = -Dpython `/usr/bin/python2.6-config --cflags`  `/usr/bin/python2.6-config --libs ` 
CFLIBS += -DLINUX -pthread `../sdl/bin/sdl-config --libs --cflags` 
GLLIBS += -lGL
# -lSDL_ttf 

else ifeq ($(pl), openbsd)
py=/usr/ports/lang/python/2.5/w-Python-2.5.4/Python-2.5.4/
CFLIBS += -I$(py) -I$(py)/Include  $(py)/libpython2.5.a -R/usr/X11R6/lib -lintl  -liconv -lreadline

else ifeq ($(pl),CYGWIN_NT-6.0)
CFLIBS += -DWINDOWS -I/usr/include/ncurses -I/usr/local/include/SDL -I/usr/include/SDL

endif

                                                                       
CC      =g++ -o $@ lemon.cpp -g -ggdb  $(GLLIBS) $(CFLIBS) -std=c++0x -W -Wall -Wextra  -pedantic



TARGETS = lsdl lgl
all: lgl
a: $(TARGETS)
lgl$(EXE): $(DEPS)
	$(CC) -pg -Dlibpng -lpng -Dnerve -lGLU  -lm -Wno-write-strings  -lXcomposite -lXdamage -lXfixes -lXrender -lX11 

test :
	g++ -o test test.cpp  

lsdl$(EXE):  $(DEPS)
	$(CC)  -DSDLD  -lpng   -Xlinker   \
	    -g  /usr/local/lib/libSDL_draw.a

clean:
	rm ./lgl ./lsdl ./lglpy
	

../sdl/lib/libSDL.so:
	cd ../SDL-1.2.14    &&\
	./configure --prefix=`pwd`/../sdl &&\
	make&&\
	make install

../yaml-cpp-0.2.5/libyaml-cpp.so:
	cd ../yaml-cpp-0.2.5&&\
	cmake CMakeLists.txt &&\
	make


