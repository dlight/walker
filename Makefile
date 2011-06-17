.PRECIOUS: mesh/%.c

OBJ = $(subst .obj,.o,$(wildcard mesh/*.obj))
HEADER = $(subst .obj,.h,$(wildcard mesh/*.obj))

OS := $(shell uname -s)

CC = gcc

CFLAGS = -std=c99 -pipe -I ./mesh

all : walker

mesh/%.c mesh/%.h : mesh/%.obj mesh/%.mtl
	./tools/obj2opengl.pl -scale 1 $<

mesh/%.o : mesh/%.c
	$(CC) -pipe -c -o $@ $<

%.o : %.c
	echo $< , $@

use = `pkg-config --cflags --libs $(1)`

ifeq ($(OS), Linux)
OPENGL_FLAGS = $(call use,gl glu) -lglut
else ifeq ($(OS), Darwin)
OPENGL_FLAGS = -framework GLUT -I /opt/local/include
OSF = -DMAC
endif

compile = $(CC) $(CFLAGS) $(OSF)                    \
	$(call OPENGL_FLAGS) $(call use,sdl libpng) \
	$(1) $(2) -c -o $(subst .c,.o,$(1))

link = $(CC) $(CFLAGS) $(OSF)             \
	$(call OPENGL_FLAGS) $(call use,sdl libpng) \
	$(2) -o $(1)

walker.o : walker.c param.h nanosec.h texture.h $(HEADER)
	$(call compile, $<)

texture.o : texture.c texture.h
	$(call compile, $<)

walker: walker.o texture.o $(OBJ)
	$(call link, walker,$^)

run : walker
	./walker

debug : walker
	gdb ./walker

fs : walker
	./walker -fs

note : walker
	./walker -fs 1024 600

clean :
	rm -f walker mesh/*.{h,c,o}
