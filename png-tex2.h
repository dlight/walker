#define PNG_DEBUG 3
#include <png.h>

#define TEXTURE_LOAD_ERROR 0

typedef struct {
    png_byte r;
    png_byte g;
    png_byte b;
    png_byte a;
 } rgba;

GLuint loadTexture(char* filename, unsigned* width, unsigned* height) 
{
    fprintf(stderr, "   Compiled with libpng %s; using libpng %s.\n",
        PNG_LIBPNG_VER_STRING, png_libpng_ver);
    fprintf(stderr, "   Compiled with zlib %s; using zlib %s.\n",
            ZLIB_VERSION, zlib_version);

    unsigned char header[8];
 
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("can't open file\n");
        return TEXTURE_LOAD_ERROR;
    }
 
    fread(header, 1, 8, fp);
 
    if (!png_check_sig(header, 8)) {
        fclose(fp);
        printf("signature error\n");
        return TEXTURE_LOAD_ERROR;
    }
 
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 NULL,
                                                 NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        printf("no png ptr\n");
        return (TEXTURE_LOAD_ERROR);
    }
 
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        printf("no info ptr\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return (TEXTURE_LOAD_ERROR);
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        printf("png error really rs\n");
        return (TEXTURE_LOAD_ERROR);
    }
 
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    png_uint_32 w, h;

    w =  png_get_image_width(png_ptr, info_ptr);

    h = png_get_image_height(png_ptr, info_ptr);

    png_bytep *row_pointers =
        (png_bytep*) malloc(sizeof(png_bytep) * h);
    for (int y=0; y<h; y++) {
        row_pointers[y] = (png_byte*)
            malloc(png_get_rowbytes(png_ptr,info_ptr));
    }
 
    png_read_image(png_ptr, row_pointers);
    
    fclose(fp);

    rgba* image_data = malloc(4 * sizeof(rgba) * w * h);

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
 
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, w, h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) image_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    free(image_data);
    free(row_pointers);
 
    return texture;
}
