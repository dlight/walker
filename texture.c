#define PNG_DEBUG 3

#include <stdlib.h>
#include <png.h>

#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "texture.h"

rgba* load_png(char* filename, unsigned* width,
               unsigned* height) 
{
    fprintf(stderr, "   Compiled with libpng %s; using libpng %s.\n",
        PNG_LIBPNG_VER_STRING, png_libpng_ver);
    fprintf(stderr, "   Compiled with zlib %s; using zlib %s.\n",
            ZLIB_VERSION, zlib_version);

    unsigned char header[8];
 
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("can't open file\n");
        return 0;
    }
 
    fread(header, 1, 8, fp);
 
    if (!png_check_sig(header, 8)) {
        fclose(fp);
        printf("signature error\n");
        return 0;
    }
 
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 NULL,
                                                 NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        printf("no png ptr\n");
        return 0;
    }
 
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        printf("no info ptr\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        printf("png error really rs\n");
        return 0;
    }
 
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    png_uint_32 w, h;

    w =  png_get_image_width(png_ptr, info_ptr);

    h = png_get_image_height(png_ptr, info_ptr);

    *width = w;
    *height = h;

    png_bytep *row_pointers =
        (png_bytep*) malloc(sizeof(png_bytep) * h);
    for (int y=0; y<h; y++) {
        row_pointers[y] = (png_byte*)
            malloc(png_get_rowbytes(png_ptr,info_ptr));
    }
 
    png_read_image(png_ptr, row_pointers);
    
    fclose(fp);

    rgba* image_data = malloc(4 * sizeof(rgba) * w*h);

    for (int y=0; y<h; y++) {
        png_byte* row = row_pointers[y];
        for (int x=0; x<w; x++) {
            png_byte* ptr = &(row[x*4]);

            rgba* ptr2 = &(image_data[y*h]);

            ptr2[x].r = ptr[0];
            ptr2[x].g = ptr[1];
            ptr2[x].b = ptr[2];
            ptr2[x].a = ptr[3];
        }
    }

    free(row_pointers);

    return image_data;
}

GLuint setup_texture(rgba* image_data, unsigned w,
                     unsigned h)
{
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, w, h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) image_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
    return texture;
}

GLuint png_texture(char* filename, unsigned* w,
                   unsigned* h)
{
    rgba* t = load_png(filename, w, h);

    return setup_texture(t, *w, *h);

    free(t);
}
