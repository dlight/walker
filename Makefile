include make.platform

all : bin/prog

mesh/%.h : mesh/%.obj mesh/%.mtl
	./tools/obj2opengl.pl -scale 20 $<

bin/prog: prog.c $(subst .obj,.h,$(wildcard mesh/*.obj))
ifeq ($(PLATFORM), linux)
	gcc -std=c99 `pkg-config --cflags --libs sdl gl glu` -lglut -pipe \
			-I ./mesh -o $@ $<
else
	gcc -std=c99 `pkg-config --cflags --libs sdl` -DMAC -framework GLUT \
		-l /opt/local/include -pipe -I ./mesh  -o $@ \
		$<
endif


run : bin/prog
	./bin/prog

fs : bin/prog
	./bin/prog -fs

clean :
	rm -f bin/prog make.platform mesh/*.h