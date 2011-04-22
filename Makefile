all : bin/prog

conf :
	./configure

mesh/%.h : mesh/%.obj mesh/%.mtl
	./tools/obj2opengl.pl $<

bin/prog: prog.c mesh/casa.h $(subst .obj,.h,$(wildcard mesh/*.obj))
	gcc `pkg-config --cflags --libs sdl gl glu` -lglut -lm -pipe \
		-I ./mesh -o $@ $<


run : bin/prog
	./bin/prog

fs : bin/prog
	./bin/prog -fs

clean :
	rm -f bin/prog mesh/*.h