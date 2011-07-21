.PRECIOUS: mesh/%.c

OBJ = $(subst .obj,.o,$(wildcard mesh/*.obj))
HEADER = $(subst .obj,.h,$(wildcard mesh/*.obj))

OS := $(shell uname -s)

#CC = gcc -pipe -std=c99

CC = clang

all : walker
	@echo
	@echo AEEEEEE
	@echo
	@echo compilou!!
	@echo
	@echo digite ./walker para rodar o programa
	@echo para mais informacoes leia o README

mesh/%.c mesh/%.h : mesh/%.obj tools/obj2opengl.pl Makefile
	./tools/obj2opengl.pl -noScale -noMove $< \
		$$(echo $< |sed 's/.obj$$/.png/')

mesh/%.o : mesh/%.c
	$(CC) -c -o $@ $<

mesh/mesh_ctrl.h : mk-mesh-ctrl.sh $(HEADER)
	./mk-mesh-ctrl.sh $^ > $@

use = `pkg-config --$(1) $(2)`

ifeq ($(OS), Linux)
flags = $(call use,$(1),gl glu sdl libpng)
OSL = -lglut
else ifeq ($(OS), Darwin)
flags = -I /opt/local/include -framework GLUT \
	$(call use,$(1),sdl libpng)
OSF = -DMAC
endif

compile = $(CC) -c $(OSF) $(call flags,cflags) $(1)

link = $(CC) $(OSL) $(call flags,libs) $(2) -o $(1)

walker.o : walker.c types.h event.h nanosec.h texture.h
	$(call compile,$<)

texture.o : texture.c texture.h
	$(call compile,$<)

nanosec.o : nanosec.c nanosec.h
	$(call compile,$<)

event.o : event.c types.h event.h
	$(call compile,$<)

gl.o : gl.c gl.h types.h texture.h event.h mesh/mesh_ctrl.h
	$(call compile,$<)

walker: nanosec.o texture.o event.o gl.o walker.o $(OBJ)
	$(call link,$@,$^)

run : walker
	./walker

debug : walker
	gdb ./walker

fs : walker
	./walker -fs

mac : walker
	./walker -fs 1280 800

note : walker
	./walker -fs 1024 600

clean :
	rm -f walker *.o

clean-mesh :
	rm -f mesh/*.o

clean-mesh-source :
	rm -f mesh/*.h mesh/*.c

clean-code : clean clean-mesh

mrproper : clean-code clean-mesh-source
