.PRECIOUS: mesh/%.c

OBJ = $(subst .obj,.o,$(wildcard mesh/*.obj))
HEADER = $(subst .obj,.h,$(wildcard mesh/*.obj))

CC=gcc

CFLAGS=-std=c99 -pipe -I ./mesh

all : bin/prog

mesh/%.c mesh/%.h : mesh/%.obj mesh/%.mtl
	./tools/obj2opengl.pl -scale 20 $<

mesh/%.o : mesh/%.c
	$(CC) -c -o $@ $<

bin/prog: prog.c nanosec.h $(OBJ) $(HEADER)
	if [[ $$(uname -s) -eq Linux ]]; then \
		$(CC) $(CFLAGS) `pkg-config --cflags --libs sdl gl glu` \
			-lglut $(OBJ) $< -o $@; \
	elif [[ $$(uname -s) -eq Darwin ]]; then \
		$(CC) $(CFLAGS) -DMAC `pkg-config --cflags --libs sdl` \
		-framework GLUT -I /opt/local/include $(OBJ) $< -o $@; \
	fi


run : bin/prog
	./bin/prog

fs : bin/prog
	./bin/prog -fs

clean :
	rm -f bin/prog mesh/*.h mesh/*.c mesh/*.o