#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
 } rgba;

rgba* load_png(char* filename, unsigned* w,
               unsigned* h);
GLuint setup_texture(rgba* image_data, unsigned w,
                     unsigned h);
GLuint png_texture(char* filename);

GLuint png_loadmap(char* filename, rgba* image,
                   unsigned* w, unsigned* h);

#endif
