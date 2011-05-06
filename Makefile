include make.platform

.PRECIOUS: mesh/%.c

OBJ = $(subst .obj,.o,$(wildcard mesh/*.obj))
HEADER = $(subst .obj,.h,$(wildcard mesh/*.obj))

CC=gcc

CFLAGS=-std=c99 -pipe -I ./mesh

all : bin/prog

mesh/%.c mesh/%.h : mesh/%.obj mesh/%.mtl
	./tools/obj2opengl.pl -scale 20 $<

<<<<<<< HEAD
mesh/%.o : mesh/%.c
	$(CC) -c -o $@ $<

bin/prog: prog.c nanosec.h $(OBJ) $(HEADER)
ifeq ($(PLATFORM), linux)
	$(CC) $(CFLAGS) `pkg-config --cflags --libs sdl gl glu` -lglut \
			$(OBJ) $< -o $@
else ifeq ($(PLATFORM), mac)
	$(CC) $(CFLAGS) -DMAC `pkg-config --cflags --libs sdl` \
	-framework GLUT -I /opt/local/include \
			$(OBJ) $< -o $@
=======
bin/prog: prog.c nanosec.h $(subst .obj,.h,$(wildcard mesh/*.obj))
ifeq ($(PLATFORM), Linux)
	gcc -std=c99 `pkg-config --cflags --libs sdl gl glu` -lglut -pipe \
			-I ./mesh -o $@ $<
else
	gcc -std=c99 `pkg-config --cflags --libs sdl` -DMAC -framework GLUT \
		-I /opt/local/include -pipe -I ./mesh  -o $@ \
		$<
>>>>>>> FETCH_HEAD
endif


run : bin/prog
	./bin/prog

fs : bin/prog
	./bin/prog -fs

clean :
	rm -f bin/prog mesh/*.h mesh/*.c mesh/*.o
