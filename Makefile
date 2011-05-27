.PRECIOUS: mesh/%.c

OBJ = $(subst .obj,.o,$(wildcard mesh/*.obj))
HEADER = $(subst .obj,.h,$(wildcard mesh/*.obj))

CC=gcc

CFLAGS=-std=c99 -pipe -I ./mesh

all : walker

mesh/%.c mesh/%.h : mesh/%.obj mesh/%.mtl Makefile
	./tools/obj2opengl.pl -scale 10 $<

mesh/%.o : mesh/%.c
	$(CC) -pipe -c -o $@ $<

walker: walker.c nanosec.h $(OBJ) $(HEADER)
	if [[ $$(uname -s) = Linux ]]; then \
		$(CC) $(CFLAGS) `pkg-config --cflags --libs sdl gl glu` \
			-lglut $(OBJ) $< -o $@; \
	elif [[ $$(uname -s) = Darwin ]]; then \
		$(CC) $(CFLAGS) -DMAC `pkg-config --cflags --libs sdl` \
		-framework GLUT -I /opt/local/include $(OBJ) $< -o $@; \
	fi


run : walker
	./walker

fs : walker
	./walker -fs

note : walker
	./walker -fs 1024 600

clean :
	rm -f walker mesh/*.{h,c,o}
