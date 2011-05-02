include make.platform

all : bin/prog

conf :
	./configure

mesh/%.h : mesh/%.obj mesh/%.mtl
	./tools/obj2opengl.pl $<

bin/prog: prog.c mesh/casa.h $(subst .obj,.h,$(wildcard mesh/*.obj))
ifeq ($(PLATFORM), linux)
	gcc `pkg-config --cflags --libs sdl gl glu` -lglut -pipe \
			-I ./mesh -o $@ $<
else
	gcc `pkg-config --cflags --libs sdl` -framework GLUT \
		-l /opt/local/include -pipe -I ./mesh  -o $@ \
		$<
endif


run : bin/prog
	./bin/prog

fs : bin/prog
	./bin/prog -fs

clean :
	rm -f bin/prog mesh/*.h